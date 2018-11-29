/************************************************************************
 *
 * Copyright (C) 2017 IRCAD France
 * Copyright (C) 2017 IHU Strasbourg
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

#ifndef __FWRENDERVTK_VTK_FWVTKWHEELWIDGET_HPP__
#define __FWRENDERVTK_VTK_FWVTKWHEELWIDGET_HPP__

#include "fwRenderVTK/config.hpp"
#include "fwRenderVTK/vtk/fwVtkWheelRepresentation.hpp"

#include <vtkAbstractWidget.h>

#include <functional>

/**
 * @brief Implements the interactions with the wheel widget.
 *
 * Drag the wheel when picking its center with a left click.
 * Rotate it by selecting and turning the outer part with a left click.
 */
class FWRENDERVTK_CLASS_API fwVtkWheelWidget : public vtkAbstractWidget
{
public:

    /// Calls the constructor.
    FWRENDERVTK_API static fwVtkWheelWidget* New();

    vtkTypeMacro(fwVtkWheelWidget, vtkAbstractWidget);

    /// Not implemented.
    FWRENDERVTK_API void PrintSelf(ostream& os, vtkIndent indent) VTK_OVERRIDE;

    /// Set associated representation.
    FWRENDERVTK_API void SetRepresentation(fwVtkWheelRepresentation* rep);

    /// Get the representation.
    FWRENDERVTK_API fwVtkWheelRepresentation* GetRepresentation() const;

    /// Creates a wheel representation as default.
    FWRENDERVTK_API void CreateDefaultRepresentation() VTK_OVERRIDE;

    FWRENDERVTK_API void SetWheelUpdateCallback(std::function<void (double, double, double)> f);

private:

    /// Constructor, sets up the callback mapper.
    fwVtkWheelWidget();

    /// Destructor.
    ~fwVtkWheelWidget() VTK_OVERRIDE
    {
    }

    /// Callback called on select action. Triggers wheel selection.
    static void SelectAction(vtkAbstractWidget*);

    /// Callback called on mouse move. Rotates the wheel if it's been selected.
    static void MoveAction(vtkAbstractWidget*);

    /// Callback called when select ends. Resets the widget state to hovering.
    static void EndSelectAction(vtkAbstractWidget*);

    /// Record the current state of the interaction.
    enum _WidgetState
    {
        Start = 0,
        Hovering,
        Selecting,
        Rotating,
        Outside
    } WidgetState;

    fwVtkWheelWidget(const fwVtkWheelWidget&) VTK_DELETE_FUNCTION;
    void operator=(const fwVtkWheelWidget&) VTK_DELETE_FUNCTION;

    /// Records the mouse position when selecting.
    int m_initMouseX;
    int m_initMouseY;

    /// Records the initial wheel orientation when rotating.
    double m_initOrientation;

    /// Triggered on wheel modification. First two parameters are the wheel position and the third is for the angle.
    std::function< void (double, double, double) > m_wheelUpdateCallback;
};

#endif // __FWRENDERVTK_VTK_FWVTKWHEELWIDGET_HPP__
