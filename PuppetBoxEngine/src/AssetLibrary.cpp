#include "puppetbox/DataStructures.h"
#include "AssetLibrary.h"

#include <utility>

#include "../generated/DefaultAssets.h"

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
            AssetStruct asset{};

            std::uint32_t splitCount;
            std::string* splitValues = nullptr;
            StringUtils::split(assetPath, "/", 1, &splitValues, &splitCount);

            if (splitCount > 1)
            {
                if (StringUtils::toLowerCase(splitValues[0]) != "default")
                {
                    asset = {splitValues[0], splitValues[1]};
                }
                else
                {
                    *error = true;
                    LOGGER_ERROR("Use of reserved asset path, '" + assetPath + "', but this is not a default resource.");
                }
            }
            else
            {
                *error = true;
                LOGGER_ERROR("Invalid asset path name, '" + assetPath + "'");
            }

            return asset;
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

            // Bind everything to a root node
            //TODO: These probably shouldn't be on the default sprite mesh?
            std::vector<std::vector<BoneWeight>> meshBoneWeights{};

            for (std::uint32_t i = 0; i < 6; ++i)
            {
                std::vector<BoneWeight> vertexBoneWeights{};
                vertexBoneWeights.push_back({0, 1.0f});
                meshBoneWeights.push_back(std::move(vertexBoneWeights));
            }

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

            return gfxApi->loadMesh(&spriteMeshVertices[0], spriteMeshVertices.size(), std::move(meshBoneWeights));
        }

        std::uint32_t loadDefaultGlyphShader(const std::shared_ptr<IGfxApi>& gfxApi, bool* error)
        {
            std::uint32_t shaderId;

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
                    shaderId = gfxApi->loadShader(shader);
                }
                else
                {
                    *error = true;
                    shaderId = 0;
                    LOGGER_ERROR("Failed to compile shader program '" + defaultAssetPath + "'");
                }
            }
            else
            {
                *error = true;
                shaderId = 0;
                LOGGER_ERROR("Failed to load shader '" + defaultAssetPath + "'");
            }

            return shaderId;
        }

        std::uint32_t loadDefaultBasicShader(const std::shared_ptr<IGfxApi>& gfxApi, bool* error)
        {
            std::uint32_t shaderId;

            const std::string defaultAssetPath = "Default/Shader/Basic";

            Shader shader = Shader{
                    defaultAssetPath,
                    defaultAssetPath + "/Vertex",
                    "",
                    defaultAssetPath + "/Fragment"};

            bool loaded;
            loaded = shader.loadVertexShader(DEFAULT_ASSET_BASIC_VERTEX_SHADER);
            loaded = loaded && shader.loadFragmentShader(DEFAULT_ASSET_BASIC_FRAGMENT_SHADER);

            if (loaded)
            {
                if (shader.init())
                {
                    LOGGER_INFO("Shader program '" + defaultAssetPath + "' loaded.");
                    shaderId = gfxApi->loadShader(shader);
                }
                else
                {
                    *error = true;
                    shaderId = 0;
                    LOGGER_ERROR("Failed to compile shader program '" + defaultAssetPath + "'");
                }
            }
            else
            {
                *error = true;
                shaderId = 0;
                LOGGER_ERROR("Failed to load shader '" + defaultAssetPath + "'");
            }

            return shaderId;
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
                std::pair<std::string, std::uint32_t>{"Default/Shader/UI/Glyph", loadDefaultGlyphShader(gfxApi_, &error)}
        );

        loadedShaders_.insert(
                std::pair<std::string, std::uint32_t>{"Default/Shader/Basic", loadDefaultBasicShader(gfxApi_, &error)}
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

    BoneMap AssetLibrary::loadSkeletonAsset(const std::string& assetPath, bool* error)
    {
        BoneMap boneMap{};

        const auto& itr = loadedSkeletons_.find(assetPath);

        if (itr == loadedSkeletons_.end())
        {
            AssetStruct asset = parseAssetPath(assetPath, error);

            if (!*error)
            {
                boneMap = assetArchives_.at(asset.archiveName).loadSkeletonAsset(asset.assetName, error);

                loadedSkeletons_.insert(
                        std::pair<std::string, BoneMap>{assetPath, boneMap}
                );
            }
            else
            {
                LOGGER_ERROR("Invalid asset, '" + assetPath + "'");
            }
        }
        else
        {
            boneMap = itr->second;
        }

        return std::move(boneMap);
    }

    std::uint32_t AssetLibrary::loadMeshAsset(const std::string& assetPath, bool* error)
    {
        std::uint32_t meshReferenceId;

        if (loadedMeshes_.find(assetPath) == loadedMeshes_.end())
        {
            AssetStruct asset = parseAssetPath(assetPath, error);

            if (!*error)
            {
                std::vector<Vertex> meshData = assetArchives_
                        .at(asset.archiveName)
                        .loadMeshDataAsset(asset.assetName, error);

                //TODO: These needs to be finished implementing (add to mesh binaries)
                std::vector<std::vector<BoneWeight>> boneWeights(meshData.size());
                meshReferenceId = gfxApi_->loadMesh(&meshData[0], meshData.size(), boneWeights);

                loadedMeshes_.insert(
                        std::pair<std::string, std::uint32_t>{assetPath, meshReferenceId}
                );
            }
            else
            {
                meshReferenceId = 0;
                LOGGER_ERROR("Invalid mesh asset, '" + assetPath + "'");
            }
        }
        else
        {
            meshReferenceId = loadedMeshes_.at(assetPath);
        }

        return meshReferenceId;
    }

    std::uint32_t AssetLibrary::loadImageMapAsset(const std::string& assetPath, bool* error)
    {
        std::uint32_t imageMapReferenceId;

        auto itr = loadedImageMaps_.find(assetPath);

        if (itr == loadedImageMaps_.end())
        {
            AssetStruct asset = parseAssetPath(assetPath, error);

            if (!*error)
            {
                ImageData imageData = assetArchives_.at(asset.archiveName).loadImageAsset(asset.assetName, error);
                imageMapReferenceId = gfxApi_->loadImage(imageData);
                imageData.clear();

                if (!*error)
                {
                    loadedImageMaps_.insert(
                            std::pair<std::string, std::uint32_t>{assetPath, imageMapReferenceId}
                    );
                }
                else
                {
                    imageMapReferenceId = 0;
                    LOGGER_ERROR("Failed to load asset '" + assetPath + "'");
                }
            }
            else
            {
                imageMapReferenceId = 0;
                LOGGER_ERROR("Invalid asset, '" + assetPath + "'");
            }
        }
        else
        {
            imageMapReferenceId = itr->second;
        }

        return imageMapReferenceId;
    }

    std::uint32_t AssetLibrary::loadShaderAsset(const std::string& assetPath, bool* error)
    {
        std::uint32_t shaderReferenceId;

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

                Shader shader = Shader{assetPath, program.vertexShaderPath, program.geometryShaderPath,
                                program.fragmentShaderPath};
                bool loaded;
                loaded = shader.loadVertexShader(vertexCode);
                loaded = loaded && shader.loadGeometryShader(geometryCode);
                loaded = loaded && shader.loadFragmentShader(fragmentCode);

                if (loaded)
                {
                    if (shader.init())
                    {
                        shaderReferenceId = gfxApi_->loadShader(shader);

                        loadedShaders_.insert(
                                std::pair<std::string, std::uint32_t>{assetPath, shaderReferenceId}
                        );
                        LOGGER_INFO("Shader program '" + assetPath + "' loaded.");
                    }
                    else
                    {
                        shaderReferenceId = 0;
                        LOGGER_ERROR("Failed to compile shader program '" + assetPath + "'");
                    }
                }
                else
                {
                    shaderReferenceId = 0;
                    LOGGER_ERROR("Failed to load shader '" + assetPath + "'");
                }
            }
            else
            {
                shaderReferenceId = 0;
                LOGGER_ERROR("Invalid asset, '" + assetPath + "'");
            }
        }
        else
        {
            shaderReferenceId = loadedShaders_.at(assetPath);
        }

        return shaderReferenceId;
    }

    bool AssetLibrary::loadAnimationAsset(
            const std::string& assetPath,
            std::unordered_map<std::string, IAnimation*>& animationMap
    )
    {
        bool error = false;

        AssetStruct asset = parseAssetPath(assetPath, &error);

        if (!error)
        {
            error = error ||
                    !assetArchives_.at(asset.archiveName).loadAnimationAsset(asset.assetName, animationMap);
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
}