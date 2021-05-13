#include "pch.h"

#include <limits>

#include <STBI/stb_image.h>

#include "AssetArchive.h"
#include "PropertyTree.h"

namespace PB
{
	namespace
	{
		/**
		* \brief Helper function to return a default value if a given key is not in the given unordered_map.
		* 
		* \param propertyName	The key that is expected to referenced the desired data in the map.
		* \param properties		The unordered_map from which the key will be used to acquire data.
		* \param defaultValue	The default value to use if the given unordered_map does not contain the given key.
		* 
		* \return Either the value from the map referenced with the given key, or the given default value if the key did not exist.
		*/
		std::string defaultIfNotInMap(std::string propertyName, std::unordered_map<std::string, std::string> properties, std::string defaultValue)
		{
			if (properties.find(propertyName) != properties.end())
			{
				return properties.at(propertyName);
			}

			return defaultValue;
		}

		/**
		* \brief Helper function to return a default value if a given key is not in the given PropertyTree
		* 
		* \param propertyName	The key that is expected to referenced the desired data in the PropertyTree.
		* \param properties		The PropertyTree from which the key will be used to acquire data.
		* \param defaultValue	The default value to use if the given PropertyTree does not contain the given key.
		* 
		* \return Either the value from the PropertyTree referenced with the given key, or the given default value if the key did not exist.
		*/
		std::string defaultIfNotInTree(std::string propertyName, PropertyTree& pTree, std::string defaultValue)
		{
			std::string value;
			PropertyTree* childNode = pTree.get(propertyName);

			if (childNode != nullptr)
			{
				value = childNode->value();

				if (value.empty())
				{
					return defaultValue;
				}
			}

			return value;
		}

		/**
		* \brief Helper function to map a properties map to a Model2D object.
		* 
		* \param properties	The properties map to use to map to a Model2D object.
		* \param bool		Flag indicating an error occured if set to True.
		* 
		* \return The Model2D object built from the given properties map.
		*/
		ModelData2D mapToModel2D(std::unordered_map<std::string, std::string> properties, bool* error)
		{
			ModelData2D model{};

			model.width = NumberUtils::parseValue(defaultIfNotInMap("width", properties, "0").c_str(), 0, error);
			model.height = NumberUtils::parseValue(defaultIfNotInMap("height", properties, "0").c_str(), 0, error);
			model.offsetX = NumberUtils::parseValue(defaultIfNotInMap("offsetX", properties, "0").c_str(), 0, error);
			model.offsetY = NumberUtils::parseValue(defaultIfNotInMap("offsetY", properties, "0").c_str(), 0, error);
			model.materialName = defaultIfNotInMap("material", properties, "");

			if (*error)
			{
				LOGGER_ERROR("Failed to parse values for Model2D object");
				return {};
			}

			return model;
		}

		/**
		* \brief Helper function to map a PropertyTree to a Material object.
		*
		* \param properties	The PropertyTree to use to map to a Material object.
		* \param bool		Flag indicating an error occured if set to True.
		*
		* \return The Material object built from the given properties map.
		*/
		Material mapToMaterial(PropertyTree& materialProperties, bool* error)
		{
			Material material;

			PropertyTree* diffuseProperties = materialProperties.get("diffuse");
			std::string image = defaultIfNotInTree("image", *diffuseProperties, "");
			uint32_t width = NumberUtils::parseValue(defaultIfNotInTree("width", *diffuseProperties, "0").c_str(), 0, error);
			uint32_t height = NumberUtils::parseValue(defaultIfNotInTree("height", *diffuseProperties, "0").c_str(), 0, error);
			uint32_t xOffset = NumberUtils::parseValue(defaultIfNotInTree("xOffset", *diffuseProperties, "0").c_str(), 0, error);
			uint32_t yOffset = NumberUtils::parseValue(defaultIfNotInTree("yOffset", *diffuseProperties, "0").c_str(), 0, error);

			std::string shader = defaultIfNotInTree("shader", materialProperties, "");

			if (*error)
			{
				LOGGER_ERROR("Failed to parse values for Material Asset");
				return {};
			}

			material.diffuseId = image;
			material.shaderId = shader;

			return material;
		}

		/**
		* \brief Helper function to map a properties map to a Shader object.
		*
		* \param properties	The properties map to use to map to a Material object.
		* \param bool		Flag indicating an error occured if set to True.
		*
		* \return The Material object built from the given properties map.
		*/
		ShaderProgram mapToShaderProgram(std::unordered_map<std::string, std::string> properties, bool* error)
		{
			ShaderProgram shaderProgram{};

			shaderProgram.vertexShaderPath = defaultIfNotInMap("vertex", properties, "");
			shaderProgram.geometryShaderPath = defaultIfNotInMap("geometry", properties, "");
			shaderProgram.fragmentShaderPath = defaultIfNotInMap("fragment", properties, "");

			return shaderProgram;
		}

		/**
		* \brief Helper function to aquire the filename associated with the given virtual asset path.
		*
		* \param assetPath			The virtual asset path of the desired asset.
		* \param archiveAssetIds	The unordered_map containing the virtual path -> filename associations.
		* \param archiveAssets		The unordered_set containing all existing assets contained in the AssetArchive.
		*
		* \return The filename for the given virtual asset path if found, or a blank string otherwise.
		*/
		std::string fileNameOfAsset(
			std::string assetPath,
			std::unordered_map<std::string, std::string>& archiveAssetIds,
			std::unordered_set<std::string>& archiveAssets
		)
		{
			if (archiveAssetIds.find(assetPath) != archiveAssetIds.end())
			{
				std::string fileName = archiveAssetIds.at(assetPath);

				if (archiveAssets.find(fileName) != archiveAssets.end())
				{
					return fileName;
				}
				else
				{
					LOGGER_ERROR("Defined asset '" + assetPath + "' could not be found");
				}
			}
			else
			{
				LOGGER_ERROR("Unknown asset '" + assetPath + "'");
			}

			return "";
		}

		/**
		* \brief Helper function that create a property map from a stream object.  Assumes whitespace
		* delimited key-value pairs within the stream.
		*
		* \param stream		The stream to read key-value pair data from.
		* \param properties	Pointer to the unordered_map to store parsed properties in.
		*
		* \return True if the properties were successfully read from the stream, False otherwise.
		*/
		bool getPropertiesFromStream(std::istream* stream, std::unordered_map<std::string, std::string>* properties)
		{
			std::string line;
			uint32_t lineNumber = 0;

			while (std::getline(*stream, line))
			{
				lineNumber++;
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
					LOGGER_ERROR("Invalid property data in stream '" + line + "' on line " + std::to_string(lineNumber));
					return false;
				}
			}

			return true;
		}

		/**
		* \brief Helper function to count the indentation level of the given string.
		* 
		* \param line	The string to calculate the indentation level for.
		* 
		* \return The number of indentations prefixed on the given string.
		*/
		uint32_t countIndents(std::string line)
		{
			uint32_t indentCount = 0;

			for (size_t i = 0; i < line.length() && line.c_str()[i] == '\t'; ++i)
			{
				indentCount++;
			}

			return indentCount;
		}

		/**
		* \brief Helper function that attempts to parse the given stream into a PropertyTree.
		* 
		* \param stream		The stream of data to parse.
		* \param root		The root of the PropertyTree to parse the data into.
		* 
		* \return True if the data was successfully parsed, False otheriwse.
		*/
		bool getPropertyTreeFromStream(std::istream* stream, PropertyTree* root)
		{
			std::string line;
			uint32_t lineNumber = 0;
			PropertyTree* currentNode = root;
			uint32_t indentLevel = 0;
			uint32_t minIndents = 0;
			uint32_t maxIndents = 0;

			while (std::getline(*stream, line))
			{
				lineNumber++;
				indentLevel = countIndents(line);

				if (indentLevel >= minIndents && indentLevel <= maxIndents)
				{
					if (indentLevel < maxIndents)
					{
						for (uint32_t i = maxIndents; i > indentLevel; --i)
						{
							currentNode = currentNode->parent();
						}
					}

					std::string* splitValues;
					uint32_t splitCount;

					StringUtils::split(line, ":", 1, &splitValues, &splitCount);

					if (splitCount == 2)
					{
						StringUtils::trim(&splitValues[0]);
						StringUtils::trim(&splitValues[1]);

						if (splitValues[1].empty())
						{
							minIndents = indentLevel + 1;
							maxIndents = minIndents;

							if (currentNode->add(splitValues[0]))
							{
								currentNode = currentNode->get(splitValues[0]);
							}
						}
						else
						{
							minIndents = 0;
							maxIndents = indentLevel;

							if (currentNode->add(splitValues[0]))
							{
								currentNode->get(splitValues[0])->add(splitValues[1]);
							}
							else
							{
								LOGGER_WARN("Property redefinition, '" + splitValues[0] + "' on line " + std::to_string(lineNumber));
							}
						}
					}
					else
					{
						LOGGER_ERROR("Invalid data '" + line + "' on line " + std::to_string(lineNumber));
						return false;
					}
				}
				else
				{
					LOGGER_ERROR("Unexpected indentation '" + line + "' on line " + std::to_string(lineNumber));
					return false;
				}
			}

			return true;
		}
	}

	bool AssetArchive::init()
	{
		bool success = true;

		success = success && FileUtils::getFileListFromArchive(archivePath(), &archiveAssets_);

		std::istream* stream = nullptr;
		success = success && FileUtils::getStreamFromArchivedFile(archivePath(), ".manifest", &stream);
		success = success && getPropertiesFromStream(stream, &archiveAssetIds_);

		if (!success)
		{
			LOGGER_ERROR("Failed to initialize archive");
		}

		delete stream;
		return success;
	}

	bool AssetArchive::hasAsset(std::string assetPath)
	{
		return !assetPath.empty() && archiveAssets_.find(assetPath) != archiveAssets_.end();
	}

	uint64_t AssetArchive::assetCount()
	{
		return archiveAssets_.size();
	}

	std::string AssetArchive::loadAsciiData(std::string assetPath, bool* error)
	{
		std::string data;
		std::string fileName = fileNameOfAsset(assetPath, archiveAssetIds_, archiveAssets_);

		if (hasAsset(fileName))
		{
			int8_t* buffer = nullptr;
			size_t bufferSize = 0;

			*error = *error || !FileUtils::getContentsFromArchivedFile(archivePath(), fileName, &buffer, &bufferSize);

			if (!*error)
			{
				data = std::string((char*)buffer, bufferSize);
			}

			delete buffer;
		}
		else
		{
			*error = true;
			LOGGER_ERROR("Failed to retrieve asset '" + assetPath + "'");
		}

		return data;
	}

	ShaderProgram AssetArchive::loadShaderAsset(std::string assetPath, bool* error)
	{
		ShaderProgram shaderProgram{ assetPath };

		std::string fileName = fileNameOfAsset(assetPath, archiveAssetIds_, archiveAssets_);

		if (hasAsset(fileName))
		{
			std::istream* stream = nullptr;

			std::unordered_map<std::string, std::string> modelPropertyData{};

			*error = *error || !FileUtils::getStreamFromArchivedFile(archivePath(), fileName, &stream);
			*error = *error || !getPropertiesFromStream(stream, &modelPropertyData);

			delete stream;
			if (!*error)
			{
				ShaderProgram program = mapToShaderProgram(modelPropertyData, error);
				program.programPath = assetPath;
				return program;
			}
		}
		else
		{
			*error = true;
			LOGGER_ERROR("Failed to retrieve asset '" + assetPath + "'");
		}

		return shaderProgram;
	}

	ImageData AssetArchive::loadImageAsset(std::string assetPath, bool* error)
	{
		ImageData data{};

		std::string fileName = fileNameOfAsset(assetPath, archiveAssetIds_, archiveAssets_);

		if (hasAsset(fileName))
		{
			stbi_set_flip_vertically_on_load(true);

			std::istream* stream = nullptr;

			if (FileUtils::getStreamFromArchivedFile(archivePath(), fileName, &stream))
			{
				stream->ignore(std::numeric_limits<std::streamsize>::max());
				int64_t streamLength = stream->gcount();
				stream->clear();
				stream->seekg(0, stream->beg);

				char* buffer = new char[streamLength];

				stream->read(buffer, streamLength);

				//TODO: Check if streamLength is too long, expecting only 32bit value
				data.bufferData = stbi_load_from_memory((uint8_t*)buffer, streamLength, &data.width, &data.height, &data.numChannels, 0);
			}
			else
			{
				LOGGER_ERROR("Failed to acquire stream for asset '" + assetPath + "' in archive '" + archivePath() + "'");
			}

			delete stream;
		}
		else
		{
			*error = true;
			LOGGER_ERROR("Failed to retrieve asset '" + assetPath + "'");
		}

		return data;
	}

	Material AssetArchive::loadMaterialAsset(std::string assetPath, bool* error)
	{
		std::string fileName = fileNameOfAsset(assetPath, archiveAssetIds_, archiveAssets_);

		if (hasAsset(fileName))
		{
			std::istream* stream = nullptr;

			std::unordered_map<std::string, std::string> modelPropertyData{};
			PropertyTree propertyData{"material"};

			*error = *error || !FileUtils::getStreamFromArchivedFile(archivePath(), fileName, &stream);
			*error = *error || !getPropertyTreeFromStream(stream, &propertyData);

			delete stream;
			if (!*error)
			{
				return mapToMaterial(propertyData, error);
			}
		}
		else
		{
			*error = true;
			LOGGER_ERROR("Failed to retrieve asset '" + assetPath + "'");
		}

		return {};
	}

	ModelData2D AssetArchive::load2DModelAsset(std::string assetPath, bool* error)
	{
		std::string fileName = fileNameOfAsset(assetPath, archiveAssetIds_, archiveAssets_);

		if (hasAsset(fileName))
		{
			std::istream* stream = nullptr;

			std::unordered_map<std::string, std::string> modelPropertyData{};

			*error = *error || !FileUtils::getStreamFromArchivedFile(archivePath(), fileName, &stream);
			*error = *error || !getPropertiesFromStream(stream, &modelPropertyData);

			delete stream;
			if (!*error)
			{
				ModelData2D model = mapToModel2D(modelPropertyData, error);

				if (*error)
				{
					LOGGER_ERROR("Failed to load Model2D data for asset '" + assetPath + "'");
				}

				return model;
			}
		}
		else
		{
			*error = true;
			LOGGER_ERROR("Failed to retrieve asset '" + assetPath + "'");
		}

		return {};
	}

	std::string AssetArchive::archivePath()
	{
		return archiveRoot_ + archiveName_ + ".zip";
	}
}