#pragma once

#include <fstream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <utility>

#include <STBI/stb_image.h>

#include "ImageData.h"
#include "Logger.h"
#include "Material.h"
#include "Utilities.h"

namespace PB
{
	/**
	* \brief Struct for holding 2D Model specific data for simple communication between archive & library.
	*/
	struct ModelData2D
	{
        std::uint32_t width = 0;
        std::uint32_t height = 0;
        std::int32_t offsetX = 0;
        std::int32_t offsetY = 0;
		std::string materialName;
	};

	/**
	* \brief Struct for holding Shader specific data for simple communication between archive & library.
	*/
	struct ShaderProgram
	{
		std::string programPath;
		std::string vertexShaderPath;
		std::string geometryShaderPath;
		std::string fragmentShaderPath;
	};

	/**
	* \brief An AssetArchive object represents a single PuppetBox asset pack, containing a variety of game assets.  AssetArchives
	* utilize a virtual file system to organize containing assets.  The AssetLibrary references these paths with the path prefixed
	* with the actual name of the AssetArchive's referenced archive.
	*/
	class AssetArchive
	{
	public:
		/**
		* \brief Create an AssetArchive for an archive with the given name, at the given archive root directory.
		* 
		* \param archiveName	The name of the desired archive to load.
		* \param archiveRoot	The root directory to look in for the archive.
		*/
		AssetArchive(std::string archiveName, std::string archiveRoot) : archiveName_(std::move(archiveName)), archiveRoot_(std::move(archiveRoot)) {};

		/**
		* \brief Initializes the AssetArchive's initial configurations.  This is needed before any other AssetArchive
		* interations.
		*/
		bool init();

		/**
		* \brief Determines if the AssetArchive has the given asset.
		* 
		* \param assetPath	The virtual asset path of the asset to look for.
		* 
		* \return True if the AssetArchive contains the given asset, False otherwise.
		*/
		bool hasAsset(const std::string& assetPath);

		/**
		* \brief Gives the asset count for the AssetArchive.
		* 
		* \return The total number of assets the AssetArchive contains (including the manifest file).
		*/
        std::uint64_t assetCount();

		/**
		* \brief Loads an asset's contents as raw ascii data.
		* 
		* \param assetPath	The virtual asset path of the desired asset.
		* \param error		Flag indicating an error occured if set to True.
		* 
		* \return A string containing the raw contents of the requested asset, or an empty string if an
		* error occured.
		*/
		std::string loadAsciiData(const std::string& assetPath, bool* error);

		/**
		* \brief Returns a ShaderProgram for the given shader asset.
		*
		* \param assetPath	The virtual asset path of the desired asset.
		* \param error		Flag indicating an error occured if set to True.
		*
		* \return A ShaderProgram of the requested asset, or an empty object if an error occured.
		*/
		ShaderProgram loadShaderAsset(const std::string& assetPath, bool* error);

		/**
		* \brief Returns a ImageData for the given shader asset.
		*
		* \param assetPath	The virtual asset path of the desired asset.
		* \param error		Flag indicating an error occured if set to True.
		*
		* \return A ImageData of the requested asset, or an empty object if an error occured.
		*/
		ImageData loadImageAsset(const std::string& assetPath, bool* error);

		/**
		* \brief Returns a Material for the given shader asset.
		*
		* \param assetPath	The virtual asset path of the desired asset.
		* \param error		Flag indicating an error occured if set to True.
		*
		* \return A Material of the requested asset, or an empty object if an error occured.
		*/
		Material loadMaterialAsset(const std::string& assetPath, bool* error);

		/**
		* \brief Returns a Model2D for the given shader asset.
		*
		* \param assetPath	The virtual asset path of the desired asset.
		* \param error		Flag indicating an error occured if set to True.
		*
		* \return A Model2D of the requested asset, or an empty object if an error occured.
		*/
		ModelData2D load2DModelAsset(const std::string& assetPath, bool* error);
	private:
		std::string archiveName_;
		std::string archiveRoot_;
		std::unordered_set<std::string> archiveAssets_{};
		std::unordered_map<std::string, std::string> archiveAssetIds_{};
	private:
		/**
		* \brief Returns the path to the AssetArchive.
		* 
		* \return A string representing the path to the AssetArchive.
		*/
		std::string archivePath();
	};
}