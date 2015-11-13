/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "ioPacs/SQueryEditor.hpp"

#include <fwComEd/helper/SeriesDB.hpp>
#include <fwDicomData/DicomSeries.hpp>
#include <fwPacsIO/helper/Series.hpp>
#include <fwPacsIO/exceptions/Base.hpp>
#include <fwGui/dialog/MessageDialog.hpp>
#include <fwGuiQt/container/QtContainer.hpp>
#include <fwServices/macros.hpp>

#include <dcmtk/dcmnet/scu.h>

#include <QGridLayout>
#include <QHBoxLayout>
#include <QLabel>

#include <boost/filesystem/operations.hpp>
#include <boost/foreach.hpp>

namespace ioPacs
{

fwServicesRegisterMacro( ::gui::editor::IEditor, ::ioPacs::SQueryEditor, ::fwMedData::SeriesDB );

//------------------------------------------------------------------------------

SQueryEditor::SQueryEditor() throw()
{
}
//------------------------------------------------------------------------------

SQueryEditor::~SQueryEditor() throw()
{
}

//------------------------------------------------------------------------------

void SQueryEditor::info(std::ostream &_sstream )
{
    _sstream << "SQueryEditor::info";
}

//------------------------------------------------------------------------------

void SQueryEditor::starting() throw(::fwTools::Failed)
{
    SLM_TRACE_FUNC();
    ::fwGui::IGuiContainerSrv::create();
    ::fwGuiQt::container::QtContainer::sptr qtContainer = fwGuiQt::container::QtContainer::dynamicCast(getContainer());

    QWidget* const container = qtContainer->getQtContainer();

    // Main Widget
    QGridLayout* layout = new QGridLayout();
    container->setLayout(layout);

    m_patientNameLineEdit    = new QLineEdit();
    m_patientNameQueryButton = new QPushButton("Send");
    layout->addWidget(new QLabel("Patient name:"),0, 0);
    layout->addWidget(m_patientNameLineEdit,0, 1);
    layout->addWidget(m_patientNameQueryButton,0, 2);

    m_beginStudyDateEdit = new QDateEdit();
    m_beginStudyDateEdit->setDate(QDate::currentDate());
    m_beginStudyDateEdit->setDisplayFormat("dd.MM.yyyy");
    m_endStudyDateEdit = new QDateEdit();
    m_endStudyDateEdit->setDate(QDate::currentDate());
    m_endStudyDateEdit->setDisplayFormat("dd.MM.yyyy");
    m_studyDateQueryButton = new QPushButton("Send");
    QHBoxLayout* dateLayout = new QHBoxLayout();
    layout->addWidget(new QLabel("Study date:"),1, 0);
    layout->addLayout(dateLayout,1, 1);
    layout->addWidget(m_studyDateQueryButton,1, 2);
    dateLayout->addWidget(m_beginStudyDateEdit);
    dateLayout->addWidget(m_endStudyDateEdit);
    //Set layout
    container->setLayout(layout);

    // Connect the signals
    QObject::connect(m_patientNameLineEdit, SIGNAL(returnPressed()), this, SLOT(queryPatientName()));
    QObject::connect(m_patientNameQueryButton, SIGNAL(clicked()), this, SLOT(queryPatientName()));
    QObject::connect(m_studyDateQueryButton, SIGNAL(clicked()), this, SLOT(queryStudyDate()));
    QObject::connect(m_beginStudyDateEdit, SIGNAL(dateChanged(QDate)), this, SLOT(queryStudyDate()));
    QObject::connect(m_endStudyDateEdit, SIGNAL(dateChanged(QDate)), this, SLOT(queryStudyDate()));

    // Create enquirer
    m_seriesEnquirer = ::fwPacsIO::SeriesEnquirer::New();

    // Get pacs configuration
    m_pacsConfiguration =
        ::fwPacsIO::data::PacsConfiguration::dynamicCast(::fwTools::fwID::getObject(m_pacsConfigurationUID));
    SLM_ASSERT("The pacs configuration object sould not be null.", m_pacsConfiguration);
}

//------------------------------------------------------------------------------

void SQueryEditor::stopping() throw(::fwTools::Failed)
{
    SLM_TRACE_FUNC();

    // Disconnect the signals
    QObject::disconnect(m_patientNameLineEdit, SIGNAL(returnPressed()), this, SLOT(queryPatientName()));
    QObject::disconnect(m_patientNameQueryButton, SIGNAL(clicked()), this, SLOT(queryPatientName()));
    QObject::disconnect(m_studyDateQueryButton, SIGNAL(clicked()), this, SLOT(queryStudyDate()));
    QObject::disconnect(m_beginStudyDateEdit, SIGNAL(dateChanged(QDate)), this, SLOT(queryStudyDate()));
    QObject::disconnect(m_endStudyDateEdit, SIGNAL(dateChanged(QDate)), this, SLOT(queryStudyDate()));

    this->getContainer()->clean();
    this->::fwGui::IGuiContainerSrv::destroy();
}

//------------------------------------------------------------------------------

void SQueryEditor::configuring() throw(::fwTools::Failed)
{
    SLM_TRACE_FUNC();
    ::fwGui::IGuiContainerSrv::initialize();

    ::fwRuntime::ConfigurationElement::sptr config = m_configuration->findConfigurationElement("config");
    SLM_ASSERT("The service ::ioPacs::SQueryEditor must have a \"config\" element.",config);

    bool success;

    // Pacs Configuration UID
    ::boost::tie(success, m_pacsConfigurationUID) = config->getSafeAttributeValue("pacsConfigurationUID");
    SLM_ASSERT("It should be a \"pacsConfigurationUID\" tag in the ::ioPacs::SQueryEditor config element.", success);

}

//------------------------------------------------------------------------------

void SQueryEditor::updating() throw(::fwTools::Failed)
{
    SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------

void SQueryEditor::queryPatientName()
{
    try
    {
        // Initialize connection
        m_seriesEnquirer->initialize(
            m_pacsConfiguration->getLocalApplicationTitle(),
            m_pacsConfiguration->getPacsHostName(),
            m_pacsConfiguration->getPacsApplicationPort(),
            m_pacsConfiguration->getPacsApplicationTitle());
        m_seriesEnquirer->connect();
        OFList< QRResponse* > responses;

        // Find series according to patient's name
        responses = m_seriesEnquirer->findSeriesByPatientName(m_patientNameLineEdit->text().toStdString());

        // Convert response to DicomSeries
        ::fwMedData::SeriesDB::ContainerType series =
            ::fwPacsIO::helper::Series::toFwMedData(responses);
        ::fwPacsIO::helper::Series::releaseResponses(responses);

        // Check whether the instance number start at 1 or 0
        for(::fwMedData::Series::sptr s: series)
        {
            ::fwDicomData::DicomSeries::sptr dicomSeries = ::fwDicomData::DicomSeries::dynamicCast(s);
            SLM_ASSERT("The PACS response should contain only DicomSeries", dicomSeries);
            const std::string instanceUID = m_seriesEnquirer->findSOPInstanceUID(dicomSeries->getInstanceUID(), 0);
            dicomSeries->setFirstInstanceNumber((instanceUID.empty() ? 1 : 0));
        }

        m_seriesEnquirer->disconnect();

        this->updateSeriesDB(series);
    }
    catch (::fwPacsIO::exceptions::Base& exception)
    {
        this->displayErrorMessage(exception.what());
    }

}

//------------------------------------------------------------------------------

void SQueryEditor::queryStudyDate()
{
    try
    {
        m_seriesEnquirer->initialize(
            m_pacsConfiguration->getLocalApplicationTitle(),
            m_pacsConfiguration->getPacsHostName(),
            m_pacsConfiguration->getPacsApplicationPort(),
            m_pacsConfiguration->getPacsApplicationTitle());
        m_seriesEnquirer->connect();
        OFList< QRResponse* > responses;

        // Find series according to study's date
        responses = m_seriesEnquirer->findSeriesByDate(
            m_beginStudyDateEdit->date().toString("yyyyMMdd").toStdString(),
            m_endStudyDateEdit->date().toString("yyyyMMdd").toStdString());

        // Convert response to DicomSeries
        ::fwMedData::SeriesDB::ContainerType series =
            ::fwPacsIO::helper::Series::toFwMedData(responses);
        ::fwPacsIO::helper::Series::releaseResponses(responses);

        // Check whether the instance number start at 1 or 0
        for(::fwMedData::Series::sptr s: series)
        {
            ::fwDicomData::DicomSeries::sptr dicomSeries = ::fwDicomData::DicomSeries::dynamicCast(s);
            SLM_ASSERT("The PACS response should contain only DicomSeries", dicomSeries);
            const std::string instanceUID = m_seriesEnquirer->findSOPInstanceUID(dicomSeries->getInstanceUID(), 0);
            dicomSeries->setFirstInstanceNumber((instanceUID.empty() ? 1 : 0));
        }

        m_seriesEnquirer->disconnect();

        this->updateSeriesDB(series);
    }
    catch (::fwPacsIO::exceptions::Base& exception)
    {
        this->displayErrorMessage(exception.what());
    }
}

//------------------------------------------------------------------------------

void SQueryEditor::updateSeriesDB(::fwMedData::SeriesDB::ContainerType series)
{
    ::fwMedData::SeriesDB::sptr seriesDB = this->getObject< ::fwMedData::SeriesDB >();
    ::fwComEd::helper::SeriesDB seriesDBHelper(seriesDB);

    // Delete old series from the SeriesDB
    seriesDBHelper.clear();

    // Push new series in the SeriesDB
    for(const ::fwMedData::Series::sptr& s: series)
    {
        ::fwDicomData::DicomSeries::sptr dicomSeries = ::fwDicomData::DicomSeries::dynamicCast(s);
        seriesDBHelper.add(dicomSeries);
    }

    // Notify th SeriesDB
    seriesDBHelper.notify();

}

//------------------------------------------------------------------------------

void SQueryEditor::displayErrorMessage(const std::string& message) const
{
    std::stringstream ss;
    ss << "Unable to connect to the pacs. Please check your configuration: \n"
       << "Pacs host name: " << m_pacsConfiguration->getPacsHostName() << "\n"
       << "Pacs application title: " << m_pacsConfiguration->getPacsApplicationTitle() << "\n"
       << "Pacs port: " << m_pacsConfiguration->getPacsApplicationPort() << "\n";

    SLM_WARN("Error: " + message);
    ::fwGui::dialog::MessageDialog messageBox;
    messageBox.setTitle("Error");
    messageBox.setMessage( ss.str() );
    messageBox.setIcon(::fwGui::dialog::IMessageDialog::CRITICAL);
    messageBox.addButton(::fwGui::dialog::IMessageDialog::OK);
    messageBox.show();
}

} // namespace ioPacs
