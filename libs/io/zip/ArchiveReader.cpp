/************************************************************************
 *
 * Copyright (C) 2021 IRCAD France
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

#include "ArchiveReader.hpp"

#include "exception/Read.hpp"

#include "minizip/unzip.h"

#include <core/exceptionmacros.hpp>

#include <boost/iostreams/stream.hpp>

namespace sight::io::zip
{

/// Implementation class
class ArchiveReaderImpl final : public ArchiveReader
{
public:
    SIGHT_DECLARE_CLASS(ArchiveReaderImpl, ArchiveReader)

    /// Delete default constructors and assignment operators, as we don't want to allow resources duplication
    ArchiveReaderImpl()                                    = delete;
    ArchiveReaderImpl(const ArchiveReaderImpl&)            = delete;
    ArchiveReaderImpl(ArchiveReaderImpl&&)                 = delete;
    ArchiveReaderImpl& operator=(const ArchiveReaderImpl&) = delete;
    ArchiveReaderImpl& operator=(ArchiveReaderImpl&&)      = delete;

    /// Constructor. It open the archive and create all resources needed to access it.
    /// @param archive_path path of the archive file. The file will be kept opened as long as the instance leave.
    ArchiveReaderImpl(const std::filesystem::path& archive_path) :
        m_archive_path(archive_path)
    {
        SIGHT_THROW_EXCEPTION_IF(
            exception::Read("Archive '" + archive_path.string() + "' doesn't exist."),
            !std::filesystem::exists(archive_path));

        m_unz_file = unzOpen(archive_path.string().c_str());

        SIGHT_THROW_EXCEPTION_IF(
            exception::Read("Archive '" + archive_path.string() + "' cannot be opened."),
            m_unz_file == NULL);
    }

    /// Destructor
    ~ArchiveReaderImpl() override
    {
        if(m_unz_file != nullptr)
        {
            const int result = unzClose(m_unz_file);

            SIGHT_THROW_EXCEPTION_IF(
                exception::Read("Archive '" + m_archive_path.string() + "' cannot be closed."),
                result != UNZ_OK);

            m_unz_file = nullptr;
        }

        // Remove cached elements
        cache_clean(m_archive_path);
    }

    /// Returns an std::istream to read an archived file
    /// @param file_path path of an archived file.
    /// @param password the password needed to decrypt the file.
    std::unique_ptr<std::istream> openFile(const std::filesystem::path& file_path,
                                           const core::crypto::secure_string& password = "") override
    {
        // Boost iostreams Source. It allows easy istream implementation
        class ZipSource
        {
        public:

            typedef char char_type;
            typedef boost::iostreams::source_tag category;

            /// Delete default constructor
            ZipSource() = delete;

            // Boost iostreams Source do not like constructor with more than one parameter
            // So, we use a sub struct to hold parameters and class attributes.
            struct Parameters
            {
                const ArchiveReaderImpl::sptr m_archive;
                const std::filesystem::path m_file_path;
                const core::crypto::secure_string m_password;

                Parameters(
                    const ArchiveReaderImpl::sptr& archive,
                    const std::filesystem::path& file_path,
                    const core::crypto::secure_string& password ) :
                    m_archive(archive),
                    m_file_path(file_path),
                    m_password(password)
                {
                }
            };

            ZipSource(const std::shared_ptr<const Parameters>& parameters) :
                m_attributes(parameters),
                m_file_keeper(std::make_shared<const ZipFileKeeper>(m_attributes)),
                m_lock_guard(std::make_shared< std::lock_guard<std::mutex> >(m_attributes->m_archive->m_operation_mutex))
            {
            }

            //------------------------------------------------------------------------------

            std::streamsize read(char* buffer, std::streamsize size)
            {
                const int read = unzReadCurrentFile(
                    m_attributes->m_archive->m_unz_file,
                    buffer,
                    static_cast< std::uint32_t >(size));

                SIGHT_THROW_EXCEPTION_IF(
                    exception::Read(
                        "Cannot read file '"
                        + m_attributes->m_file_path.string()
                        + "' in archive '"
                        + m_attributes->m_archive->m_archive_path.string()
                        + "'."),
                    read < 0);

                return read;
            }
        private:

            // Used to create and destroy minizip file handle
            struct ZipFileKeeper
            {
                // Store constructor parameters
                const std::shared_ptr<const Parameters> m_attributes;

                ZipFileKeeper(const std::shared_ptr<const Parameters>& parameters) :
                    m_attributes(parameters)
                {
                    // Locate the file
                    int result = unzLocateFile(
                        m_attributes->m_archive->m_unz_file,
                        m_attributes->m_file_path.string().c_str(),
                        nullptr);

                    SIGHT_THROW_EXCEPTION_IF(
                        exception::Read(
                            "File '"
                            + m_attributes->m_file_path.string()
                            + "' doesn't exist in archive '"
                            + m_attributes->m_archive->m_archive_path.string()
                            + "'"
                            ),
                        result != UNZ_OK);

                    // Open the file
                    if(m_attributes->m_password.empty())
                    {
                        result = unzOpenCurrentFile(m_attributes->m_archive->m_unz_file);
                    }
                    else
                    {
                        result = unzOpenCurrentFilePassword(
                            m_attributes->m_archive->m_unz_file,
                            m_attributes->m_password.c_str());
                    }

                    SIGHT_THROW_EXCEPTION_IF(
                        exception::Read(
                            "Cannot open file '"
                            + m_attributes->m_file_path.string()
                            + "' in archive '"
                            + m_attributes->m_archive->m_archive_path.string()
                            + "'."),
                        result != MZ_OK);
                }

                ~ZipFileKeeper()
                {
                    // Do not forget to clean file handle in zip
                    const int result = unzCloseCurrentFile(m_attributes->m_archive->m_unz_file);

                    SIGHT_THROW_EXCEPTION_IF(
                        exception::Read(
                            "Cannot close file '"
                            + m_attributes->m_file_path.string()
                            + "' in archive '"
                            + m_attributes->m_archive->m_archive_path.string()
                            + "'."),
                        result != MZ_OK);
                }
            };

            // Store constructor parameters
            const std::shared_ptr<const Parameters> m_attributes;

            // Used to create and destroy minizip file handle
            const std::shared_ptr<const ZipFileKeeper> m_file_keeper;

            // Locks the archive mutex so nobody could open another file.
            const std::shared_ptr< const std::lock_guard<std::mutex> > m_lock_guard;

        };

        auto parameters = std::make_shared<ZipSource::Parameters>(
            ArchiveReaderImpl::dynamicCast(ArchiveReaderImpl::shared(m_archive_path)),
            file_path,
            password);

        // Creating a ZipSource also lock the archive mutex.
        // Due to its design, minizip only allows one archive with the same path with one file operation.
        return std::make_unique< boost::iostreams::stream<ZipSource> >(parameters);
    }

private:

    /// Internal class
    friend class ZipSource;

    /// Path of the archive file
    std::filesystem::path m_archive_path;

    /// Archive context handle
    void* m_unz_file {nullptr};

    /// Mutex to prevent multiple read/write operation on same file
    std::mutex m_operation_mutex;
};

//------------------------------------------------------------------------------

ArchiveReader::sptr ArchiveReader::shared(const std::filesystem::path& archive_path)
{
    // First try the cache
    ArchiveReader::sptr new_archive;
    Archive::sptr cached_archive = cache_find(archive_path);

    if(cached_archive)
    {
        // Try to cast in the correct type
        new_archive = ArchiveReader::dynamicCast(cached_archive);

        SIGHT_THROW_EXCEPTION_IF(
            exception::Read("Archive '" + archive_path.string() + "' is already opened in WRITE mode."),
            !new_archive);
    }

    // If null create a new one
    if(!new_archive)
    {
        new_archive = std::make_shared<ArchiveReaderImpl>(archive_path);

        // Store the created archive for later use
        cache_store(archive_path, new_archive);
    }

    return new_archive;
}

} // namespace sight::io::zip