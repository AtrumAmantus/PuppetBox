#include <streambuf>

#include <zip/zip.h>

#include "FileUtils.h"
#include "Logger.h"

namespace PB
{
    namespace FileUtils
    {
        namespace
        {
            /**
            * \brief Custom buffer for building stream objects.
            */
            struct membuf : std::streambuf
            {
                membuf(char const* base, std::size_t size)
                {
                    char* p(const_cast<char*>(base));
                    this->setg(p, p, p + size);
                }

            protected:
                pos_type seekoff(off_type off, std::ios_base::seekdir dir, std::ios_base::openmode which) override
                {
                    if (dir == std::ios_base::cur)
                        gbump(static_cast<std::int32_t>(off));
                    else if (dir == std::ios_base::end)
                        setg(eback(), egptr() + off, egptr());
                    else if (dir == std::ios_base::beg)
                        setg(eback(), eback() + off, egptr());
                    return gptr() - eback();
                }
            };

            /**
            * \brief Custom stream type for storing zip data.
            */
            struct imemstream : virtual membuf, std::istream
            {
                imemstream(char const* base, std::size_t size)
                        : membuf(base, size), std::istream(static_cast<std::streambuf*>(this))
                {
                }
            };
        }

        bool getFileListFromArchive(const std::string& archivePath, std::unordered_set<std::string>* files)
        {
            struct zip_t* zip = zip_open(archivePath.c_str(), 0, 'r');

            if (zip != nullptr)
            {
                std::int32_t n = zip_entries_total(zip);

                if (n > -1)
                {
                    if (files == nullptr)
                    {
                        files = new std::unordered_set<std::string>{};
                    }

                    for (std::int32_t i = 0; i < n; ++i)
                    {
                        zip_entry_openbyindex(zip, i);
                        {
                            files->insert(zip_entry_name(zip));
                        }
                    }

                    return true;
                }
                else
                {
                    LOGGER_ERROR("Error reading files from archive '" + archivePath + "'");
                }
            }
            else
            {
                LOGGER_ERROR("Error reading archive '" + archivePath + "'");
            }

            return false;
        }

        bool getContentsFromArchivedFile(
                const std::string& archivePath,
                const std::string& filePath,
                std::int8_t** buffer,
                std::size_t* bufferSize)
        {
            bool success = false;

            // Open zip archive
            struct zip_t* zip = zip_open(archivePath.c_str(), 0, 'r');
            {
                if (zip != nullptr)
                {
                    // Open compressed file within archive
                    zip_entry_open(zip, filePath.c_str());
                    {
                        if (zip != nullptr)
                        {
                            // Read file contents to buffer
                            if (zip_entry_read(zip, (void**) buffer, bufferSize) >= 0)
                            {
                                success = true;
                            }
                        }
                        else
                        {
                            LOGGER_ERROR("Error reading file '" + filePath + "' from archive '" + archivePath + "'");
                        }
                    }
                }
                else
                {
                    LOGGER_ERROR("Error reading archive '" + archivePath + "'");
                }
            }

            zip_close(zip);

            return success;
        }

        bool getStreamFromArchivedFile(
                const std::string& archivePath,
                const std::string& filePath,
                std::istream** stream)
        {
            bool success = true;
            std::int8_t* buffer = nullptr;
            std::size_t bufferSize = 0;

            if (getContentsFromArchivedFile(archivePath, filePath, &buffer, &bufferSize))
            {
                *stream = new imemstream((char*) buffer, bufferSize);
            }
            else
            {
                success = false;
                LOGGER_ERROR("Failed to read contents of '" + archivePath + "/" + filePath + "'");
            }

            return success;
        }
    }
}