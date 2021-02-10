/************************************************************************
 *
 * Copyright (C) 2017-2021 IRCAD France
 * Copyright (C) 2017-2020 IHU Strasbourg
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

#include "uiHandEye/config.hpp"

#include <data/Composite.hpp>
#include <data/MatrixTL.hpp>
#include <data/PointList.hpp>
#include <data/Vector.hpp>

#include <QLabel>
#include <QListWidget>
#include <QPointer>
#include <QPushButton>

#include <ui/base/editor/IEditor.hpp>

namespace uiHandEye
{
/**
 * @brief   SHandEyeEditor service is used to handle the calibration points acquisition.
 * This service takes two matrixTL as inputs, and fills the two vector of matrix when the 'addSlot' is called.
 *
 * @see modules::navigation::calibration::SHandEyeCalibration
 *
 * @deprecated This service is not supported anymore, it will be removed in sight 21.0.
 *
 * @section slots slots
 * - \b add(): Add new calibration points
 * - \b remove(): Remove calibration points
 * - \b reset(): Reset currently acquiered calibration points
 * - \b getSelection(): Display a dialog with the current matrices
 *
 * @code{.xml}
    <service uid="..." type="::uiHandEye::SHandEyeEditor">
        <in key="matrixTL1" uid="..." />
        <in key="matrixTL2" uid="..." />
        <inout key="matrixVector1" uid="..." />
        <inout key="matrixVector2" uid="..." />
    </service>
   @endcode

 * @subsection InOut InOut
 * - \b matrixVector1 [sight::data::Vector]: first vector filled with {Ai, Aj, ...} matrix
 *[data::TransformationMatrix3D] when 'addSlot' is called
 * - \b matrixVector2 [sight::data::Vector]: second vector filled with {Bi, Bj, ...} matrix
 *[data::TransformationMatrix3D] when 'addSlot' is called
 *
 * @subsection Input Input
 * - \b matrixTL1 [sight::data::FrameTL]: the first matrixTL filled with {Ai, Aj, ...} matrices
 *[data::TransformationMatrix3D]
 * - \b matrixTL2 [sight::data::FrameTL]: the second matrixTL filled with {Bi, Bj, ...} matrices
 *[data::TransformationMatrix3D]
 */
class UIHANDEYE_CLASS_API SHandEyeEditor : public QObject,
                                           public sight::ui::base::editor::IEditor
{
Q_OBJECT;

public:
    fwCoreServiceMacro(SHandEyeEditor, sight::ui::base::editor::IEditor)

    /**
     * @name Slots API
     * @{
     */

    UIHANDEYE_API static const core::com::Slots::SlotKeyType s_ADD_SLOT;
    typedef core::com::Slot<void ()> AddSlotType;

    UIHANDEYE_API static const core::com::Slots::SlotKeyType s_REMOVE_SLOT;
    typedef core::com::Slot<void ()> RemoveSlotType;

    UIHANDEYE_API static const core::com::Slots::SlotKeyType s_RESET_SLOT;
    typedef core::com::Slot<void ()> ResetSlotType;

    UIHANDEYE_API static const core::com::Slots::SlotKeyType s_GET_SELECTION_SLOT;
    typedef core::com::Slot<void ()> GetSelectionSlotType;
    ///@}

    /**
     * @brief Constructor.
     */
    [[deprecated("This service is not supported anymore, it will be removed in sight 21.0.")]]
    UIHANDEYE_API SHandEyeEditor() noexcept;

    /**
     * @brief Destructor.
     */
    ~SHandEyeEditor() noexcept
    {
    }

protected:

    UIHANDEYE_API void configuring() override;

    /**
     * @brief Starting method : This method is used to initialize the service.
     */
    UIHANDEYE_API void starting() override;

    /**
     * @brief Stopping method : This method is used to stop the service.
     */
    UIHANDEYE_API void stopping() override;

    /**
     * @brief Updating method : This method is used to update the service.
     */
    UIHANDEYE_API void updating() override;

    /**
     * @brief Slot called when the user presses the add acquisition button.
     */
    UIHANDEYE_API void add();

    /**
     * @brief Slot called when the user presses the remove acquisition button.
     */
    UIHANDEYE_API void remove();

    /**
     * @brief Slot called when the user presses the reset button.
     */
    UIHANDEYE_API void reset();

    /**
     * @brief Slot called when the user presses the display button.
     */
    UIHANDEYE_API void getSelection();

private Q_SLOTS:
    /**
     * @brief Slot called when an element is double-clicked in the vector widget.
     */
    void onItemDoubleClicked(QListWidgetItem*);

private:

    /**
     * @brief Index of the last acquisition.
     */
    int m_captureIdx;

    /**
     * @brief Label displaying the number of point acquisitions.
     */
    QPointer< QLabel > m_nbCapturesLabel;

    /**
     * @brief Calibration point vector.
     */
    QPointer< QListWidget > m_capturesListWidget;

    /// Last timestamp
    core::HiResClock::HiResClockType m_lastTimestamp;

};
} // namespace uiHandEye
