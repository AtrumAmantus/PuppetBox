#include "pch.h"

#include <limits>

#include <STBI/stb_image.h>

#include "AssetArchive.h"

namespace PB
{
	namespace
	{
		std::string defaultIfNotInMap(std::string propertyName, std::unordered_map<std::string, std::string> properties, std::string defaultValue)
		{
			if (properties.find(propertyName) != properties.end())
			{
				return properties.at(propertyName);
			}

			return defaultValue;
		}

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

		Material mapToMaterial(std::unordered_map<std::string, std::string> properties, bool* error)
		{
			Material material;

			material.diffuseId = defaultIfNotInMap("diffuse", properties, "");
			material.shaderId = defaultIfNotInMap("shader", properties, "");

			return material;
		}

		ShaderProgram mapToShaderProgram(std::unordered_map<std::string, std::string> properties, bool* error)
		{
			ShaderProgram shaderProgram{};

			shaderProgram.vertexShaderPath = defaultIfNotInMap("vertex", properties, "");
			shaderProgram.geometryShaderPath = defaultIfNotInMap("geometry", properties, "");
			shaderProgram.fragmentShaderPath = defaultIfNotInMap("fragment", properties, "");

			return shaderProgram;
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

	bool AssetArchive::hasAsset(std::string assetName)
	{
		return !assetName.empty() && archiveAssets_.find(assetName) != archiveAssets_.end();
	}

	uint64_t AssetArchive::assetCount()
	{
		return archiveAssets_.size();
	}

	std::string AssetArchive::loadAsciiData(std::string assetName, bool* error)
	{
		std::string data;
		std::string fileName = fileNameOfAsset(assetName);

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
			LOGGER_ERROR("Failed to retrieve asset '" + assetName + "'");
		}

		return data;
	}

	ShaderProgram AssetArchive::loadShaderAsset(std::string assetName, bool* error)
	{
		ShaderProgram shaderProgram{ assetName };

		std::string fileName = fileNameOfAsset(assetName);

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
				program.programPath = assetName;
				return program;
			}
		}
		else
		{
			*error = true;
			LOGGER_ERROR("Failed to retrieve asset '" + assetName + "'");
		}

		return shaderProgram;
	}

	ImageData AssetArchive::loadImageAsset(std::string assetName, bool* error)
	{
		ImageData data{};

		std::string fileName = fileNameOfAsset(assetName);

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
				LOGGER_ERROR("Failed to acquire stream for asset '" + assetName + "' in archive '" + archivePath() + "'");
			}

			delete stream;
		}
		else
		{
			*error = true;
			LOGGER_ERROR("Failed to retrieve asset '" + assetName + "'");
		}

		return data;
	}

	Material AssetArchive::loadMaterialAsset(std::string assetName, bool* error)
	{
		std::string fileName = fileNameOfAsset(assetName);

		if (hasAsset(fileName))
		{
			std::istream* stream = nullptr;

			std::unordered_map<std::string, std::string> modelPropertyData{};

			*error = *error || !FileUtils::getStreamFromArchivedFile(archivePath(), fileName, &stream);
			*error = *error || !FileUtils::getPropertiesFromStream(stream, &modelPropertyData);

			delete stream;
			if (!*error)
			{
				return mapToMaterial(modelPropertyData, error);
			}
		}
		else
		{
			*error = true;
			LOGGER_ERROR("Failed to retrieve asset '" + assetName + "'");
		}

		return {};
	}

	Model2D AssetArchive::load2DModelAsset(std::string assetName, bool* error)
	{
		std::string fileName = fileNameOfAsset(assetName);

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
					LOGGER_ERROR("Failed to load Model2D data for asset '" + assetName + "'");
				}

				return model;
			}
		}
		else
		{
			*error = true;
			LOGGER_ERROR("Failed to retrieve asset '" + assetName + "'");
		}

		return {};
	}

	std::string AssetArchive::fileNameOfAsset(std::string assetName)
	{
		if (archiveAssetIds_.find(assetName) != archiveAssetIds_.end())
		{
			std::string fileName = archiveAssetIds_.at(assetName);

			if (archiveAssets_.find(fileName) != archiveAssets_.end())
			{
				return fileName;
			}
			else
			{
				LOGGER_ERROR("Defined asset '" + assetName + "' could not be found");
			}
		}
		else
		{
			LOGGER_ERROR("Unknown asset '" + assetName + "'");
		}

		return "";
	}

	std::string AssetArchive::archivePath()
	{
		return archiveRoot_ + archiveName_ + ".zip";
	}
}