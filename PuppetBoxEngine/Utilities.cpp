#include "pch.h"

#include <ctype.h>
#include <istream>
#include <streambuf>

#include <zip/zip.h>

#include "Logger.h"
#include "Utilities.h"

namespace PB
{
	/**
	* \brief These hidden functions provide support for all whitespace characters, dependent on system definition of std::isspace().
	*/
	namespace
	{
		void ltrimWhitespace(std::string* str)
		{
			while (!str->empty() && std::isspace(str->at(strlen((*str).c_str()) - 1)))
			{
				str->erase((strlen((*str).c_str()) - 1), 1);
			}
		}
		std::string ltrimWhitespace(std::string str)
		{
			ltrimWhitespace(&str);

			return str;
		}
		void rtrimWhitespace(std::string* str)
		{
			while (!str->empty() && std::isspace(str->at(strlen((*str).c_str()) - 1)))
			{
				str->erase((strlen((*str).c_str()) - 1), 1);
			}
		}
		std::string rtrimWhitespace(std::string str)
		{
			rtrimWhitespace(&str);

			return str;
		}
		void trimWhitespace(std::string* str)
		{
			ltrimWhitespace(str);
			rtrimWhitespace(str);
		}
		std::string trimWhitespace(std::string str)
		{
			trimWhitespace(&str);

			return str;
		}
		/**
		* \brief Finds the first occurance of any whitespace character in the given string.
		* 
		* \param str The string to search for whitespace.
		* 
		* \return The index of the first whitespace character found, returns std::string::npos if not found.
		*/
		size_t findWhitespace(std::string str)
		{
			size_t i;

			for (i = 0; i < str.length(); ++i)
			{
				if (isspace(str.at(i)))
				{
					return i;
				}
			}
			
			return std::string::npos;
		}
	}

	namespace StringUtils
	{
		void trim(std::string* str)
		{
			trimWhitespace(str);
		}

		std::string trim(std::string str)
		{
			return trimWhitespace(str);
		}

		void trim(std::string* str, const char& c)
		{
			ltrim(str, c);
			rtrim(str, c);
		}

		std::string trim(std::string str, const char& c)
		{
			trim(&str, c);

			return str;
		}

		void ltrim(std::string* str)
		{
			ltrimWhitespace(str);
		}

		std::string ltrim(std::string str)
		{
			return ltrimWhitespace(str);
		}

		void ltrim(std::string* str, const char& c)
		{
			while (!str->empty() && str->at(0) == c)
			{
				str->erase(0, 1);
			}
		}

		std::string ltrim(std::string str, const char& c)
		{
			ltrim(&str, c);

			return str;
		}

		void rtrim(std::string* str)
		{
			rtrimWhitespace(str);
		}

		std::string rtrim(std::string str)
		{
			return rtrimWhitespace(str);
		}

		void rtrim(std::string* str, const char& c)
		{
			while (!str->empty() && str->at(strlen((*str).c_str()) - 1) == c)
			{
				str->erase((strlen((*str).c_str()) - 1), 1);
			}
		}

		std::string rtrim(std::string str, const char& c)
		{
			rtrim(&str, c);

			return str;
		}

		bool startsWith(std::string str, std::string prefix)
		{
			return str.substr(0, prefix.length()) == prefix;
		}

		void split(std::string str, std::string** splitValues, uint32_t* splitCount, bool excludeNull)
		{
			if (!str.empty())
			{
				*splitCount = 1;
				std::string currentSearchString = str;
				*splitValues = new std::string[*splitCount];
				size_t pos = findWhitespace(currentSearchString);

				while (pos != std::string::npos)
				{
					if (pos > 0 || !excludeNull)
					{
						++(*splitCount);

						// Copy previously found slices
						std::string* tmpBits = new std::string[*splitCount];

						for (size_t i = 0; i < *splitCount - 2; ++i)
						{
							// If the arraySize is 2, then 0 entries have been initialized
							tmpBits[i] = (*splitValues)[i];
						}

						delete[] *splitValues;
						*splitValues = tmpBits;

						// Add new slice
						(*splitValues)[*splitCount - 2] = currentSearchString.substr(0, pos);
						currentSearchString = currentSearchString.substr(pos + 1, currentSearchString.length() - pos);

						// Prep for next iteration
						pos = findWhitespace(currentSearchString);
					}
					else
					{
						// The currentSearchString started with the delimiter, just remove it and run again
						currentSearchString = currentSearchString.substr(1, currentSearchString.length() - 1);
					}
				}

				// Add the remaining string as the last entry in the array
				(*splitValues)[*splitCount - 1] = std::string{ currentSearchString };
			}
			else
			{
				*splitValues = nullptr;
				*splitCount = 0;
			}
		}

		void split(std::string str, std::string delimiter, uint32_t splitLimit, std::string** splitValues, uint32_t* splitCount, bool excludeNull)
		{
			if (!str.empty() && !delimiter.empty())
			{
				uint32_t maxNumberOfSplits = (splitLimit > 0) ? splitLimit : std::numeric_limits<uint32_t>::max();
				*splitCount = 1;
				std::string currentSearchString = str;
				*splitValues = new std::string[*splitCount];
				size_t pos = currentSearchString.find(delimiter);

				while (maxNumberOfSplits > 0 && pos != std::string::npos)
				{
					--maxNumberOfSplits;

					if (pos > 0 || !excludeNull)
					{
						++(*splitCount);

						// Copy previously found slices
						std::string* tmpBits = new std::string[*splitCount];

						for (size_t i = 0; i < *splitCount - 2; ++i)
						{
							// If the arraySize is 2, then 0 entries have been initialized
							tmpBits[i] = (*splitValues)[i];
						}

						delete[] *splitValues;
						*splitValues = tmpBits;

						// Add new slice
						(*splitValues)[*splitCount - 2] = currentSearchString.substr(0, pos);
						currentSearchString = currentSearchString.substr(pos + delimiter.length(), currentSearchString.length() - pos);

						// Prep for next iteration
						pos = currentSearchString.find(delimiter);
					}
					else
					{
						// The currentSearchString started with the delimiter, just remove it and run again
						currentSearchString = currentSearchString.substr(1, currentSearchString.length() - 1);
					}
				}

				// Add the remaining string as the last entry in the array
				(*splitValues)[*splitCount - 1] = currentSearchString;
			}
			else if (!delimiter.empty())
			{
				*splitValues = new std::string[1]{ str };
				*splitCount = 1;
			}
			else
			{
				*splitValues = nullptr;
				*splitCount = 0;
			}
		}

		void split(std::string str, std::string delimiter, std::string** splitValues, uint32_t* splitCount, bool excludeNull)
		{
			split(str, delimiter, 0, splitValues, splitCount, excludeNull);
		}
	}

	namespace FileUtils
	{
		namespace
		{
			/**
			* \brief Custom buffer for building stream objects.
			*/
			struct membuf : std::streambuf {
				membuf(char const* base, size_t size) {
					char* p(const_cast<char*>(base));
					this->setg(p, p, p + size);
				}
			protected:
				pos_type seekoff(off_type off, std::ios_base::seekdir dir, std::ios_base::openmode which = std::ios_base::in)
				{
					if (dir == std::ios_base::cur)
						gbump(off);
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
			struct imemstream : virtual membuf, std::istream {
				imemstream(char const* base, size_t size)
					: membuf(base, size)
					, std::istream(static_cast<std::streambuf*>(this)) {
				}
			};
		}

		bool getFileListFromArchive(std::string archivePath, std::unordered_set<std::string>* files)
		{
			struct zip_t* zip = zip_open(archivePath.c_str(), 0, 'r');

			if (zip != nullptr)
			{
				int32_t n = zip_entries_total(zip);

				if (n > -1)
				{
					if (files == nullptr)
					{
						files = new std::unordered_set<std::string>{};
					}

					for (size_t i = 0; i < n; ++i)
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

		bool getContentsFromArchivedFile(std::string archivePath, std::string filePath, int8_t** buffer, size_t* bufferSize)
		{
			bool success = false;

			struct zip_t* zip = zip_open(archivePath.c_str(), 0, 'r');
			{
				if (zip != nullptr)
				{
					zip_entry_open(zip, filePath.c_str());
					{
						if (zip != nullptr)
						{
							if (zip_entry_read(zip, (void**)buffer, bufferSize) >= 0)
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

		bool getStreamFromArchivedFile(std::string archivePath, std::string filePath, std::istream** stream)
		{
			bool success = true;
			int8_t* buffer = nullptr;
			size_t bufferSize = 0;

			if (getContentsFromArchivedFile(archivePath, filePath, &buffer, &bufferSize))
			{
				*stream = new imemstream((char*)buffer, bufferSize);
			}
			else
			{
				success = false;
				LOGGER_ERROR("Failed to read contents of '" + archivePath + "/" + filePath + "'");
			}

			return success;
		}

		bool getPropertiesFromStream(std::istream* stream, std::unordered_map<std::string, std::string>* properties)
		{
			std::string line;
			
			while (std::getline(*stream, line))
			{
				StringUtils::trim(&line);

				std::string* splitValues;
				uint32_t splitCount;

				StringUtils::split(line, &splitValues, &splitCount);

				if (splitCount == 2)
				{
					properties->insert(
						std::pair<std::string, std::string>(splitValues[0], splitValues[1])
					);
				}
				else
				{
					LOGGER_ERROR("Invalid property data in stream");
					return false;
				}
			}

			return true;
		}
	}
}