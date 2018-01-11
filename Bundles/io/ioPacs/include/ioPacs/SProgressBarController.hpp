/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __IOPACS_SPROGRESSBARCONTROLLER_HPP__
#define __IOPACS_SPROGRESSBARCONTROLLER_HPP__

#include "ioPacs/config.hpp"

#include <fwCom/Slot.hpp>

#include <fwCore/mt/types.hpp>

#include <fwGui/dialog/ProgressDialog.hpp>

#include <fwServices/IController.hpp>

#include <boost/filesystem/path.hpp>

namespace ioPacs
{

/**
 * @brief   This editor service is used to display a progress bar
 */
class IOPACS_CLASS_API SProgressBarController : public ::fwServices::IController
{

public:

    fwCoreServiceClassDefinitionsMacro( (SProgressBarController)( ::fwServices::IController ) );

    /// Type of progress bar container
    typedef std::map< std::string, ::fwGui::dialog::ProgressDialog::sptr > ProgressDialogContainerType;

    /**
     * @brief constructor
     */
    IOPACS_API SProgressBarController() noexcept;

    /**
     * @brief destructor
     */
    IOPACS_API virtual ~SProgressBarController() noexcept;

protected:

    /**
     * @brief Configuring method. This method is used to configure the service.
     *
     * XML configuration sample:
       @code{.xml}
       <service uid="progressBarController" impl="::ioPacs::SProgressBarController"
         autoConnect="no">
       </service>
       @endcode
     */
    IOPACS_API virtual void configuring() override;

    /// Override
    IOPACS_API virtual void starting() override;

    /// Override
    IOPACS_API virtual void stopping() override;

    /// Override
    IOPACS_API void updating() override;

    /// Override
    IOPACS_API void info(std::ostream& _sstream ) override;

    /**
     * @name Slots
     * @{
     */
    void startProgress(std::string id);

    void updateProgress(std::string id, float percentage, std::string message);

    void stopProgress(std::string id);
    /**
     * @}
     */

    /// Progress Dialog
    ProgressDialogContainerType m_progressDialogs;

    ::fwCore::mt::Mutex m_mutex; ///< mutex for progress dialog access

};

} // namespace ioPacs

#endif // __IOPACS_SPROGRESSBARCONTROLLER_HPP__
