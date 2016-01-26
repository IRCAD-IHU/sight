/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __PREFERENCES_SPREFERENCES_HPP__
#define __PREFERENCES_SPREFERENCES_HPP__

#include <string>

#include <fwCore/base.hpp>

#include "preferences/IPreferencesService.hpp"

namespace preferences
{
/**
 * @brief Service to load and save preferences.
 * @class SPreferences
 */
class PREFERENCES_CLASS_API SPreferences : public ::preferences::IPreferencesService
{

public:
    fwCoreServiceClassDefinitionsMacro ( (SPreferences)(::preferences::IPreferencesService) );

    ///Destructor

    PREFERENCES_API SPreferences() throw()
    {
    }
    PREFERENCES_API virtual ~SPreferences() throw()
    {
    }

protected:

    /// Starts service and loads preferences if exists.
    PREFERENCES_API virtual void starting() throw( ::fwTools::Failed );

    /// Stops service and saves preferences.
    PREFERENCES_API virtual void stopping() throw( ::fwTools::Failed );

    /// Updates service and saves preferences.
    PREFERENCES_API virtual void updating() throw( ::fwTools::Failed );

    /**
     * @brief Configure service
     * @code{.xml}
             <filename>path/to/my/preferences.json</filename>
     * @endcode
     * @throw ::fwTools::Failed
     */
    PREFERENCES_API virtual void configuring() throw(::fwTools::Failed);

    /// preference file
    ::boost::filesystem::path m_prefFile;

private:

    /// loads preferences
    void load();
    /// saves preferences
    void save();
};

} // namespace preferences

#endif /*__PREFERENCES_SPREFERENCES_HPP__*/
