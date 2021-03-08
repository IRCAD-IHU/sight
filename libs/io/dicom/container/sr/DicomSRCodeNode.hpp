/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
 * Copyright (C) 2012-2017 IHU Strasbourg
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

#include "io/dicom/config.hpp"
#include "io/dicom/container/DicomCodedAttribute.hpp"
#include "io/dicom/container/sr/DicomSRNode.hpp"

#include <gdcmDataSet.h>

namespace sight::io::dicom
{
namespace container
{
namespace sr
{

/**
 * @brief This class defines a CODE node in a SR document
 */
class IO_DICOM_CLASS_API DicomSRCodeNode : public io::dicom::container::sr::DicomSRNode
{
public:

    /// Constructor
    IO_DICOM_API DicomSRCodeNode(const DicomCodedAttribute& codedAttribute, const std::string& relationship,
                                 const DicomCodedAttribute& codedEntry);

    /// Destructor
    IO_DICOM_API virtual ~DicomSRCodeNode();

    /**
     * @brief Write the SR node in the dataset
     * @param[in] dataset Destination dataset
     */
    IO_DICOM_API virtual void write(::gdcm::DataSet& dataset) const;

    /// Get Coded Entry
    const DicomCodedAttribute& getCodedEntry() const
    {
        return m_codedEntry;
    }

    /// Set Coded Entry
    void setCodedEntry(const DicomCodedAttribute& codedEntry)
    {
        m_codedEntry = codedEntry;
    }

protected:

    /// Dump function
    virtual void print(std::ostream& os) const;

    /// Coded Entry
    DicomCodedAttribute m_codedEntry;

};

} //namespace sr
} //namespace container
} //namespace sight::io::dicom
