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

#include "Matrix4Deserializer.hpp"

#include <core/exceptionmacros.hpp>

#include <data/Matrix4.hpp>

namespace sight::io::session
{

namespace detail::data
{

//------------------------------------------------------------------------------

sight::data::Object::sptr Matrix4Deserializer::deserialize(
    const zip::ArchiveReader::sptr&,
    const boost::property_tree::ptree& tree,
    const std::map<std::string, sight::data::Object::sptr>&,
    const sight::data::Object::sptr& object,
    const core::crypto::secure_string&
) const
{
    // Create or reuse the object
    const auto& matrix = safeCast<sight::data::Matrix4>(object);

    // Check version number. Not mandatory, but could help for future release
    readVersion<sight::data::Matrix4>(tree, 0, 1);

    std::array<double, 16> coefficients;
    for(size_t index = 0, end = coefficients.size() ; index < end ; ++index)
    {
        coefficients[index] = tree.get<double>(std::to_string(index));
    }

    matrix->setCoefficients(coefficients);

    return matrix;
}

} // detail::data

} // namespace sight::io::session
