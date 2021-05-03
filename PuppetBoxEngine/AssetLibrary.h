#pragma once

#include <string>

#include "AssetArchive.h"
#include "Logger.h"
#include "IGfxApi.h"
#include "ImageReference.h"
#include "Material.h"
#include "Mesh.h"
#include "Shader.h"
#include "../include/puppetbox/SceneObject.h"

namespace PB
{
	namespace Asset
	{
		enum Type
		{
			UNKNOWN,
			MODEL_2D
		};
	}

	/**
	* \brief Manages loading and organization of all asset archives.  Asset loading is all done through
	* the AssetLibrary, which will then facilitate specific archive loads and caching of assets.
	* 
	* Currently only SceneObject assets can be loaded from the AssetLibrary, future assets will need to be added where necessary.
	*/
	class AssetLibrary
	{
	public:
		/**
		* \brief Create a AssetLibrary instance, providing the root directory for where all archives should be fetched, and
		* IGfxApi that will be used for loading of assets.
		* 
		* \param archiveRoot	The root directory that AssetLibrary should look for archives when they are loaded.
		* \param gfxApi			The derived gfxApi that will be used for loading assets.
		*/
		AssetLibrary(std::string archiveRoot, std::shared_ptr<IGfxApi> gfxApi);
		
		/**
		* \brief Initializes the AssetLibrary, making initial configurations.  This call needs to be made before any others
		* on the AssetLibrary.
		*/
		void init();

		/**
		* \brief Attempts to load the archive with the given name so that it's contained assets can be used in future
		* asset load calls.
		* 
		* \param archiveName	The name of the archive to attempt to load.
		* 
		* \return True if the archive was loaded successfully, False otherwise.
		*/
		bool loadArchive(std::string archiveName);

		/**
		* \brief Attempts to load the given asset into the provided SceneObject.  This logic handles loading the mesh,
		* textures, animations, etc. for the given SceneObject.
		* 
		* The path is a virtual path to the asset, beginning with the actual AssetArchive name.
		* 
		* \param assetPath		Virtual path to the asset, beginning with actual AssetArchive name.
		* \param sceneObject	Pointer to the SceneObject that will be impregnated with the requested asset references.
		* \param type			The type of asset to be loaded, which will dictate asset loading configurations.  Currently
		* only supports MODEL_2D.
		*
		* \return True if the asset was loaded successfully, False otherwise.
		*/
		bool loadAsset(std::string assetPath, SceneObject* sceneObject, Asset::Type type);
	private:
		std::string archiveRoot_;
		std::shared_ptr<IGfxApi> gfxApi_;
		std::unordered_map<std::string, AssetArchive> assetArchives_{};
		std::unordered_map<std::string, Mesh> loadedMeshes_{};
	private:

		/**
		* \brief Loads a Shader asset given by the provided virtual asset path.
		* 
		* \param assetPath	Virtual path to the requested asset.
		* \param error		Flag indicating an error occured if set to True.
		* 
		* \return The loaded Shader object for the respective virtual asset path, or an empty
		* object if an error occured loading the asset.
		*/
		Shader loadShaderAsset(std::string assetPath, bool* error);

		/**
		* \brief Loads an ImageReference asset given by the provided virtual asset path.
		*
		* \param assetPath	Virtual path to the requested asset.
		* \param error		Flag indicating an error occured if set to True.
		*
		* \return The loaded ImageReference object for the respective virtual asset path, or an empty
		* object if an error occured loading the asset.
		*/
		ImageReference loadImageAsset(std::string assetPath, bool* error);

		/**
		* \brief Loads a Material asset given by the provided virtual asset path.
		*
		* \param assetPath	Virtual path to the requested asset.
		* \param error		Flag indicating an error occured if set to True.
		*
		* \return The loaded Material object for the respective virtual asset path, or an empty
		* object if an error occured loading the asset.
		*/
		Material loadMaterialAsset(std::string assetPath, bool* error);

		/**
		* \brief Loads a Mesh asset given by the provided virtual asset path.
		*
		* \param assetPath	Virtual path to the requested asset.
		* \param error		Flag indicating an error occured if set to True.
		*
		* \return The loaded Mesh object for the respective virtual asset path, or an empty
		* object if an error occured loading the asset.
		*/
		Mesh loadMeshAsset(std::string assetPath, bool* error);

		/**
		* \brief Loads a SceneObject for a 2D model asset given by the provided virtual asset path.
		*
		* \param assetPath	Virtual path to the requested asset.
		* \param error		Flag indicating an error occured if set to True.
		*
		* \return The loaded 2DModel SceneObject object for the respective virtual asset path, or an empty
		* object if an error occured loading the asset.
		*/
		SceneObject* load2DModelAsset(std::string assetPath, bool* error);
	};
}