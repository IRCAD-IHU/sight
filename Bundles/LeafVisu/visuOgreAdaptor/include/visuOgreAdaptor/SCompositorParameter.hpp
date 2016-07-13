/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __VISUOGREADAPTOR_SCOMPOSITORPARAMETER_HPP__
#define __VISUOGREADAPTOR_SCOMPOSITORPARAMETER_HPP__

#include "visuOgreAdaptor/IParameter.hpp"

#include "visuOgreAdaptor/config.hpp"

namespace visuOgreAdaptor
{

/**
 * @brief   Binds a FW4SPL data to a shader uniform from a specific compositor
 *
 * @section XML XML Configuration
 * @code{.xml}
    <adaptor uid="paramAdaptor" class="::visuOgreAdaptor::SCompositorParameter">
        <config renderer="layerName" compositorName="compositor" parameter="u_value" shaderType="fragment" />
    </adaptor>
   @endcode
 *
 * - \b compositorName (mandatory) : the name of the associated Ogre compositor
 * - \b parameter (mandatory) : name of the shader parameter to set
 * - \b technique (optional) : name of the technique, default to the first in the compositor
 * - \b shaderType (optional) : the type of the shader (vertex, geometry, fragment). Default to fragment.
 */
class VISUOGREADAPTOR_CLASS_API SCompositorParameter : public ::visuOgreAdaptor::IParameter
{

public:

    fwCoreServiceClassDefinitionsMacro ( (SCompositorParameter)(::visuOgreAdaptor::IParameter) );

    /// Constructor.
    VISUOGREADAPTOR_API SCompositorParameter() throw();

    /// Destructor. Does nothing
    VISUOGREADAPTOR_API virtual ~SCompositorParameter() throw();

    /// Updates parameter according to the attached fwData::Object
    VISUOGREADAPTOR_API void updateValue(::Ogre::MaterialPtr& _mat);

protected:

    /// Configure the adaptor
    VISUOGREADAPTOR_API virtual void doConfigure()  throw ( ::fwTools::Failed );
    /// Do nothing
    VISUOGREADAPTOR_API virtual void doStart()  throw ( ::fwTools::Failed );
    /// Do nothing
    VISUOGREADAPTOR_API virtual void doStop()  throw ( ::fwTools::Failed );
    /// Do nothing
    VISUOGREADAPTOR_API virtual void doSwap() throw ( ::fwTools::Failed );

private:

    /// Material name
    std::string m_compositorName;
};

} // visuOgreAdaptor

#endif // __VISUOGREADAPTOR_SCOMPOSITORPARAMETER_HPP__
