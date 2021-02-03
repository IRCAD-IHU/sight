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

#include "basicRegistration/SPointListRegistration.hpp"

#include <fwCom/Signal.hxx>
#include <fwCom/Slots.hxx>

#include <fwData/Composite.hpp>
#include <fwData/Mesh.hpp>
#include <fwData/PointList.hpp>
#include <fwData/String.hpp>
#include <fwData/TransformationMatrix3D.hpp>

#include <fwDataTools/fieldHelper/Image.hpp>

#include <fwGui/dialog/MessageDialog.hpp>

#include <fwServices/macros.hpp>

#include <vtkLandmarkTransform.h>
#include <vtkMatrix4x4.h>
#include <vtkPoints.h>
#include <vtkSmartPointer.h>

fwServicesRegisterMacro( ::fwServices::IRegisterer, ::basicRegistration::SPointListRegistration )

namespace basicRegistration
{

const ::fwCom::Slots::SlotKeyType SPointListRegistration::s_CHANGE_MODE = "changeMode";
static const ::fwCom::Signals::SignalKeyType s_ERROR_COMPUTED_SIG       = "errorComputed";

SPointListRegistration::SPointListRegistration() :
    m_registrationMode(RIGID)
{
    newSignal<ErrorComputedSignalType>(s_ERROR_COMPUTED_SIG);
    newSlot(s_CHANGE_MODE, &SPointListRegistration::changeMode, this);
}

// ----------------------------------------------------------------------------

SPointListRegistration::~SPointListRegistration()
{
}

// ----------------------------------------------------------------------------

void SPointListRegistration::configuring()
{
    const auto configTree = this->getConfigTree();
    const auto config     = configTree.get_child_optional("config.<xmlattr>");

    if (config)
    {
        const std::string mode = config->get< std::string >("mode", "rigid");

        if(mode == "rigid")
        {
            m_registrationMode = RIGID;
        }
        else if(mode == "similarity")
        {
            m_registrationMode = SIMILARITY;
        }
        else if(mode == "affine")
        {
            m_registrationMode = AFFINE;
        }
        else
        {
            SLM_ERROR("Unknown registration mode: '" + mode + "', it must be 'rigid', 'similarity' or 'affine'."
                      " Defaulting to 'rigid'.");
        }
    }
    else
    {
        m_registrationMode = RIGID;
    }
}

// ----------------------------------------------------------------------------

void SPointListRegistration::starting()
{
}

// ----------------------------------------------------------------------------

void SPointListRegistration::stopping()
{
}

//-----------------------------------------------------------------------------

void SPointListRegistration::computeRegistration(core::HiResClock::HiResClockType)
{
    ::fwData::PointList::sptr registeredPL        = this->getInOut< ::fwData::PointList >("registeredPL");
    ::fwData::PointList::sptr referencePL         = this->getInOut< ::fwData::PointList >("referencePL");
    ::fwData::TransformationMatrix3D::sptr matrix = this->getInOut< ::fwData::TransformationMatrix3D >("output");

    if( registeredPL->getPoints().size() >= 3 &&
        registeredPL->getPoints().size() == referencePL->getPoints().size() )
    {
        vtkSmartPointer<vtkLandmarkTransform> landmarkTransform = vtkSmartPointer<vtkLandmarkTransform>::New();

        vtkSmartPointer<vtkPoints> sourcePts = vtkSmartPointer<vtkPoints>::New();
        vtkSmartPointer<vtkPoints> targetPts = vtkSmartPointer<vtkPoints>::New();

        const auto& firstPoint    = referencePL->getPoints()[0];
        const auto& firstPointReg = registeredPL->getPoints()[0];

        // If the points have labels ...
        if(firstPoint->getField< ::fwData::String >(::fwDataTools::fieldHelper::Image::m_labelId ) != nullptr
           && firstPointReg->getField< ::fwData::String >(::fwDataTools::fieldHelper::Image::m_labelId ) != nullptr)
        {
            // ... Then match them according to that label.
            for( ::fwData::Point::sptr pointRef : referencePL->getPoints() )
            {
                const std::string& labelRef =
                    pointRef->getField< ::fwData::String >(::fwDataTools::fieldHelper::Image::m_labelId )->value();

                for( ::fwData::Point::sptr pointReg : registeredPL->getPoints() )
                {
                    const std::string& labelReg =
                        pointReg->getField< ::fwData::String >(::fwDataTools::fieldHelper::Image::m_labelId )->value();

                    if(labelRef == labelReg)
                    {
                        auto coord = pointRef->getCoord();
                        sourcePts->InsertNextPoint(coord[0], coord[1], coord[2]);

                        coord = pointReg->getCoord();
                        targetPts->InsertNextPoint(coord[0], coord[1], coord[2]);
                    }
                }
            }
        }
        else
        {
            // ... Else match them according to their order.
            for(const auto& refPoint : referencePL->getPoints())
            {
                const auto& coords = refPoint->getCoord();
                sourcePts->InsertNextPoint(coords[0], coords[1], coords[2]);
            }

            for(const auto& regPoint : registeredPL->getPoints())
            {
                const auto& coords = regPoint->getCoord();
                targetPts->InsertNextPoint(coords[0], coords[1], coords[2]);
            }
        }

        landmarkTransform->SetSourceLandmarks(sourcePts);
        landmarkTransform->SetTargetLandmarks(targetPts);

        if(m_registrationMode == AFFINE)
        {
            landmarkTransform->SetModeToAffine();
        }
        else if(m_registrationMode == SIMILARITY)
        {
            landmarkTransform->SetModeToSimilarity();
        }
        else
        {
            landmarkTransform->SetModeToRigidBody();
        }

        landmarkTransform->Update();

        // Get the resulting transformation matrix (this matrix takes the source points to the target points)
        vtkSmartPointer<vtkMatrix4x4> m = landmarkTransform->GetMatrix();
        m->Invert();
        for(std::uint8_t l = 0; l < 4; ++l)
        {
            for(std::uint8_t c = 0; c < 4; ++c)
            {
                matrix->setCoefficient(l, c, m->GetElement(l, c));
            }
        }

        //compute RMSE
        double errorValue = 0.;

        for(vtkIdType i = 0; i < sourcePts->GetNumberOfPoints(); ++i)
        {
            double p1[3];
            sourcePts->GetPoint(i, p1);
            double p2[3];
            targetPts->GetPoint(i, p2);

            // to have homogeneous coordinates (x,y,z,w)
            double p2H[4] = { 1., 1., 1., 1.};
            std::copy(std::begin(p2), std::end(p2), std::begin(p2H));

            //p' = M*p
            double newP[4];
            m->MultiplyPoint(p2H, newP);

            errorValue += std::sqrt(((p1[0] - newP[0]) * (p1[0] - newP[0])) +
                                    ((p1[1] - newP[1]) * (p1[1] - newP[1])) +
                                    ((p1[2] - newP[2]) * (p1[2] - newP[2])));
        }

        errorValue /= sourcePts->GetNumberOfPoints();

        this->signal<ErrorComputedSignalType>(s_ERROR_COMPUTED_SIG)->asyncEmit(errorValue);

        // Notify Matrix modified
        auto sig = matrix->signal< ::fwData::Object::ModifiedSignalType >(::fwData::Object::s_MODIFIED_SIG);
        {
            ::fwCom::Connection::Blocker block(sig->getConnection(m_slotUpdate));
            sig->asyncEmit();
        }
    }
    else
    {
        if(registeredPL->getPoints().size() < 3)
        {
            ::fwGui::dialog::MessageDialog::show("Error",
                                                 "You must enter 3 or more points for the registration to work.",
                                                 ::fwGui::dialog::IMessageDialog::WARNING);
        }
        else
        {
            std::string msg = "The pointlists doesn't have the same number of points : ";
            msg += std::to_string(registeredPL->getPoints().size()) + " != " + std::to_string(
                referencePL->getPoints().size());
            ::fwGui::dialog::MessageDialog::show("Error", msg, ::fwGui::dialog::IMessageDialog::WARNING);
        }
    }
}

// ----------------------------------------------------------------------------

void SPointListRegistration::updating()
{
    const core::HiResClock::HiResClockType timestamp = core::HiResClock::getTimeInMilliSec();
    this->computeRegistration(timestamp);
}

//----------------------------------------------------------------------------

void SPointListRegistration::swapping()
{
    // Classic default approach to update service when object change
    this->stopping();
    this->starting();
}

//----------------------------------------------------------------------------

void SPointListRegistration::changeMode(std::string _value)
{
    if(_value == "RIGID")
    {
        m_registrationMode = RIGID;
    }
    else if( _value == "SIMILARITY")
    {
        m_registrationMode = SIMILARITY;
    }
    else if(_value == "AFFINE")
    {
        m_registrationMode = AFFINE;
    }
    else
    {
        SLM_ERROR("key "+ _value +" is not handled.");
    }
}

//----------------------------------------------------------------------------

} // namespace basicRegistration
