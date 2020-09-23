/************************************************************************
 *
 * Copyright (C) 2009-2020 IRCAD France
 * Copyright (C) 2012-2020 IHU Strasbourg
 *
 * This file is part of Sight.
 *
 * Sight is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Sight is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with Sight. If not, see <https://www.gnu.org/licenses/>.
 *
 ***********************************************************************/

#pragma once

#include "fwServices/config.hpp"
#include <fwServices/IAppConfigManager.hpp>
#include <fwServices/IXMLParser.hpp>

#include <fwRuntime/ConfigurationElement.hpp>

#include <fwTools/Failed.hpp>
#include <fwTools/Object.hpp>

namespace dataReg
{
namespace parser
{
/**
 * @brief   Specific service for the construction of a List and its associated services from an XML-based description.
 * @see     ::fwServices::IXMLParser
 */
class FWSERVICES_CLASS_API List : public ::fwServices::IXMLParser
{
public:

    fwCoreServiceMacro(List, ::fwServices::IXMLParser)

    /**
     * @brief   Constructor : does nothing.
     */
    List( )
    {
    }

    /**
     * @brief   Destructor : does nothing.
     */
    virtual ~List()
    {
    }

    FWSERVICES_API void createConfig( ::fwTools::Object::sptr _obj ) override;

    FWSERVICES_API void startConfig() override;

    FWSERVICES_API void updateConfig() override;

    FWSERVICES_API void stopConfig() override;

    FWSERVICES_API void destroyConfig() override;

protected:

    /**
     * @brief   Updating method : create List object.
     *
     * Parse the List configuration element to configure and add its objects.
     */
    FWSERVICES_API virtual void updating( ) override;

private:

    /// To verify some conditions in xml file
    bool refObjectValidator( ::fwRuntime::ConfigurationElement::sptr _cfgElement );

    std::vector< ::fwServices::IAppConfigManager::sptr > m_ctmContainer;
};

} //namespace parser
} //namespace dataReg
