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

#include <QVBoxLayout>
#include <QPushButton>

#include <fwItkIO/itk.hpp>

#include <core/com/Signal.hxx>

#include <core/spyLog.hpp>

// Service associated data
#include <data/Image.hpp>

#include <data/tools/fieldHelper/Image.hpp>

#include <ui/qt/container/QtContainer.hpp>
#include <ui/base/dialog/MessageDialog.hpp>

// Services tools
#include <services/macros.hpp>

#include "basicRegistration/SImagesSubstract.hpp"

#include <itkSubtractImageFilter.h>

fwServicesRegisterMacro( ::sight::ui::base::editor::IEditor, ::basicRegistration::SImagesSubstract )

namespace basicRegistration
{

SImagesSubstract::SImagesSubstract() noexcept :
    sight::ui::base::editor::IEditor(),
    mpComputeButton(0)
{

}

SImagesSubstract::~SImagesSubstract() noexcept
{
}

//------------------------------------------------------------------------------

void SImagesSubstract::configuring()
{
    this->initialize();
}

//------------------------------------------------------------------------------

void SImagesSubstract::starting()
{
    this->create();
    auto qtContainer = sight::ui::qt::container::QtContainer::dynamicCast(
        this->getContainer() );
    QWidget* const container = qtContainer->getQtContainer();
    SLM_ASSERT("container not instanced", container);

    QVBoxLayout* layout = new QVBoxLayout(container);
    mpComputeButton = new QPushButton(tr("Compute"), container );
    QObject::connect(mpComputeButton, SIGNAL(clicked()), this, SLOT(OnCompute()));

    layout->addWidget(mpComputeButton, 0);
    container->setLayout( layout );
}

//------------------------------------------------------------------------------

void SImagesSubstract::stopping()
{
    this->destroy();

}

//------------------------------------------------------------------------------

void SImagesSubstract::updating()
{
    core::tools::Type REQUESTED_TYPE = core::tools::Type::create("int16");

    data::Image::csptr image1     = this->getInput< data::Image>("image1");
    data::Image::csptr image2     = this->getInput< data::Image>("image2");
    data::Image::sptr imageResult = this->getInOut< data::Image>("result");

    // Test if the both images have the same type and it is signed short.
    const bool isSameType = ( image1->getType() == image2->getType() && image1->getType() == REQUESTED_TYPE);

    if(isSameType)
    {
        // test if the both images have the same size.
        const bool isSameSize = (image1->getSize2() == image2->getSize2());
        if(isSameSize)
        {
            typedef itk::Image< std::int16_t, 3 > ImageType;

            ImageType::Pointer itkImage1 = ::fwItkIO::itkImageFactory< ImageType >( image1 );
            SLM_ASSERT("Unable to convert data::Image to itkImage", itkImage1);

            ImageType::Pointer itkImage2 = ::fwItkIO::itkImageFactory< ImageType >( image2 );
            SLM_ASSERT("Unable to convert data::Image to itkImage", itkImage2);

            ImageType::Pointer output;

            //Create filter
            typedef ::itk::SubtractImageFilter< ImageType, ImageType, ImageType > SubtractImageFilterType;
            SubtractImageFilterType::Pointer filter;
            filter = SubtractImageFilterType::New();
            assert(filter);

            filter->SetInput1( itkImage1 );
            filter->SetInput2( itkImage2 );
            filter->Update();
            output = filter->GetOutput();
            assert(output->GetSource());
            ::fwItkIO::dataImageFactory< ImageType >( output, imageResult, true );

            auto sig = imageResult->signal< data::Object::ModifiedSignalType >(data::Object::s_MODIFIED_SIG);
            sig->asyncEmit();
        }
        else
        {
            sight::ui::base::dialog::MessageDialog::show("Warning",
                                                         "Both images must have the same size.",
                                                         sight::ui::base::dialog::IMessageDialog::WARNING);
        }
    }
    else
    {
        sight::ui::base::dialog::MessageDialog::show("Warning",
                                                     "Both Images must have signed short as type.",
                                                     sight::ui::base::dialog::IMessageDialog::WARNING);
    }
}

//------------------------------------------------------------------------------

void SImagesSubstract::swapping()
{
    // Classic default approach to update service when oject change
    this->updating();
}

//------------------------------------------------------------------------------

void SImagesSubstract::OnCompute()
{
    this->updating();
}
// -----------------------------------------------------------------------------
} // namespace basicRegistration
