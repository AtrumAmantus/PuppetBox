#include "puppetbox/DataStructures.h"
#include "AssetLibrary.h"

#include <utility>

#include "GfxMath.h"
#include "OpenGLModel.h"
#include "Rendered2DMesh.h"
#include "Utilities.h"

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
        template<class T>
        void insertIntoMap(std::string key, T& value, std::unordered_map<std::string, T>& map)
        {
            map.insert(
                    std::pair<std::string, T>{key, value}
            );
        }

        /**
        * \brief Helper function to break up a given virtual asset path into it's separate archive and asset paths.
        *
        * \param assetPath	The virtual asset path to destruct.
        * \param error		Flag indicating an error occurred if set to True.
        *
        * \return The AssetStruct containing the separated archive name and asset path, or an empty struct if
        * an error occurred destructing the virtual asset path.
        */
        AssetStruct parseAssetPath(const std::string& assetPath, bool* error)
        {
            std::uint32_t splitCount;
            std::string* splitValues = nullptr;
            StringUtils::split(assetPath, "/", 1, &splitValues, &splitCount);

            if (splitCount > 1)
            {
                return AssetStruct{splitValues[0], splitValues[1]};
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
        * \param error				Flag indicating an error occurred if set to True.
        *
        * \return A string containing the raw shader code of the requested shader asset, or an empty
        * string if an error occurred fetching the asset.
        */
        std::string loadShaderCode(
                const std::string& assetPath,
                std::unordered_map<std::string, AssetArchive>& assetArchiveMap,
                bool* error)
        {
            if (!StringUtils::trim(assetPath).empty())
            {
                AssetStruct asset = parseAssetPath(assetPath, error);

                if (!*error)
                {
                    std::string data = assetArchiveMap.at(asset.archiveName).loadAsciiData(asset.assetName, error);

                    if (!*error)
                    {
                        LOGGER_INFO("Shader '" + assetPath + "' read...");
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

    AssetLibrary::AssetLibrary(std::string archiveRoot, std::shared_ptr<IGfxApi> gfxApi, FontLoader* fontLoader)
            : archiveRoot_(std::move(archiveRoot)), gfxApi_(std::move(gfxApi)), fontLoader_(fontLoader)
    {

    }

    bool AssetLibrary::loadArchive(const std::string& archiveName)
    {
        bool error = false;

        if (assetArchives_.find(archiveName) == assetArchives_.end())
        {
            AssetArchive archive{archiveName, archiveRoot_};

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

        return !error;
    }

    Shader AssetLibrary::loadShaderAsset(const std::string& assetPath, bool* error)
    {
        Shader shader{assetPath};

        if (loadedShaders_.find(assetPath) == loadedShaders_.end())
        {
            AssetStruct asset = parseAssetPath(assetPath, error);

            if (!*error)
            {
                ShaderProgram program = assetArchives_.at(asset.archiveName).loadShaderAsset(asset.assetName, error);

                std::string vertexCode;
                std::string geometryCode;
                std::string fragmentCode;

                if (!*error)
                {
                    vertexCode = loadShaderCode(program.vertexShaderPath, assetArchives_, error);
                }

                if (!*error)
                {
                    geometryCode = loadShaderCode(program.geometryShaderPath, assetArchives_, error);
                }

                if (!*error)
                {
                    fragmentCode = loadShaderCode(program.fragmentShaderPath, assetArchives_, error);
                }

                shader = Shader{assetPath, program.vertexShaderPath, program.geometryShaderPath,
                                program.fragmentShaderPath};
                bool loaded;
                loaded = shader.loadVertexShader(vertexCode);
                loaded = loaded && shader.loadGeometryShader(geometryCode);
                loaded = loaded && shader.loadFragmentShader(fragmentCode);

                if (loaded)
                {
                    if (shader.init())
                    {
                        LOGGER_INFO("Shader program '" + assetPath + "' loaded.");
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

    bool AssetLibrary::loadAnimationSetAsset(
            const std::string& assetPath,
            std::unordered_map<std::string, IAnimation*>& animationMap
    )
    {
        bool error = false;

//        if (loadedAnimationSets_.find(assetPath) == loadedAnimationSets_.end())
//        {
        AssetStruct asset = parseAssetPath(assetPath, &error);

        if (!error)
        {
            std::unordered_map<std::string, std::string> animMap{};
            error = error || !assetArchives_.at(asset.archiveName).loadAnimationSetAsset(asset.assetName, animMap);

            if (!error)
            {
                for (const auto& entry: animMap)
                {
                    error = error || !loadAnimationAsset(entry.first, entry.second, animationMap);
                }
            }
            else
            {
                LOGGER_ERROR("Failed to load animation set, '" + assetPath + "'");
            }
        }
        else
        {
            LOGGER_ERROR("Invalid asset, '" + assetPath + "'");
        }
//        }

        return !error;
    }

    bool AssetLibrary::loadAnimationAsset(
            const std::string& animName,
            const std::string& assetPath,
            std::unordered_map<std::string, IAnimation*>& animationMap
    )
    {
        bool error = false;

        AssetStruct asset = parseAssetPath(assetPath, &error);

        if (!error)
        {
            error = error ||
                    !assetArchives_.at(asset.archiveName).loadAnimationAsset(animName, asset.assetName, animationMap);
        }
        else
        {
            LOGGER_ERROR("Invalid asset, '" + assetPath + "'");
        }

        return !error;
    }

    Font AssetLibrary::loadFontAsset(const std::string& fontPath, std::uint8_t fontSize, bool* error)
    {
        // TODO: Currently, the fontSize attribute only applies to the first time the font is loaded,
        //then it is ignored, since it's pulled from cache.  Maybe revisit this.
        Font font;

        if (loadedFonts_.find(fontPath) == loadedFonts_.end())
        {
            AssetStruct asset = parseAssetPath(fontPath, error);

            if (!*error)
            {
                font = assetArchives_
                        .at(asset.archiveName)
                        .loadFontAsset(asset.assetName, fontSize, fontLoader_, error);

                if (!*error)
                {
                    loadedFonts_.insert(
                            std::pair<std::string, Font>(fontPath, font)
                    );
                }
                else
                {
                    *error = true;
                    LOGGER_ERROR("Failed to load font asset, '" + fontPath + "'");
                }
            }
            else
            {
                *error = true;
                LOGGER_ERROR("Invalid font asset, '" + fontPath + "'");
            }
        }
        else
        {
            font = loadedFonts_.at(fontPath);
        }

        return font;
    }

    Mesh AssetLibrary::loadMeshAsset(const std::string& assetPath, bool* error)
    {
        Mesh mesh{};

        if (loadedMeshes_.find(assetPath) == loadedMeshes_.end())
        {
            AssetStruct asset = parseAssetPath(assetPath, error);

            if (!*error)
            {
                std::vector<Vertex> meshData = assetArchives_
                        .at(asset.archiveName)
                        .loadMeshDataAsset(asset.assetName, error);

                mesh = gfxApi_->loadMesh(&meshData[0], meshData.size());
                loadedMeshes_.insert(
                        std::pair<std::string, Mesh>{assetPath, mesh}
                );
            }
            else
            {
                LOGGER_ERROR("Invalid mesh asset, '" + assetPath + "'");
            }
        }
        else
        {
            mesh = loadedMeshes_.at(assetPath);
        }

        return mesh;
    }

    ImageReference AssetLibrary::loadImageAsset(const std::string& assetPath, ImageOptions imageOptions, bool* error)
    {
        ImageReference imageReference{0};

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
                        ImageReference imageReference = loadImageAsset(material.diffuseData.image,
                                                                       {ImageOptions::Mode::CLAMP_TO_BORDER}, error);

                        if (!*error)
                        {
                            material.diffuseMap = imageReference;
                            material.requiresAlphaBlending = imageReference.requiresAlphaBlending;
                        }
                        else
                        {
                            LOGGER_ERROR("Failed to load image, '" + material.diffuseData.image + "' for asset '" +
                                         assetPath + "'");
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

    ModelData AssetLibrary::loadModelDataAsset(const std::string& assetPath, bool* error)
    {
        ModelData data;

        if (loadedModelData_.find(assetPath) == loadedModelData_.end())
        {
            AssetStruct asset = parseAssetPath(assetPath, error);

            if (!*error)
            {
                data = assetArchives_.at(asset.archiveName).loadModelAsset(asset.assetName, error);
            }
            else
            {
                LOGGER_ERROR("Invalid asset, '" + assetPath + "'");
            }
        }
        else
        {
            data = loadedModelData_.at(assetPath);
        }

        return data;
    }

    bool AssetLibrary::loadSceneObject(
            const std::string& assetPath,
            SceneObject* sceneObject,
            UUID uuid,
            IAnimationCatalogue* animationCatalogue)
    {
        bool error = false;

        ModelData modelData = loadModelDataAsset(assetPath, &error);

        if (!error)
        {
            IModel* model;

            std::unordered_map<std::uint32_t, RenderedMesh*> meshes{};
            BoneMap bones{};
            error = !buildMeshAndBones(modelData, "", bones, meshes);

            if (!error)
            {
                model = new OpenGLModel{bones, meshes, animationCatalogue};

                /**
                * Make a copy of the previous SceneObject property data so that
                * it can be restored after a new instance is made.
                */
                vec3 position = sceneObject->position;
                vec3 rotation = sceneObject->rotation;
                vec3 scale = sceneObject->scale;
                float speed = sceneObject->speed;
                vec3 velocity = sceneObject->velocity;

                //TODO: Revisit "base scale"
                *sceneObject = SceneObject{uuid, vec3{1.0f, 1.0f, 1.0f}, model};
                sceneObject->position = position;
                sceneObject->rotation = rotation;
                sceneObject->scale = scale;
                sceneObject->speed = speed;
                sceneObject->velocity = velocity;
            }
            else
            {
                LOGGER_ERROR("Failed to load model data for asset '" + assetPath + "'");
            }
        }
        else
        {
            LOGGER_ERROR("Model data was corrupt for asset '" + assetPath + "'");
        }

        return !error;
    }

    bool AssetLibrary::buildMeshAndBones(
            ModelData modelData,
            const std::string& parentName,
            BoneMap& bones,
            std::unordered_map<std::uint32_t, RenderedMesh*>& meshes
    )
    {
        bool error = false;

        Bone bone{
                vec4{
                        modelData.offset.x,
                        modelData.offset.y,
                        modelData.offset.z,
                        1.0f
                },
                vec4{modelData.scale.x, modelData.scale.y, modelData.scale.z, 1.0f},
                vec4{modelData.rotation.x, modelData.rotation.y, modelData.rotation.z, 0.0f}
        };

        bone.transform = GfxMath::CreateTransformation(
                bone.rotation.vec3(),
                bone.scale.vec3(),
                bone.position.vec3()
        );

        std::uint32_t boneId = bones.addBone(modelData.name, parentName, bone);

        if (!modelData.mesh.dataPath.empty())
        {
            Mesh mesh = loadMeshAsset(modelData.mesh.dataPath, &error);

            if (!error)
            {
                if (!modelData.mesh.materialPath.empty())
                {
                    Material material = loadMaterialAsset(modelData.mesh.materialPath, &error);

                    if (!error)
                    {
                        material.shader = loadShaderAsset(material.shaderId, &error);

                        if (!error)
                        {
                            mesh.scale = vec3{modelData.mesh.scale.x, modelData.mesh.scale.y,
                                              modelData.mesh.scale.z};
                            mesh.offset = vec3{modelData.mesh.offset.x, modelData.mesh.offset.y,
                                               modelData.mesh.offset.z};

                            mat4 meshOffset = mat4::eye();
                            meshOffset[3] = vec4{mesh.offset.x, mesh.offset.y, mesh.offset.z, 1.0f};
                            mat4 meshScale = mat4::eye();
                            meshScale[0][0] = mesh.scale.x;
                            meshScale[1][1] = mesh.scale.y;
                            meshScale[2][2] = mesh.scale.z;

                            mesh.transform = meshOffset * meshScale;

                            meshes.insert(
                                    std::pair<std::uint32_t , RenderedMesh*>{
                                        boneId,
                                        new Rendered2DMesh(mesh, material)}
                            );
                        }
                        else
                        {
                            LOGGER_ERROR("Could not load shader '" + material.shaderId + "' for model asset");
                        }
                    }
                    else
                    {
                        LOGGER_ERROR("Could not load material '" + modelData.mesh.materialPath + "' for model asset");
                    }
                }
            }
            else
            {
                LOGGER_ERROR("Could not load mesh '" + modelData.mesh.dataPath + "' for model asset");
            }
        }

        for (auto itr = modelData.children.begin(); !error && itr != modelData.children.end(); ++itr)
        {
            error = !buildMeshAndBones(
                    modelData.children.at(itr->first),
                    modelData.name,
                    bones,
                    meshes
            );
        }

        return !error;
    }
}