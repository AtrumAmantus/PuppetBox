#pragma once

#include <cstdint>
#include <istream>
#include <string>
#include <unordered_set>

namespace PB
{
    /**
    * \brief Utility namespace for common file oriented functions.
    */
    namespace FileUtils
    {
        /**
        * \brief Gets list of contents of the target archive.
        *
        * \param archivePath	The path to the archive to read from.
        * \param files			A pointer to the unordered_set the file list will be saved to.
        *
        * \return True if the list was read successfully, False otherwise.
        */
        bool getFileListFromArchive(const std::string& archivePath, std::unordered_set<std::string>* files);

        /**
        * \brief Gets the raw bytes of a file inside an archive.
        *
        * \param archivePath	The path to the archive to read from.
        * \param filePath		The path to the inner file within the archive relative to it's root directory.
        * \param buffer			Pointer to the byte array that the file contents will be saved to.
        * \param bufferSize		Size of the created byte array containing the file's contents.
        *
        * \return True if the contents were read successfully, False otherwise.
        */
        bool getContentsFromArchivedFile(
                const std::string& archivePath,
                const std::string& filePath,
                std::int8_t** buffer,
                std::size_t* bufferSize);

        /**
        * \brief Gets a stream object for the contents of a given file in the given archive.
        *
        * \param archivePath	Path to the archive to read from.
        * \param filePath		Relative path to the inner file of the archive, relative to it's root directory.
        * \param stream			Pointer to the stream that will be created from the contents of the file in the archive.
        *
        * \return True if the file was read and stream created successfully, False otherwise.
        */
        bool getStreamFromArchivedFile(
                const std::string& archivePath,
                const std::string& filePath,
                std::istream** stream);
    }
}