#pragma once

#include <fstream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

#include <STBI/stb_image.h>

#include "puppetbox/IAnimationCatalogue.h"

#include "FontLoader.h"
#include "ImageData.h"
#include "Logger.h"
#include "Material.h"
#include "Utilities.h"

namespace PB
{
    enum MeshType
    {
        SPRITE
    };

    struct MeshData
    {
        MeshType type = SPRITE;
        vec2 offset{0, 0};
        std::string materialPath;
    };

    /**
    * \brief Struct for holding 2D Model specific data for simple communication between archive & library.
    */
    struct ModelData2D
    {
        vec3 offset{0, 0, 0};
        vec2 baseScale{1, 1};
        vec2 scale{1, 1};
        MeshData mesh{};
        std::string name;
        std::unordered_map<std::string, ModelData2D> children{};
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
        AssetArchive(std::string archiveName, std::string archiveRoot) : archiveName_(std::move(archiveName)),
                                                                         archiveRoot_(std::move(archiveRoot)) {};

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
        * \param error		Flag indicating an error occurred if set to True.
        *
        * \return A string containing the raw contents of the requested asset, or an empty string if an
        * error occurred.
        */
        std::string loadAsciiData(const std::string& assetPath, bool* error);

        /**
        * \brief Returns a ShaderProgram for the given shader asset.
        *
        * \param assetPath	The virtual asset path of the desired asset.
        * \param error		Flag indicating an error occurred if set to True.
        *
        * \return A ShaderProgram of the requested asset, or an empty object if an error occurred.
        */
        ShaderProgram loadShaderAsset(const std::string& assetPath, bool* error);

        /**
         * \brief Loads a set of animation assets, sets are a collection of animations associated with
         * a particular skeleton.
         *
         * \param assetPath Path to the animation set.
         * \param map       The map to add the animations of the animation set to.
         * \return True if the animation set was loaded successfully, False otherwise.
         */
        bool loadAnimationSetAsset(const std::string& assetPath, std::unordered_map<std::string, std::string>& map);

        /**
         * \brief Loads an individual animation asset.
         *
         * \param name      The desired reference name for the animation.
         * \param assetPath The path to the desired animation.
         * \param map       The map to add the animation to, using the given name.
         * \return True if the animation was loaded and added to the map successfully, False otherwise.
         */
        bool loadAnimationAsset(const std::string& name, const std::string& assetPath,
                                std::unordered_map<std::string, IAnimation*>& map);

        /**
         * \brief Load the given font asset.
         *
         * <p>On the first load of a font, the given size dictates how large to create it's associated
         * glyph images.  Future calls used these cached images and the requested size is ignored.</p>
         *
         * \param assetPath  The path of the font to load.
         * \param fontSize   The size to load the font at.
         * \param fontLoader The {\link FontLoader} to use to load the font with.
         * \param error      Error flag to indicate if there was an issue loading the font.
         * \return The {\link Font} object associated with the loaded font.  Returns an uninitialized {\link Font}
         * object if the font failed to load.
         */
        Font loadFontAsset(const std::string& assetPath, std::uint8_t fontSize, FontLoader* fontLoader, bool* error);

        /**
         * Loads mesh data assets as a {\link std::vector} of {\link Vertex} elements.
         *
         * \param assetPath The path to the desired mesh data to load.
         * \param error     Error flag to indicate if there was an issue loading the mesh data.
         * \return A {\link std::vector} of {\link Vertex} elements holding the requested mesh data.
         */
        std::vector<Vertex> loadMeshDataAsset(const std::string& assetPath, bool* error);

        /**
        * \brief Returns a ImageData for the given shader asset.
        *
        * \param assetPath	The virtual asset path of the desired asset.
        * \param error		Flag indicating an error occurred if set to True.
        *
        * \return A ImageData of the requested asset, or an empty object if an error occurred.
        */
        ImageData loadImageAsset(const std::string& assetPath, bool* error);

        /**
        * \brief Returns a Material for the given shader asset.
        *
        * \param assetPath	The virtual asset path of the desired asset.
        * \param error		Flag indicating an error occurred if set to True.
        *
        * \return A Material of the requested asset, or an empty object if an error occurred.
        */
        Material loadMaterialAsset(const std::string& assetPath, bool* error);

        /**
        * \brief Returns a Model2D for the given shader asset.
        *
        * \param assetPath	The virtual asset path of the desired asset.
        * \param error		Flag indicating an error occurred if set to True.
        *
        * \return A Model2D of the requested asset, or an empty object if an error occurred.
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