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

#include "CompositeDeserializer.hpp"

#include <data/Composite.hpp>

namespace sight::io::session
{
namespace detail::data
{

/// Factories
CompositeDeserializer::sptr CompositeDeserializer::shared()
{
    struct make_shared_enabler final : public CompositeDeserializer {};
    return std::make_shared< make_shared_enabler >();
}

//------------------------------------------------------------------------------

CompositeDeserializer::uptr CompositeDeserializer::unique()
{
    struct make_unique_enabler final : public CompositeDeserializer {};
    return std::make_unique< make_unique_enabler >();
}

//------------------------------------------------------------------------------

sight::data::Object::sptr CompositeDeserializer::deserialize(
    const zip::ArchiveReader::sptr& archive,
    const boost::property_tree::ptree& tree,
    const std::map<std::string, sight::data::Object::sptr>& children,
    const sight::data::Object::sptr& object,
    const core::crypto::secure_string& password)
{
    // Create or reuse the object
    const auto& composite =
        object ? sight::data::Composite::dynamicCast(object) : sight::data::Composite::New();

    SIGHT_ASSERT(
        "Object '" << composite->getClassname() << "' is not a '" << sight::data::Composite::classname() << "'",
            composite);

    composite->setContainer(children);

    return composite;
}

//------------------------------------------------------------------------------

sight::data::Object::sptr CompositeDeserializer::create()
{
    // Create the object
    return sight::data::Composite::New();
}

} // detail::data
} // namespace sight::io::session