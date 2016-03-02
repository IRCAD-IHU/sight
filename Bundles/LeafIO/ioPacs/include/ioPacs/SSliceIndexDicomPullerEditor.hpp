/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __IOPACS_SSLICEINDEXDICOMPULLEREDITOR_HPP__
#define __IOPACS_SSLICEINDEXDICOMPULLEREDITOR_HPP__

#include "ioPacs/config.hpp"

#include <fwCom/Slot.hpp>
#include <fwPacsIO/data/PacsConfiguration.hpp>
#include <fwPacsIO/SeriesEnquirer.hpp>
#include <gui/editor/IEditor.hpp>
#include <io/IReader.hpp>

#include <QLineEdit>
#include <QObject>
#include <QPointer>
#include <QSlider>
#include <QWidget>

#include <boost/asio/deadline_timer.hpp>
#include <boost/asio/io_service.hpp>
#include <boost/filesystem/path.hpp>

namespace fwData
{
class Composite;
class Integer;
}

namespace fwThread
{
class Timer;
}

namespace fwMedData
{
class SeriesDB;
}

namespace fwRuntime
{
struct ConfigurationElement;
}

namespace ioPacs
{

/**
 * @brief   This editor service is used to select a slice index and pull the image from the pacs if it is not
 *           available on the local computer.
 * @class   SSliceIndexDicomPullerEditor
 * @date    2013.
 */
class IOPACS_CLASS_API SSliceIndexDicomPullerEditor : public QObject,
                                                      public ::gui::editor::IEditor
{
Q_OBJECT;

public:

    fwCoreServiceClassDefinitionsMacro ( (SSliceIndexDicomPullerEditor)( ::gui::editor::IEditor ) );

    IOPACS_API static const ::fwCom::Slots::SlotKeyType s_READ_IMAGE_SLOT;
    typedef ::fwCom::Slot<void (std::size_t)> ReadImageSlotType;

    IOPACS_API static const ::fwCom::Slots::SlotKeyType s_DISPLAY_MESSAGE_SLOT;
    typedef ::fwCom::Slot<void (const std::string&)> DisplayMessageSlotType;

    /**
     * @brief Constructor
     */
    IOPACS_API SSliceIndexDicomPullerEditor() throw();

    /**
     * @brief Destructor
     */
    IOPACS_API virtual ~SSliceIndexDicomPullerEditor() throw();

private Q_SLOTS:
    /**
     * @brief Slot called when the slider is moved
     * @param[in] value Slider value
     */
    IOPACS_API void changeSliceIndex(int value);

protected:

    /**
     * @brief Configuring method. This method is used to configure the service.
     *
     * XML configuration sample:
       @code{.xml}
       <service uid="sliderIndexDicomPullerEditor" type="::gui::editor::IEditor"
         impl="::ioPacs::SSliceIndexDicomPullerEditor" autoConnect="yes">
         <config compositeUID="previewComposite" imageKey="image" dicomReader="::ioGdcm::SSeriesDBReader"
            pacsConfigurationUID="pacsConfiguration" delay="500">
            <dicomReaderConfig> <!-- optional -->
                <!-- here goes configuration for dicom reader implementation -->
            </dicomReaderConfig>
         </config>
       </service>
       @endcode
     */
    IOPACS_API virtual void configuring() throw(::fwTools::Failed);

    /// Override
    IOPACS_API virtual void starting() throw(::fwTools::Failed);

    /// Override
    IOPACS_API virtual void stopping() throw(::fwTools::Failed);

    /// Override
    IOPACS_API void updating() throw(::fwTools::Failed);

    /// Override
    IOPACS_API void info(std::ostream &_sstream );

    /// Function called when a new slice must be displayed
    IOPACS_API void triggerNewSlice();

    /**
     * @brief Read the selected image
     * @param[in] selectedSliceIndex Selected slice of the image that must be read
     */
    IOPACS_API void readImage(std::size_t selectedSliceIndex);

    /// Pull the selected slice from the pacs
    IOPACS_API void pullInstance();

    /**
     * @brief Displays a dialog box with the error message
     */
    IOPACS_API void displayErrorMessage(const std::string& message) const;

    /**
     * @brief Store instance callback
     * @param[in] seriesInstanceUID Series instance UID
     * @param[in] instanceNumber Instance number
     * @param[in] filePath File path
     */
    IOPACS_API void storeInstanceCallback(
        const std::string& seriesInstanceUID, unsigned int instanceNumber, const std::string& filePath);

    /// Slot to call readLocalSeries method
    ReadImageSlotType::sptr m_slotReadImage;

    /// Slot to call displayErrorMessage method;
    DisplayMessageSlotType::sptr m_slotDisplayMessage;

    /// Slice index slider
    QPointer< QSlider > m_sliceIndexSlider;

    /// Slice index line edit
    QPointer< QLineEdit > m_sliceIndexLineEdit;

    /// Number of instances
    std::size_t m_numberOfSlices;

    /// Pacs Configuration UID
    std::string m_pacsConfigurationUID;

    /// Pacs Configuration object
    ::fwPacsIO::data::PacsConfiguration::sptr m_pacsConfiguration;

    /// IOPACS Reader
    std::string m_dicomReaderType;

    /// Reader
    ::io::IReader::wptr m_dicomReader;

    /// Image Key
    std::string m_imageKey;

    /// Composite UID
    std::string m_compositeUID;

    /// Composite
    SPTR(::fwData::Composite) m_composite;

    /// Temporary SeriesDB
    SPTR(::fwMedData::SeriesDB) m_tempSeriesDB;

    /// Axial slice index
    SPTR(fwData::Integer) m_axialIndex;
    /// Frontal slice index
    SPTR(::fwData::Integer) m_frontalIndex;
    /// Sagittal slice index
    SPTR(::fwData::Integer) m_sagittalIndex;

    /// Pull Worker
    ::fwThread::Worker::sptr m_pullSeriesWorker;

    /// Series enquirer
    ::fwPacsIO::SeriesEnquirer::sptr m_seriesEnquirer;

    /// Timer used to generate the new slice selection delay
    SPTR(::fwThread::Timer) m_delayTimer2;

    /// Delay
    unsigned int m_delay;

    /// Optional configuration to set to reader implementation
    SPTR(::fwRuntime::ConfigurationElement) m_readerConfig;
};

} // namespace ioPacs

#endif // __IOPACS_SSLICEINDEXDICOMPULLEREDITOR_HPP__
