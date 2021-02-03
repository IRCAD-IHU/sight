/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
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

#include "fwData/config.hpp"
#include "fwData/factory/new.hpp"
#include "fwData/Object.hpp"
#include "fwData/Port.hpp"

#include <vector>

fwCampAutoDeclareDataMacro((fwData)(Node));

namespace fwData
{

/**
 * @brief This class defines a node.
 * A node is represented by input and output ports.
 */
class FWDATA_CLASS_API Node : public ::fwData::Object
{
public:

    fwCoreClassMacro(Node, ::fwData::Object, ::fwData::factory::New< Node >)
    fwCampMakeFriendDataMacro((fwData)(Node));

    /// Port container
    typedef std::vector< ::fwData::Port::sptr > PortContainer;

    /// Type of 'updated' signal
    typedef core::com::Signal< void () > UpdatedSignalType;

    /**
     * @brief Constructor
     * @param key Private construction key
     */
    FWDATA_API Node(::fwData::Object::Key key);

    /// Destructor
    FWDATA_API virtual ~Node();

    /// Add an input port
    FWDATA_API void addInputPort(const ::fwData::Port::sptr& port);

    /// Add an output port
    FWDATA_API void addOutputPort(const ::fwData::Port::sptr& port);

    /// Get the container of input ports
    FWDATA_API PortContainer& getInputPorts();

    /// Get the container of output ports
    FWDATA_API PortContainer& getOutputPorts();

    /// Set object to the node
    FWDATA_API void setObject(const ::fwData::Object::sptr& object );

    /// Get node object
    FWDATA_API ::fwData::Object::sptr getObject() const;

    /**
     * @brief Get the port with given identifier
     *
     * @param[in] _identifier port identifier
     * @param[in] _modeInput if true find input port, else find output port
     *
     * @return input or output port with given identifier
     */
    FWDATA_API Port::sptr findPort(const std::string& _identifier, bool _modeInput) const;

    /// Defines shallow copy
    FWDATA_API void shallowCopy( const Object::csptr& _source ) override;

    /// Defines deep copy
    FWDATA_API void cachedDeepCopy(const Object::csptr& _source, DeepCopyCacheType& cache) override;

    /// Updated signal key
    FWDATA_API static const core::com::Signals::SignalKeyType s_UPDATED_SIG;

protected:

    /// node object
    ::fwData::Object::sptr m_object;

    //! Input port container
    PortContainer m_inputs;

    //! Output port container
    PortContainer m_outputs;

    /// Updated signal
    UpdatedSignalType::sptr m_sigUpdated;
};

} // namespace fwData
