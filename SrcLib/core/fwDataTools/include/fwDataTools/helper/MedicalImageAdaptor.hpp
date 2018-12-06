/************************************************************************
 *
 * Copyright (C) 2009-2018 IRCAD France
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

#pragma once

#include "fwDataTools/config.hpp"
#include "fwDataTools/helper/ImageGetter.hpp"

#include <fwCom/helper/SigSlotConnection.hpp>
#include <fwCom/Slot.hpp>
#include <fwCom/Slots.hpp>

#include <fwData/Composite.hpp>
#include <fwData/Integer.hpp>
#include <fwData/String.hpp>
#include <fwData/TransferFunction.hpp>

#include <vector>

namespace fwDataTools
{

namespace helper
{

/**
 * @brief   Helpers for medical image.
 *
 * @section Slots Slots
 *
 * The two slots must be reimplemented by the services managing transfer function
 * - \b updateTFPoints(): called when the transfer function points are modified
 * - \b updateTFWindowing(double window, double level): called when the transfer function windowing is modified
 *
 * @deprecated this class is deprecated since 17.0, please use fwDataTools::helper::MedicalImage or
 * fwDataTools::helper::TransferFunction instead
 */
class FWDATATOOLS_CLASS_API MedicalImageAdaptor
{

public:
    fwCoreBaseClassDefinitionsMacro( (MedicalImageAdaptor) );

    /// Image orientation
    typedef enum
    {
        X_AXIS = 0,
        Y_AXIS,
        Z_AXIS
    } Orientation;

    /// Destructor. Do nothing.
    FWDATATOOLS_API virtual ~MedicalImageAdaptor();

    /// Set the image orientation.
    FWDATATOOLS_API virtual void setOrientation( Orientation orientation );

    /// Return the image orientation.
    Orientation getOrientation() const
    {
        return m_orientation;
    }

    /// Set the current TransferFunction
    FWDATATOOLS_API void setTransferFunction( const ::fwData::TransferFunction::sptr& tf );

protected:

    /// Constructor. Do nothing.
    FWDATATOOLS_API MedicalImageAdaptor(); // this class VISUVTKADAPTOR_CLASS_API must be specialized

    /**
     * @brief Set the orientation of the image.
     * @param[in] orientation must be 0 (X_AXIS), 1 (Y_AXIS) or 2 (Z_AXIS).
     */
    FWDATATOOLS_API void setOrientation( int orientation );

    /**
     * @brief Get the image spacing.
     * @param[out] spacing : the image spacing
     */
    FWDATATOOLS_API void getImageSpacing(double spacing[3]) const;

    /**
     * @brief Get the image origin.
     * @param[out] origin : the image origin
     */
    FWDATATOOLS_API void getImageOrigin(double origin[3]) const;

    /**
     * @brief Get the image data size (number of slices).
     * @param[out] size : the image size
     */
    FWDATATOOLS_API void getImageDataSize(int size[3]) const;

    /**
     * @brief Get the image size ( = dataSize * spacing ).
     * @param[out] size : the image size
     */
    FWDATATOOLS_API void getImageSize(double size[3]) const;

    /**
     * @brief Get the slice center
     * @param[out] center : the slice center
     */
    FWDATATOOLS_API void getCurrentSliceCenter(double center[3]);

    /**
     * @brief Convert world coordinates to slice index coordinates
     * @param[in] world : coordinate in the world
     * @param[out] index : coordinate in the slice index
     */
    FWDATATOOLS_API void worldToSliceIndex(const double world[3], int index[3] );

    /**
     * @brief Convert coordinates in the world to coordinates in the image
     * @param[in] world : coordinate in the world
     * @param[out] index : coordinate in the image
     */
    FWDATATOOLS_API void worldToImageSliceIndex(const double world[3], int index[3] );

    /**
     * @brief Convert from world coordinates system to image coordinates system
     * @param[in] index : coordinate in the image
     * @param[out] world : coordinate in the world
     */
    FWDATATOOLS_API void sliceIndexToWorld(const int index[3], double world[3] );

    /// Return the image
    FWDATATOOLS_API ::fwData::Image::sptr getImage() const;

    /**
     * @brief Get the image spacing.
     * @param[out] spacing : the image spacing
     */
    template< typename FLOAT_ARRAY_3 >
    void getImageSpacing(FLOAT_ARRAY_3 spacing);

    /**
     * @brief Get the image data size (number of slices).
     * @param[out] size : the image size
     */
    template< typename INT_INDEX >
    void getImageDataSize(INT_INDEX size);

    /**
     * @brief Convert world coordinates to slice index coordinates
     * @param[in] world : coordinate in the world
     * @param[out] index : coordinate in the slice index
     */
    template< typename WORLD, typename INT_INDEX >
    void worldToSliceIndex(const WORLD world, INT_INDEX* index );

    /**
     * @brief Convert coordinates in the world to coordinates in the image
     * @param[in] world : coordinate in the world
     * @param[out] index : coordinate in the image
     */
    template< typename WORLD, typename INT_INDEX >
    void worldToImageSliceIndex(const WORLD world, INT_INDEX* index );

    /**
     * @brief Return the 4 points of the image plane
     *
     * - points are ordered in the following way : image origin is the first point
     * - points are inserted using the preference follow X axis if exists, if not exists follow Y axis
     * if Y axis is not present follow Z axis
     */
    FWDATATOOLS_API void getPlane( double points[4][3], int sliceNumber );

    /// Set the slice index
    FWDATATOOLS_API bool setSliceIndex(const int index[3]);
    /// Get the slice index
    FWDATATOOLS_API void getSliceIndex(::fwData::Integer::sptr index[3]);

    /// Update the image information (slice index, min/max,...)
    FWDATATOOLS_API void updateImageInfos( ::fwData::Image::sptr image  );

    /**
     * @brief Create and set the default transfer function.
     *
     * Create the image's transfer function pool if if does not exist and then create the defaut transfer function.
     * Set the current transfer function to the default grey-level if no transfer function was set (using
     * setTransferFunction() ).
     */
    FWDATATOOLS_API void createTransferFunction( ::fwData::Image::sptr image );

    /**
     * @brief Sets the transfer function, creates one if _tf is null (@see createTransferFunction).
     *
     * Connects the TF's signals to the updateTFPoints and updateTFWindowing slots.
     */
    FWDATATOOLS_API void setOrCreateTF(const ::fwData::TransferFunction::sptr& _tf, const fwData::Image::sptr& _image);

    /// Get the current transfer function
    FWDATATOOLS_API ::fwData::TransferFunction::sptr getTransferFunction() const;

    /**
     * @name Connections to transfer function
     * @{
     */

    /// Install connections to listen TF modifications
    FWDATATOOLS_API void installTFConnections();

    /// Remove the TF connections
    FWDATATOOLS_API void removeTFConnections();

    /**
     * @brief Install the slots to managed TF modifications.
     *
     * Creates slots to listen TF selection Composite and TransferFunction signals.
     *
     * @warning It must be called in the service constructor
     */
    FWDATATOOLS_API void installTFSlots(::fwCom::HasSlots* hasslots);

    /// Slot: called when transfer function points are modified
    FWDATATOOLS_API virtual void updateTFPoints();

    /// Slot: called when transfer function windowing is modified
    FWDATATOOLS_API virtual void updateTFWindowing(double window, double level);

    typedef ::fwCom::Slot<void ()> UpdateTFPointsSlotType;
    typedef ::fwCom::Slot<void (double, double)> UpdateTFWindowingSlotType;

    /// Slot called when transfer function points are modified
    UpdateTFPointsSlotType::sptr m_slotUpdateTFPoints;

    /// Slot called when transfer function windowing is modified
    UpdateTFWindowingSlotType::sptr m_slotUpdateTFWindowing;
    /**
     * @}
     */

    /// Image orientation
    Orientation m_orientation;

    /// Axial slice index
    ::fwData::Integer::sptr m_axialIndex;
    /// Frontal slice index
    ::fwData::Integer::sptr m_frontalIndex;
    /// Sagittal slice index
    ::fwData::Integer::sptr m_sagittalIndex;

private:

    /// Current image
    ::fwData::Image::wptr m_weakImage;

    /// Transfer function selection
    ::fwData::TransferFunction::wptr m_transferFunction;

    /// Connections to the transfer function
    ::fwCom::helper::SigSlotConnection m_tfConnections;
};

//------------------------------------------------------------------------------
template< typename FLOAT_ARRAY_3 >
void MedicalImageAdaptor::getImageSpacing(FLOAT_ARRAY_3 spacing)
{
    ::fwData::Image::sptr image = this->getImage();

    const ::fwData::Image::SpacingType imSpacing = image->getSpacing();
    std::copy(imSpacing.begin(), imSpacing.end(), spacing);
}

//------------------------------------------------------------------------------
template< typename INT_INDEX >
void MedicalImageAdaptor::getImageDataSize(INT_INDEX size)
{
    ::fwData::Image::sptr image = this->getImage();

    const ::fwData::Image::SizeType imSize = image->getSize();
    std::copy(imSize.begin(), imSize.end(), size);
}

//------------------------------------------------------------------------------

template< typename WORLD, typename INT_INDEX >
void MedicalImageAdaptor::worldToSliceIndex(const WORLD world, INT_INDEX* index )
{
    double spacing[3];
    this->getImageSpacing(spacing);
    double origin[3];
    this->getImageOrigin(origin);
    for ( int i = 0; i < 3; ++i )
    {
        index[i] =
            static_cast< INT_INDEX >( ( (world[i] - origin[i])/spacing[i] ) +
                                      ( ( (world[i] - origin[i])/spacing[i] ) >= 0 ? 0.5 : -0.5 ) );
    }
}

//------------------------------------------------------------------------------

template< typename WORLD, typename INT_INDEX >
void MedicalImageAdaptor::worldToImageSliceIndex(const WORLD world, INT_INDEX* index )
{
    INT_INDEX imageSize[3];
    this->getImageDataSize(imageSize);
    this->worldToSliceIndex(world, index);

    INT_INDEX idval;
    for (int i = 0; i < 3; i++)
    {
        INT_INDEX max = imageSize[i]-1;
        idval = index[i];
        if (idval < 0)
        {
            index[i] = 0;
        }
        else if (idval > max)
        {
            index[i] = max;
        }
    }
}

} //namespace helper

} //namespace fwDataTools
