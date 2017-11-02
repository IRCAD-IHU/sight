/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwGui/dialog/LocationDialog.hpp"

#include <fwServices/registry/ActiveWorkers.hpp>

namespace fwGui
{
namespace dialog
{
//-----------------------------------------------------------------------------

LocationDialog::LocationDialog()
{

    ::fwServices::registry::ActiveWorkers::getDefaultWorker()->postTask< void >(std::function< void() >(
                                                                                    [&] {
                ::fwGui::GuiBaseObject::sptr guiObj = ::fwGui::factory::New(ILocationDialog::REGISTRY_KEY);
                m_implementation = ::fwGui::dialog::ILocationDialog::dynamicCast(guiObj);
            })
                                                                                ).wait();
}

//------------------------------------------------------------------------------

LocationDialog::~LocationDialog()
{
}

//-----------------------------------------------------------------------------

::fwData::location::ILocation::sptr LocationDialog::show()
{
    typedef SPTR (::fwData::location::ILocation) R;

    std::function< R() > func = std::bind(&ILocationDialog::show, m_implementation);
    std::shared_future< R > f = ::fwServices::registry::ActiveWorkers::getDefaultWorker()->postTask< R >(func);

    f.wait();
    return f.get();
}

//-----------------------------------------------------------------------------

void LocationDialog::setType( ::fwGui::dialog::ILocationDialog::Types type)
{
    m_implementation->setType( type );
}

//-----------------------------------------------------------------------------

void LocationDialog::addFilter(const std::string& filterName, const std::string& wildcardList )
{
    m_implementation->addFilter(filterName, wildcardList);
}

//-----------------------------------------------------------------------------

ILocationDialog& LocationDialog::setOption( ::fwGui::dialog::ILocationDialog::Options option)
{
    return m_implementation->setOption(option);
}

//-----------------------------------------------------------------------------

void LocationDialog::setTitle(const std::string& title)
{
    m_implementation->setTitle(title);
}

//-----------------------------------------------------------------------------

const std::string& LocationDialog::getTitle()
{
    return m_implementation->getTitle();
}

//-----------------------------------------------------------------------------

void LocationDialog::setDefaultLocation(::fwData::location::ILocation::sptr loc)
{
    m_implementation->setDefaultLocation(loc);
}

//-----------------------------------------------------------------------------

const ::boost::filesystem::path LocationDialog::getDefaultLocation()
{
    return m_implementation->getDefaultLocation();
}

//-----------------------------------------------------------------------------

void LocationDialog::saveDefaultLocation(::fwData::location::ILocation::sptr loc)
{
    m_implementation->saveDefaultLocation(loc);
}

//-----------------------------------------------------------------------------

std::string LocationDialog::getCurrentSelection() const
{
    return m_implementation->getCurrentSelection();
}

//-----------------------------------------------------------------------------

} //namespace dialog
} //namespace fwGui
