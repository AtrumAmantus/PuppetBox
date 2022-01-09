#include <utility>

#include <STBI/stb_image.h>

#include "puppetbox/IAnimationCatalogue.h"
#include "AnimationCatalogue.h"
#include "AssetArchive.h"
#include "PropertyTree.h"

namespace PB
{
    namespace
    {
        /**
        * \brief Helper function to return a default value if a given key is not in the given unordered_map.
        *
        * \param propertyName	The key that is expected to referenced the desired data in the map.
        * \param properties		The unordered_map from which the key will be used to acquire data.
        * \param defaultValue	The default value to use if the given unordered_map does not contain the given key.
        *
        * \return Either the value from the map referenced with the given key, or the given default value if the key did not exist.
        */
        std::string
        defaultIfNotInMap(const std::string& propertyName, std::unordered_map<std::string, std::string> properties,
                          std::string defaultValue)
        {
            if (properties.find(propertyName) != properties.end())
            {
                return properties.at(propertyName);
            }

            return defaultValue;
        }

        /**
        * \brief Helper function to return a default value if a given key is not in the given PropertyTree
        *
        * \param propertyName	The key that is expected to referenced the desired data in the PropertyTree.
        * \param properties		The PropertyTree from which the key will be used to acquire data.
        * \param defaultValue	The default value to use if the given PropertyTree does not contain the given key.
        *
        * \return Either the value from the PropertyTree referenced with the given key, or the given default value if the key did not exist.
        */
        std::string defaultIfNotInTree(std::string propertyName, PropertyTree& pTree, std::string defaultValue)
        {
            std::string value;
            PropertyTree* childNode = pTree.get(std::move(propertyName));

            if (childNode != nullptr)
            {
                value = childNode->value();

                if (value.empty())
                {
                    return defaultValue;
                }
            }

            return value;
        }

        /**
        * \brief Helper function to map a properties map to a Model2D object.
        *
        * \param properties	The properties map to use to map to a Model2D object.
        * \param bool		Flag indicating an error occurred if set to True.
        *
        * \return The Model2D object built from the given properties map.
        */
        ModelData2D mapToModel2D(PropertyTree& rootProperties, bool* error)
        {
            ModelData2D model{};

            if (rootProperties.has("scale"))
            {
                PropertyTree* scaleProperties = rootProperties.get("scale");
                model.scale.x = NumberUtils::parseValue(defaultIfNotInTree("x", *scaleProperties, "0").c_str(), 0,
                                                        error);
                model.scale.y = NumberUtils::parseValue(defaultIfNotInTree("y", *scaleProperties, "0").c_str(), 0,
                                                        error);
            }

            if (rootProperties.has("offset"))
            {
                PropertyTree* offsetProperties = rootProperties.get("offset");
                model.offset.x = NumberUtils::parseValue(defaultIfNotInTree("x", *offsetProperties, "0").c_str(), 0,
                                                         error);
                model.offset.y = NumberUtils::parseValue(defaultIfNotInTree("y", *offsetProperties, "0").c_str(), 0,
                                                         error);
                model.offset.z = NumberUtils::parseValue(defaultIfNotInTree("z", *offsetProperties, "0").c_str(), 0,
                                                         error);
                //TODO: Fix this hack
                model.offset.z *= 0.1;
            }

            PropertyTree* meshProperties = rootProperties.get("mesh");
            //TODO: Make this not hardcoded.
            model.mesh.type = SPRITE;
            model.mesh.materialPath = meshProperties->get("material")->value();

            PropertyTree* meshOffsetProperties = meshProperties->get("offset");
            model.mesh.offset.x = NumberUtils::parseValue(defaultIfNotInTree("x", *meshOffsetProperties, "0").c_str(),
                                                          0, error);
            model.mesh.offset.y = NumberUtils::parseValue(defaultIfNotInTree("y", *meshOffsetProperties, "0").c_str(),
                                                          0, error);

            model.name = rootProperties.name();

            if (rootProperties.has("children"))
            {
                PropertyTree* childrenProperties = rootProperties.get("children");

                for (auto& childName: childrenProperties->children())
                {
                    ModelData2D child = mapToModel2D(*childrenProperties->get(childName), error);

                    model.children[childName] = child;
                }
            }

            if (*error)
            {
                LOGGER_ERROR("Failed to parse values for Model2D object");
                return {};
            }

            return model;
        }

        /**
        * \brief Helper function to map a PropertyTree to a Material object.
        *
        * \param properties	The PropertyTree to use to map to a Material object.
        * \param bool		Flag indicating an error occurred if set to True.
        *
        * \return The Material object built from the given properties map.
        */
        Material mapToMaterial(PropertyTree& materialProperties, bool* error)
        {
            Material material;

            PropertyTree* diffuseProperties = materialProperties.get("diffuse");
            std::string image = defaultIfNotInTree("image", *diffuseProperties, "");
            std::uint32_t width = NumberUtils::parseValue(defaultIfNotInTree("width", *diffuseProperties, "0").c_str(),
                                                          0, error);
            std::uint32_t height = NumberUtils::parseValue(
                    defaultIfNotInTree("height", *diffuseProperties, "0").c_str(), 0, error);
            std::uint32_t xOffset = NumberUtils::parseValue(
                    defaultIfNotInTree("xOffset", *diffuseProperties, "0").c_str(), 0, error);
            std::uint32_t yOffset = NumberUtils::parseValue(
                    defaultIfNotInTree("yOffset", *diffuseProperties, "0").c_str(), 0, error);

            std::string shader = defaultIfNotInTree("shader", materialProperties, "");

            if (*error)
            {
                LOGGER_ERROR("Failed to parse values for Material Asset");
                return {};
            }

            material.diffuseData = {
                    image,
                    width,
                    height,
                    xOffset,
                    yOffset
            };
            material.shaderId = shader;

            return material;
        }

        std::unordered_map<std::string, BoneMap>
        mapToBones(PropertyTree* pTree, const std::string& parentName, bool* error)
        {
            std::unordered_map<std::string, BoneMap> boneMap{};

            if (!pTree->children().empty())
            {
                for (auto& p: pTree->children())
                {
                    auto childMap = mapToBones(pTree->get(p), pTree->name(), error);
                    boneMap.insert(childMap.begin(), childMap.end());
                }
            }
            boneMap.insert(
                    std::pair<std::string, BoneMap>{pTree->name(), BoneMap{pTree->name(), parentName}}
            );

            return boneMap;
        }

        Keyframe mapToKeyframe(PropertyTree* pTree, bool* error)
        {
            Keyframe keyframe{};

            keyframe.boneName = pTree->name();

            if (pTree->has("position"))
            {
                auto position = pTree->get("position");
                keyframe.position.x = NumberUtils::parseValue(
                        defaultIfNotInTree("x", *position, "0").c_str(),
                        0, error
                );
                keyframe.position.y = NumberUtils::parseValue(
                        defaultIfNotInTree("y", *position, "0").c_str(),
                        0, error
                );
                keyframe.position.z = NumberUtils::parseValue(
                        defaultIfNotInTree("z", *position, "0").c_str(),
                        0, error
                );
            }

            if (pTree->has("rotation"))
            {
                auto rotation = pTree->get("rotation");
                keyframe.rotation.x = NumberUtils::parseValue(
                        defaultIfNotInTree("x", *rotation, "0").c_str(),
                        0, error
                );
                keyframe.rotation.y = NumberUtils::parseValue(
                        defaultIfNotInTree("y", *rotation, "0").c_str(),
                        0, error
                );
                keyframe.rotation.z = NumberUtils::parseValue(
                        defaultIfNotInTree("z", *rotation, "0").c_str(),
                        0, error
                );
            }

            if (pTree->has("scale"))
            {
                auto scale = pTree->get("scale");
                keyframe.scale.x = NumberUtils::parseValue(
                        defaultIfNotInTree("x", *scale, "1").c_str(),
                        1, error
                );
                keyframe.scale.y = NumberUtils::parseValue(
                        defaultIfNotInTree("y", *scale, "1").c_str(),
                        1, error
                );
                keyframe.scale.z = NumberUtils::parseValue(
                        defaultIfNotInTree("z", *scale, "1").c_str(),
                        1, error
                );
            }
            else
            {
                keyframe.scale = {1, 1, 1};
            }

            return keyframe;
        }

        std::vector<Keyframe> mapToKeyframeVector(PropertyTree* pTree, const std::uint8_t frameIndex, bool* error)
        {
            std::vector<Keyframe> keyframes{};

            for (auto& childName: pTree->children())
            {
                if (!*error)
                {
                    Keyframe keyframe = mapToKeyframe(pTree->get(childName), error);

                    keyframe.frameIndex = frameIndex;

                    if (!*error)
                    {
                        keyframes.push_back(keyframe);
                    }
                    else
                    {
                        LOGGER_ERROR("Invalid keyframe data for bone '" + childName + "'");
                    }
                }
            }

            return keyframes;
        }

        std::unordered_map<std::uint8_t, std::vector<Keyframe>> mapToKeyframes(PropertyTree* pTree, bool* error)
        {
            std::unordered_map<std::uint8_t, std::vector<Keyframe>> keyframes{};

            if (!pTree->children().empty())
            {
                for (auto& childName: pTree->children())
                {
                    if (!*error)
                    {
                        std::uint8_t frameIndex = NumberUtils::parseValue(childName.c_str(), 0, error);

                        if (!*error)
                        {
                            auto keyframeVector = mapToKeyframeVector(pTree->get(childName), frameIndex, error);
                            keyframes.insert(
                                    std::pair<std::uint8_t, std::vector<Keyframe>>{frameIndex, keyframeVector}
                            );
                        }
                        else
                        {
                            LOGGER_ERROR("Failed to parse data for keyframe '" + childName + "'");
                        }
                    }
                }
            }
            else
            {
                *error = true;
                LOGGER_ERROR("No defined keyframes for animation");
            }

            return keyframes;
        }

        /**
        * \brief Helper function to map a properties map to a Shader object.
        *
        * \param shaderProperties	The properties tree to use to map to a Shader object.
        * \param bool		Flag indicating an error occurred if set to True.
        *
        * \return The Material object built from the given properties map.
        */
        ShaderProgram mapToShaderProgram(PropertyTree& shaderProperties, bool* error)
        {
            ShaderProgram shaderProgram{};

            shaderProgram.vertexShaderPath = defaultIfNotInTree("vertex", shaderProperties, "");
            shaderProgram.fragmentShaderPath = defaultIfNotInTree("fragment", shaderProperties, "");
            shaderProgram.geometryShaderPath = defaultIfNotInTree("geometry", shaderProperties, "");

            if (shaderProgram.vertexShaderPath == "" || shaderProgram.fragmentShaderPath == "")
            {
                *error = true;
                LOGGER_ERROR("Failed to load shader program, invalid vertex/fragment shader");
            }

            return shaderProgram;
        }

        /**
        * \brief Helper function to acquire the filename associated with the given virtual asset path.
        *
        * \param assetPath			The virtual asset path of the desired asset.
        * \param archiveAssetIds	The unordered_map containing the virtual path -> filename associations.
        * \param archiveAssets		The unordered_set containing all existing assets contained in the AssetArchive.
        *
        * \return The filename for the given virtual asset path if found, or a blank string otherwise.
        */
        std::string fileNameOfAsset(
                const std::string& assetPath,
                std::unordered_map<std::string, std::string>& archiveAssetIds,
                std::unordered_set<std::string>& archiveAssets
        )
        {
            if (archiveAssetIds.find(assetPath) != archiveAssetIds.end())
            {
                std::string fileName = archiveAssetIds.at(assetPath);

                if (archiveAssets.find(fileName) != archiveAssets.end())
                {
                    return fileName;
                }
                else
                {
                    LOGGER_ERROR("Defined asset '" + assetPath + "' could not be found");
                }
            }
            else
            {
                LOGGER_ERROR("Unknown asset '" + assetPath + "'");
            }

            return "";
        }

        /**
        * \brief Helper function that create a property map from a stream object.  Assumes whitespace
        * delimited key-value pairs within the stream.
        *
        * \param stream		The stream to read key-value pair data from.
        * \param properties	Pointer to the unordered_map to store parsed properties in.
        *
        * \return True if the properties were successfully read from the stream, False otherwise.
        */
        bool getPropertiesFromStream(std::istream* stream, std::unordered_map<std::string, std::string>* properties)
        {
            std::string line;
            std::uint32_t lineNumber = 0;

            while (std::getline(*stream, line))
            {
                lineNumber++;
                StringUtils::trim(&line);

                std::string* splitValues;
                std::uint32_t splitCount;

                StringUtils::split(line, &splitValues, &splitCount);

                if (splitCount == 2)
                {
                    properties->insert(
                            std::pair<std::string, std::string>(splitValues[0], splitValues[1])
                    );
                }
                else
                {
                    LOGGER_ERROR(
                            "Invalid property data in stream '" + line + "' on line " + std::to_string(lineNumber));
                    return false;
                }
            }

            return true;
        }

        /**
        * \brief Helper function to count the indentation level of the given string.
        *
        * \param line	The string to calculate the indentation level for.
        *
        * \return The number of indentations prefixed on the given string.
        */
        std::uint32_t countIndents(const std::string& line)
        {
            std::uint32_t indentCount = 0;

            for (std::size_t i = 0; i < line.length() && line.c_str()[i] == ' '; ++i)
            {
                indentCount++;
            }

            return indentCount;
        }

        /**
        * \brief Helper function that attempts to parse the given stream into a PropertyTree.
        *
        * \param stream		The stream of data to parse.
        * \param root		The root of the PropertyTree to parse the data into.
        *
        * \return True if the data was successfully parsed, False otheriwse.
        */
        bool getPropertyTreeFromStream(std::istream* stream, PropertyTree* root)
        {
            bool success = true;
            std::string line;
            std::uint32_t lineNumber = 0;
            PropertyTree* currentNode = root;
            std::uint32_t indentLevel;
            std::uint32_t minIndents = 0;
            std::uint32_t maxIndents = 0;

            while (std::getline(*stream, line))
            {
                lineNumber++;
                indentLevel = countIndents(line);
                line = StringUtils::trim(line);

                if (indentLevel % 2 == 0)
                {
                    if (indentLevel >= minIndents && indentLevel <= maxIndents)
                    {
                        bool isList;

                        if (StringUtils::startsWith(line, "-"))
                        {
                            isList = true;
                            line = StringUtils::ltrim(line.substr(1));
                        }
                        else
                        {
                            isList = false;
                        }
                        if (indentLevel < maxIndents)
                        {
                            if (isList)
                            {
                                success = false;
                                LOGGER_ERROR(
                                        "Unexpected list item '" + line + "' on line " + std::to_string(lineNumber));
                            }
                            else
                            {
                                for (std::uint32_t i = maxIndents; i > indentLevel; i -= 2)
                                {
                                    currentNode = currentNode->parent();
                                }
                            }
                        }

                        if (success)
                        {
                            if (isList)
                            {
                                currentNode->add(line);
                                minIndents = 0;
                                maxIndents = indentLevel;
                            }
                            else
                            {
                                std::string* splitValues;
                                std::uint32_t splitCount;

                                StringUtils::split(line, ":", 1, &splitValues, &splitCount);

                                if (splitCount == 2)
                                {
                                    StringUtils::rtrim(&splitValues[0]);
                                    StringUtils::ltrim(&splitValues[1]);

                                    if (splitValues[1].empty())
                                    {
                                        minIndents = indentLevel + 2;
                                        maxIndents = minIndents;

                                        if (currentNode->add(splitValues[0]))
                                        {
                                            currentNode = currentNode->get(splitValues[0]);
                                        }
                                    }
                                    else
                                    {
                                        minIndents = 0;
                                        maxIndents = indentLevel;

                                        if (currentNode->add(splitValues[0]))
                                        {
                                            currentNode->get(splitValues[0])->add(splitValues[1]);
                                        }
                                        else
                                        {
                                            LOGGER_WARN("Property redefinition, '" + splitValues[0] + "' on line " +
                                                        std::to_string(lineNumber));
                                        }
                                    }
                                }
                                else
                                {
                                    LOGGER_ERROR("Invalid data '" + line + "' on line " + std::to_string(lineNumber));
                                    return false;
                                }
                            }
                        }
                    }
                    else
                    {
                        LOGGER_ERROR("Unexpected indentation '" + line + "' on line " + std::to_string(lineNumber));
                        return false;
                    }
                }
                else
                {
                    LOGGER_ERROR(
                            "Invalid indentation (odd spaces) '" + line + "' on line " + std::to_string(lineNumber));
                    return false;
                }
            }

            return true;
        }
    }

    bool AssetArchive::init()
    {
        bool success;

        success = FileUtils::getFileListFromArchive(archivePath(), &archiveAssets_);

        std::istream* stream = nullptr;
        success = success && FileUtils::getStreamFromArchivedFile(archivePath(), ".manifest", &stream);
        success = success && getPropertiesFromStream(stream, &archiveAssetIds_);

        if (!success)
        {
            LOGGER_ERROR("Failed to initialize archive");
        }

        delete stream;
        return success;
    }

    bool AssetArchive::hasAsset(const std::string& assetPath)
    {
        return !assetPath.empty() && archiveAssets_.find(assetPath) != archiveAssets_.end();
    }

    std::uint64_t AssetArchive::assetCount()
    {
        return archiveAssets_.size();
    }

    std::string AssetArchive::loadAsciiData(const std::string& assetPath, bool* error)
    {
        std::string data;
        std::string fileName = fileNameOfAsset(assetPath, archiveAssetIds_, archiveAssets_);

        if (hasAsset(fileName))
        {
            std::int8_t* buffer = nullptr;
            std::size_t bufferSize = 0;

            *error = *error || !FileUtils::getContentsFromArchivedFile(archivePath(), fileName, &buffer, &bufferSize);

            if (!*error)
            {
                data = std::string((char*) buffer, bufferSize);
            }

            delete buffer;
        }
        else
        {
            *error = true;
            LOGGER_ERROR("Failed to retrieve asset '" + assetPath + "'");
        }

        return data;
    }

    ShaderProgram AssetArchive::loadShaderAsset(const std::string& assetPath, bool* error)
    {
        ShaderProgram shaderProgram{assetPath};

        std::string fileName = fileNameOfAsset(assetPath, archiveAssetIds_, archiveAssets_);

        if (hasAsset(fileName))
        {
            std::istream* stream = nullptr;

            PropertyTree propertyData{"shader"};

            *error = *error || !FileUtils::getStreamFromArchivedFile(archivePath(), fileName, &stream);
            *error = *error || !getPropertyTreeFromStream(stream, &propertyData);

            delete stream;

            if (!*error)
            {
                ShaderProgram program = mapToShaderProgram(propertyData, error);
                program.programPath = assetPath;
                return program;
            }
        }
        else
        {
            *error = true;
            LOGGER_ERROR("Failed to retrieve asset '" + assetPath + "'");
        }

        return shaderProgram;
    }

    bool
    AssetArchive::loadAnimationSetAsset(const std::string& assetPath, std::unordered_map<std::string, std::string>& map)
    {
        bool error;
        std::string fileName = fileNameOfAsset(assetPath, archiveAssetIds_, archiveAssets_);

        if (hasAsset(fileName))
        {
            std::istream* stream = nullptr;

            PropertyTree propertyData{"animations"};

            error = !FileUtils::getStreamFromArchivedFile(archivePath(), fileName, &stream);
            error = error || !getPropertyTreeFromStream(stream, &propertyData);

            delete stream;

            if (!error)
            {
                for (auto& childName: propertyData.children())
                {
                    map.insert(
                            std::pair<std::string, std::string>(childName, propertyData.get(childName)->value())
                    );
                }
            }
            else
            {
                LOGGER_ERROR("Failed to read asset '" + assetPath + "'");
            }
        }
        else
        {
            error = true;
            LOGGER_ERROR("Failed to retrieve asset '" + assetPath + "'");
        }

        return !error;
    }

    bool AssetArchive::loadAnimationAsset(const std::string& name, const std::string& assetPath,
                                          std::unordered_map<std::string, IAnimation*>& map)
    {
        bool error;
        std::string fileName = fileNameOfAsset(assetPath, archiveAssetIds_, archiveAssets_);

        if (hasAsset(fileName))
        {
            std::istream* stream = nullptr;

            PropertyTree propertyData{"animations"};

            error = !FileUtils::getStreamFromArchivedFile(archivePath(), fileName, &stream);
            error = error || !getPropertyTreeFromStream(stream, &propertyData);

            delete stream;

            if (!error)
            {
                auto skeleton = propertyData.get("skeleton");
                std::unordered_map<std::string, BoneMap> boneMap = mapToBones(skeleton->get("root"), "", &error);

                std::uint8_t fps = NumberUtils::parseValue(propertyData.get("fps")->value().c_str(), 0, &error);
                std::uint8_t length = NumberUtils::parseValue(propertyData.get("length")->value().c_str(), 0, &error);

                if (!error)
                {
                    std::unordered_map<std::uint8_t, std::vector<Keyframe>> keyframes = mapToKeyframes(
                            propertyData.get("keyframes"),
                            &error);

                    if (!error)
                    {
                        map.insert(
                                std::pair<std::string, IAnimation*>(name,
                                                                    new Animation(boneMap, fps, length, keyframes))
                        );
                    }
                }
                else
                {
                    LOGGER_ERROR("Invalid/missing fps attribute for animation asset '" + assetPath + "'");
                }
            }
            else
            {
                LOGGER_ERROR("Failed to read asset '" + assetPath + "'");
            }
        }
        else
        {
            error = true;
            LOGGER_ERROR("Failed to retrieve asset '" + assetPath + "'");
        }

        return !error;
    }

    ImageData AssetArchive::loadImageAsset(const std::string& assetPath, bool* error)
    {
        ImageData data{};

        std::string fileName = fileNameOfAsset(assetPath, archiveAssetIds_, archiveAssets_);

        if (hasAsset(fileName))
        {
            stbi_set_flip_vertically_on_load(true);

            std::istream* stream = nullptr;

            if (FileUtils::getStreamFromArchivedFile(archivePath(), fileName, &stream))
            {
                stream->ignore(INTMAX_MAX);
                std::int64_t streamLength = stream->gcount();
                stream->clear();
                stream->seekg(0, std::istream::beg);

                char* buffer = new char[streamLength];

                stream->read(buffer, streamLength);

                //TODO: Check if streamLength is too long, expecting only 32bit value
                data.bufferData = stbi_load_from_memory((std::uint8_t*) buffer, streamLength, &data.width,
                                                        &data.height, &data.numChannels, 0);
            }
            else
            {
                LOGGER_ERROR(
                        "Failed to acquire stream for asset '" + assetPath + "' in archive '" + archivePath() + "'");
            }

            delete stream;
        }
        else
        {
            *error = true;
            LOGGER_ERROR("Failed to retrieve asset '" + assetPath + "'");
        }

        return data;
    }

    Material AssetArchive::loadMaterialAsset(const std::string& assetPath, bool* error)
    {
        std::string fileName = fileNameOfAsset(assetPath, archiveAssetIds_, archiveAssets_);

        if (hasAsset(fileName))
        {
            std::istream* stream = nullptr;

            PropertyTree propertyData{"material"};

            *error = *error || !FileUtils::getStreamFromArchivedFile(archivePath(), fileName, &stream);
            *error = *error || !getPropertyTreeFromStream(stream, &propertyData);

            delete stream;
            if (!*error)
            {
                return mapToMaterial(propertyData, error);
            }
        }
        else
        {
            *error = true;
            LOGGER_ERROR("Failed to retrieve asset '" + assetPath + "'");
        }

        return {};
    }

    ModelData2D AssetArchive::load2DModelAsset(const std::string& assetPath, bool* error)
    {
        std::string fileName = fileNameOfAsset(assetPath, archiveAssetIds_, archiveAssets_);

        if (hasAsset(fileName))
        {
            std::istream* stream = nullptr;

            PropertyTree propertyData{"model"};

            *error = *error || !FileUtils::getStreamFromArchivedFile(archivePath(), fileName, &stream);
            *error = *error || !getPropertyTreeFromStream(stream, &propertyData);

            delete stream;
            if (!*error)
            {
                ModelData2D model{};

                if (propertyData.has("root"))
                {
                    model = mapToModel2D(*propertyData.get("root"), error);
                }
                else
                {
                    *error = true;
                    LOGGER_ERROR("Invalid Model2D data, must contain root node.");
                }

                if (*error)
                {
                    LOGGER_ERROR("Failed to load Model2D data for asset '" + assetPath + "'");
                }

                return model;
            }
        }
        else
        {
            *error = true;
            LOGGER_ERROR("Failed to retrieve asset '" + assetPath + "'");
        }

        return {};
    }

    std::string AssetArchive::archivePath()
    {
        return archiveRoot_ + archiveName_ + ".zip";
    }
}