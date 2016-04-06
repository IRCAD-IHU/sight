/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "echoEdSimu/SImageManager.hpp"

#include <fwCom/Signal.hxx>
#include <fwCom/Slots.hxx>

#include <fwData/Composite.hpp>
#include <fwData/Image.hpp>

#include <fwServices/Base.hpp>

#include <fwVtkIO/ImageReader.hpp>
#include <fwVtkIO/vtk.hpp>

#include <boost/filesystem.hpp>

namespace echoEdSimu
{

fwServicesRegisterMacro( ::arServices::ISimulator, ::echoEdSimu::SImageManager, ::fwData::Composite );

const std::string s_ctPath   = "./Data/CTImages";
const std::string s_echoPath = "./Data/EchoImages";

const ::fwCom::Slots::SlotKeyType SImageManager::s_UPDATE_CRANIOCAUDAL_SLOT = "updateCranioCaudal";
const ::fwCom::Slots::SlotKeyType SImageManager::s_UPDATE_RADIAL_SLOT       = "updateRadial";
const ::fwCom::Slots::SlotKeyType SImageManager::s_UPDATE_BOTH_SLOT         = "updateBoth";

const std::string s_ctImageKey   = "echoCTImage";
const std::string s_echoImageKey = "echoImage";

const int s_minI = 95;
const int s_maxI = 194;
const int s_minJ = 160;
const int s_maxJ = 259;

//-----------------------------------------------------------------------------

SImageManager::SImageManager() throw() :
    m_cranioCaudalIndex(s_minI),
    m_radialIndex(s_minJ)
{
    newSlot( s_UPDATE_CRANIOCAUDAL_SLOT, &SImageManager::updateCranioCaudalImage, this );
    newSlot( s_UPDATE_RADIAL_SLOT, &SImageManager::updateRadialImage, this );
    newSlot( s_UPDATE_BOTH_SLOT, &SImageManager::updateBoth, this );
}

//------------------------------------------------------------------------------

SImageManager::~SImageManager() throw()
{
}

//------------------------------------------------------------------------------

void SImageManager::configuring() throw(fwTools::Failed)
{
}

//------------------------------------------------------------------------------

void SImageManager::starting() throw(::fwTools::Failed)
{
    ::fwData::Composite::sptr composite = this->getObject< ::fwData::Composite >();

    m_ctImage   = composite->at< ::fwData::Image >(s_ctImageKey);
    m_echoImage = composite->at< ::fwData::Image >(s_echoImageKey);
}

//------------------------------------------------------------------------------

void SImageManager::stopping() throw(::fwTools::Failed)
{
}

//------------------------------------------------------------------------------

void SImageManager::updateCranioCaudalImage(int i)
{
    updateBoth(i, m_radialIndex);
}

//------------------------------------------------------------------------------

void SImageManager::updateRadialImage(int j)
{
    updateBoth(m_cranioCaudalIndex, j);
}

//------------------------------------------------------------------------------

void SImageManager::load(::boost::filesystem::path dir, ImagesVecType & images)
{
    unsigned int dI = s_maxI - s_minI +1;
    unsigned int dJ = s_maxJ - s_minJ +1;

    images.resize(dI);

    for (unsigned int i = 0; i<dI; ++i)
    {
        images[i].resize(dJ);
        const unsigned int idI = i + s_minI;
        for (unsigned int j = 0; j<dJ; ++j)
        {
            const unsigned int idJ = j + s_minJ;
            std::stringstream str;
            str << "image_" << idI << "_" << idJ << ".vtk";
            ::boost::filesystem::path path(dir / str.str());

            if (::boost::filesystem::exists(path))
            {
                ::fwData::Image::sptr image         = ::fwData::Image::New();
                ::fwVtkIO::ImageReader::sptr reader = ::fwVtkIO::ImageReader::New();
                reader->setFile(path);
                reader->setObject(image);
                reader->read();

                images[i][j] = image;
            }
        }
    }
}

//------------------------------------------------------------------------------

void SImageManager::updating() throw(::fwTools::Failed)
{
}

//------------------------------------------------------------------------------

void SImageManager::swapping() throw(::fwTools::Failed)
{
}

//------------------------------------------------------------------------------

void SImageManager::updateBoth(int i, int j)
{
    if (i < s_minI || i > s_maxI || j < s_minJ || j > s_maxJ)
    {
        OSLM_ERROR("Image [" << i - s_minI << "][" << j - s_minJ <<"] index out of bound");
        return;
    }
    m_cranioCaudalIndex = i;
    m_radialIndex       = j;

    std::stringstream str;
    str << "image_" << m_cranioCaudalIndex << "_" << m_radialIndex << ".vtk";
    ::boost::filesystem::path path1(s_ctPath + "/" + str.str());
    ::boost::filesystem::path path2(s_echoPath + "/" + str.str());

    ::fwVtkIO::ImageReader::sptr reader1 = ::fwVtkIO::ImageReader::New();
    reader1->setFile(path1);
    reader1->setObject(m_ctImage);
    reader1->read();

    ::fwVtkIO::ImageReader::sptr reader2 = ::fwVtkIO::ImageReader::New();
    reader2->setFile(path2);
    reader2->setObject(m_echoImage);
    reader2->read();

    auto sig1 = m_ctImage->signal< ::fwData::Object::ModifiedSignalType >(::fwData::Object::s_MODIFIED_SIG);
    sig1->asyncEmit();

    auto sig2 = m_echoImage->signal< ::fwData::Object::ModifiedSignalType >(::fwData::Object::s_MODIFIED_SIG);
    sig2->asyncEmit();
}

//------------------------------------------------------------------------------

} // namespace echoEdSimu
