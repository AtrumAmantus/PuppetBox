#pragma once

#include <fstream>
#include <string>
#include <unordered_map>
#include <unordered_set>

#include <STBI/stb_image.h>

#include "ImageData.h"
#include "Logger.h"
#include "Material.h"
#include "Utilities.h"

namespace PB
{
	struct Model2D
	{
		uint32_t width;
		uint32_t height;
		int32_t offsetX;
		int32_t offsetY;
		std::string materialName;
	};

	struct ShaderProgram
	{
		std::string programPath;
		std::string vertexShaderPath;
		std::string geometryShaderPath;
		std::string fragmentShaderPath;
	};

	class AssetArchive
	{
	public:
		AssetArchive(std::string archiveName, std::string archiveRoot) : archiveName_(archiveName), archiveRoot_(archiveRoot) {};
		bool init();
		bool hasAsset(std::string assetName);
		uint64_t assetCount();
		std::string loadAsciiData(std::string assetName, bool* error);
		ShaderProgram loadShaderAsset(std::string assetName, bool* error);
		ImageData loadImageAsset(std::string assetName, bool* error);
		Material loadMaterialAsset(std::string assetName, bool* error);
		Model2D load2DModelAsset(std::string assetName, bool* error);
	private:
		std::string archiveName_;
		std::string archiveRoot_;
		std::unordered_set<std::string> archiveAssets_{};
		std::unordered_map<std::string, std::string> archiveAssetIds_{};
	private:
		std::string fileNameOfAsset(std::string assetName);
		std::string archivePath();
	};
}