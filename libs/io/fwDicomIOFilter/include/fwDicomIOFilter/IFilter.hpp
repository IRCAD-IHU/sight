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

#include "fwDicomIOFilter/config.hpp"
#include "fwDicomIOFilter/exceptions/FilterFailure.hpp"
#include "fwDicomIOFilter/factory/new.hpp"
#include "fwDicomIOFilter/registry/detail.hpp"

#include <core/log/Logger.hpp>
#include <core/tools/Object.hpp>

#include <data/DicomSeries.hpp>

namespace fwDicomIOFilter
{

/**
 * @brief Base class for Dicom instance filter.
 */
class FWDICOMIOFILTER_CLASS_API IFilter :   public core::tools::Object
{
public:

    typedef ::fwDicomIOFilter::factory::Key Key;

    /**
     * @brief Class used to register a class factory in factory registry.
     * This class defines also the object factory ( 'create' )
     *
     * @tparam T Factory product type
     */
    template <typename T>
    class Registrar
    {
    public:
        Registrar()
        {
            ::fwDicomIOFilter::registry::get()->addFactory(T::classname(), &::fwDicomIOFilter::factory::New<T>);
        }
    };

    fwCoreClassMacro(IFilter, core::tools::Object)

    typedef std::vector< data::DicomSeries::sptr > DicomSeriesContainerType;

    /**
     * @brief Filter types
     */
    typedef enum
    {
        MODIFIER  = 0,  /*! Modifier */
        SORTER    = 1,  /*! Sorter */
        SPLITTER  = 2,  /*! Splitter */
        COMPOSITE = 3,  /*! Composite */
        CUSTOM    = 4,  /*! Custom */
    } FilterType;

    /// Destructor
    FWDICOMIOFILTER_API virtual ~IFilter();

    /**
     * @brief Apply the filter
     * @return Returns one or more Dicom Instance Group
     */
    FWDICOMIOFILTER_API virtual DicomSeriesContainerType apply(
        const data::DicomSeries::sptr& series, const core::log::Logger::sptr& logger) const = 0;

    /// Return the name of the filter
    FWDICOMIOFILTER_API virtual std::string getName() const = 0;

    /// Return the description of the filter
    FWDICOMIOFILTER_API virtual std::string getDescription() const = 0;

    /// Return filter type
    FWDICOMIOFILTER_API virtual FilterType getFilterType() const = 0;

    /// Return true if a configuration is required
    FWDICOMIOFILTER_API virtual bool isConfigurationRequired() const;

    /// Return true if the filter is configurable using GUI
    FWDICOMIOFILTER_API virtual bool isConfigurableWithGUI() const;

    /// Configure the filter using GUI dialog
    FWDICOMIOFILTER_API virtual void configureWithGUI();

protected:

    /// Constructor
    FWDICOMIOFILTER_API IFilter();

};

} // namespace fwDicomIOFilter
