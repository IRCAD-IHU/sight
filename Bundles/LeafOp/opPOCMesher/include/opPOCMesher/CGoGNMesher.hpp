/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __OPPOCMESHER_CGOGNMESHER_HPP__
#define __OPPOCMESHER_CGOGNMESHER_HPP__

#include <string>

#include <fwCom/Slots.hpp>
#include <opVTKMesh/IMesher.hpp>
#include "opPOCMesher/config.hpp"

namespace opPOCMesher
{

/**
 * @brief Service used to generate a mesh from an image.
 */
class OPPOCMESHER_CLASS_API CGoGNMesher : public ::opVTKMesh::IMesher
{

public:

    fwCoreServiceClassDefinitionsMacro ((CGoGNMesher)(::opVTKMesh::IMesher) );

    /**
     * @brief Constructor. Initializes slots.
     */
    OPPOCMESHER_API CGoGNMesher() throw ();

    /**
     * @brief Destructor. Does nothing.
     */
    OPPOCMESHER_API virtual ~CGoGNMesher() throw ();

protected:

    /**
     * @brief This method starts the operator. Does nothing.
     */
    OPPOCMESHER_API void starting() throw (::fwTools::Failed);

    /**
     * @brief This method stops the operator. Does nothing.
     */
    OPPOCMESHER_API void stopping() throw (::fwTools::Failed);

    /**
     * @brief Declare the configuration to define percent of reduction, image source and ModelSeries destination.
     *
     * Example of this service configuration
     * @code{.xml}
       <service impl="::opPOCMesher::CGoGNMesher" type="::opVTKMesh::IMesher">
           <config>
                <image>IMAGE_KEY</image>
                <modelSeries>MODELSERIES_KEY</modelSeries>

                <!-- Optionnal -->
                <valueMin>1</valueMin>
                <valueMax>255</valueMax>
                <radius>5</radius>
                <adapt>5</adapt>
                <faces>5</faces>
                <percentage>true</percentage>
                <closing>true</closing>
            </config>
       </service>
       @endcode
     */
    OPPOCMESHER_API void configuring() throw (::fwTools::Failed);

    /**
     * @brief This method is used to update services. Does nothing.
     */
    OPPOCMESHER_API void updating() throw (::fwTools::Failed);

private:

    /**
     * @brief This method is used to set an integer parameter.
     */
    OPPOCMESHER_API virtual void setInteger(int val, std::string key);

    /**
     * @brief This method is used to set an boolean parameter.
     */
    OPPOCMESHER_API virtual void setBoolean(bool val, std::string key);

    ///The image to mesh
    std::string m_imageKey;

    ///The modelSeries to modify
    std::string m_modelSeriesKey;

    ///The minimum value in the image mesh
    unsigned int m_valueMin;

    ///The maximum value to mesh
    unsigned int m_valueMax;

    ///The adaptativity factor used to simplify the mesh
    unsigned int m_adapt;

    ///The radius of the sphere used to simplify the mesh
    unsigned int m_radius;

    ///The percentage of faces of the marching cube or the number of faces needed for the mesh
    unsigned int m_faces;

    ///If true the variable m_faces is a percentage, if false it is a number
    bool m_percentage;

    ///If true the mesh is closed at the border of the image
    bool m_closing;

};

} // namespace opPOCMesher

#endif // __OPPOCMESHER_CGOGNMESHER_HPP__
