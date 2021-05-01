#include "pch.h"

#include "AssetLibrary.h"
#include "OpenGLModel.h"
#include "Utilities.h"
#include "Vertex.h"

namespace PB
{
	namespace
	{
		struct AssetStruct
		{
			std::string archiveName;
			std::string assetName;
		};

		template <class T>
		void insertIntoMap(std::string key, T& value, std::unordered_map<std::string, T>& map)
		{
			map.insert(
				std::pair<std::string, T> { key, value }
			);
		}

		AssetStruct parseAssetPath(std::string assetPath, bool* error)
		{
			uint32_t splitCount;
			std::string* splitValues = nullptr;
			StringUtils::split(assetPath, "/", 1, &splitValues, &splitCount);

			if (splitCount > 1)
			{
				return AssetStruct{ splitValues[0], splitValues[1] };
			}
			else
			{
				LOGGER_ERROR("Invalid asset path name, '" + assetPath + "'");
			}

			*error = true;
			return AssetStruct{};
		}

		std::string loadShaderCode(std::string assetPath, std::unordered_map<std::string, AssetArchive>& assetArchiveMap, bool* error)
		{
			if (!StringUtils::trim(assetPath).empty())
			{
				AssetStruct asset = parseAssetPath(assetPath, error);

				if (!*error)
				{
					std::string data = assetArchiveMap.at(asset.archiveName).loadAsciiData(asset.assetName, error);
					
					if (!*error)
					{
						return data;
					}
					else
					{
						LOGGER_ERROR("Unable to load ascii data for asset '" + assetPath + "'");
					}
				}
				else
				{
					LOGGER_ERROR("Invalid asset, '" + assetPath + "'");
				}
			}

			return "";
		}
	}

	AssetLibrary::AssetLibrary(std::string archiveRoot, std::shared_ptr<IGfxApi> gfxApi) : archiveRoot_(archiveRoot), gfxApi_(gfxApi)
	{

	}

	void AssetLibrary::init()
	{
		Vertex* vData = new Vertex[]{
			{{ 0.5,  0.5, 0.0}, {0.0, 0.0, 1.0}, {1.0, 1.0}},
			{{-0.5,  0.5, 0.0}, {0.0, 0.0, 1.0}, {0.0, 1.0}},
			{{-0.5, -0.5, 0.0}, {0.0, 0.0, 1.0}, {0.0, 0.0}},

			{{-0.5, -0.5, 0.0}, {0.0, 0.0, 1.0}, {0.0, 0.0}},
			{{ 0.5, -0.5, 0.0}, {0.0, 0.0, 1.0}, {1.0, 0.0}},
			{{ 0.5,  0.5, 0.0}, {0.0, 0.0, 1.0}, {1.0, 1.0}}
		};

		Mesh spriteMesh = gfxApi_->loadMesh(vData, 6);

		delete[] vData;

		insertIntoMap("sprite", spriteMesh, loadedMeshes_);
	}

	bool AssetLibrary::loadArchive(std::string archiveName)
	{
		bool error = false;

		if (!loadedMeshes_.empty())
		{
			if (assetArchives_.find(archiveName) == assetArchives_.end())
			{
				AssetArchive archive{ archiveName, archiveRoot_ };

				if (archive.init())
				{
					assetArchives_.insert(
						std::pair<std::string, AssetArchive>{archiveName, archive}
					);

					LOGGER_DEBUG("'" + archiveName + "' loaded " + std::to_string(archive.assetCount()) + " assets");
				}
				else
				{
					LOGGER_ERROR("Failed to load archive '" + archiveName + "'");
				}
			}
			else
			{
				LOGGER_WARN("Archive '" + archiveName + "' is already loaded");
			}
		}
		else
		{
			error = true;
			LOGGER_ERROR("AssetLibrary needs to be initialized before loading archives");
		}

		return !error;
	}

	bool AssetLibrary::loadAsset(std::string assetPath, SceneObject* sceneObject, Asset::Type type)
	{
		bool error = false;

		switch (type)
		{
		case Asset::Type::MODEL_2D:
		{
			SceneObject* so = load2DModelAsset(assetPath, &error);
			if (!error)
			{
				*sceneObject = *so;
				delete so;
			}
			else
			{
				LOGGER_ERROR("Failed to load asset '" + assetPath + "'");
			}
			return true;
			break;
		}
		default:
			LOGGER_ERROR("Can not load asset '" + assetPath + "' of undefined type");
		}

		return false;
	}

	Shader AssetLibrary::loadShaderAsset(std::string assetPath, bool* error)
	{
		Shader shader{ assetPath };

		AssetStruct asset = parseAssetPath(assetPath, error);

		if (!*error)
		{
			ShaderProgram program = assetArchives_.at(asset.archiveName).loadShaderAsset(asset.assetName, error);

			std::string vertexCode;
			std::string geometryCode;
			std::string fragmentCode;

			if (!*error) vertexCode = loadShaderCode(program.vertexShaderPath, assetArchives_, error);
			if (!*error) geometryCode = loadShaderCode(program.geometryShaderPath, assetArchives_, error);
			if (!*error) fragmentCode = loadShaderCode(program.fragmentShaderPath, assetArchives_, error);

			shader = Shader{ assetPath, program.vertexShaderPath, program.geometryShaderPath, program.fragmentShaderPath };
			bool loaded = true;
			loaded = loaded && shader.loadVertexShader(vertexCode);
			loaded = loaded && shader.loadGeometryShader(geometryCode);
			loaded = loaded && shader.loadFragmentShader(fragmentCode);

			if (loaded)
			{
				shader.init();
			}
			else
			{
				LOGGER_ERROR("Failed to load shader '" + assetPath + "'");
			}
		}
		else
		{
			LOGGER_ERROR("Invalid asset, '" + assetPath + "'");
		}

		return shader;
	}

	ImageReference AssetLibrary::loadImageAsset(std::string assetPath, bool* error)
	{
		ImageReference imageReference{ 0 };
		AssetStruct asset = parseAssetPath(assetPath, error);

		if (!*error)
		{
			ImageData imageData = assetArchives_.at(asset.archiveName).loadImageAsset(asset.assetName, error);
			imageReference = gfxApi_->loadImage(imageData);
			imageData.drop();
		}
		else
		{
			LOGGER_ERROR("Invalid asset, '" + assetPath + "'");
		}

		return imageReference;
	}

	Material AssetLibrary::loadMaterialAsset(std::string assetPath, bool* error)
	{
		AssetStruct asset = parseAssetPath(assetPath, error);

		if (!*error)
		{
			Material material = assetArchives_.at(asset.archiveName).loadMaterialAsset(asset.assetName, error);

			if (!*error)
			{
				if (!material.diffuseId.empty())
				{
					ImageReference imageReference = loadImageAsset(material.diffuseId, error);

					if (!*error)
					{
						material.diffuseMap = imageReference;
					}
					else
					{
						LOGGER_ERROR("Failed to load image, '" + material.diffuseId + "' for asset '" + assetPath + "'");
					}
				}

				return material;
			}
			else
			{
				LOGGER_ERROR("Failed to load material, '" + assetPath + "'");
			}
		}
		else
		{
			LOGGER_ERROR("Invalid asset, '" + assetPath + "'");
		}

		return {};
	}

	Mesh AssetLibrary::loadMeshAsset(std::string assetPath, bool* error)
	{
		if (loadedMeshes_.find(assetPath) != loadedMeshes_.end())
		{
			return loadedMeshes_.at(assetPath);
		}
		else
		{
			*error = true;
			LOGGER_ERROR("Mesh '" + assetPath + "' does not exist");
		}

		return Mesh{};
	}

	SceneObject* AssetLibrary::load2DModelAsset(std::string assetPath, bool* error)
	{
		SceneObject* sceneObject = nullptr;
		AssetStruct asset = parseAssetPath(assetPath, error);

		if (!*error)
		{
			Model2D model2D = assetArchives_.at(asset.archiveName).load2DModelAsset(asset.assetName, error);

			if (!*error)
			{
				Material material = loadMaterialAsset(model2D.materialName, error);

				if (!*error)
				{
					material.shader = loadShaderAsset(material.shaderId, error);

					if (!*error)
					{
						Mesh mesh = loadMeshAsset("sprite", error);

						if (!*error)
						{
							sceneObject = new SceneObject{ 0, new OpenGLModel{mesh, material} };
						}
						else
						{
							LOGGER_ERROR("Could not load mesh 'sprite' for model asset '" + assetPath + "'");
						}
					}
					else
					{
						LOGGER_ERROR("Could not load shader '" + material.shaderId + "' for model asset '" + assetPath + "'");
					}
				}
				else
				{
					LOGGER_ERROR("Could not load material '" + model2D.materialName + "' for model asset '" + assetPath + "'");
				}
			}
			else
			{
				LOGGER_ERROR("Model data was corrupt for asset '" + assetPath + "'");
			}
		}
		else
		{
			LOGGER_ERROR("Invalid asset, '" + assetPath + "'");
		}

		return sceneObject;
	}
}