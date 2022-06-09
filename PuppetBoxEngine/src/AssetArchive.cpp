#include <utility>

#include <STBI/stb_image.h>

#include "puppetbox/IAnimationCatalogue.h"
#include "puppetbox/Utilities.h"

#include "AnimationCatalogue.h"
#include "AssetArchive.h"
#include "FileUtils.h"
#include "GfxMath.h"
#include "PropertyTree.h"

namespace PB
{
    namespace
    {
        /**
         * \brief Maps an input stream to a {\link SizedArray}.
         *
         * \param stream The stream to read from.
         * \return A {\link SizedArray} loaded with the given stream data.
         */
        SizedArray<std::uint8_t> mapStreamToByteArray(std::istream* stream)
        {
            SizedArray<std::uint8_t> bytes{};

            stream->seekg(0, std::istream::end);
            bytes.length = stream->tellg();
            stream->seekg(0, std::istream::beg);

            char* buffer = new char[bytes.length];

            stream->read(buffer, bytes.length);

            bytes.array = (std::uint8_t*) buffer;

            return bytes;
        }

        /**
         * Checks the given {\link PropertyTree} for the given node, returning it's value
         * in a {\link Result} object (or an empty {\link Result} if not found).
         *
         * @param nodeName  The name of the property to search for.
         * @param pTree     The {\link PropertyTree} to search through for the node.
         * @return A {\link Result} object with the property node's value, or an empty
         * {\link Result} if the node wasn't found, or had no value.
         */
        Result<std::string> getStringResultAtNode(const std::string& nodeName, PropertyTree& pTree)
        {
            Result<std::string> result;

            auto propertyNode = pTree.get(nodeName);

            if (propertyNode.hasResult && !propertyNode.result->value().empty())
            {
                result = {propertyNode.result->value(), true};
            }
            else
            {
                result = {"", false};
            }

            return result;
        }

        /**
         * \brief Gets the parsed float value of the node associated with the given name if it exists,
         * returning a {\link Result<T>} object to indicate if it was found.
         *
         * \param nodeName The name associated with the desired node value.
         * \param pTree    The tree to search on for the node associated with the given name.
         * \return A {\link Result<T>} that possibly contains the requested node object.
         */
        template<typename T>
        Result<T> getNumericResultAtNode(const std::string& nodeName, PropertyTree& pTree, bool* error)
        {
            Result<T> result;
            auto propertyNode = pTree.get(nodeName);

            if (propertyNode.hasResult && !propertyNode.result->value().empty())
            {
                T value = NumberUtils::parseValue(propertyNode.result->value().c_str(), (T) 0, error);

                if (*error)
                {
                    LOGGER_ERROR("Could not parse value '" + propertyNode.result->value() + "' as float");
                }

                result = {value, !*error};
            }
            else
            {
                result = {(T) 0, false};
            }

            return result;
        }

        /**
        * \brief Helper function to map a skeletal properties map to a {\link PB::BoneMap} object.
        *
        * \param boneMap    The {\link PB::BoneMap} to add the skeleton data to.
        * \param properties	The properties map to use to map to a {\link PB::BoneMap} object.
        * \param parentName The name of the bone the current iteration's bone is a child of.
        * \param error		Flag indicating an error occurred when set to True.
        *
        * \return The {\link PB::BoneMap} object built from the given skeletal properties map.
        */
        void mapToBoneMap(BoneMap& boneMap, PropertyTree& properties, const std::string& parentName, bool* error)
        {
            std::string boneName = properties.name();

            auto offsetNode = properties.get("offset");

            vec3 offset{};

            if (offsetNode.hasResult)
            {
                PropertyTree* offsetProperties = offsetNode.result;

                offset.x = getNumericResultAtNode<float>("x", *offsetProperties, error).orElse(0.0f);
                offset.y = getNumericResultAtNode<float>("y", *offsetProperties, error).orElse(0.0f);
                offset.z = getNumericResultAtNode<float>("z", *offsetProperties, error).orElse(0.0f);
            }

            vec3 scale{1.0f, 1.0f, 1.0f};

            // Create bone, adding a default transform for faster calculations in the pipeline
            Bone bone{offset, scale, {}};
            bone.defaultTransform = GfxMath::CreateTransformation({}, scale, offset);

            if (!parentName.empty())
            {
                std::uint32_t parentId = boneMap.getBoneId(parentName);

                bone.defaultTransform = boneMap.getBone(parentId).result->bone.defaultTransform * bone.defaultTransform;
            }

            boneMap.addBone(boneName, parentName, bone);

            auto rootChildrenNode = properties.get("children");

            if (!*error && rootChildrenNode.hasResult)
            {
                PropertyTree* childrenProperties = rootChildrenNode.result;

                for (auto& childName: childrenProperties->children())
                {
                    mapToBoneMap(
                            boneMap,
                            *childrenProperties->get(childName).result,
                            boneName,
                            error);
                }
            }
        }

        BoneMap mapToBoneMap(PropertyTree& rootProperties, bool* error)
        {
            BoneMap boneMap{};

            mapToBoneMap(boneMap, rootProperties, "", error);

            if (*error)
            {
                LOGGER_ERROR("Failed to parse values for BoneMap object");
            }

            return std::move(boneMap);
        }

        RawKeyframe mapToKeyframe(PropertyTree* pTree, bool* error)
        {
            RawKeyframe keyframe{};

            keyframe.boneName = pTree->name();

            auto positionNode = pTree->get("position");

            if (positionNode.hasResult)
            {
                PropertyTree* position = positionNode.result;

                keyframe.position.x = getNumericResultAtNode<float>("x", *position, error);
                keyframe.position.y = getNumericResultAtNode<float>("y", *position, error);
                keyframe.position.z = getNumericResultAtNode<float>("z", *position, error);
            }

            auto rotationNode = pTree->get("rotation");

            if (rotationNode.hasResult)
            {
                auto rotation = rotationNode.result;

                std::function<void(float&)> consumer = [](float& value) { value *= GfxMath::RADS_PER_DEGREE; };

                keyframe.rotation.x = getNumericResultAtNode<float>("x", *rotation, error).ifPresent(consumer);
                keyframe.rotation.y = getNumericResultAtNode<float>("y", *rotation, error).ifPresent(consumer);
                keyframe.rotation.z = getNumericResultAtNode<float>("z", *rotation, error).ifPresent(consumer);
            }

            auto scaleNode = pTree->get("scale");

            if (scaleNode.hasResult)
            {
                auto scale = scaleNode.result;

                keyframe.scale.x = getNumericResultAtNode<float>("x", *scale, error);
                keyframe.scale.y = getNumericResultAtNode<float>("y", *scale, error);
                keyframe.scale.z = getNumericResultAtNode<float>("z", *scale, error);
            }

            return keyframe;
        }

        std::vector<RawKeyframe> mapToKeyframeVector(PropertyTree* pTree, const std::uint8_t frameIndex, bool* error)
        {
            std::vector<RawKeyframe> keyframes{};

            for (auto& childName: pTree->children())
            {
                if (!*error)
                {
                    RawKeyframe keyframe = mapToKeyframe(pTree->get(childName).result, error);

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

        std::unordered_map<std::uint8_t, std::vector<RawKeyframe>> mapToKeyframes(PropertyTree* pTree, bool* error)
        {
            std::unordered_map<std::uint8_t, std::vector<RawKeyframe>> keyframes{};

            if (!pTree->children().empty())
            {
                for (auto& childName: pTree->children())
                {
                    if (!*error)
                    {
                        std::uint8_t frameIndex = NumberUtils::parseValue(childName.c_str(), 0, error);

                        if (!*error)
                        {
                            auto keyframeVector = mapToKeyframeVector(pTree->get(childName).result, frameIndex, error);
                            keyframes.insert(
                                    std::pair<std::uint8_t, std::vector<RawKeyframe>>{frameIndex, keyframeVector}
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
        * \param bool		        Flag indicating an error occurred if set to True.
        *
        * \return The {\link ShaderProgram} object built from the given properties map.
        */
        ShaderProgram mapToShaderProgram(PropertyTree& shaderProperties, bool* error)
        {
            ShaderProgram shaderProgram{};

            shaderProgram.vertexShaderPath = getStringResultAtNode("vertex", shaderProperties).orElse("");
            shaderProgram.fragmentShaderPath = getStringResultAtNode("fragment", shaderProperties).orElse("");
            shaderProgram.geometryShaderPath = getStringResultAtNode("geometry", shaderProperties).orElse("");

            if (shaderProgram.vertexShaderPath.empty() || shaderProgram.fragmentShaderPath.empty())
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
        * \return True if the data was successfully parsed, False otherwise.
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
                                            currentNode = currentNode->get(splitValues[0]).result;
                                        }
                                    }
                                    else
                                    {
                                        minIndents = 0;
                                        maxIndents = indentLevel;

                                        if (currentNode->add(splitValues[0]))
                                        {
                                            currentNode->get(splitValues[0]).result->add(splitValues[1]);
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

    BoneMap AssetArchive::loadSkeletonAsset(const std::string& assetPath, bool* error)
    {
        BoneMap boneMap{};

        std::string fileName = fileNameOfAsset(assetPath, archiveAssetIds_, archiveAssets_);

        if (hasAsset(fileName))
        {
            std::istream* stream = nullptr;

            PropertyTree propertyData{"skeleton"};

            *error = *error || !FileUtils::getStreamFromArchivedFile(archivePath(), fileName, &stream);
            *error = *error || !getPropertyTreeFromStream(stream, &propertyData);

            delete stream;
            if (!*error)
            {
                auto rootNode = propertyData.get("root");

                if (rootNode.hasResult)
                {
                    boneMap = std::move(mapToBoneMap(*rootNode.result, error));
                }
                else
                {
                    *error = true;
                    LOGGER_ERROR("Invalid skeleton data, must contain root node.");
                }

                if (*error)
                {
                    LOGGER_ERROR("Failed to load skeleton data for asset '" + assetPath + "'");
                }
            }
        }
        else
        {
            *error = true;
            LOGGER_ERROR("Failed to retrieve asset '" + assetPath + "'");
        }

        return std::move(boneMap);
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
                            std::pair<std::string, std::string>(childName, propertyData.get(childName).result->value())
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

    bool AssetArchive::loadAnimationAsset(
            const std::string& assetPath,
            std::unordered_map<std::string, IAnimation*>& animationMap
    )
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
                auto skeletonNode = propertyData.get("skeleton");

                if (skeletonNode.hasResult)
                {
                    auto rootNode = skeletonNode.result->get("root");

                    if (rootNode.hasResult)
                    {
                        //TODO: Remove this from the animation format or implement it fully
                        //BoneMap boneMap = mapToBones(rootNode.result, "", &error);

                        auto fpsNode = propertyData.get("fps");

                        if (fpsNode.hasResult)
                        {
                            std::uint8_t fps = NumberUtils::parseValue(fpsNode.result->value().c_str(), 0, &error);

                            auto lengthNode = propertyData.get("length");

                            if (lengthNode.hasResult)
                            {
                                std::uint8_t frameCount = NumberUtils::parseValue(
                                        lengthNode.result->value().c_str(),
                                        0,
                                        &error
                                );

                                auto keyframesNode = propertyData.get("keyframes");

                                if (keyframesNode.hasResult)
                                {
                                    std::unordered_map<std::uint8_t, std::vector<RawKeyframe>> keyframes = mapToKeyframes(
                                            keyframesNode.result,
                                            &error);

                                    if (!error)
                                    {
                                        animationMap.insert(
                                                std::pair<std::string, IAnimation*>(archiveName_ + "/" + assetPath,
                                                                                    new Animation(
                                                                                            this->archiveName_ + "/" +
                                                                                            assetPath,
                                                                                            fps,
                                                                                            frameCount,
                                                                                            keyframes
                                                                                    )
                                                )
                                        );
                                    }
                                }
                                else
                                {
                                    error = true;
                                    LOGGER_ERROR("No keyframes defined in the loaded animation.");
                                }
                            }
                            else
                            {
                                error = true;
                                LOGGER_ERROR("No length defined in the loaded animatino.");
                            }
                        }
                        else
                        {
                            error = true;
                            LOGGER_ERROR("No FPS defined in the loaded animation.");
                        }
                    }
                    else
                    {
                        error = true;
                        LOGGER_ERROR("No root node defined in the loaded animation skeleton.");
                    }
                }
                else
                {
                    error = true;
                    LOGGER_ERROR("No skeleton defined in the loaded animation.");
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

    SizedArray<std::uint8_t> AssetArchive::loadAssetBytes(const std::string& assetPath, bool* error)
    {
        SizedArray<std::uint8_t> bytesArray;

        std::string fileName = fileNameOfAsset(assetPath, archiveAssetIds_, archiveAssets_);

        if (hasAsset(fileName))
        {
            std::istream* stream = nullptr;

            *error = *error || !FileUtils::getStreamFromArchivedFile(archivePath(), fileName, &stream);

            if (!*error)
            {
                bytesArray = mapStreamToByteArray(stream);
            }
        }
        else
        {
            *error = true;
            LOGGER_ERROR("Failed to retrieve font asset, '" + assetPath + "'");
        }

        return bytesArray;
    }

    std::vector<Vertex> AssetArchive::loadMeshDataAsset(const std::string& assetPath, bool* error)
    {
        std::vector<Vertex> meshData{};

        std::string fileName = fileNameOfAsset(assetPath, archiveAssetIds_, archiveAssets_);

        if (hasAsset(fileName))
        {
            std::istream* stream = nullptr;

            *error = !FileUtils::getStreamFromArchivedFile(archivePath(), fileName, &stream);

            float values[8]{};
            std::uint8_t i = 0;

            while (!stream->eof())
            {
                *stream >> values[i++];

                if (i >= 8)
                {
                    meshData.push_back(Vertex{
                            vec3{values[0], values[1], values[2]},  // Vertex Coord
                            vec3{values[3], values[4], values[5]},  // Vertex Normal
                            vec2{values[6], values[7]}              // UV Coord
                    });

                    i = 0;
                }
            }

            if (i != 0)
            {
                *error = true;
                LOGGER_ERROR("Incomplete/Corrupt mesh data for asset '" + assetPath + "'");
            }
        }
        else
        {
            *error = true;
            LOGGER_ERROR("Invalid mesh data asset, '" + assetPath + "'");
        }

        return meshData;
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
                //TODO: Revisit this, work on 32 bit max value
                stream->ignore(INTMAX_MAX);
                std::int64_t streamLength = stream->gcount();
                stream->clear();
                stream->seekg(0, std::istream::beg);

                char* buffer = new char[streamLength];

                stream->read(buffer, streamLength);

                //TODO: Check if streamLength is too long, expecting only 32bit value
                data.bufferData = stbi_load_from_memory(
                        (std::uint8_t*) buffer,
                        streamLength, &data.width,
                        &data.height,
                        &data.numChannels,
                        0);
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

    std::string AssetArchive::archivePath()
    {
        return archiveRoot_ + archiveName_ + ".zip";
    }
}