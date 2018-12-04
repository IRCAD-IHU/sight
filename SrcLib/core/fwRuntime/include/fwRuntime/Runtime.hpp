/************************************************************************
 *
 * Copyright (C) 2009-2017 IRCAD France
 * Copyright (C) 2012-2017 IHU Strasbourg
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

#ifndef __FWRUNTIME_RUNTIME_HPP__
#define __FWRUNTIME_RUNTIME_HPP__

#include "fwRuntime/config.hpp"
#include "fwRuntime/RuntimeException.hpp"
#include "fwRuntime/Version.hpp"

#include <boost/filesystem/path.hpp>

#include <set>
#include <vector>

namespace fwRuntime
{
struct Bundle;
struct ConfigurationElement;
struct Extension;
struct ExtensionPoint;
struct IExecutable;
struct ExecutableFactory;
struct IPlugin;
}

namespace fwRuntime
{

/**
 * @brief   Defines the runtime class.
 *
 */
struct Runtime
{
    /**
     * @name    Type Definitions
     */
    //@{
    /// Defines the bundle container type.
    typedef std::set< std::shared_ptr<Bundle> >     BundleContainer;

    /// Defines the bundle container iterator type.
    typedef BundleContainer::iterator BundleIterator;
    /// Defines the extension container type.
    typedef std::set< std::shared_ptr<Extension> >  ExtensionContainer;
    /// Defines the extension container type.
    typedef ExtensionContainer::iterator ExtensionIterator;
    //@}

    /**
     * @brief   Retrieves the default runtime instance.
     */
    FWRUNTIME_API static Runtime* getDefault();

    /**
     * @brief   Destructor : does nothing.
     */
    ~Runtime();

    /**
     * @brief       Set the working path where Bundles and share folder are located.
     *
     * @param[in]   workingPath a boost path.
     */
    FWRUNTIME_API void setWorkingPath(const ::boost::filesystem::path& workingPath);

    /**
     * @brief       Get the path where Bundles and share folder are located.
     *
     * @return      a boost path.
     */
    FWRUNTIME_API ::boost::filesystem::path getWorkingPath() const;

    /**
     * @name    Bundles
     *
     * @{
     */

    /**
     * @brief       Adds a new bundle instance to the runtime system.
     *
     * @remark      The given bundle state will be altered according to the current configuration rules.
     * @param[in]   bundle  a shared pointer to the bundle instance to add
     */
    FWRUNTIME_API void addBundle( std::shared_ptr< Bundle > bundle );

    /**
     * @brief       Unregister a bundle instance to the runtime system.
     *
     * @param[in]   bundle  a shared pointer to the bundle instance to unregister
     */
    FWRUNTIME_API void unregisterBundle( std::shared_ptr< Bundle > bundle );

    /**
     * @brief       Adds all bundle found in the given path.
     *
     * @param[in]   repository  a path that may containing bundles
     */
    FWRUNTIME_API void addBundles( const ::boost::filesystem::path& repository );

    /**
     * @brief       Adds all bundle found at the default location.
     *
     * @remark      The given bundle state will be altered according to the current configuration rules.
     */
    FWRUNTIME_API void addDefaultBundles();

    /**
     * @brief   Retrieves the iterator on the begining of the bundle collection.
     *
     * @return  an iterator
     */
    FWRUNTIME_API BundleIterator bundlesBegin();

    /**
     * @brief   Retrieves the iterator on the end of the bundle collection.
     *
     * @return  an iterator
     */
    FWRUNTIME_API BundleIterator bundlesEnd();

    /**
     * @brief       Retrieves the bundle for the specified idenfier.
     *
     * @param[in]   identifier  a string containing a bundle identifier
     * @param[in]   version     the version of the bundle (undefined by default)
     *
     * @return      a shared pointer to the found bundle or null if none
     */
    FWRUNTIME_API std::shared_ptr< Bundle > findBundle( const std::string& identifier,
                                                        const Version& version = Version() ) const;

    /**
     * @brief       Retrieves the enabled bundle for the specified idenfier.
     *
     * @param[in]   identifier  a string containing a bundle identifier
     * @param[in]   version     the version of the bundle (undefined by default)
     *
     * @return      a shared pointer to the found bundle or null if none
     */
    FWRUNTIME_API std::shared_ptr< Bundle > findEnabledBundle( const std::string& identifier,
                                                               const Version& version = Version() ) const;

    //@}

    /**
     * @name    Executable, Executable Factories & Dynamic Executable Instanciation
     *
     * @{
     */

    /**
     * @brief       Adds a new executable factory instance to the runtime system.
     *
     * @param[in]   factory a shared pointer to an executable factory
     */
    FWRUNTIME_API void addExecutableFactory( std::shared_ptr< ExecutableFactory > factory );

    /**
     * @brief       Unregister a new executable factory instance to the runtime system.
     *
     * @param[in]   factory a shared pointer to an executable factory
     */
    FWRUNTIME_API void unregisterExecutableFactory( std::shared_ptr< ExecutableFactory > factory );

    /**
     * @brief   Create an instance of the given executable object type.
     *
     * An attempt is made to retrieve a registered executable factory. If none
     * is found, the creation will fail.
     *
     * @remark      This method will not try to load any bundle.
     *
     * @param[in]   type    a string containing an executable type
     *
     * @return      a pointer to the created executable instance
     */
    FWRUNTIME_API IExecutable* createExecutableInstance( const std::string& type );

    /**
     * @brief   Create an instance of the given executable object type and configuration element.
     *
     * An attempt is made to find a registered executable factory. If none
     * is found, the bundle of the given configuration element is started in the
     * hope it will register a executable factory for the given type. Then an
     * executable factory for the given type is searched once again and the
     * instantiation procedure goes further.
     *
     * @param[in]   type                    a string containing an executable type
     * @param[in]   configurationElement    a shared pointer to the configuration element to use for the executable
     * initialization
     *
     * @return  a pointer to the created executable instance
     */
    FWRUNTIME_API IExecutable* createExecutableInstance( const std::string& type,
                                                         std::shared_ptr< ConfigurationElement > configurationElement );

    /**
     * @brief       Retrieves the executable factory for the given identifier.
     *
     * @param[in]   type    a string containing a type identifier
     *
     * @return      a sgared pointer to the found executable factory or null of none
     */
    FWRUNTIME_API std::shared_ptr< ExecutableFactory > findExecutableFactory( const std::string& type ) const;

    /**
     * @brief       Retrieves the plugin instance for the specified bundle.
     *
     * @param[in]   bundle  a shared pointer to a bundle instance
     *
     * @return      a shared pointer to a plugin instance or null if none
     */
    FWRUNTIME_API std::shared_ptr< IPlugin > getPlugin( const std::shared_ptr< Bundle > bundle ) const;

    //@}

    /**
     * @name    Extensions
     *
     * @{
     */

    /**
     * @brief       Registers a new extension.
     *
     * @param[in]   extension   a shared pointer to the extension to register
     */
    FWRUNTIME_API void addExtension( std::shared_ptr<Extension> extension);

    /**
     * @brief       Unregister a new extension.
     *
     * @param[in]   extension   a shared pointer to the extension to register
     */
    FWRUNTIME_API void unregisterExtension( std::shared_ptr<Extension> extension);

    /**
     * @brief   Retrieves the iterator on the beginning of the extension collection.
     *
     * @return  an iterator
     */
    FWRUNTIME_API ExtensionIterator extensionsBegin();

    /**
     * @brief   Retrieves the iterator on the end of the extension collection.
     *
     * @return  an iterator
     */
    FWRUNTIME_API ExtensionIterator extensionsEnd();

    /**
     * @brief       Retrieves the extension instance matching the specified identifier.
     *
     * @param[in]   identifier  a string containing an extension identifier
     *
     * @return      a shared pointer to the found extension instance or null if none
     */
    FWRUNTIME_API std::shared_ptr< Extension > findExtension( const std::string& identifier ) const;

    //@}

    /**
     * @name    Extension Points
     */
    //@{
    /**
     * @brief       Registers a new extension point.
     *
     * @param[in]   point   a pointer to the extension point to register
     */
    FWRUNTIME_API void addExtensionPoint( std::shared_ptr<ExtensionPoint> point);

    /**
     * @brief       Unregister a new extension point.
     *
     * @param[in]   point   a pointer to the extension point to register
     */
    FWRUNTIME_API void unregisterExtensionPoint( std::shared_ptr<ExtensionPoint> point);

    /**
     * @brief       Retrieves the extension point instance matching the specified identifier.
     *
     * @param[in]   identifier  a string containing an extension point identifier
     *
     * @return      a shared pointer to the found extension point instance or null if none
     */
    FWRUNTIME_API std::shared_ptr< ExtensionPoint > findExtensionPoint( const std::string& identifier ) const;
    //@}

    private:

        ///< Defines the executable factory container type.
        typedef std::set< std::shared_ptr< ExecutableFactory > > ExecutableFactoryContainer;

        ///< Defines the extension point container type.
        typedef std::set< std::shared_ptr<ExtensionPoint> > ExtensionPointContainer;

        ///< Defines the plugin container type.
        typedef std::vector< std::shared_ptr<IPlugin> > PluginContainer;

        static std::shared_ptr<Runtime> m_instance; ///< The runtime instance.

        ExecutableFactoryContainer m_executableFactories; ///< Contains all executable factories.
        ExtensionContainer m_extensions;                  ///< Contains all registered extensions.
        ExtensionPointContainer m_extensionPoints;        ///< Contains all registered extension points.
        BundleContainer m_bundles;                        ///< Contains all bundles.
        PluginContainer m_plugins;                        ///< Contains all plugins.

        ::boost::filesystem::path m_workingPath; ///< Path where Bundles and share folder are located.

        /**
         * @brief   Constructor.
         */
        Runtime();
};

} // namespace fwRuntime

#endif // __FWRUNTIME_RUNTIME_HPP__
