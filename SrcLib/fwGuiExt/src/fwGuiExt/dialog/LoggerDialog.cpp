/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwGuiExt/dialog/LoggerDialog.hpp"

#include <fwGui/registry/worker.hpp>

#include <fwThread/Worker.hpp>
#include <fwThread/Worker.hxx>

#include <boost/function.hpp>

namespace fwGuiExt
{
namespace dialog
{

//-----------------------------------------------------------------------------

bool LoggerDialog::showLoggerDialog(
    const std::string& title, const std::string& message, const ::fwLog::Logger::sptr& logger)
{
    ::fwGuiExt::dialog::LoggerDialog dialog(title, message, logger);
    return dialog.show();
}

//-----------------------------------------------------------------------------

LoggerDialog::LoggerDialog()
{
    ::fwGui::GuiBaseObject::sptr guiObj = ::fwGui::factory::New(ILoggerDialog::REGISTRY_KEY);
    m_implementation                    = ::fwGuiExt::dialog::ILoggerDialog::dynamicCast(guiObj);
}

//-----------------------------------------------------------------------------

LoggerDialog::~LoggerDialog()
{
}

//-----------------------------------------------------------------------------

LoggerDialog::LoggerDialog(const std::string& title, const std::string& message, const ::fwLog::Logger::sptr& logger)
{
    ::fwGui::GuiBaseObject::sptr guiObj = ::fwGui::factory::New(ILoggerDialog::REGISTRY_KEY);
    m_implementation                    = ::fwGuiExt::dialog::ILoggerDialog::dynamicCast(guiObj);
    m_implementation->setTitle(title);
    m_implementation->setMessage(message);
    m_implementation->setLogger(logger);
}

//-----------------------------------------------------------------------------

void LoggerDialog::setTitle(const std::string& title)
{
    m_implementation->setTitle(title);
}

//-----------------------------------------------------------------------------

void LoggerDialog::setMessage(const std::string& message)
{
    m_implementation->setMessage(message);
}

//-----------------------------------------------------------------------------

void LoggerDialog::setLogger(const ::fwLog::Logger::sptr& logger)
{
    m_implementation->setLogger(logger);
}

//-----------------------------------------------------------------------------

bool LoggerDialog::show()
{
    if(m_implementation)
    {
        ::boost::function<bool()> func = ::boost::bind(&ILoggerDialog::show, m_implementation);
        ::boost::shared_future<bool> f = ::fwGui::registry::worker::get()->postTask<bool>(func);
        f.wait();

        return f.get();
    }

    return false;
}

//-----------------------------------------------------------------------------

} //namespace dialog
} //namespace fwGuiExt
