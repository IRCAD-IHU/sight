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

#include "ioGdcm/config.hpp"

#include <core/com/Signal.hpp>

#include <data/SeriesDB.hpp>

#include <ui/base/IActionSrv.hpp>

namespace sight::core::jobs
{
class IJob;
}

namespace ioGdcm
{

/**
 * @brief   This service is used to convert DicomSeries from source SeriesDB
 * and push result (ImageSeries, ModelSeries, ...) in target SeriesDB
 *
 * @section Signals Signals
 * - \b jobCreated( SPTR(core::jobs::IJob) ) : Emitted when a job is created.
 *
 * @section XML XML Configuration
 *
   @code{.xml}
   <service uid="..." impl="::ioGdcm::SDicomSeriesConverter">
     <in key="source" uid="..." />
     <inout key="target" uid="..." />
   </service>
   @endcode
 * @subsection Input Input:
 * - \b source [data::SeriesDB]: Source SeriesDB containing DicomSeries.
 * @subsection In-Out In-Out:
 * - \b target [data::SeriesDB]: Destination SeriesDB.
 */
class IOGDCM_CLASS_API SDicomSeriesConverter : public ::sight::ui::base::IActionSrv
{
public:

    fwCoreServiceMacro(SDicomSeriesConverter, sight::ui::base::IActionSrv )

    typedef core::com::Signal< void ( SPTR(core::jobs::IJob) ) > JobCreatedSignal;

    /**
     * @brief Constructor
     */
    IOGDCM_API SDicomSeriesConverter() noexcept;

    /**
     * @brief Destructor
     */
    IOGDCM_API virtual ~SDicomSeriesConverter() noexcept override;

protected:

    /// Configuring method. This method is used to configure the service.
    IOGDCM_API virtual void configuring() override;

    /// Override
    IOGDCM_API virtual void starting() override;

    /// Override
    IOGDCM_API virtual void stopping() override;

    /// Override
    IOGDCM_API void updating() override;

    /// Override
    IOGDCM_API void info(std::ostream& _sstream ) override;

protected:

    /// Signal emitted when a job is created
    SPTR(JobCreatedSignal) m_sigJobCreated;
};

} // namespace ioGdcm
