/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __VISUVTKADAPTOR_SINTERACTORSTYLE_HPP__
#define __VISUVTKADAPTOR_SINTERACTORSTYLE_HPP__

#include "visuVTKAdaptor/config.hpp"

#include <fwRenderVTK/IAdaptor.hpp>

class vtkInteractorStyle;

namespace visuVTKAdaptor
{

/**
 */

/**
 * @brief Managing vtk interactor style
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
   <service type="::visuVTKAdaptor::SInteractorStyle">
       <config renderer="default" style="..." />
   </service>
   @endcode
 * @subsection Configuration Configuration
 * - \b config(mandatory) : contains the adaptor configuration
 *    - \b renderer (mandatory): defines the renderer to add the interactor
 *    - \b style (mandatory): defines the vtkInteractorStyle to use (ex : InteractorStyle2DForNegato)
 */
class VISUVTKADAPTOR_CLASS_API SInteractorStyle : public ::fwRenderVTK::IAdaptor
{

public:

    fwCoreServiceClassDefinitionsMacro( (SInteractorStyle)(::fwRenderVTK::IAdaptor) );

    VISUVTKADAPTOR_API SInteractorStyle() noexcept;

    VISUVTKADAPTOR_API virtual ~SInteractorStyle() noexcept;

protected:

    VISUVTKADAPTOR_API void configuring() override;
    VISUVTKADAPTOR_API void starting() override;
    VISUVTKADAPTOR_API void updating() override;
    VISUVTKADAPTOR_API void stopping() override;

    void setInteractorStyle(vtkInteractorStyle* interactor);

    std::string m_configuredStyle;
    vtkInteractorStyle* m_interactorStyle;
};

} //namespace visuVTKAdaptor

#endif // __VISUVTKADAPTOR_SINTERACTORSTYLE_HPP__
