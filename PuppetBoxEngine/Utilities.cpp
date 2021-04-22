#include "pch.h"

#include <ctype.h>

#include "Utilities.h"

namespace PB
{
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
		};

		std::string trim(std::string str)
		{
			return trimWhitespace(str);
		}

		void trim(std::string* str, const char& c)
		{
			ltrim(str, c);
			rtrim(str, c);
		};

		std::string trim(std::string str, const char& c)
		{
			trim(&str, c);

			return str;
		}

		void ltrim(std::string* str)
		{
			ltrimWhitespace(str);
		};

		std::string ltrim(std::string str)
		{
			return ltrimWhitespace(str);
		};

		void ltrim(std::string* str, const char& c)
		{
			while (!str->empty() && str->at(0) == c)
			{
				str->erase(0, 1);
			}
		};

		std::string ltrim(std::string str, const char& c)
		{
			ltrim(&str, c);

			return str;
		};

		void rtrim(std::string* str)
		{
			rtrimWhitespace(str);
		};

		std::string rtrim(std::string str)
		{
			return rtrimWhitespace(str);
		};

		void rtrim(std::string* str, const char& c)
		{
			while (!str->empty() && str->at(strlen((*str).c_str()) - 1) == c)
			{
				str->erase((strlen((*str).c_str()) - 1), 1);
			}
		};

		std::string rtrim(std::string str, const char& c)
		{
			rtrim(&str, c);

			return str;
		};

		bool startsWith(std::string str, std::string prefix)
		{
			return str.substr(0, prefix.length()) == prefix;
		}

		void split(std::string str, std::string** values, uint32_t* valueCount, bool excludeNull)
		{
			if (!str.empty())
			{
				*valueCount = 1;
				std::string currentSearchString = str;
				*values = new std::string[*valueCount];
				size_t pos = findWhitespace(currentSearchString);

				while (pos != std::string::npos)
				{
					if (pos > 0 || !excludeNull)
					{
						++(*valueCount);

						// Copy previously found slices
						std::string* tmpBits = new std::string[*valueCount];

						for (size_t i = 0; i < *valueCount - 2; ++i)
						{
							// If the arraySize is 2, then 0 entries have been initialized
							tmpBits[i] = *values[i];
						}

						delete[] * values;
						*values = tmpBits;

						// Add new slice
						(*values)[*valueCount - 2] = currentSearchString.substr(0, pos);
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
				(*values)[*valueCount - 1] = currentSearchString;
			}
			else
			{
				*values = nullptr;
				*valueCount = 0;
			}
		}

		void split(std::string str, std::string delimiter, std::string** values, uint32_t* valueCount, bool excludeNull)
		{
			if (!str.empty() && !delimiter.empty())
			{
				*valueCount = 1;
				std::string currentSearchString = str;
				*values = new std::string[*valueCount];
				size_t pos = currentSearchString.find(delimiter);

				while (pos != std::string::npos)
				{
					if (pos > 0 || !excludeNull)
					{
						++(*valueCount);

						// Copy previously found slices
						std::string* tmpBits = new std::string[*valueCount];

						for (size_t i = 0; i < *valueCount - 2; ++i)
						{
							// If the arraySize is 2, then 0 entries have been initialized
							tmpBits[i] = *values[i];
						}

						delete[] *values;
						*values = tmpBits;

						// Add new slice
						(*values)[*valueCount - 2] = currentSearchString.substr(0, pos);
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
				(*values)[*valueCount - 1] = currentSearchString;
			}
			else if (!delimiter.empty())
			{
				*values = new std::string[1]{ str };
				*valueCount = 1;
			}
			else
			{
				*values = nullptr;
				*valueCount = 0;
			}
		}
	}
}