#include "puppetbox/DataStructures.h"
#include "AssetLibrary.h"

#include <utility>

#include "OpenGLModel.h"

namespace PB
{
	namespace
	{
		/**
		* \brief Structure used to help destruct virtual asset paths into separate archive and asset paths.
		*/
		struct AssetStruct
		{
			std::string archiveName;
			std::string assetName;
		};

		/**
		* \brief Helper function for placing values into a map for terser code.
		* 
		* \param key	The key to use for a value reference in the map.
		* \param value	The value to place at the specified key reference in the map.
		* \param map	The unordered_map to place the key-value pair into.
		*/
		template <class T>
		void insertIntoMap(std::string key, T& value, std::unordered_map<std::string, T>& map)
		{
			map.insert(
				std::pair<std::string, T> { key, value }
			);
		}

		/**
		* \brief Helper function to break up a given virtual asset path into it's separate archive and asset paths.
		* 
		* \param assetPath	The virtual asset path to destruct.
		* \param error		Flag indicating an error occured if set to True.
		* 
		* \return The AssetStruct containing the separated archive name and asset path, or an empty struct if
		* an error occured destructing the virtual asset path.
		*/
		AssetStruct parseAssetPath(const std::string& assetPath, bool* error)
		{
            std::uint32_t splitCount;
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

		/**
		* \brief Helper function to get the raw shader code for a given shader asset.
		* 
		* \param assetPath			The virtual asset path for the desired shader asset.
		* \param assetArchiveMap	The AssetArchive to search for the given shader asset.
		* \param error				Flag indicating an error occured if set to True.
		* 
		* \return A string containing the raw shader code of the requested shader asset, or an empty
		* string if an error occured fetching the asset.
		*/
		std::string loadShaderCode(const std::string& assetPath, std::unordered_map<std::string, AssetArchive>& assetArchiveMap, bool* error)
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

	AssetLibrary::AssetLibrary(std::string archiveRoot, std::shared_ptr<IGfxApi> gfxApi) : archiveRoot_(std::move(archiveRoot)), gfxApi_(std::move(gfxApi))
	{

	}

	void AssetLibrary::init()
	{
		auto* vData = new Vertex[]{
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

	bool AssetLibrary::loadArchive(const std::string& archiveName)
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

	bool AssetLibrary::loadAsset(const std::string& assetPath, SceneObject* sceneObject, Asset::Type type)
	{
		bool error = false;

		switch (type)
		{
		case Asset::Type::MODEL_2D:
		{
			if (load2DSceneObject(assetPath, sceneObject))
			{
				return true;
			}
			else
			{
				LOGGER_ERROR("Failed to load asset '" + assetPath + "'");
			}
			break;
		}
		default:
			LOGGER_ERROR("Can not load asset '" + assetPath + "' of undefined type");
		}

		return false;
	}

	Shader AssetLibrary::loadShaderAsset(const std::string& assetPath, bool* error)
	{
		Shader shader{ assetPath };

		if (loadedShaders_.find(assetPath) == loadedShaders_.end())
		{
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
				bool loaded;
				loaded = shader.loadVertexShader(vertexCode);
				loaded = loaded && shader.loadGeometryShader(geometryCode);
				loaded = loaded && shader.loadFragmentShader(fragmentCode);

				if (loaded)
				{
					if (shader.init())
					{
						insertIntoMap(assetPath, shader, loadedShaders_);
					}
					else
					{
						LOGGER_ERROR("Failed to compile shader program '" + assetPath + "'");
					}
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
		}
		else
		{
			shader = loadedShaders_.at(assetPath);
		}

		return shader;
	}

	ImageReference AssetLibrary::loadImageAsset(const std::string& assetPath, ImageOptions imageOptions, bool* error)
	{
		ImageReference imageReference{ 0 };

		if (loadedImages_.find(assetPath) == loadedImages_.end())
		{
			AssetStruct asset = parseAssetPath(assetPath, error);

			if (!*error)
			{
				ImageData imageData = assetArchives_.at(asset.archiveName).loadImageAsset(asset.assetName, error);
				imageReference = gfxApi_->loadImage(imageData, imageOptions);
				imageReference.width = imageData.width;
				imageReference.height = imageData.height;
				imageReference.requiresAlphaBlending = imageData.numChannels == 4;
                imageData.clear();

				if (!*error)
				{
					insertIntoMap(assetPath, imageReference, loadedImages_);
				}
				else
				{
					LOGGER_ERROR("Failed to load asset '" + assetPath + "'");
				}
			}
			else
			{
				LOGGER_ERROR("Invalid asset, '" + assetPath + "'");
			}
		}
		else
		{
			imageReference = loadedImages_.at(assetPath);
		}

		return imageReference;
	}

	Material AssetLibrary::loadMaterialAsset(const std::string& assetPath, bool* error)
	{
		Material material{};

		if (loadedMaterials_.find(assetPath) == loadedMaterials_.end())
		{
			AssetStruct asset = parseAssetPath(assetPath, error);

			if (!*error)
			{
				material = assetArchives_.at(asset.archiveName).loadMaterialAsset(asset.assetName, error);

				if (!*error)
				{
					if (!material.diffuseData.image.empty())
					{
						ImageReference imageReference = loadImageAsset(material.diffuseData.image, { ImageOptions::Mode::CLAMP_TO_BORDER }, error);

						if (!*error)
						{
							material.diffuseMap = imageReference;
							material.requiresAlphaBlending = imageReference.requiresAlphaBlending;
						}
						else
						{
							LOGGER_ERROR("Failed to load image, '" + material.diffuseData.image + "' for asset '" + assetPath + "'");
						}
					}

					insertIntoMap(assetPath, material, loadedMaterials_);
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
		}
		else
		{
			material = loadedMaterials_.at(assetPath);
		}

		return material;
	}

	Mesh AssetLibrary::loadMeshAsset(const std::string& assetPath, bool* error)
	{
		Mesh mesh{};

		if (loadedMeshes_.find(assetPath) == loadedMeshes_.end())
		{
			//TODO: Implement mesh assets
			*error = true;
			LOGGER_ERROR("Mesh '" + assetPath + "' does not exist");
		}
		else
		{
			mesh = loadedMeshes_.at(assetPath);
		}

		return mesh;
	}

	ModelData2D AssetLibrary::loadModelData2DAsset(const std::string& assetPath, bool* error)
	{
		ModelData2D data;

		if (loadedModelData2D_.find(assetPath) == loadedModelData2D_.end())
		{
			AssetStruct asset = parseAssetPath(assetPath, error);

			if (!*error)
			{
				data = assetArchives_.at(asset.archiveName).load2DModelAsset(asset.assetName, error);
			}
			else
			{
				LOGGER_ERROR("Invalid asset, '" + assetPath + "'");
			}
		}
		else
		{
			data = loadedModelData2D_.at(assetPath);
		}

		return data;
	}

	bool AssetLibrary::load2DSceneObject(const std::string& assetPath, SceneObject* sceneObject)
	{
		bool error = false;

		ModelData2D modelData2D = loadModelData2DAsset(assetPath, &error);

		if (!error)
		{
			IModel* model = nullptr;

			if (loadedIModels_.find(assetPath) == loadedIModels_.end())
			{
				if (!error)
				{
					Material material = loadMaterialAsset(modelData2D.materialName, &error);

					if (!error)
					{
						material.shader = loadShaderAsset(material.shaderId, &error);

						if (!error)
						{
							Mesh mesh = loadMeshAsset("sprite", &error);

							if (!error)
							{
                                std::vector<RenderedMesh> renderedMeshes{};
                                renderedMeshes.push_back(RenderedMesh{mesh, material});

								loadedIModels_.insert(
									std::pair<std::string, std::unique_ptr<IModel>> { assetPath, std::unique_ptr<IModel>{ new OpenGLModel(renderedMeshes)} }
								);

								model = &(*loadedIModels_.at(assetPath));
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
						LOGGER_ERROR("Could not load material '" + modelData2D.materialName + "' for model asset '" + assetPath + "'");
					}
				}
				else
				{
					LOGGER_ERROR("Invalid asset, '" + assetPath + "'");
				}
			}
			else
			{
				model = &(*loadedIModels_.at(assetPath));
			}
			
			/**
			* Make a copy of the previous SceneObject proprety data so that
			* it can be restored after a new instance is made.
			*/
			std::string id = sceneObject->id;
			vec3 position = sceneObject->position;
			vec3 rotation = sceneObject->rotation;
			vec3 scale = sceneObject->scale;
			float speed = sceneObject->speed;
			vec3 velocity = sceneObject->velocity;

			*sceneObject = SceneObject{ vec3{ static_cast<float>(modelData2D.width), static_cast<float>(modelData2D.height), 1.0f }, model };
			sceneObject->id = id;
			sceneObject->position = position;
			sceneObject->rotation = rotation;
			sceneObject->scale = scale;
			sceneObject->speed = speed;
			sceneObject->velocity = velocity;
		}
		else
		{
			LOGGER_ERROR("Model data was corrupt for asset '" + assetPath + "'");
		}

		return !error;
	}
}