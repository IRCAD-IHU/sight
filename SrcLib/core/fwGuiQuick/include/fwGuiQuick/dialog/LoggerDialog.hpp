/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWGUIQUICK_DIALOG_LOGGERDIALOG_HPP__
#define __FWGUIQUICK_DIALOG_LOGGERDIALOG_HPP__

#include "fwGuiQuick/config.hpp"

#include <fwGui/dialog/ILoggerDialog.hpp>

#include <QDialog>
#include <QPointer>
#include <QTableWidget>

#include <vector>

namespace fwGuiQuick
{
namespace dialog
{
//------------------------------------------------------------------------------

/**
 * @brief   LoggerDialog allowing the choice of an element among severals (_selections)
 */

class FWGUIQUICK_CLASS_API LoggerDialog : public QDialog,
                                          public ::fwGui::dialog::ILoggerDialog
{
Q_OBJECT

public:

    fwCoreClassDefinitionsWithFactoryMacro( (LoggerDialog)(::fwGui::dialog::ILoggerDialog),
                                            (()),
                                            ::fwGui::factory::New< LoggerDialog > );

    /// Constructor
    FWGUIQUICK_API LoggerDialog(::fwGui::GuiBaseObject::Key key);

    /// Destructor
    FWGUIQUICK_API virtual ~LoggerDialog();

    /**
     * @brief Set the dialog title.
     * @param[in] title Dialog title
     */
    FWGUIQUICK_API virtual void setTitle(const std::string& title);

    /**
     * @brief Set the dialog message.
     * @param[in] message Dialog message
     */
    FWGUIQUICK_API virtual void setMessage(const std::string& message);

    /**
     * @brief Set the dialog logger.
     * @param[in] logger Dialog logger
     */
    FWGUIQUICK_API virtual void setLogger(const ::fwLog::Logger::sptr& logger);

    /**
     * @brief Show the dialog and return whether the user has selected the Ok or Cancel button
     */
    FWGUIQUICK_API virtual bool show();

protected Q_SLOTS:

    /**
     * @brief Slot called when the user wants to display the logs
     * @param[in] state Checkbox state
     */
    void displayLogs(int state);

private:
    /// Dialog title
    std::string m_title;

    /// Dialog message
    std::string m_message;

    /// Logger
    ::fwLog::Logger::sptr m_logger;

    /// Qt dialog
    QPointer<QDialog> m_dialog;

    /// Table widget used to display logs
    QPointer<QTableWidget> m_logTableWidget;
};

} // namespace dialog
} // namespace fwGuiQuick

#endif /*__FWGUIQUICK_DIALOG_LOGGERDIALOG_HPP__*/
