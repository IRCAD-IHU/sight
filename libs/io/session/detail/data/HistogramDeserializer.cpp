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

#include "HistogramDeserializer.hpp"

#include <core/exceptionmacros.hpp>

#include <data/Histogram.hpp>

namespace sight::io::session
{

namespace detail::data
{

//------------------------------------------------------------------------------

sight::data::Object::sptr HistogramDeserializer::deserialize(
    const zip::ArchiveReader::sptr&,
    const boost::property_tree::ptree& tree,
    const std::map<std::string, sight::data::Object::sptr>& children,
    const sight::data::Object::sptr& object,
    const core::crypto::secure_string&
) const
{
    // Create or reuse the object
    const auto& histogram = safeCast<sight::data::Histogram>(object);

    // Check version number. Not mandatory, but could help for future release
    readVersion<sight::data::Histogram>(tree, 0, 1);

    // Deserialize values
    std::vector<long> values;

    for(const auto& valueTree : tree.get_child("Values"))
    {
        const auto& value = valueTree.second.get_value<long>();
        values.push_back(value);
    }

    histogram->setValues(values);

    histogram->setBinsWidth(tree.get<float>("BinsWidth"));
    histogram->setMinValue(tree.get<float>("MinValue"));
    histogram->setMaxValue(tree.get<float>("MaxValue"));

    return histogram;
}

} // detail::data

} // namespace sight::io::session
