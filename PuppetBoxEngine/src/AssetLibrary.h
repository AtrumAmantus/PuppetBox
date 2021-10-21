#pragma once

#include <string>

#include "AssetArchive.h"
#include "Logger.h"
#include "IGfxApi.h"
#include "ImageReference.h"
#include "IModel.h"
#include "Material.h"
#include "Mesh.h"
#include "RenderedMesh.h"
#include "Shader.h"
#include "puppetbox/SceneObject.h"

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
		bool loadArchive(const std::string& archiveName);

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
		bool loadAsset(const std::string& assetPath, SceneObject* sceneObject, Asset::Type type);
	private:
		std::string archiveRoot_;
		std::shared_ptr<IGfxApi> gfxApi_;
		std::unordered_map<std::string, AssetArchive> assetArchives_{};
		std::unordered_map<std::string, Mesh> loadedMeshes_{};
		std::unordered_map<std::string, Material> loadedMaterials_{};
		std::unordered_map<std::string, ImageReference> loadedImages_{};
		std::unordered_map<std::string, ModelData2D> loadedModelData2D_{};
		std::unordered_map<std::string, Shader> loadedShaders_{};
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
		Shader loadShaderAsset(const std::string& assetPath, bool* error);

		/**
		* \brief Loads an ImageReference asset given by the provided virtual asset path.
		*
		* \param assetPath		Virtual path to the requested asset.
		* \param imageOptions	Image loading options to use when loading into memory.
		* \param error			Flag indicating an error occured if set to True.
		*
		* \return The loaded ImageReference object for the respective virtual asset path, or an empty
		* object if an error occured loading the asset.
		*/
		ImageReference loadImageAsset(const std::string& assetPath, ImageOptions imageOptions, bool* error);

		/**
		* \brief Loads a Material asset given by the provided virtual asset path.
		*
		* \param assetPath	Virtual path to the requested asset.
		* \param error		Flag indicating an error occured if set to True.
		*
		* \return The loaded Material object for the respective virtual asset path, or an empty
		* object if an error occured loading the asset.
		*/
		Material loadMaterialAsset(const std::string& assetPath, bool* error);

		/**
		* \brief Loads a Mesh asset given by the provided virtual asset path.
 		*
		* \pa&ram assetPath	Virtual path to the requested asset.
		* \param error		Flag indicating an error occured if set to True.
		*
		* \return The loaded Mesh object for the respective virtual asset path, or an empty
		* object if an error occured loading the asset.
		*/
		Mesh loadMeshAsset(const std::string& assetPath, bool* error);

		/**
		* \brief Loads a ModelData2D asset given by the provided virtual asset path.
		*
		* \param assetPath	Virtual path to the requested asset.
		* \param error		Flag indicating an error occured if set to True.
		*
		* \return The loaded ModelData2D object for the respective virtual asset path, or an empty
		* object if an error occured loading the asset.
		*/
		ModelData2D loadModelData2DAsset(const std::string& assetPath, bool* error);

		/**
		* \brief Loads the given scene object with 2D asset data.
		*
		* \param assetPath		Virtual path to the requested asset.
		* \param sceneObject	The instantiated scene object to load with asset data.
		*
		* \return True if the scene object was successfully loaded with assets, False otherwise.
		*/
		bool load2DSceneObject(const std::string& assetPath, SceneObject* sceneObject);

        bool buildMeshAndBones(ModelData2D modelData, std::string parent, std::uint32_t depth, std::unordered_map<std::string, BoneMap> &bones,
                               std::unordered_map<std::string, RenderedMesh*> &meshes);
	};
}