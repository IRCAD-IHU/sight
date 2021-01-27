/************************************************************************
 *
 * Copyright (C) 2019-2021 IRCAD France
 * Copyright (C) 2019 IHU Strasbourg
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

#include "module_guiQt/config.hpp"

#include <gui/editor/IEditor.hpp>

#include <QObject>

#include <filesystem>

namespace sight::modules::guiQt
{

namespace editor
{

/**
 * @brief   This editor displays an image.
 *
 * @section XML XML configuration
 * @code{.xml}
   <service uid="..." type="::modules::guiQt::editor::SImage" >
        <path>...</path>
        <width>...</width>
        <height>...</height>
   </service>
   @endcode
 * @subsection Configuration Configuration
 * - \b path: path of the image.
 * - \b width (optional): width of the image in pixels.
 * - \b height (optional): height of the image in pixels.
 */
class MODULE_GUIQT_CLASS_API SImage : public QObject,
                                      public gui::editor::IEditor
{
Q_OBJECT
public:

    fwCoreServiceMacro(SImage, gui::editor::IEditor)

    /// Constructor. Do nothing.
    MODULE_GUIQT_API SImage() noexcept;

    /// Destructor. Do nothing.
    MODULE_GUIQT_API virtual ~SImage() noexcept override;

protected:

    /** @name Service methods ( override from services::IService )
     * @{
     */
    /**
     * @brief This method launches the IEditor::starting method.
     */
    MODULE_GUIQT_API virtual void starting() override;

    /**
     * @brief This method launches the IEditor::stopping method.
     */
    MODULE_GUIQT_API virtual void stopping() override;

    /**
     * @brief This method is used to update services. Do nothing.
     */
    MODULE_GUIQT_API virtual void updating() override;

    /// This method is used to configure the class parameters.
    MODULE_GUIQT_API virtual void configuring() override;

    /** @} */
private:

    std::filesystem::path m_path; /// Path of the image
    int m_width{-1}; /// Width of the image (pixels)
    int m_height{-1}; /// Height of the image (pixels)
};

}
}
