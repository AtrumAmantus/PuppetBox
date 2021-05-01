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
	* \brief Manages loading and organization of all asset archives.
	*/
	class AssetLibrary
	{
	public:
		AssetLibrary(std::string archiveRoot, std::shared_ptr<IGfxApi> gfxApi);
		void init();
		bool loadArchive(std::string archiveName);
		bool loadAsset(std::string assetPath, SceneObject* object, Asset::Type type);
	private:
		std::string archiveRoot_;
		std::shared_ptr<IGfxApi> gfxApi_;
		std::unordered_map<std::string, AssetArchive> assetArchives_{};
		std::unordered_map<std::string, Mesh> loadedMeshes_{};
	private:
		Shader loadShaderAsset(std::string assetPath, bool* error);
		ImageReference loadImageAsset(std::string assetPath, bool* error);
		Material loadMaterialAsset(std::string assetPath, bool* error);
		Mesh loadMeshAsset(std::string assetPath, bool* error);
		SceneObject* load2DModelAsset(std::string assetPath, bool* error);
	};
}