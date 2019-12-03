/************************************************************************
 *
 * Copyright (C) 2009-2019 IRCAD France
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

#include "fwServices/config.hpp"
#include "fwServices/IService.hpp"

#include <fwCom/HasSignals.hpp>

#include <fwCore/LogicStamp.hpp>
#include <fwCore/mt/types.hpp>

#include <fwTools/Failed.hpp>
#include <fwTools/Object.hpp>

#include <boost/bimap.hpp>
#include <boost/bimap/multiset_of.hpp>
#include <boost/bimap/unordered_set_of.hpp>
#include <boost/static_assert.hpp>
#include <boost/type_traits.hpp>

#include <map>
#include <unordered_map>
#include <set>

namespace fwData
{
class Object;
}

namespace fwServices
{
namespace registry
{

/**
 * @brief maintain the relation between objects and services
 *
 * The API of ObjectService should not be directly invoked,
 * fwServices/op methods (i.e. add, get,...) should be used instead
 */
class FWSERVICES_CLASS_API ObjectService : public ::fwCore::BaseObject,
                                           public ::fwCom::HasSignals
{
public:

    fwCoreClassMacro(ObjectService, ::fwCore::BaseObject)
    fwCoreAllowSharedFromThis();

    /**
     * @brief Service container
     * keeps relation between objects identifiers and attached services
     */
    typedef ::boost::bimaps::bimap<
            ::boost::bimaps::multiset_of< ::fwData::Object::cwptr, std::owner_less< ::fwData::Object::cwptr > >,
            ::boost::bimaps::multiset_of< ::fwServices::IService::sptr >
            > ServiceContainerType;

    typedef std::set< CSPTR( ::fwData::Object ) >      ObjectVectorType;
    typedef std::set< SPTR( ::fwServices::IService ) > ServiceVectorType;

    typedef std::map< ::fwServices::IService::KeyType, WPTR( ::fwData::Object )> ObjectMapType;

    /**
     * @name Signals API
     * @{
     */
    /// Type of signal m_sigRenderRequested
    typedef ::fwCom::Signal< void (::fwData::Object::sptr, const std::string&) > RegisterSignalType;

    FWSERVICES_API static const ::fwCom::Signals::SignalKeyType s_REGISTERED_SIG;
    FWSERVICES_API static const ::fwCom::Signals::SignalKeyType s_UNREGISTERED_SIG;
    ///@}

    /// Constructor
    FWSERVICES_API ObjectService();

    /// Return some informations contain in the registry
    FWSERVICES_API std::string getRegistryInformation() const;

    /**
     * @name Management of registrations
     */
    //@{

    /**
     * @brief Register the service alone
     *
     * @param service Service to add to the OSR
     */
    FWSERVICES_API void registerService( ::fwServices::IService::sptr service );

    /**
     * @brief Register the service (service) for the object (obj) at the given service key.
     * It also updates IService::m_associatedObject of service to point to obj
     * removal at obj destruction.
     *
     * @param object Object to register
     * @param objKey Key of the object
     * @param access Access (INPUT, INOUT, OUTPUT) of this key
     * @param service Service whose key should be added
     */
    FWSERVICES_API void registerService(::fwData::Object::sptr object, const ::fwServices::IService::KeyType& objKey,
                                        ::fwServices::IService::AccessType access,
                                        ::fwServices::IService::sptr service);

    /**
     * @brief Register the service (service) for the input object (obj) at the given service key.
     * It also updates IService::m_associatedObject of service to point to obj
     * removal at obj destruction.
     *
     * @param object Object to register
     * @param objKey Key of the object
     * @param service Service whose key should be added
     */
    FWSERVICES_API void registerServiceInput(const ::fwData::Object::csptr& object,
                                             const ::fwServices::IService::KeyType& objKey,
                                             const ::fwServices::IService::sptr& service);

    /**
     * @brief Emit the signal 'registered'
     *
     * @param object Object to register
     * @param objKey Key of the object
     * @param service Service whose key should be added
     */
    FWSERVICES_API void registerServiceOutput(::fwData::Object::sptr object,
                                              const ::fwServices::IService::KeyType& objKey,
                                              ::fwServices::IService::sptr service);
    //@}

    /**
     * @name Management of unregistrations
     */

    //@{

    /**
     * @brief Remove the service (service) from the m_container
     *
     * @param service Service whose key should be removed
     */
    FWSERVICES_API void unregisterService(  ::fwServices::IService::sptr service );

    /**
     * @brief Remove an object key from a service
     *
     * @param objKey Key of the object
     * @param access Access (INPUT, INOUT, OUTPUT) of this key
     * @param service Service whose key should be removed
     */
    FWSERVICES_API void unregisterService( const ::fwServices::IService::KeyType& objKey,
                                           ::fwServices::IService::AccessType access,
                                           ::fwServices::IService::sptr service );

    /**
     * @brief Emit the signal 'unregistered'
     *
     * @param objKey Key of the object
     * @param service Service whose key should be removed
     */
    FWSERVICES_API void unregisterServiceOutput( const ::fwServices::IService::KeyType& objKey,
                                                 ::fwServices::IService::sptr service );

    /**
     * @brief Return true if a key is registered for a given service
     *
     * @param objKey Key of the object
     * @param access Access (INPUT, INOUT, OUTPUT) of this key
     * @param service Service where to look for the key
     */
    FWSERVICES_API bool isRegistered( const ::fwServices::IService::KeyType& objKey,
                                      ::fwServices::IService::AccessType access,
                                      ::fwServices::IService::sptr service) const;

    /**
     * @brief Return the object pointer of a key of a given service
     *
     * @param objKey Key of the object
     * @param access Access (INPUT, INOUT, OUTPUT) of this key
     * @param service Service where to look for the key
     */
    FWSERVICES_API ::fwData::Object::csptr getRegistered(const ::fwServices::IService::KeyType& objKey,
                                                         ::fwServices::IService::AccessType access,
                                                         IService::sptr service) const;
    //@}

    /**
     * @name Some useful getters
     */

    //@{

    /**
     * @brief Return a container with all services of type SERVICE registered in m_container
     * @note Services may be associated to different object
     */
    template<class SERVICE>
    std::set< SPTR(SERVICE) > getServices() const;

    /**
     * @brief Return registered services matching serviceType
     * @note Should be optimized
     * @note Invoke getServices( ::fwData::Object::sptr , const std::string & ) for each registered object
     *
     */
    FWSERVICES_API ServiceVectorType getServices( const std::string& serviceType ) const;

    //@}

protected:

    /// Registered services
    ServiceVectorType m_services;

    mutable ::fwCore::mt::ReadWriteMutex m_containerMutex;

private:

    /**
     * @brief Register the service (service) for the object (obj)
     * It also updates IService::m_associatedObject of service to point to obj
     * removal at obj destruction.
     * @warning not thread-safe
     */
    void internalRegisterService( ::fwData::Object::sptr obj, ::fwServices::IService::sptr service,
                                  const ::fwServices::IService::KeyType& objKey,
                                  ::fwServices::IService::AccessType access);

    /**
     * @brief Register the service (service) for the input object (obj)
     * It also updates IService::m_associatedObject of service to point to obj
     * removal at obj destruction.
     * @warning not thread-safe
     */
    void internalRegisterServiceInput( const ::fwData::Object::csptr& obj, const ::fwServices::IService::sptr& service,
                                       const ::fwServices::IService::KeyType& objKey);

    /**
     * @brief container manipulator (Helper)
     * remove service from ObjectService containers, do not perform any "semantic action" like stopping service
     * @note (internal use) use with care
     */
    void  removeFromContainer( ::fwServices::IService::sptr service );
};

} // namespace registry

/**
 * @brief OSR wraps main Object-Service registry access
 */
namespace OSR
{

/**
 * @brief returns the Object-Service registry main instance
 */
FWSERVICES_API ::fwServices::registry::ObjectService::sptr get();

/**
 * @brief Wraps ObjectService::getRegistryInformation
 */
FWSERVICES_API std::string getRegistryInformation();

/**
 * @brief Wraps ObjectService::getServices
 */
template<class SERVICE>
std::set< SPTR(SERVICE) > getServices();

/**
 * @brief Wraps ObjectService::getServices
 */
FWSERVICES_API ::fwServices::registry::ObjectService::ServiceVectorType getServices( const std::string& serviceType );

/**
 * @brief Register the service alone
 *
 * @param service Service to add to the OSR
 */
FWSERVICES_API void registerService( ::fwServices::IService::sptr service );

/**
 * @brief Register the service (service) for the object (obj) at the given service key.
 * It also updates IService::m_associatedObject of service to point to obj
 * removal at obj destruction.
 *
 * @param object Object to register
 * @param objKey Key of the object
 * @param access Access (INPUT, INOUT, OUTPUT) of this key
 * @param service Service whose key should be added
 */
FWSERVICES_API void registerService(::fwData::Object::sptr obj, const ::fwServices::IService::KeyType& objKey,
                                    ::fwServices::IService::AccessType access, ::fwServices::IService::sptr service);

/**
 * @brief Register the service (service) for the input object (obj) at the given service key.
 * It also updates IService::m_associatedObject of service to point to obj
 * removal at obj destruction.
 *
 * @param object Object to register
 * @param objKey Key of the object
 * @param service Service whose key should be added
 */
FWSERVICES_API void registerServiceInput(::fwData::Object::csptr obj, const ::fwServices::IService::KeyType& objKey,
                                         ::fwServices::IService::sptr service);

/**
 * @brief Emit the signal 'registered'
 *
 * @param object Object to register
 * @param objKey Key of the object
 * @param service Service whose key should be added
 */
FWSERVICES_API void registerServiceOutput(::fwData::Object::sptr obj, const ::fwServices::IService::KeyType& objKey,
                                          ::fwServices::IService::sptr service);

/**
 * @brief Remove the service (service) from the m_container
 *
 * @param service Service whose key should be removed
 */
FWSERVICES_API void unregisterService( ::fwServices::IService::sptr service );

/**
 * @brief Remove an object key from a service
 *
 * @param objKey Key of the object
 * @param access Access (INPUT, INOUT, OUTPUT) of this key
 * @param service Service whose key should be removed
 */
FWSERVICES_API void unregisterService( const ::fwServices::IService::KeyType& objKey,
                                       ::fwServices::IService::AccessType access,
                                       ::fwServices::IService::sptr service );

/**
 * @brief Emit the signal 'unregistered'
 *
 * @param objKey Key of the object
 * @param service Service whose key should be removed
 */
FWSERVICES_API void unregisterServiceOutput( const ::fwServices::IService::KeyType& objKey,
                                             ::fwServices::IService::sptr service );

/**
 * @brief Return true if a key is registered for a given service
 *
 * @param objKey Key of the object
 * @param access Access (INPUT, INOUT, OUTPUT) of this key
 * @param service Service where to look for the key
 */
FWSERVICES_API bool isRegistered(const ::fwServices::IService::KeyType& objKey,
                                 ::fwServices::IService::AccessType access,
                                 ::fwServices::IService::sptr service);

/**
 * @brief Return the object pointer of a key of a given service
 *
 * @param objKey Key of the object
 * @param access Access (INPUT, INOUT, OUTPUT) of this key
 * @param service Service where to look for the key
 */
FWSERVICES_API ::fwData::Object::csptr getRegistered( const ::fwServices::IService::KeyType& objKey,
                                                      ::fwServices::IService::AccessType access,
                                                      ::fwServices::IService::sptr service );

FWSERVICES_API SPTR( ::fwServices::registry::ObjectService::RegisterSignalType ) getRegisterSignal();
FWSERVICES_API SPTR( ::fwServices::registry::ObjectService::RegisterSignalType ) getUnregisterSignal();

} // namespace OSR

} // namespace fwServices

#include "fwServices/registry/ObjectService.hxx"
