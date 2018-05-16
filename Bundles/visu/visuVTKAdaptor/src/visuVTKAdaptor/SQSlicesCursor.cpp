/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "visuVTKAdaptor/SQSlicesCursor.hpp"

#include <fwCom/Slot.hpp>
#include <fwCom/Slot.hxx>
#include <fwCom/Slots.hpp>
#include <fwCom/Slots.hxx>

#include <fwCore/base.hpp>

#include <fwData/Float.hpp>
#include <fwData/Image.hpp>
#include <fwData/String.hpp>

#include <fwDataTools/fieldHelper/Image.hpp>
#include <fwDataTools/fieldHelper/MedicalImageHelpers.hpp>

#include <fwServices/macros.hpp>
#include <fwServices/QtQmlRegistar.hxx>

#include <boost/assign/list_of.hpp>

#include <vtkActor.h>
#include <vtkCellArray.h>
#include <vtkCellData.h>
#include <vtkLine.h> // CELL
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderer.h>
#include <vtkTransform.h>

namespace visuVTKAdaptor
{

const ::fwCom::Slots::SlotKeyType SQSlicesCursor::s_UPDATE_SLICE_INDEX_SLOT = "updateSliceIndex";
const ::fwCom::Slots::SlotKeyType SQSlicesCursor::s_UPDATE_SLICE_TYPE_SLOT  = "updateSliceType";
const ::fwCom::Slots::SlotKeyType SQSlicesCursor::s_UPDATE_IMAGE_SLOT       = "updateImage";

const ::fwCom::Slots::SlotKeyType SQSlicesCursor::s_SHOW_FULL_CROSS_SLOT   = "showFullCross";
const ::fwCom::Slots::SlotKeyType SQSlicesCursor::s_SHOW_NORMAL_CROSS_SLOT = "showNormalCross";
const ::fwCom::Slots::SlotKeyType SQSlicesCursor::s_SET_CROSS_SCALE_SLOT   = "setCrossScale";

static const ::fwServices::IService::KeyType s_IMAGE_INOUT = "image";

static const ::fwServices::QtQmlRegistar<SQSlicesCursor>    registar("::visuVTKAdaptor::SSlicesCursor");

//-----------------------------------------------------------------------------

SQSlicesCursor::SQSlicesCursor()  noexcept :
    m_cursorPolyData( vtkPolyData::New() ),
    m_cursorMapper( vtkPolyDataMapper::New() ),
    m_cursorActor( vtkActor::New() ),
    m_scale(0.5f),
    m_isSelected(false)
{
    newSlot(s_UPDATE_SLICE_INDEX_SLOT, &SQSlicesCursor::updateSliceIndex, this);
    newSlot(s_UPDATE_SLICE_TYPE_SLOT, &SQSlicesCursor::updateSliceType, this);
    newSlot(s_UPDATE_IMAGE_SLOT, &SQSlicesCursor::updateImage, this);
    newSlot(s_SHOW_FULL_CROSS_SLOT, &SQSlicesCursor::showFullCross, this);
    newSlot(s_SHOW_NORMAL_CROSS_SLOT, &SQSlicesCursor::showNormalCross, this);
    newSlot(s_SET_CROSS_SCALE_SLOT, &SQSlicesCursor::setCrossScale, this);
}

//-----------------------------------------------------------------------------

SQSlicesCursor::~SQSlicesCursor()  noexcept
{
    m_cursorActor->Delete();
    m_cursorActor = NULL;
    m_cursorMapper->Delete();
    m_cursorMapper = NULL;
    m_cursorPolyData->Delete();
}

//-----------------------------------------------------------------------------

void SQSlicesCursor::setCrossScale(double scale)
{
    m_scale = static_cast<float>(scale);
    this->updating();
}

//-----------------------------------------------------------------------------

void SQSlicesCursor::configuring()
{
    this->configureParams();

    m_scale = get<float>(m_configMap, "scale", 0.5f);
}

//-----------------------------------------------------------------------------

void SQSlicesCursor::starting()
{
    this->initialize();

    ::fwData::Image::sptr image = this->getInOut< ::fwData::Image >(s_IMAGE_INOUT);
    SLM_ASSERT("Missing image", image);

    this->buildPolyData();
    this->buildColorAttribute();
    this->updateImageInfos(image);
    this->updateColors();
    m_cursorMapper->SetInputData( m_cursorPolyData );
    m_cursorActor->SetMapper(m_cursorMapper);
    m_cursorActor->GetProperty()->SetOpacity(0.9);

    if(!this->getTransformId().empty())
    {
        m_cursorActor->SetUserTransform(this->getTransform());
    }
    this->addToRenderer(m_cursorActor);
    this->setVtkPipelineModified();
    this->requestRender();
}

//-----------------------------------------------------------------------------

void SQSlicesCursor::stopping()
{
    m_isSelected = false;
    this->removeAllPropFromRenderer();
    this->requestRender();
}

//////////////////////////////
// SHAPE
//    B---------BC----------C
//    |         |           |
//    |         |           |
//    |        BCM          |
//    |                     |
//    |                     |
//   AB----ABM      CDM-----CD
//    |                     |
//    |        ADM          |
//    |         |           |
//    |         |           |
//    A---------AD----------D
//
// no for id point A,B,C,D
// id point AB, BC, CD, AD  = 0,1,...,3
// id point ABM, BCM, CDM, ADM = 4,..,7

void SQSlicesCursor::buildPolyData()
{
    int nbPoints      = 8;
    vtkPoints* points = vtkPoints::New(VTK_DOUBLE);
    points->SetNumberOfPoints(nbPoints);
    int i;
    for (i = 0; i < nbPoints; i++)
    {
        points->SetPoint(i, 0.0, 0.0, 0.0);
    }

    vtkCellArray* cells = vtkCellArray::New();
    cells->Allocate(cells->EstimateSize(nbPoints, 2));

    for ( int line = 0; line < 4; ++line)
    {
        vtkLine* lineCell = vtkLine::New();
        lineCell->GetPointIds()->SetId(0, line );
        lineCell->GetPointIds()->SetId(1, line+ 4 );
        cells->InsertNextCell(lineCell);
        lineCell->Delete();
    }

    m_cursorPolyData->SetPoints(points);
    points->Delete();
    m_cursorPolyData->SetLines(cells);
    cells->Delete();
    this->setVtkPipelineModified();
}

//-----------------------------------------------------------------------------

void SQSlicesCursor::barycenter( double ptA[3], double ptB[3], float scale, double result[3] )
{
    for (int i = 0; i < 3; ++i )
    {
        result[i] = scale*ptA[i] + (1-scale)*ptB[i];
    }
}

//-----------------------------------------------------------------------------

void SQSlicesCursor::computeCrossPoints( double _ptA[3], double _ptB[3], double _ptP[3], double _scale,
                                        double _ptAprime[3], double _ptBprime[3] )
{
    double ptPBprime[3];
    double norm2PBprime = 0.0;
    double norm2BP      = 0;
    double norm2AP      = 0;

    // Compute AB x scale / 2 = |A'P| x AB = |PB'| x AB
    for (int i = 0; i < 3; ++i )
    {
        ptPBprime[i]  = ( _ptB[i] - _ptA[i] ) * (1-_scale)/2.0;
        norm2PBprime += ptPBprime[i]*ptPBprime[i];
        norm2AP      += (_ptP[i] - _ptA[i])*(_ptP[i] - _ptA[i]);
        norm2BP      += (_ptP[i] - _ptB[i])*(_ptP[i] - _ptB[i]);
    }

    // Compute  APrime = P + PA' = P - |A'P| x AB
    //          BPrime = P + pB' = P + |PB'| x AB
    if ( norm2PBprime > norm2BP )
    {
        for (int i = 0; i < 3; ++i )
        {
            _ptAprime[i] = _ptP[i] - ptPBprime[i];
            _ptBprime[i] = _ptB[i];
        }
    }
    else if ( norm2PBprime > norm2AP )
    {
        for (int i = 0; i < 3; ++i )
        {
            _ptAprime[i] = _ptA[i];
            _ptBprime[i] = _ptP[i] + ptPBprime[i];
        }
    }
    else
    {
        for (int i = 0; i < 3; ++i )
        {
            _ptAprime[i] = _ptP[i] - ptPBprime[i];
            _ptBprime[i] = _ptP[i] + ptPBprime[i];
        }
    }
}

//-----------------------------------------------------------------------------

void SQSlicesCursor::buildColorAttribute()
{
    unsigned char red[3]   = {255, 0, 0};
    unsigned char green[3] = {0, 255, 0};
    unsigned char blue[3]  = {0, 0, 255};

    typedef  unsigned char* RGBColor;
    typedef std::map< std::string, std::pair< RGBColor, RGBColor> >  DicoType;
    DicoType dict;
    dict["colorXAxis"] = std::make_pair(green, red);
    dict["colorYAxis"] = std::make_pair(red, blue);
    dict["colorZAxis"] = std::make_pair(blue, green );

    for ( DicoType::iterator i = dict.begin(); i != dict.end(); ++i )
    {
        vtkUnsignedCharArray* colors = vtkUnsignedCharArray::New();
        colors->SetNumberOfComponents(3);
        colors->SetName(  i->first.c_str() );

        // Since VTK 7.1, InsertNextTupleValue is deprecated in favor of InsertNextTypedTuple.
 #if (VTK_MAJOR_VERSION == 7 && VTK_MINOR_VERSION >= 1) ||  VTK_MAJOR_VERSION > 7
        colors->InsertNextTypedTuple(  i->second.first  );
        colors->InsertNextTypedTuple(  i->second.second );
        colors->InsertNextTypedTuple(  i->second.first  );
        colors->InsertNextTypedTuple(  i->second.second );
#else
        colors->InsertNextTupleValue(  i->second.first  );
        colors->InsertNextTupleValue(  i->second.second );
        colors->InsertNextTupleValue(  i->second.first  );
        colors->InsertNextTupleValue(  i->second.second );
#endif
        m_cursorPolyData->GetCellData()->AddArray(colors);
        colors->Delete();
    }
    m_cursorMapper->SetScalarModeToUseCellFieldData();
    this->setVtkPipelineModified();
}

//-----------------------------------------------------------------------------

void SQSlicesCursor::updateColors()
{
    switch (m_orientation )
    {
        case 0: m_cursorMapper->SelectColorArray("colorXAxis"); break;
        case 1: m_cursorMapper->SelectColorArray("colorYAxis"); break;
        case 2: m_cursorMapper->SelectColorArray("colorZAxis"); break;

    }
    this->setVtkPipelineModified();
}

//-----------------------------------------------------------------------------

void SQSlicesCursor::updating()
{
    ::fwData::Image::sptr image = this->getInOut< ::fwData::Image >(s_IMAGE_INOUT);
    SLM_ASSERT("Missing image", image);

    bool imageIsValid = ::fwDataTools::fieldHelper::MedicalImageHelpers::checkImageValidity( image );

    if ( imageIsValid)
    {
        this->updateImageSliceIndex(image);
        this->updateColors();
        this->requestRender();
    }
}

//-----------------------------------------------------------------------------

void SQSlicesCursor::updateImageSliceIndex( ::fwData::Image::sptr image )
{
    float scale = m_isSelected ? 1.0 : m_scale;
    if (scale <= 0)
    {
        m_cursorActor->VisibilityOff();
    }
    else
    {
        m_cursorActor->VisibilityOn();

        int pos[3];

        pos[2] = m_axialIndex->value();
        pos[1] = m_frontalIndex->value();
        pos[0] = m_sagittalIndex->value();

        const ::fwData::Image::SpacingType spacing = image->getSpacing();
        const ::fwData::Image::OriginType origin   = image->getOrigin();
        const ::fwData::Image::SizeType size       = image->getSize();
        double sliceWorld[3];
        for (unsigned int dim = 0; dim < 3; ++dim )
        {
            sliceWorld[dim] = pos[dim] * spacing[dim] + origin.at(dim);
        }

        double cursorPoints[8][3]; // point AB,BC,CD,AD,ABM,BCM,CDM,ADM
        for (unsigned int p = 0; p < 2; ++p )
        {
            for (unsigned int dim = 0; dim < 3; ++dim )
            {
                if ( (dim + p + 1)%3 == m_orientation )
                {
                    cursorPoints[p][dim]   = origin.at(dim);
                    cursorPoints[p+2][dim] = (size[dim]-1) * spacing[dim] + origin.at(dim);
                }
                else
                {
                    cursorPoints[p][dim]   = sliceWorld[dim];
                    cursorPoints[p+2][dim] = sliceWorld[dim];
                }
            }
        }

        // Compute ABM & CDM
        computeCrossPoints( cursorPoints[0], cursorPoints[2], sliceWorld, scale, cursorPoints[4], cursorPoints[6] );
        // Compute BCM & ADM
        computeCrossPoints( cursorPoints[1], cursorPoints[3], sliceWorld, scale, cursorPoints[5], cursorPoints[7] );

        vtkPoints* points = m_cursorPolyData->GetPoints();

        for ( int i = 0; i < 8; ++i)
        {
            points->SetPoint(i, cursorPoints[i]);
        }
    }
    m_cursorPolyData->Modified();
    this->setVtkPipelineModified();
}

//-----------------------------------------------------------------------------

void SQSlicesCursor::updateSliceIndex(int axial, int frontal, int sagittal)
{
    m_axialIndex->value()    = axial;
    m_frontalIndex->value()  = frontal;
    m_sagittalIndex->value() = sagittal;

    ::fwData::Image::sptr image = this->getInOut< ::fwData::Image >(s_IMAGE_INOUT);

    this->updateImageSliceIndex(image);
    this->updating();
}
//-----------------------------------------------------------------------------

void SQSlicesCursor::showFullCross()
{
    m_isSelected = true;
}

//-----------------------------------------------------------------------------

void SQSlicesCursor::showNormalCross()
{
    m_isSelected = false;
}

//-----------------------------------------------------------------------------

void SQSlicesCursor::updateSliceType(int from, int to)
{
    if( to == static_cast<int>(m_orientation) )
    {
        setOrientation( static_cast< Orientation >( from ));
    }
    else if( from == static_cast<int>(m_orientation) )
    {
        setOrientation( static_cast< Orientation >( to ));
    }
    this->updating();
}

//-----------------------------------------------------------------------------

void SQSlicesCursor::updateImage()
{
    ::fwData::Image::sptr image = this->getInOut< ::fwData::Image >(s_IMAGE_INOUT);
    SLM_ASSERT("Missing image", image);

    this->updateImageInfos(image);
    this->updating();
}

//-----------------------------------------------------------------------------

} //namespace visuVTKAdaptor
