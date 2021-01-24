/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
 * Copyright (C) 2012-2018 IHU Strasbourg
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

#include "fwGdcmIO/writer/tid/Fiducial.hpp"

#include "fwGdcmIO/container/DicomCodedAttribute.hpp"
#include "fwGdcmIO/container/sr/DicomSRCodeNode.hpp"
#include "fwGdcmIO/container/sr/DicomSRImageNode.hpp"
#include "fwGdcmIO/container/sr/DicomSRSCoord3DNode.hpp"
#include "fwGdcmIO/container/sr/DicomSRSCoordNode.hpp"
#include "fwGdcmIO/container/sr/DicomSRTextNode.hpp"
#include "fwGdcmIO/container/sr/DicomSRUIDRefNode.hpp"
#include "fwGdcmIO/helper/DicomDataTools.hpp"

#include <core/tools/Stringizer.hpp>

#include <data/PointList.hpp>
#include <data/String.hpp>
#include <data/Vector.hpp>

#include <fwDataTools/fieldHelper/Image.hpp>

#include <fwMedData/Series.hpp>
#include <fwMedData/types.hpp>

#include <boost/algorithm/string/split.hpp>

#include <gdcmUIDGenerator.h>

#include <sstream>

namespace fwGdcmIO
{
namespace writer
{
namespace tid
{

//------------------------------------------------------------------------------

Fiducial::Fiducial(const SPTR(::gdcm::Writer)& writer,
                   const SPTR(::fwGdcmIO::container::DicomInstance)& instance,
                   const data::Image::csptr& image) :
    ::fwGdcmIO::writer::tid::TemplateID< data::Image >(writer, instance, image)
{
}

//------------------------------------------------------------------------------

Fiducial::~Fiducial()
{
}

//------------------------------------------------------------------------------

void Fiducial::createNodes(const SPTR(::fwGdcmIO::container::sr::DicomSRNode)& parent,
                           bool useSCoord3D)
{
    data::PointList::sptr pointList =
        m_object->getField< data::PointList >(::fwDataTools::fieldHelper::Image::m_imageLandmarksId);
    if (pointList)
    {
        unsigned int id = 1;
        for(const data::Point::sptr& point : pointList->getPoints())
        {
            this->createFiducial(parent, point, id++, useSCoord3D);
        }
    }
}

//------------------------------------------------------------------------------

void Fiducial::createFiducial(const SPTR(::fwGdcmIO::container::sr::DicomSRNode)& parent,
                              const data::Point::csptr& point,
                              unsigned int id, bool useSCoord3D)
{
    // Create Fiducial node
    SPTR(::fwGdcmIO::container::sr::DicomSRCodeNode) rootNode =
        std::make_shared< ::fwGdcmIO::container::sr::DicomSRCodeNode >(
            ::fwGdcmIO::container::DicomCodedAttribute("122340", "DCM", "Fiducial feature"), "CONTAINS",
            ::fwGdcmIO::container::DicomCodedAttribute("111123", "DCM", "Marker placement")); //FIXME : Find a better
                                                                                              // representation
    parent->addSubNode(rootNode);

    // Create Fiducial ID node
    SPTR(::fwGdcmIO::container::sr::DicomSRUIDRefNode) idNode =
        std::make_shared< ::fwGdcmIO::container::sr::DicomSRUIDRefNode >(
            ::fwGdcmIO::container::DicomCodedAttribute("dd1201", "DCM",
                                                       "Fiducial ID"), "HAS PROPERTIES", core::tools::getString(id));
    rootNode->addSubNode(idNode);

    // Create Fiducial UID node
    ::gdcm::UIDGenerator generator;
    SPTR(::fwGdcmIO::container::sr::DicomSRUIDRefNode) uidNode =
        std::make_shared< ::fwGdcmIO::container::sr::DicomSRUIDRefNode >(
            ::fwGdcmIO::container::DicomCodedAttribute("dd1202", "DCM", "Fiducial UID"), "HAS PROPERTIES",
            generator.Generate());
    rootNode->addSubNode(uidNode);

    // Create Fiducial intent node
    const std::string label =
        point->getField< data::String >(::fwDataTools::fieldHelper::Image::m_labelId)->value();
    SPTR(::fwGdcmIO::container::sr::DicomSRTextNode) intentNode =
        std::make_shared< ::fwGdcmIO::container::sr::DicomSRTextNode >(
            ::fwGdcmIO::container::DicomCodedAttribute("122369", "DCM", "Fiducial intent"), "HAS PROPERTIES", label);
    rootNode->addSubNode(intentNode);

    if(useSCoord3D)
    {
        // Create SCoord3D Node
        const float scoord[] = {
            static_cast<float>(point->getCoord()[0]),
            static_cast<float>(point->getCoord()[1]),
            static_cast<float>(point->getCoord()[2])
        };
        std::vector<float> scoordVector(scoord, scoord + 3);
        SPTR(::fwGdcmIO::container::sr::DicomSRSCoord3DNode) scoord3DNode =
            std::make_shared< ::fwGdcmIO::container::sr::DicomSRSCoord3DNode >(
                ::fwGdcmIO::container::DicomCodedAttribute(), "HAS PROPERTIES", "POINT", scoordVector,
                m_instance->getSOPInstanceUIDContainer()[0]);
        rootNode->addSubNode(scoord3DNode);
    }
    else
    {
        // Create SCoord Node
        const float scoord[] = {
            static_cast<float>(point->getCoord()[0]),
            static_cast<float>(point->getCoord()[1])
        };
        std::vector<float> scoordVector(scoord, scoord + 2);
        SPTR(::fwGdcmIO::container::sr::DicomSRSCoordNode) scoordNode =
            std::make_shared< ::fwGdcmIO::container::sr::DicomSRSCoordNode >(
                ::fwGdcmIO::container::DicomCodedAttribute(), "HAS PROPERTIES", "POINT", scoordVector);
        rootNode->addSubNode(scoordNode);

        // Create Image Node
        const std::size_t frameNumber = ::fwGdcmIO::helper::DicomDataTools::convertPointToFrameNumber(m_object, point);
        SPTR(::fwGdcmIO::container::sr::DicomSRImageNode) imageNode =
            std::make_shared< ::fwGdcmIO::container::sr::DicomSRImageNode >(
                ::fwGdcmIO::container::DicomCodedAttribute(), "SELECTED FROM", m_instance->getSOPClassUID(),
                m_instance->getSOPInstanceUIDContainer()[frameNumber-1], frameNumber);
        scoordNode->addSubNode(imageNode);
    }

}

//------------------------------------------------------------------------------

} // namespace tid
} // namespace writer
} // namespace fwGdcmIO
