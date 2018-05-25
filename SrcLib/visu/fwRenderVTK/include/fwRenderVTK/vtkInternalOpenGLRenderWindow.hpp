/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#pragma once

#ifndef VTKINTERNALOPENGLRENDERWINDOW_HPP
#define VTKINTERNALOPENGLRENDERWINDOW_HPP

# include "fwRenderVTK/config.hpp"

#include <QOpenGLFramebufferObject>
#include <QOpenGLFunctions_2_0>
#include <QQuickFramebufferObject>
#include <vtkCamera.h>
#include <vtkExternalOpenGLRenderWindow.h>
#include <vtkGenericOpenGLRenderWindow.h>
#include <vtkObjectFactory.h>
#include <vtkRendererCollection.h>

namespace fwRenderVTK
{

class FrameBufferRenderer;

/**
 * @brief The vtkInternalOpenGLRenderWindow class
 *
 * This class is used to render a offscreen VTK window in a FrameBuffer
 */
class FWRENDERVTK_CLASS_API vtkInternalOpenGLRenderWindow : public vtkExternalOpenGLRenderWindow,
                                                            protected QOpenGLFunctions_2_0
{
public:
    friend class FrameBufferRenderer;

    static vtkInternalOpenGLRenderWindow FWRENDERVTK_API* New();
    vtkTypeMacro(vtkInternalOpenGLRenderWindow, vtkGenericOpenGLRenderWindow);

protected:
    FWRENDERVTK_API vtkInternalOpenGLRenderWindow();
    FWRENDERVTK_API ~vtkInternalOpenGLRenderWindow();

public:
    /**
     *  @brief: initialize openGL context/functions
     */
    virtual FWRENDERVTK_API void OpenGLInitState();

    virtual FWRENDERVTK_API void OpenGLEndState();

    /**
     * @brief: Call render method of the window
     */
    virtual FWRENDERVTK_API void Render();
    /**
     *  @brief: render into the framebuffer
     */
    FWRENDERVTK_API void  internalRender();
    /**
     *  @brief: set frame buffer to the window
     */
    FWRENDERVTK_API void setFrameBufferObject(QOpenGLFramebufferObject*);
    /**
     * @brief: configure renderer
     */
    FWRENDERVTK_API void setRenderer(FrameBufferRenderer*);
    /**
     * @brief: return renderer
     */
    FWRENDERVTK_API FrameBufferRenderer* getRenderer() const;

private:
    FrameBufferRenderer* m_qtParentRenderer;
};

}

#endif // VTKINTERNALOPENGLRENDERWINDOW_HPP
