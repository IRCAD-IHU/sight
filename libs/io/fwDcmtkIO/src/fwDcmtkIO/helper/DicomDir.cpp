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

#include "fwDcmtkIO/helper/DicomDir.hpp"

#include <core/exceptionmacros.hpp>
#include <core/spyLog.hpp>

#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>

#include <dcmtk/config/osconfig.h>
#include <dcmtk/dcmdata/dcdeftag.h>
#include <dcmtk/dcmdata/dcdicdir.h>
#include <dcmtk/dcmnet/diutil.h>

#include <filesystem>
#include <regex>

namespace fwDcmtkIO
{
namespace helper
{

//------------------------------------------------------------------------------

bool DicomDir::readDicomDir(const std::filesystem::path& root, std::vector<std::string>& dicomFiles)
{
    SLM_ASSERT("You must specify a valid directory.", std::filesystem::is_directory(root));

    bool result = false;

    std::filesystem::path dicomDirPath = root / "dicomdir";
    bool dicomdirExists                = std::filesystem::exists(dicomDirPath) && !std::filesystem::is_directory(
        dicomDirPath);

    if(!dicomdirExists)
    {
        dicomDirPath   = root / "DICOMDIR";
        dicomdirExists = std::filesystem::exists(dicomDirPath) && !std::filesystem::is_directory(dicomDirPath);
    }

    if(dicomdirExists)
    {
        DcmDicomDir dicomDir(dicomDirPath.string().c_str());
        DcmDirectoryRecord* rootRecord    = &(dicomDir.getRootRecord());
        DcmDirectoryRecord* patientRecord = NULL;
        DcmDirectoryRecord* studyRecord   = NULL;
        DcmDirectoryRecord* seriesRecord  = NULL;
        DcmDirectoryRecord* fileRecord    = NULL;
        OFString tmpString;

        //Root level
        if (rootRecord != NULL)
        {
            result = true;

            try
            {
                //Patient level
                while (((patientRecord = rootRecord->nextSub(patientRecord)) != NULL))
                {
                    //Study level
                    while (((studyRecord = patientRecord->nextSub(studyRecord)) != NULL))
                    {
                        //Series level
                        while (((seriesRecord = studyRecord->nextSub(seriesRecord)) != NULL))
                        {
                            //Instance level
                            while ((fileRecord = seriesRecord->nextSub(fileRecord)) != NULL)
                            {
                                if (fileRecord->findAndGetOFStringArray(DCM_ReferencedFileID, tmpString).good())
                                {
                                    std::filesystem::path realPath = DicomDir::getRealFilename(root,
                                                                                               tmpString.c_str());
                                    dicomFiles.push_back(realPath.string());
                                }
                                else
                                {
                                    SLM_WARN("Dicom instance doesn't have a referenced file id.");
                                }
                            }
                        }
                    }
                }
            }
            catch(std::exception e)
            {
                result = false;
                dicomFiles.clear();
            }
        }
        else
        {
            SLM_WARN("Root record is null, unable to get information from dicomdir file.");
        }

    }
    else
    {
        SLM_WARN("Dicomdir file is missing");
    }

    return result;
}

// ----------------------------------------------------------------------------

std::string DicomDir::createRegex(std::string filename)
{
    std::string regex = "";
    for(std::string::iterator it = filename.begin(); it != filename.end(); ++it)
    {
        char upper = *it;
        char lower = static_cast<char>(upper + 32);

        std::stringstream ss;

        //For uppercase characters allow lowercase version
        if(upper >= 'A' && upper <= 'Z')
        {
            ss << "[" << upper << lower << "]";
            regex += ss.str();
        }
        //For other characters just copy them
        else
        {
            ss << upper;
            regex += ss.str();
        }
    }

    return regex;
}

// ----------------------------------------------------------------------------

std::filesystem::path DicomDir::getRealFilename(
    const std::filesystem::path& root, const std::string& filename)
{
    std::filesystem::path result = root;
    std::vector<std::string> elements;
    ::boost::split(elements, filename, ::boost::is_any_of("/\\"));

    //For every part of the path (going through the hierarchy)
    for(std::vector<std::string>::iterator it = elements.begin(); it != elements.end(); ++it)
    {
        std::string current = *it;
        std::regex regex(DicomDir::createRegex(current));

        std::filesystem::directory_iterator dirIt(result), dirItEnd;
        std::string child;
        bool matchFound = false;

        //Check if a file in the directory matches the regex of the dicom path
        for (; dirIt != dirItEnd; ++dirIt)
        {
            child = dirIt->path().filename().string();
            if (std::regex_match(child, regex))
            {
                FW_RAISE_IF("There is more than one file matching the uppercase filename stored "
                            "in the dicom instance: \""+filename+"\"", matchFound);
                matchFound = true;
                result    /= child;
            }

        }

        FW_RAISE_IF("There is no file matching the uppercase filename stored in the dicom instance: \""+filename+"\"",
                    !matchFound);
    }

    return result;
}

} //helper
} //fwDcmtkIO
