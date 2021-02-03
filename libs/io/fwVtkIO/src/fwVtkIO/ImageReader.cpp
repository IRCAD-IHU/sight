/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
 * Copyright (C) 2012-2020 IHU Strasbourg
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

#include "fwVtkIO/ImageReader.hpp"

#include "fwVtkIO/helper/vtkLambdaCommand.hpp"
#include "fwVtkIO/vtk.hpp"

#include <core/base.hpp>
#include <core/jobs/IJob.hpp>
#include <core/jobs/Observer.hpp>

#include <fwDataIO/reader/registry/macros.hpp>

#include <vtkGenericDataObjectReader.h>
#include <vtkImageData.h>
#include <vtkSmartPointer.h>

fwDataIOReaderRegisterMacro( ::fwVtkIO::ImageReader );

namespace fwVtkIO
{
//------------------------------------------------------------------------------

ImageReader::ImageReader(::fwDataIO::reader::IObjectReader::Key) :
    data::location::enableSingleFile< ::fwDataIO::reader::IObjectReader >(this),
    m_job(core::jobs::Observer::New("VTK Image reader"))
{
}

//------------------------------------------------------------------------------

ImageReader::~ImageReader()
{
}

//------------------------------------------------------------------------------

void ImageReader::read()
{
    using namespace fwVtkIO::helper;

    assert( !m_object.expired() );
    assert( m_object.lock() );

    data::Image::sptr pImage = getConcreteObject();

    vtkSmartPointer< vtkGenericDataObjectReader > reader = vtkSmartPointer< vtkGenericDataObjectReader >::New();
    reader->SetFileName(this->getFile().string().c_str());

    vtkSmartPointer<vtkLambdaCommand> progressCallback;
    progressCallback = vtkSmartPointer<vtkLambdaCommand>::New();
    progressCallback->SetCallback(
        [this](vtkObject* caller, long unsigned int, void* )
        {
            auto filter = static_cast<vtkGenericDataObjectReader*>(caller);
            m_job->doneWork(static_cast<std::uint64_t>(filter->GetProgress() * 100.));
        }
        );
    reader->AddObserver(vtkCommand::ProgressEvent, progressCallback);

    m_job->addSimpleCancelHook( [&]()
        {
            reader->AbortExecuteOn();
        } );

    reader->Update();
    reader->UpdateInformation();
    reader->PropagateUpdateExtent();

    vtkDataObject* obj = reader->GetOutput();
    vtkImageData* img  = vtkImageData::SafeDownCast(obj);

    m_job->finish();

    FW_RAISE_IF("ImageReader cannot read VTK image file :"<<this->getFile().string(), !img);
    try
    {
        ::fwVtkIO::fromVTKImage( img, pImage);
    }
    catch( std::exception& e)
    {
        FW_RAISE("VTKImage to data::Image failed "<<e.what());
    }
}

//------------------------------------------------------------------------------

std::string ImageReader::extension()
{
    return ".vtk";
}

//------------------------------------------------------------------------------

::core::jobs::IJob::sptr ImageReader::getJob() const
{
    return m_job;
}

//------------------------------------------------------------------------------

} // namespace fwVtkIO
