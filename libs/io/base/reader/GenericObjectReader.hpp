/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
 * Copyright (C) 2012-2019 IHU Strasbourg
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

#include "io/base/config.hpp"
#include "io/base/reader/IObjectReader.hpp"

namespace sight::io::base
{

namespace reader
{

/**
 * @brief   generic class for all object readers.
 *
 *
 * This class adds 2 methods to the API of IObjectReader. This
 * generic reader test the object type given to the method setObject.
 * And add a new get method getConcreteObject() that returns an object
 * already cast.
 */
template<class DATATYPE>
class GenericObjectReader : virtual public io::base::reader::IObjectReader
{

public:

    /// the object type related
    typedef DATATYPE DataType;

    fwCoreClassMacro(GenericObjectReader<DATATYPE>, io::base::reader::IObjectReader)

    /// Constructor. Do nothing.
    GenericObjectReader()
    {
    }

    //------------------------------------------------------------------------------

    virtual std::string extension() override
    {
        return ("");
    }

    /// Destructor. Do nothing.
    virtual ~GenericObjectReader()
    {
    }

    /**
     * @brief m_object setter.
     * @param[out] obj replace weakptr m_object of the instance reader
     *
     * This method overload setObject to ensure that given object correspond to DATATYPE
     */
    virtual void setObject( core::tools::Object::sptr obj ) override
    {
        assert( std::dynamic_pointer_cast< DataType >( obj ) );
        m_object = obj;
    }

    /**
     * @brief m_object getter.
     * @return m_object
     *
     * This method automatic cast object in correct DataType.
     */
    virtual std::shared_ptr< DataType > getConcreteObject()
    {
        return std::dynamic_pointer_cast< DataType >( getObject() );
    }

};

} // namespace reader

} // namespace sight::io::base
