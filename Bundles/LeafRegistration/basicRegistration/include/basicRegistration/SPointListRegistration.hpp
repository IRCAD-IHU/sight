/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __BASICREGISTRATION_SPOINTLISTREGISTRATION_HPP__
#define __BASICREGISTRATION_SPOINTLISTREGISTRATION_HPP__

#include "basicRegistration/config.hpp"

#include <fwCom/Slot.hpp>

#include <fwServices/IController.hpp>

namespace basicRegistration
{

/**
 * @brief   Register a point list against a reference point list. Each point in the list to register is matched
 *          with a point in the reference list according to their order or to their labels if they have any.
 *          The output is a transformation matrix.
 * @section Signals Signals
 * - \b errorComputed(double): emitted when registration error is computed.
 * @section Slots Slots
 * - \b changeMode(std::string, std::string): called when registration mode is changed.
 * Three modes are available : RIGID, SIMILARITY or AFFINE
 * @section XML XML Configuration
 * @code{.xml}
       <service uid="..." type="::basicRegistration::SPointListRegistration">
           <inout key="registeredPL" uid="..." />
           <inout key="referencePL" uid="..." />
           <inout key="output" uid="..." />
       </service>
   @endcode
 * @subsection In-Out In-Out
 * - \b registeredPL [::fwData::PointList]: List containing the points to register.
 * - \b referencePL [::fwData::PointList]: List containing the reference points to match with the points to register.
 * - \b output [::fwData::TransformationMatrix3D]: Transformation matrix between the source and target points.
 *
 */
class BASICREGISTRATION_CLASS_API SPointListRegistration : public ::fwServices::IController
{

public:

    fwCoreServiceClassDefinitionsMacro( (SPointListRegistration)(::fwServices::IController) );

    /**
     * @name Slots API
     * @{
     */
    BASICREGISTRATION_API static const ::fwCom::Slots::SlotKeyType s_CHANGE_MODE;
    ///@}

    /**
     * @name Signals API
     * @{
     */
    /// Emitted when registration error is computed
    typedef ::fwCom::Signal< void (double) > ErrorComputedSignalType;
    ///@}

    BASICREGISTRATION_API SPointListRegistration();

    BASICREGISTRATION_API virtual ~SPointListRegistration();

protected:

    /// Registration Mode (default: RIGID)
    typedef enum Mode
    {
        RIGID,      /*!< rigid mode of VTK registration */
        SIMILARITY, /*!< similarity mode of VTK registration */
        AFFINE      /*!< affine mode of VTK registration */
    }RegistrationModeType;

    /// Configures the service
    BASICREGISTRATION_API virtual void configuring() override;

    /// Do nothing
    BASICREGISTRATION_API virtual void starting() override;

    /// Do nothing
    BASICREGISTRATION_API virtual void stopping() override;

    /// Perform the registration
    BASICREGISTRATION_API virtual void updating() override;

    /// Do nothing
    BASICREGISTRATION_API virtual void swapping() override;

private:

    ///SLOT: changeMode
    void changeMode(std::string _value, std::string _key);

    /// Key source point list
    std::string m_registeredPointsKey;

    /// Key dest point list
    std::string m_referencePointsKey;

    /// Key for computed registration matrix
    std::string m_matrixKey;

    ///Registration Mode
    RegistrationModeType m_registrationMode;
};

} // namespace basicRegistration

#endif /*__BASICREGISTRATION_SPOINTLISTREGISTRATION_HPP__*/
