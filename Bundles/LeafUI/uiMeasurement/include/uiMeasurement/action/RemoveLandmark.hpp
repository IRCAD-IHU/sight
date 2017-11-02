/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __UIMEASUREMENT_ACTION_REMOVELANDMARK_HPP__
#define __UIMEASUREMENT_ACTION_REMOVELANDMARK_HPP__

#include "uiMeasurement/config.hpp"

#include <fwData/Image.hpp>
#include <fwData/Point.hpp>

#include <fwGui/IActionSrv.hpp>

namespace uiMeasurement
{

namespace action
{

/**
 * @brief   This action removes landmarks.
 */
class UIMEASUREMENT_CLASS_API RemoveLandmark : public ::fwGui::IActionSrv
{
public:
    fwCoreServiceClassDefinitionsMacro( (RemoveLandmark)( ::fwGui::IActionSrv) );

    UIMEASUREMENT_API RemoveLandmark() noexcept;

    UIMEASUREMENT_API virtual ~RemoveLandmark() noexcept;

protected:

    void configuring() override;

    void starting() override;

    void updating() override;

    void stopping() override;

    void info(std::ostream& _sstream ) override;

private:
    void notify( ::fwData::Image::sptr image, ::fwData::Point::sptr backup );

    ::fwData::Point::sptr  getLandmarkToRemove(::fwData::Image::sptr image, bool& removeAll);
};

} // namespace action

} // namespace uiMeasurement

#endif // __UIMEASUREMENT_ACTION_REMOVELANDMARK_HPP__
