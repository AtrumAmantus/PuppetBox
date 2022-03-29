#include "puppetbox/DataStructures.h"
#include "AssetLibrary.h"

#include <utility>

#include "../generated/DefaultAssets.h"

#include "GfxMath.h"
#include "OpenGLModel.h"
#include "Rendered2DMesh.h"
#include "puppetbox/Utilities.h"

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

        std::uint32_t loadDefaultSpriteMesh(const std::shared_ptr<IGfxApi>& gfxApi)
        {
            float spriteMeshData[] = {
                    -0.5,  0.5, 0.0,    0.0, 0.0, 1.0,      0.0, 1.0,
                    -0.5, -0.5, 0.0,    0.0, 0.0, 1.0,      0.0, 0.0,
                    0.5, -0.5, 0.0,     0.0, 0.0, 1.0,      1.0, 0.0,
                    -0.5,  0.5, 0.0,    0.0, 0.0, 1.0,      0.0, 1.0,
                    0.5, -0.5, 0.0,     0.0, 0.0, 1.0,      1.0, 0.0,
                    0.5,  0.5, 0.0,     0.0, 0.0, 1.0,      1.0, 1.0
            };

            std::vector<Vertex> spriteMeshVertices{};

            std::uint32_t dataSize = sizeof(spriteMeshData) / sizeof(float);

            for (std::uint32_t i = 0; i < dataSize; i += 8)
            {
                spriteMeshVertices.push_back(Vertex {
                        {spriteMeshData[i], spriteMeshData[i + 1], spriteMeshData[i + 2]},
                        {spriteMeshData[i + 3], spriteMeshData[i + 4], spriteMeshData[i + 5]},
                        {spriteMeshData[i + 6], spriteMeshData[i + 7]}
                });
            }

            return gfxApi->loadMesh(&spriteMeshVertices[0], spriteMeshVertices.size());
        }

        Shader loadDefaultGlyphShader(const std::shared_ptr<IGfxApi>& gfxApi, bool* error)
        {
            const std::string defaultAssetPath = "Default/Shader/UI/Glyph";

            Shader shader = Shader{
                    defaultAssetPath,
                    defaultAssetPath + "/Vertex",
                    defaultAssetPath + "/Geometry",
                    defaultAssetPath + "/Fragment"};

            bool loaded;
            loaded = shader.loadVertexShader(DEFAULT_ASSET_UI_GLYPH_VERTEX_SHADER);
            loaded = loaded && shader.loadGeometryShader(DEFAULT_ASSET_UI_GLYPH_GEOMETRY_SHADER);
            loaded = loaded && shader.loadFragmentShader(DEFAULT_ASSET_UI_GLYPH_FRAGMENT_SHADER);

            if (loaded)
            {
                if (shader.init())
                {
                    LOGGER_INFO("Shader program '" + defaultAssetPath + "' loaded.");
                }
                else
                {
                    LOGGER_ERROR("Failed to compile shader program '" + defaultAssetPath + "'");
                }
            }
            else
            {
                *error = true;
                LOGGER_ERROR("Failed to load shader '" + defaultAssetPath + "'");
            }

            return shader;
        }
    }

    AssetLibrary::AssetLibrary(std::string archiveRoot, std::shared_ptr<IGfxApi> gfxApi, FontLoader* fontLoader)
            : archiveRoot_(std::move(archiveRoot)), gfxApi_(std::move(gfxApi)), fontLoader_(fontLoader)
    {

    }

    bool AssetLibrary::init()
    {
        bool error = false;

        loadedMeshes_.insert(
                std::pair<std::string, std::uint32_t>{"Default/Mesh/Sprite", loadDefaultSpriteMesh(gfxApi_)}
        );

        loadedShaders_.insert(
                std::pair<std::string, Shader>{"Default/Shader/UI/Glyph", loadDefaultGlyphShader(gfxApi_, &error)}
        );

        return !error;
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

    BoneMap AssetLibrary::loadSkeleton(const std::string& assetPath)
    {
        bool error = false;

        BoneMap boneMap{};

        const auto& itr = loadedSkeletons_.find(assetPath);

        if (itr == loadedSkeletons_.end())
        {
            AssetStruct asset = parseAssetPath(assetPath, &error);

            if (!error)
            {
                assetArchives_.at(asset.archiveName).loadSkeleton(asset.assetName, &error);
            }
            else
            {
                LOGGER_ERROR("Invalid asset, '" + assetPath + "'");
            }
        }
        else
        {
            boneMap = BoneMap{itr->second};
        }

        return boneMap;
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
                        loadedShaders_.insert(
                                std::pair<std::string, Shader>{assetPath, shader}
                        );
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
                SizedArray<std::uint8_t> fontBytes = assetArchives_
                        .at(asset.archiveName)
                        .loadAssetBytes(asset.assetName, error);

                font = fontLoader_->loadFontFromBytes(fontBytes, fontSize, error);

                delete[] fontBytes.array;

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

    std::uint32_t AssetLibrary::loadMeshAsset(const std::string& assetPath, bool* error)
    {
        std::uint32_t referenceId;

        if (loadedMeshes_.find(assetPath) == loadedMeshes_.end())
        {
            AssetStruct asset = parseAssetPath(assetPath, error);

            if (!*error)
            {
                std::vector<Vertex> meshData = assetArchives_
                        .at(asset.archiveName)
                        .loadMeshDataAsset(asset.assetName, error);

                referenceId = gfxApi_->loadMesh(&meshData[0], meshData.size());

                loadedMeshes_.insert(
                        std::pair<std::string, std::uint32_t>{assetPath, referenceId}
                );
            }
            else
            {
                LOGGER_ERROR("Invalid mesh asset, '" + assetPath + "'");
            }
        }
        else
        {
            referenceId = loadedMeshes_.at(assetPath);
        }

        return referenceId;
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
                    loadedImages_.insert(
                            std::pair<std::string, ImageReference>{assetPath, imageReference}
                    );
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

                    loadedMaterials_.insert(
                            std::pair<std::string, Material>{assetPath, material}
                    );
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
}