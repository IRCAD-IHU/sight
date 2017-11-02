/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __TUTO16MULTITHREADCONSOLE_SREADARRAY_HPP__
#define __TUTO16MULTITHREADCONSOLE_SREADARRAY_HPP__

#include "Tuto16MultithreadConsole/config.hpp"

#include <fwServices/IController.hpp>

namespace Tuto16MultithreadConsole
{

/**
 * @brief   This service initializes a ::fwData::Array with 10 values (0 to 9).

 * @section XML XML Configuration
 *
 * @code{.xml}
        <service type="::Tuto16MultithreadConsole::SReadArray">
            <inout key="array" uid="..." />
       </service>
   @endcode
 * @subsection In-Out In-Out:
 * - \b array [::fwData::Array]: array which values should be initialized.
 */
class TUTO16MULTITHREADCONSOLE_CLASS_API SReadArray : public ::fwServices::IController
{
public:

    fwCoreServiceClassDefinitionsMacro( (SReadArray)(::fwServices::IController) );

    TUTO16MULTITHREADCONSOLE_API SReadArray() noexcept;
    TUTO16MULTITHREADCONSOLE_API virtual ~SReadArray() noexcept;

protected:

    /// Do nothing
    virtual void starting() override;

    /// Do nothing
    virtual void stopping() override;

    /// Initialize the current ::fwData::Array with 10 values (0 to 9).
    virtual void updating() override;

    /// Do nothing
    virtual void configuring() override;
};

}  // namespace Tuto16MultithreadConsole

#endif  // __TUTO16MULTITHREADCONSOLE_SREADARRAY_HPP__

