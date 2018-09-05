/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWRENDEROGRE_PCH_HPP__
#define __FWRENDEROGRE_PCH_HPP__

// We need a specific pch because we need OpenMP support
// Compile flags will be automatically forwarded so that we get a pch with OpenMP inside

// Cause an internal compiler error on windows...
#ifndef _WIN32
#include <fwCom/HasSignals.hpp>
#include <fwCom/Signal.hpp>
#include <fwCom/Signal.hxx>
#include <fwCom/HasSlots.hpp>
#include <fwCom/Slots.hpp>
#include <fwCom/Slots.hxx>
#endif // _WIN32

#include <fwServices/IService.hpp>
#include <fwServices/macros.hpp>

#include <boost/filesystem/path.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/tokenizer.hpp>

#include <Ogre.h>

#endif // __FWRENDEROGRE_PCH_HPP__
