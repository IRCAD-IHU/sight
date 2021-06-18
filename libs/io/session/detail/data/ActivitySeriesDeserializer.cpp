/************************************************************************
 *
 * Copyright (C) 2021 IRCAD France
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

#include "ActivitySeriesDeserializer.hpp"

#include "SeriesDeserializer.hpp"

#include <core/crypto/Base64.hpp>
#include <core/exceptionmacros.hpp>

#include <data/ActivitySeries.hpp>
#include <data/Composite.hpp>

namespace sight::io::session
{

namespace detail::data
{

//------------------------------------------------------------------------------

sight::data::Object::sptr ActivitySeriesDeserializer::deserialize(
    const zip::ArchiveReader::sptr& archive,
    const boost::property_tree::ptree& tree,
    const std::map<std::string, sight::data::Object::sptr>& children,
    const sight::data::Object::sptr& object,
    const core::crypto::secure_string& password
) const
{
    // Create or reuse the object
    const auto& activitySeries = IDataDeserializer::safeCast<sight::data::ActivitySeries>(object);

    // Check version number. Not mandatory, but could help for future release
    IDataDeserializer::readVersion<sight::data::ActivitySeries>(tree, 0, 1);

    // Since ActivitySeries inherits from Series, we could use SeriesDeserializer
    const SeriesDeserializer seriesDeserializer;
    seriesDeserializer.deserialize(archive, tree, children, activitySeries, password);

    // Deserialize the remaining
    // Deserialize children properties
    activitySeries->setData(sight::data::Composite::dynamicCast(children.at("Data")));

    // Serialize trivial properties
    activitySeries->setActivityConfigId(readString(tree, "ActivityConfigId"));

    return activitySeries;
}

} // detail::data

} // namespace sight::io::session
