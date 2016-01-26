/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "ioDicom/SSliceIndexDicomEditor.hpp"

#include <fwCom/Signal.hpp>
#include <fwCom/Signal.hxx>
#include <fwCom/Signals.hpp>
#include <fwCom/Slot.hpp>
#include <fwCom/Slots.hpp>
#include <fwCom/Slots.hxx>
#include <fwComEd/Dictionary.hpp>
#include <fwComEd/helper/Array.hpp>
#include <fwComEd/helper/Composite.hpp>
#include <fwComEd/helper/SeriesDB.hpp>
#include <fwData/Array.hpp>
#include <fwData/Composite.hpp>
#include <fwData/Image.hpp>
#include <fwData/Integer.hpp>
#include <fwDicomData/DicomSeries.hpp>
#include <fwGui/dialog/MessageDialog.hpp>
#include <fwGuiQt/container/QtContainer.hpp>
#include <fwMedData/ImageSeries.hpp>
#include <fwMedData/SeriesDB.hpp>
#include <fwServices/macros.hpp>
#include <fwServices/registry/ActiveWorkers.hpp>
#include <fwServices/registry/ObjectService.hpp>
#include <fwThread/Timer.hpp>
#include <fwTools/System.hpp>

#include <QApplication>
#include <QComboBox>
#include <QHBoxLayout>
#include <QMouseEvent>

#include <boost/asio/placeholders.hpp>
#include <boost/filesystem/fstream.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/foreach.hpp>

#include <iterator>

namespace ioDicom
{

fwServicesRegisterMacro( ::gui::editor::IEditor, ::ioDicom::SSliceIndexDicomEditor,
                         ::fwDicomData::DicomSeries );

const ::fwCom::Slots::SlotKeyType SSliceIndexDicomEditor::s_READ_IMAGE_SLOT      = "readImage";
const ::fwCom::Slots::SlotKeyType SSliceIndexDicomEditor::s_DISPLAY_MESSAGE_SLOT = "displayErrorMessage";

//------------------------------------------------------------------------------

SSliceIndexDicomEditor::SSliceIndexDicomEditor() throw() :
    m_delay(500)
{

    m_slotReadImage = ::fwCom::newSlot(&SSliceIndexDicomEditor::readImage, this);
    ::fwCom::HasSlots::m_slots(s_READ_IMAGE_SLOT, m_slotReadImage);

    m_slotDisplayMessage = ::fwCom::newSlot(&SSliceIndexDicomEditor::displayErrorMessage, this);
    ::fwCom::HasSlots::m_slots(s_DISPLAY_MESSAGE_SLOT, m_slotDisplayMessage);

#ifdef COM_LOG
    m_slotReadImage->setID( s_READ_IMAGE_SLOT );
    m_slotDisplayMessage->setID( s_DISPLAY_MESSAGE_SLOT );
    ::fwCom::HasSignals::m_signals.setID();
#endif

    ::fwCom::HasSlots::m_slots.setWorker( m_associatedWorker );


}
//------------------------------------------------------------------------------

SSliceIndexDicomEditor::~SSliceIndexDicomEditor() throw()
{
}

//------------------------------------------------------------------------------

void SSliceIndexDicomEditor::info(std::ostream &_sstream )
{
    _sstream << "SSliceIndexDicomEditor::info";
}

//------------------------------------------------------------------------------

void SSliceIndexDicomEditor::starting() throw(::fwTools::Failed)
{
    m_delayTimer2 = m_associatedWorker->createTimer();

    // Composite
    m_composite = ::fwData::Composite::dynamicCast(::fwTools::fwID::getObject(m_compositeUID));
    SLM_ASSERT("Composite should not be null !", m_composite);

    ::fwGui::IGuiContainerSrv::create();
    ::fwGuiQt::container::QtContainer::sptr qtContainer = fwGuiQt::container::QtContainer::dynamicCast(getContainer());

    QWidget* const container = qtContainer->getQtContainer();
    QHBoxLayout* layout      = new QHBoxLayout();
    container->setLayout(layout);

    ::fwDicomData::DicomSeries::sptr dicomSeries = this->getObject< ::fwDicomData::DicomSeries >();
    SLM_ASSERT("DicomSeries should not be null !", dicomSeries);
    m_numberOfSlices = dicomSeries->getNumberOfInstances();

    // Slider
    m_sliceIndexSlider = new QSlider(Qt::Horizontal);
    layout->addWidget(m_sliceIndexSlider,1);
    m_sliceIndexSlider->setRange(0, static_cast<int>(m_numberOfSlices-1));
    m_sliceIndexSlider->setValue(static_cast<int>(m_numberOfSlices/2));

    // Line Edit
    m_sliceIndexLineEdit = new QLineEdit();
    layout->addWidget(m_sliceIndexLineEdit,0);
    m_sliceIndexLineEdit->setReadOnly(true);
    m_sliceIndexLineEdit->setMaximumWidth(80);

    std::stringstream ss;
    ss << m_sliceIndexSlider->value() << " / " << (m_numberOfSlices-1);
    m_sliceIndexLineEdit->setText(std::string(ss.str()).c_str());


    // Connect the signals
    QObject::connect(m_sliceIndexSlider, SIGNAL(valueChanged(int)), this, SLOT(changeSliceIndex(int)));

    // Create temporary SeriesDB
    m_tempSeriesDB = ::fwMedData::SeriesDB::New();

    // Create reader
    ::fwServices::registry::ServiceFactory::sptr srvFactory = ::fwServices::registry::ServiceFactory::getDefault();


    ::io::IReader::sptr dicomReader;
    dicomReader = ::io::IReader::dynamicCast(srvFactory->create(m_dicomReaderType));
    SLM_ASSERT("Unable to create a reader of type: \"" + m_dicomReaderType + "\" in "
               "::ioDicom::SSliceIndexDicomEditor.", dicomReader);
    ::fwServices::OSR::registerService(m_tempSeriesDB, dicomReader);

    if(m_readerConfig)
    {
        dicomReader->setConfiguration(m_readerConfig);
    }

    dicomReader->configure();
    dicomReader->start();

    m_dicomReader = dicomReader;

    // Image Indecies
    m_axialIndex    = ::fwData::Integer::New(0);
    m_frontalIndex  = ::fwData::Integer::New(0);
    m_sagittalIndex = ::fwData::Integer::New(0);

    // Load a slice
    ::boost::chrono::milliseconds duration = ::boost::chrono::milliseconds(m_delay);
    m_delayTimer2->setFunction(  [ = ]()
        {
            this->triggerNewSlice();
        }  );
    m_delayTimer2->setDuration(duration);
    m_delayTimer2->setOneShot(true);

    this->triggerNewSlice();

}

//------------------------------------------------------------------------------

void SSliceIndexDicomEditor::stopping() throw(::fwTools::Failed)
{
    SLM_TRACE_FUNC();

    // Stop dicom reader
    if(!m_dicomReader.expired())
    {
        m_dicomReader.lock()->stop();
        ::fwServices::OSR::unregisterService(m_dicomReader.lock());
    }

    // Disconnect the signals
    QObject::disconnect(m_sliceIndexSlider, SIGNAL(valueChanged(int)), this, SLOT(changeSliceIndex(int)));

    this->getContainer()->clean();
    this->::fwGui::IGuiContainerSrv::destroy();
}

//------------------------------------------------------------------------------

void SSliceIndexDicomEditor::configuring() throw(::fwTools::Failed)
{
    SLM_TRACE_FUNC();
    ::fwGui::IGuiContainerSrv::initialize();

    ::fwRuntime::ConfigurationElement::sptr config = m_configuration->findConfigurationElement("config");
    SLM_ASSERT("The service ::ioDicom::SSliceIndexDicomEditor must have "
               "a \"config\" element.",config);

    bool success;

    // Composite UID
    ::boost::tie(success, m_compositeUID) = config->getSafeAttributeValue("compositeUID");
    SLM_ASSERT("It should be a \"compositeUID\" tag in the ::ioDicom::SSliceIndexDicomEditor config element.",
               success);

    // Image Key
    ::boost::tie(success, m_imageKey) = config->getSafeAttributeValue("imageKey");
    SLM_ASSERT("It should be an \"imageKey\" tag in the "
               "::ioDicom::SSliceIndexDicomEditor config element.", success);

    // Reader
    ::boost::tie(success, m_dicomReaderType) = config->getSafeAttributeValue("dicomReader");
    SLM_ASSERT("It should be a \"dicomReader\" tag in the ::ioDicom::SSliceIndexDicomEditor "
               "config element.", success);

    // Reader configuration
    ::fwRuntime::ConfigurationElement::sptr readerConfig = config->findConfigurationElement("dicomReaderConfig");
    m_readerConfig                                       =
        (readerConfig && readerConfig->size() == 1) ? readerConfig->getElements()[0] : nullptr;

    // Delay
    std::string delayStr;
    ::boost::tie(success, delayStr) = config->getSafeAttributeValue("delay");
    if(success)
    {
        m_delay = ::boost::lexical_cast< std::size_t >(delayStr);
    }

}

//------------------------------------------------------------------------------

void SSliceIndexDicomEditor::updating() throw(::fwTools::Failed)
{
}

//------------------------------------------------------------------------------

void SSliceIndexDicomEditor::changeSliceIndex(int value)
{
    // Update text
    std::stringstream ss;
    ss << m_sliceIndexSlider->value() << " / " << (m_numberOfSlices-1);
    m_sliceIndexLineEdit->setText(std::string(ss.str()).c_str());

    // Get the new slice if there is no change for m_delay milliseconds
    m_delayTimer2->start();

}

//------------------------------------------------------------------------------

void SSliceIndexDicomEditor::triggerNewSlice()
{
    // DicomSeries
    ::fwDicomData::DicomSeries::sptr dicomSeries = this->getObject< ::fwDicomData::DicomSeries >();
    SLM_ASSERT("DicomSeries should not be null !", dicomSeries);

    // Compute slice index
    std::size_t selectedSliceIndex = m_sliceIndexSlider->value() + dicomSeries->getFirstInstanceNumber();
    OSLM_TRACE("triggered new slice : " << selectedSliceIndex);

    SLM_ERROR_IF("There is no instance available for selected slice index.",
                 !dicomSeries->isInstanceAvailable(selectedSliceIndex));

    if(dicomSeries->isInstanceAvailable(selectedSliceIndex))
    {
        m_slotReadImage->asyncRun(selectedSliceIndex);
    }
}

//------------------------------------------------------------------------------

void SSliceIndexDicomEditor::readImage(std::size_t selectedSliceIndex)
{
    // DicomSeries
    ::fwDicomData::DicomSeries::sptr dicomSeries = this->getObject< ::fwDicomData::DicomSeries >();
    SLM_ASSERT("DicomSeries should not be null !", dicomSeries);
    if( dicomSeries->getModality() != "CT" && dicomSeries->getModality() != "MR")
    {
        return;
    }

    // Clear temporary seriesDB
    ::fwComEd::helper::SeriesDB sDBTempohelper(m_tempSeriesDB);
    sDBTempohelper.clear();

    // Creates unique temporary folder, no need to check if exists before (see ::fwTools::System::getTemporaryFolder)
    ::boost::filesystem::path path    = ::fwTools::System::getTemporaryFolder("dicom");
    ::boost::filesystem::path tmpPath = path / "tmp";

    SLM_INFO("Create " + tmpPath.string());
    ::boost::filesystem::create_directories(tmpPath);


    SLM_ASSERT("Dicom data shall be available before reading them.",
               dicomSeries->getDicomAvailability() != ::fwDicomData::DicomSeries::NONE
               || dicomSeries->isInstanceAvailable(selectedSliceIndex));

    if(dicomSeries->getDicomAvailability() != ::fwDicomData::DicomSeries::BINARIES )
    {
        ::fwDicomData::DicomSeries::DicomPathContainerType paths = dicomSeries->getLocalDicomPaths();
        ::boost::filesystem::path& src                           = paths[selectedSliceIndex];

        const ::boost::filesystem::path& dest = tmpPath / src.filename();

        ::boost::system::error_code err;
        ::boost::filesystem::create_hard_link(src, dest, err);
        if (err.value() != 0)
        {
            SLM_INFO("Copying " + src.string() + " to " + dest.string());

            // Use stream instead of boost::copy_file (Unix c++11 issue)
            ::boost::filesystem::ifstream inStream(src, std::ios::binary);
            OSLM_ERROR_IF("Unable to read file :" << src.string(), !inStream.good());
            ::boost::filesystem::ofstream outStream(dest, std::ios::binary);
            OSLM_ERROR_IF("Unable to write file :" << dest.string(), !outStream.good());

            outStream << inStream.rdbuf();

            inStream.close();
            outStream.close();

            ::boost::filesystem::permissions(dest, ::boost::filesystem::owner_all, err);
            SLM_ERROR_IF("set permission error : " + err.message(), err.value());
        }
    }
    else if(dicomSeries->getDicomAvailability() == ::fwDicomData::DicomSeries::BINARIES)
    {

        const ::fwDicomData::DicomSeries::DicomBinaryContainerType& binaries = dicomSeries->getDicomBinaries();
        ::fwDicomData::DicomSeries::DicomBinaryContainerType::const_iterator binary = binaries.begin();
        std::advance(binary, selectedSliceIndex);

        ::fwData::Array::sptr array = binary->second;
        ::fwComEd::helper::Array arrayHelper(array);
        char* buffer = static_cast<char*>(arrayHelper.getBuffer());
        size_t size  = array->getSizeInBytes();

        const ::boost::filesystem::path dest = tmpPath / binary->first;
        ::boost::filesystem::ofstream fs(dest, std::ios::binary|std::ios::trunc);
        FW_RAISE_IF("Can't open '" << tmpPath << "' for write.", !fs.good());

        fs.write(buffer, size);
        fs.close();
    }

    // Read image
    m_dicomReader.lock()->setFolder(tmpPath);
    if(!m_dicomReader.expired())
    {
        m_dicomReader.lock()->update();

        if(m_dicomReader.expired() || m_dicomReader.lock()->isStopped())
        {
            return;
        }
    }
    else
    {
        return;
    }

    //Copy image
    ::fwMedData::ImageSeries::sptr imageSeries;

    if(m_tempSeriesDB->getContainer().size() > 0)
    {
        auto series = *(m_tempSeriesDB->getContainer().begin());
        if( series->getModality() == "CT" || series->getModality() == "MR")
        {
            imageSeries = ::fwMedData::ImageSeries::dynamicCast(series);
        }
    }

    if(imageSeries)
    {
        ::fwComEd::helper::Composite helper(m_composite);
        ::fwData::Image::sptr newImage    = imageSeries->getImage();
        ::fwData::Image::SizeType newSize = newImage->getSize();

        if(m_composite->find(m_imageKey) == m_composite->end())
        {
            helper.add(m_imageKey, newImage);
        }
        else
        {
            ::fwData::Image::sptr oldImage    = ::fwData::Image::dynamicCast(m_composite->getContainer()[m_imageKey]);
            ::fwData::Image::SizeType oldSize = oldImage->getSize();
            if(dicomSeries->getDicomAvailability() != ::fwDicomData::DicomSeries::BLOB
               && newSize[0] == oldSize[0] && newSize[1] == oldSize[1] && newSize[2] == oldSize[2])
            {
                // Copy buffer
                oldImage->setDataArray(newImage->getDataArray(), false);
                auto sig = oldImage->signal< ::fwData::Object::ModifiedSignalType >(::fwData::Object::s_MODIFIED_SIG);
                {
                    ::fwCom::Connection::Blocker block(sig->getConnection(m_slotUpdate));
                    sig->asyncEmit();
                }
            }
            else
            {
                // Swap image
                helper.swap(m_imageKey, newImage);
            }

        }

        newImage->setField(::fwComEd::Dictionary::m_axialSliceIndexId, m_axialIndex);
        m_frontalIndex->setValue(static_cast< int >(newSize[0]/2));
        newImage->setField(::fwComEd::Dictionary::m_frontalSliceIndexId, m_frontalIndex);
        m_sagittalIndex->setValue(static_cast< int >(newSize[1]/2));
        newImage->setField(::fwComEd::Dictionary::m_sagittalSliceIndexId, m_sagittalIndex);

        helper.notify();
    }

    ::boost::system::error_code ec;
    ::boost::filesystem::remove_all(path, ec);
    SLM_ERROR_IF("remove_all error for path " + path.string() + ": " + ec.message(), ec.value());
}

//------------------------------------------------------------------------------

void SSliceIndexDicomEditor::displayErrorMessage(const std::string& message) const
{
    SLM_WARN("Error: " + message);
    ::fwGui::dialog::MessageDialog messageBox;
    messageBox.setTitle("Error");
    messageBox.setMessage( message );
    messageBox.setIcon(::fwGui::dialog::IMessageDialog::CRITICAL);
    messageBox.addButton(::fwGui::dialog::IMessageDialog::OK);
    messageBox.show();
}

} // namespace ioDicom
