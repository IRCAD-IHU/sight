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

#include "ioDicom/SSeriesDBMerger.hpp"

#include <data/Vector.hpp>

#include <fwGui/dialog/MessageDialog.hpp>

#include <fwMedDataTools/helper/SeriesDB.hpp>

#include <fwServices/macros.hpp>

namespace ioDicom
{

fwServicesRegisterMacro( ::fwGui::IActionSrv, ::ioDicom::SSeriesDBMerger, data::Vector )

//------------------------------------------------------------------------------

SSeriesDBMerger::SSeriesDBMerger() noexcept
{
}

//------------------------------------------------------------------------------

SSeriesDBMerger::~SSeriesDBMerger() noexcept
{
}

//------------------------------------------------------------------------------

void SSeriesDBMerger::info(std::ostream& _sstream )
{
    _sstream << "SSeriesDBMerger::info";
}

//------------------------------------------------------------------------------

void SSeriesDBMerger::starting()
{
}

//------------------------------------------------------------------------------

void SSeriesDBMerger::stopping()
{
}

//------------------------------------------------------------------------------

void SSeriesDBMerger::configuring()
{
}

//------------------------------------------------------------------------------

void SSeriesDBMerger::updating()
{
    auto destinationSeriesDB = this->getInOut< ::fwMedData::SeriesDB>("seriesDB");
    SLM_ASSERT("'seriesDB' key is not found.", destinationSeriesDB);

    ::fwMedDataTools::helper::SeriesDB sDBhelper(destinationSeriesDB);
    ::fwMedData::SeriesDB::ContainerType container = destinationSeriesDB->getContainer();
    data::Vector::csptr selectedSeries = this->getInput< data::Vector >("selectedSeries");

    // Save added series in this container in order to display information on the push
    std::vector< ::fwMedData::Series::sptr > addedSeries;

    // Loop through all selected series
    for(const data::Object::sptr& obj: selectedSeries->getContainer())
    {
        ::fwMedData::Series::sptr series = ::fwMedData::Series::dynamicCast(obj);

        if(series && std::find(container.begin(), container.end(), series) == container.end())
        {
            sDBhelper.add(series);
            addedSeries.push_back(series);
        }
    }
    sDBhelper.notify();

    // Display the informations
    ::fwGui::dialog::MessageDialog messageBox;
    messageBox.setIcon(::fwGui::dialog::IMessageDialog::INFO);
    messageBox.addButton(::fwGui::dialog::IMessageDialog::OK);
    messageBox.setTitle("Push Series");

    if(selectedSeries->empty())
    {
        messageBox.setMessage( "Unable to push series, there is no series selected." );
    }
    else if(addedSeries.empty())
    {
        messageBox.setMessage( "Unable to push series, the series are already present in the database." );
    }
    else
    {
        std::stringstream ss;
        if(addedSeries.size() == 1)
        {
            ss << addedSeries.size() << " series has been correctly pushed in the database:\n";
        }
        else
        {
            ss << addedSeries.size() << " series have been correctly pushed in the database:\n";
        }

        for(const ::fwMedData::Series::sptr& series: addedSeries)
        {
            std::string description = series->getDescription();
            description = (description.empty()) ? "[No description]" : description;
            ss << "- " << description << std::endl;
        }
        messageBox.setMessage( ss.str() );
    }

    messageBox.show();
}

//------------------------------------------------------------------------------

} // namespace ioDicom
