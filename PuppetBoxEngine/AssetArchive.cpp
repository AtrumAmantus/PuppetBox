#include "pch.h"

#include <limits>

#include <STBI/stb_image.h>

#include "AssetArchive.h"

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
		* \brief Helper function to map a properties map to a Model2D object.
		* 
		* \param properties	The properties map to use to map to a Model2D object.
		* \param bool		Flag indicating an error occured if set to True.
		* 
		* \return The Model2D object built from the given properties map.
		*/
		Model2D mapToModel2D(std::unordered_map<std::string, std::string> properties, bool* error)
		{
			Model2D model{};

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
		* \brief Helper function to map a properties map to a Material object.
		*
		* \param properties	The properties map to use to map to a Material object.
		*
		* \return The Material object built from the given properties map.
		*/
		Material mapToMaterial(std::unordered_map<std::string, std::string> properties)
		{
			Material material;

			material.diffuseId = defaultIfNotInMap("diffuse", properties, "");
			material.shaderId = defaultIfNotInMap("shader", properties, "");

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
		* \brief Helper fucking to aquire the filename associated with the given virtual asset path.
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
	}

	bool AssetArchive::init()
	{
		bool success = true;

		success = success && FileUtils::getFileListFromArchive(archivePath(), &archiveAssets_);

		std::istream* stream = nullptr;
		success = success && FileUtils::getStreamFromArchivedFile(archivePath(), ".manifest", &stream);
		success = success && FileUtils::getPropertiesFromStream(stream, &archiveAssetIds_);

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
			*error = *error || !FileUtils::getPropertiesFromStream(stream, &modelPropertyData);

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

			*error = *error || !FileUtils::getStreamFromArchivedFile(archivePath(), fileName, &stream);
			*error = *error || !FileUtils::getPropertiesFromStream(stream, &modelPropertyData);

			delete stream;
			if (!*error)
			{
				return mapToMaterial(modelPropertyData);
			}
		}
		else
		{
			*error = true;
			LOGGER_ERROR("Failed to retrieve asset '" + assetPath + "'");
		}

		return {};
	}

	Model2D AssetArchive::load2DModelAsset(std::string assetPath, bool* error)
	{
		std::string fileName = fileNameOfAsset(assetPath, archiveAssetIds_, archiveAssets_);

		if (hasAsset(fileName))
		{
			std::istream* stream = nullptr;

			std::unordered_map<std::string, std::string> modelPropertyData{};

			*error = *error || !FileUtils::getStreamFromArchivedFile(archivePath(), fileName, &stream);
			*error = *error || !FileUtils::getPropertiesFromStream(stream, &modelPropertyData);

			delete stream;
			if (!*error)
			{
				Model2D model = mapToModel2D(modelPropertyData, error);

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