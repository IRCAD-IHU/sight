/************************************************************************
 *
 * Copyright (C) 2015-2021 IRCAD France
 * Copyright (C) 2015-2020 IHU Strasbourg
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

#include "activities/registry/ActivityMsg.hpp"

#include <core/tools/UUID.hpp>

#include <data/Composite.hpp>
#include <data/reflection/getObject.hpp>
#include <data/String.hpp>

#include <boost/algorithm/string/replace.hpp>

#include <regex>

namespace sight::activities
{
namespace registry
{

//-----------------------------------------------------------------------------

ActivityMsg::ActivityMsg(const data::ActivitySeries::sptr& series,
                         const activities::registry::ActivityInfo& info,
                         const ParametersType& parameters)
{
    SLM_ASSERT("ActivitySeries instantiation failed", series);

    const std::string asUID = "AS_UID";

    m_title       = info.title;
    m_appConfigID = info.appConfig.id;
    m_closable    = true;
    m_tabID       = "TABID_" + core::tools::UUID::generateUUID();

    if(info.tabInfo.empty())
    {
        m_tabInfo = info.title;
    }
    else
    {
        std::string newTabInfo = info.tabInfo;
        std::regex e("(!(([\\w]+\\.?)+[\\w]))");
        std::smatch what;
        if(std::regex_search(newTabInfo, what, e))
        {
            std::string submatch(what[1].first, what[1].second);

            submatch.replace(0, 1, "@");

            data::Object::sptr obj = data::reflection::getObject(series->getData(), submatch);
            SLM_ASSERT("Invalid seshat path : '" << submatch <<"'", obj);

            data::String::sptr stringParameter = data::String::dynamicCast(obj);

            std::string tabInfoSeshat;

            if(stringParameter)
            {
                tabInfoSeshat = stringParameter->getValue();
            }
            else
            {
                SLM_WARN("Seshat path '" << submatch << "' doesn't reference an data::String");
            }

            submatch.replace(0, 1, "!");
            ::boost::algorithm::replace_all(newTabInfo, submatch, tabInfoSeshat);

        }
        m_tabInfo = newTabInfo;
    }
    m_iconPath = info.icon;
    m_tooltip  = m_tabInfo;
    m_series   = series;

    namespace ActiReg = activities::registry;
    ActiReg::ActivityAppConfig::ActivityAppConfigParamsType params = info.appConfig.parameters;
    params.reserve(params.size() + parameters.size() + 1);
    params.insert(params.end(), parameters.begin(), parameters.end());
    ActiReg::ActivityAppConfigParam asConfigParam;
    asConfigParam.replace = asUID;
    asConfigParam.by      = series->getID();
    params.push_back(asConfigParam);
    for(const ActiReg::ActivityAppConfigParam& param :  params)
    {
        if(!param.isSeshat())
        {
            m_replaceMap[param.replace] = param.by;
        }
        else
        {
            std::string parameterToReplace = param.by;
            if (parameterToReplace.substr(0, 1) == "!")
            {
                parameterToReplace.replace(0, 1, "@");
            }

            data::Object::sptr obj = data::reflection::getObject(series->getData(), parameterToReplace);
            SLM_ASSERT("Invalid seshat path : '"<<param.by<<"'", obj);

            data::String::sptr stringParameter = data::String::dynamicCast(obj);

            std::string parameterValue = obj->getID();

            if(stringParameter && param.by.substr(0, 1) == "!")
            {
                parameterValue = stringParameter->getValue();
            }
            m_replaceMap[param.replace] = parameterValue;
        }
    }
}

//-----------------------------------------------------------------------------

} // namespace registry

} // namespace sight::activities
