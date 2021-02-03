/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
 * Copyright (C) 2012-2019 IHU Strasbourg
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

#include "uiImageQt/config.hpp"

#include <core/tools/Failed.hpp>

#include <fwDataTools/helper/MedicalImage.hpp>

#include <fwGui/editor/IEditor.hpp>

#include <fwGuiQt/SliceSelector.hpp>

namespace uiImageQt
{

/**
 * @brief   SliceIndexPositionEditor service allows to change the slice index of an image.
 *
 * This is represented by
 *  - a slider to select the slice index
 *  - a choice list to select the slice orientation (axial, frontal, sagittal)
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
   <service uid="..." type="::uiImageQt::SliceIndexPositionEditor" autoConnect="yes">
      <inout key="image" uid="..."/>
      <sliceIndex>${orientationValue}</sliceIndex>
   </service>
   @endcode
 * @subsection In-Out In-Out
 * - \b image [::fwData::Image]: image on which the slice index will be changed
 *
 * @subsection Configuration Configuration
 * - \b sliceIndex : Axis on which the index will be changed, must be "axial", "frontal" or "sagittal".
 */
class UIIMAGEQT_CLASS_API SliceIndexPositionEditor : public ::fwGui::editor::IEditor
{

public:

    fwCoreServiceMacro(SliceIndexPositionEditor, ::fwGui::editor::IEditor)

    /// Constructor. Do nothing.
    UIIMAGEQT_API SliceIndexPositionEditor() noexcept;

    /// Destructor. Do nothing.
    UIIMAGEQT_API virtual ~SliceIndexPositionEditor() noexcept;

protected:

    /// @brief The slice type: axial, frontal, sagittal.
    typedef ::fwDataTools::helper::MedicalImage::Orientation Orientation;

    /**
     * @brief Install the layout.
     */
    virtual void starting() override;

    /**
     * @brief Destroy the layout.
     */
    virtual void stopping() override;

    /// Update editor information from the image
    virtual void updating() override;

    /// Swap of image
    virtual void swapping() override;

    /**
     * @brief Configure the editor.
     *
     * Example of configuration
     * @code{.xml}
       <service uid="slider_negato1" type="::uiImageQt::SliceIndexPositionEditor" autoConnect="yes">
        <sliceIndex>axial</sliceIndex>
       </service>
       @endcode
       \b sliceIndex must be "axial", "frontal" or "sagittal".
     */
    virtual void configuring() override;

    /**
     * @brief Returns proposals to connect service slots to associated object signals,
     * this method is used for obj/srv auto connection
     *
     * Connect Image::s_MODIFIED_SIG to this::s_UPDATE_SLOT
     * Connect Image::s_SLICE_INDEX_MODIFIED_SIG to this::s_UPDATE_SLICE_INDEX_SLOT
     * Connect Image::s_SLICE_TYPE_MODIFIED_SIG to this::s_UPDATE_SLICE_TYPE_SLOT
     * Connect Image::s_BUFFER_MODIFIED_SIG to this::s_UPDATE_BUFFER_SLOT
     */
    UIIMAGEQT_API virtual KeyConnectionsMap getAutoConnections() const override;

    /// Overrides
    UIIMAGEQT_API virtual void info( std::ostream& _sstream ) override;

    /// Update the editor slider from the image slice index.
    UIIMAGEQT_API void updateSliceIndexFromImg();

    /// Update the editor slice type choice from the image slice type.
    UIIMAGEQT_API void updateSliceTypeFromImg(Orientation type );

    /// This method is called when the slider is move. Notify the slice index is modified.
    UIIMAGEQT_API void sliceIndexNotification(unsigned int index);

    /// This method is called when the slice type selected change. Notify the slice type is modified.
    UIIMAGEQT_API void sliceTypeNotification( int type );

private:

    /**
     * @name Slots
     * @{
     */
    /// Slot: update image slice index
    void updateSliceIndex(int axial, int frontal, int sagittal);

    /// Slot: update image slice type
    void updateSliceType(int from, int to);

    /// Slot: update image buffer
    void updateBuffer();
    /**
     * @}
     */

    /// @brief The field IDs for the slice index.
    static const std::string* SLICE_INDEX_FIELDID[ 3 ];

    ::fwGuiQt::SliceSelector* m_sliceSelectorPanel;

    ::fwDataTools::helper::MedicalImage m_helper;

};

} // uiImageQt
