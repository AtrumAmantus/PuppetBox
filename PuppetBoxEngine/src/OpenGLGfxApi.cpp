#include <iostream>
#include <vector>
#include <unordered_map>

#include <glad/glad.h>

#include "puppetbox/DataStructures.h"

#include "AbstractRenderComponent.h"
#include "GfxMath.h"
#include "Logger.h"
#include "OpenGLGfxApi.h"
#include "Shader.h"

const std::uint32_t MAX_BONE_WEIGHTS = 4;

namespace PB
{
    namespace
    {
        std::vector<Mesh> RENDER_REFERENCES{};
        std::vector<Mesh> INSTANCE_RENDER_REFERENCES{};
        std::vector<ImageMapReference> IMAGE_MAP_REFERENCES{};
        std::vector<Shader> SHADER_PROGRAM_REFERENCES{};

        std::uint32_t INSTANCE_TRANSFORM_VBO = 0;
        std::uint32_t INSTANCE_DATA_VBO = 0;

        /**
        * \brief Searches for the provided vertex in the provided vector.  Two vertices are considered equal if each of
        * their matching attributes fall within a certain defined tolerance.
        *
        * \param vertex The vertex to search for
        * \param vector The vector to search in
        *
        * \return The index of the matching vertex in the vector, or -1 if not found.
        */
        std::int32_t findVertexInVector(Vertex& vertex, std::vector<Vertex>& vector)
        {
            for (std::int32_t i = 0; i < vector.size() && i < INT32_MAX; ++i)
            {
                Vertex v = vector.at(i);

                if (GfxMath::BasicallyEqual(vertex, v))
                {
                    return i;
                }
            }

            return -1;
        }

        /**
        * \brief Callback function for the OpenGL debug events.
        */
        void
        debug_callback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, GLchar const* message,
                       void const* userParam)
        {
            // ignore non-significant error/warning codes
            if (id == 131169 || id == 131185 || id == 131218 || id == 131204) return;

            std::cout << "---------------" << std::endl;
            std::cout << "Debug message (" << id << "): " << message << std::endl;

            switch (source)
            {
                case GL_DEBUG_SOURCE_API:
                    std::cout << "Source: API";
                    break;
                case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
                    std::cout << "Source: Window System";
                    break;
                case GL_DEBUG_SOURCE_SHADER_COMPILER:
                    std::cout << "Source: Shader Compiler";
                    break;
                case GL_DEBUG_SOURCE_THIRD_PARTY:
                    std::cout << "Source: Third Party";
                    break;
                case GL_DEBUG_SOURCE_APPLICATION:
                    std::cout << "Source: Application";
                    break;
                case GL_DEBUG_SOURCE_OTHER:
                    std::cout << "Source: Other";
                    break;
            }
            std::cout << std::endl;

            switch (type)
            {
                case GL_DEBUG_TYPE_ERROR:
                    std::cout << "Type: Error";
                    break;
                case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
                    std::cout << "Type: Deprecated Behaviour";
                    break;
                case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
                    std::cout << "Type: Undefined Behaviour";
                    break;
                case GL_DEBUG_TYPE_PORTABILITY:
                    std::cout << "Type: Portability";
                    break;
                case GL_DEBUG_TYPE_PERFORMANCE:
                    std::cout << "Type: Performance";
                    break;
                case GL_DEBUG_TYPE_MARKER:
                    std::cout << "Type: Marker";
                    break;
                case GL_DEBUG_TYPE_PUSH_GROUP:
                    std::cout << "Type: Push Group";
                    break;
                case GL_DEBUG_TYPE_POP_GROUP:
                    std::cout << "Type: Pop Group";
                    break;
                case GL_DEBUG_TYPE_OTHER:
                    std::cout << "Type: Other";
                    break;
            }
            std::cout << std::endl;

            switch (severity)
            {
                case GL_DEBUG_SEVERITY_HIGH:
                    std::cout << "Severity: high";
                    break;
                case GL_DEBUG_SEVERITY_MEDIUM:
                    std::cout << "Severity: medium";
                    break;
                case GL_DEBUG_SEVERITY_LOW:
                    std::cout << "Severity: low";
                    break;
                case GL_DEBUG_SEVERITY_NOTIFICATION:
                    std::cout << "Severity: notification";
                    break;
            }
            std::cout << std::endl;
            std::cout << std::endl;
        }
    }

    class OpenglRenderComponent : public AbstractRenderComponent
    {
        void render(const std::vector<mat4>& boneTransforms, const Model& model, const mat4 transform) const override
        {
            //TODO: Revisit this, just leave it on? Material specific?
            glEnable(GL_BLEND);

            const Shader& shader = SHADER_PROGRAM_REFERENCES[model.shaderProgramID - 1];
            shader.use();

            // Bind each image to the shader program
            for (std::uint32_t i = 0; i < model.imageMaps.size(); ++i)
            {
                const auto& imageMap = model.imageMaps.at(i);
                const auto& imageReference = IMAGE_MAP_REFERENCES[imageMap.imageMapID - 1];

                glActiveTexture((GL_TEXTURE0 + i));
                glBindTexture(GL_TEXTURE_2D, imageReference.imageMapId);

                switch (imageMap.mapType)
                {
                    case DIFFUSE:
                        shader.setInt("material.diffuseMap", i);
                        break;
                    case EMISSION:
                        shader.setInt("material.emissionMap", i);
                        break;
                    default:
                        LOGGER_ERROR("Unsupported ImageMapType");
                        break;
                }
            }

            shader.setMat4("boneTransforms",boneTransforms.size(), &boneTransforms[0]);
            shader.setMat4("meshTransform", model.meshTransform);
            shader.setMat4("model", transform);
            Mesh mesh = RENDER_REFERENCES[model.meshID - 1];

            glBindVertexArray(mesh.VAO);

            if (mesh.EBO != 0)
            {
                //                           v-- number of indices to draw
                glDrawElements(GL_TRIANGLES, mesh.drawCount, GL_UNSIGNED_INT, 0); // NOLINT(modernize-use-nullptr)
            }
            else
            {
                //                            v-- number of vertices to draw
                glDrawArrays(GL_TRIANGLES, 0, mesh.drawCount);
            }

            // Unset VAO for next render
            glBindVertexArray(0);

            // Unbind each image to the shader program
            for (std::uint32_t i = 0; i < model.imageMaps.size(); ++i)
            {
                glActiveTexture((GL_TEXTURE0 + i));
                glBindTexture(GL_TEXTURE_2D, 0);
            }

            shader.unuse();
        }

        void renderInstance(
                const Model& model,
                const mat4 transform,
                const std::vector<mat4>& instanceTransforms,
                const std::vector<mat4>& instanceData) const override
        {
            const Shader& shader = SHADER_PROGRAM_REFERENCES[model.shaderProgramID - 1];
            shader.use();

            // Bind each image to the shader program
            for (std::uint32_t i = 0; i < model.imageMaps.size(); ++i)
            {
                const auto& imageMap = model.imageMaps.at(i);
                const auto& imageReference = IMAGE_MAP_REFERENCES[imageMap.imageMapID - 1];

                glActiveTexture((GL_TEXTURE0 + i));
                glBindTexture(GL_TEXTURE_2D, imageReference.imageMapId);

                switch (imageMap.mapType)
                {
                    case DIFFUSE:
                        shader.setInt("material.diffuseMap", i);
                        break;
                    case EMISSION:
                        shader.setInt("material.emissionMap", i);
                        break;
                    default:
                        LOGGER_ERROR("Unsupported ImageMapType");
                        break;
                }
            }

            shader.setMat4("model", transform);

            auto mesh = INSTANCE_RENDER_REFERENCES[model.meshID];

            glBindVertexArray(mesh.VAO);

            glBindBuffer(GL_ARRAY_BUFFER, INSTANCE_TRANSFORM_VBO);
            glBufferData(GL_ARRAY_BUFFER, instanceTransforms.size() * sizeof(mat4), &instanceTransforms[0], GL_STATIC_DRAW);

            glEnableVertexAttribArray(2);
            glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(mat4), (void*) 0);
            glVertexAttribDivisor(2, 1);
            glEnableVertexAttribArray(3);
            glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(mat4), (void*)(sizeof(mat4)));
            glVertexAttribDivisor(3, 1);
            glEnableVertexAttribArray(4);
            glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(mat4), (void*)(2 * sizeof(mat4)));
            glVertexAttribDivisor(4, 1);
            glEnableVertexAttribArray(5);
            glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(mat4), (void*)(3 * sizeof(mat4)));
            glVertexAttribDivisor(5, 1);

            glBindBuffer(GL_ARRAY_BUFFER, INSTANCE_DATA_VBO);
            glBufferData(GL_ARRAY_BUFFER, instanceData.size() * sizeof(mat4), &instanceData[0], GL_STATIC_DRAW);

            glEnableVertexAttribArray(6);
            glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(mat4), (void*) 0);
            glVertexAttribDivisor(6, 1);
            glEnableVertexAttribArray(7);
            glVertexAttribPointer(7, 4, GL_FLOAT, GL_FALSE, sizeof(mat4), (void*)(sizeof(mat4)));
            glVertexAttribDivisor(7, 1);
            glEnableVertexAttribArray(8);
            glVertexAttribPointer(8, 4, GL_FLOAT, GL_FALSE, sizeof(mat4), (void*)(2 * sizeof(mat4)));
            glVertexAttribDivisor(8, 1);
            glEnableVertexAttribArray(9);
            glVertexAttribPointer(9, 4, GL_FLOAT, GL_FALSE, sizeof(mat4), (void*)(3 * sizeof(mat4)));
            glVertexAttribDivisor(9, 1);

            shader.setMat4("meshTransform", model.meshTransform);

            if (mesh.EBO != 0)
            {
                //                                    v-- number of indices to draw
                glDrawElementsInstanced(GL_TRIANGLES, mesh.drawCount, GL_UNSIGNED_INT, 0, instanceData.size());
            }
            else
            {
                //                                     v-- number of vertices to draw
                glDrawArraysInstanced(GL_TRIANGLES, 0, mesh.drawCount, instanceData.size());
            }
            glBindVertexArray(0);

            // Unbind each image to the shader program
            for (std::uint32_t i = 0; i < model.imageMaps.size(); ++i)
            {
                glActiveTexture((GL_TEXTURE0 + i));
                glBindTexture(GL_TEXTURE_2D, 0);
            }

            shader.unuse();
        }
    };

    bool OpenGLGfxApi::init(const PB::ProcAddress procAddress)
    {
        bool error = false;

        if (gladLoadGLLoader(procAddress))
        {
            std::cout << "OpenGL Version: " << GLVersion.major << "." << GLVersion.minor << std::endl;
            std::cout << "OpenGL Shading Language Version: " << (char*) glGetString(GL_SHADING_LANGUAGE_VERSION)
                      << std::endl;
            std::cout << "OpenGL Vendor: " << (char*) glGetString(GL_VENDOR) << std::endl;
            std::cout << "OpenGL Renderer: " << (char*) glGetString(GL_RENDERER) << std::endl;

#ifdef _DEBUG
            std::cout << "Hardware Specs:" << std::endl;

            {
                std::int32_t vuv = 0;
                glGetIntegerv(GL_MAX_VERTEX_UNIFORM_VECTORS, &vuv);

                std::cout << "\t" << vuv << " max uniform vectors" << std::endl;
            }
#endif

            glEnable(GL_DEPTH_TEST);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

            // Store the card's minimum UBO offset value for later.
            std::int32_t value;
            glGetIntegerv(GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT, &value);
            minimumUBOOffset_ = static_cast<std::uint32_t>(value);
            glGenBuffers(1, &INSTANCE_TRANSFORM_VBO);
            glGenBuffers(1, &INSTANCE_DATA_VBO);
        }
        else
        {
            error = true;
            LOGGER_ERROR("Failed to initialize OpenGL api pointers");
        }

        return !error;
    }

    void OpenGLGfxApi::preLoopCommands() const
    {
        glViewport(0, 0, static_cast<std::int32_t>(width_), static_cast<std::int32_t>(height_));
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    void OpenGLGfxApi::setRenderDimensions(std::uint32_t width, std::uint32_t height)
    {
        width_ = width;
        height_ = height;
    }

    void OpenGLGfxApi::setRenderDistance(std::uint32_t distance)
    {
        distance_ = distance;
    }

    const RenderWindow OpenGLGfxApi::getRenderWindow()
    {
        return RenderWindow{
                &width_,
                &height_,
                &distance_
        };
    }

    std::uint32_t OpenGLGfxApi::loadImage(ImageData imageData) const
    {
        std::uint32_t imageMapId;

        ImageMapReference reference{};

        if (imageData.bufferData)
        {
            std::uint32_t openGLID;

            glGenTextures(1, &openGLID);
            glBindTexture(GL_TEXTURE_2D, openGLID);

            // Clamp border
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

            const float BLACK[4] = {0.0f, 0.0f, 0.0f, 0.0f};
            glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, BLACK);

            // Linear sampling
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            // Support alpha enabled images (PNGs)
            std::int32_t format = imageData.numChannels == 4 ? GL_RGBA : GL_RGB;
            // Generate texture from previously bound image
            glTexImage2D(GL_TEXTURE_2D, 0, format, imageData.width, imageData.height, 0, format, GL_UNSIGNED_BYTE,
                         imageData.bufferData);
            // Attaches texture images to texture object currently bound
            glGenerateMipmap(GL_TEXTURE_2D);

            // Free up binding after we create it
            glBindTexture(GL_TEXTURE_2D, 0);

            reference.imageMapId = openGLID;
            reference.width = imageData.width;
            reference.height = imageData.height;
            reference.requiresAlphaBlending = imageData.numChannels == 4;

            IMAGE_MAP_REFERENCES.push_back(reference);
            imageMapId = IMAGE_MAP_REFERENCES.size();
        }
        else
        {
            imageMapId = 0;
        }

        return imageMapId;
    }

    bool OpenGLGfxApi::buildCharacterMap(
            FT_Face face,
            std::unordered_map<std::int8_t, TypeCharacter>& loadedCharacters) const
    {
        const std::uint32_t MAX_ATLAS_COLUMNS = 512;
        const std::uint32_t MAX_CHARACTERS = 128;

        struct AtlasRow
        {
            std::uint8_t data[MAX_ATLAS_COLUMNS];
        };

        std::vector<AtlasRow> atlasRows{};

        uivec2 nextAtlasPosition{};
        TypeCharacter typeCharacters[MAX_CHARACTERS];

        bool success = true;

        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);  // Disable byte-alignment restriction

        for (std::uint8_t c = 0; success && c < MAX_CHARACTERS; ++c)
        {
            if (FT_Load_Char(face, c, FT_LOAD_RENDER))
            {
                success = false;
                LOGGER_ERROR("Failed to load glyph for '" + std::to_string(static_cast<char>(c)) + "'");
            }
            else
            {
                /*
                 * OpenGL uses the bottom left corner as the point of origin,
                 * so we need to invert the "rows" of byte data, since image
                 * data on the host machine is stored with the origin in the
                 * top left corner.
                 */
                std::uint32_t bitmapWidth = face->glyph->bitmap.width;
                std::uint32_t bitmapHeight = face->glyph->bitmap.rows;

                for (std::uint32_t row = 0; row < bitmapHeight / 2; ++row)
                {
                    for (std::uint32_t column = 0; column < bitmapWidth; ++column)
                    {
                        std::uint32_t currentIndex = (bitmapWidth * row) + column;
                        std::uint32_t invertedIndex = (bitmapWidth * (bitmapHeight - row - 1)) + column;

                        std::uint8_t tmp = face->glyph->bitmap.buffer[currentIndex];
                        face->glyph->bitmap.buffer[currentIndex] = face->glyph->bitmap.buffer[invertedIndex];
                        face->glyph->bitmap.buffer[invertedIndex] = tmp;
                    }
                }

                // Check if we're overrunning the atlas row width
                if (nextAtlasPosition.x + bitmapWidth > MAX_ATLAS_COLUMNS)
                {
                    // Move down to next data row
                    nextAtlasPosition.y = atlasRows.size();
                    // Starting from the left again
                    nextAtlasPosition.x = 0;
                }

                // Now write all the data to the atlas for this character
                for (std::uint32_t row = 0; row < bitmapHeight; ++row)
                {
                    for (std::uint32_t column = 0; column < bitmapWidth; ++column)
                    {
                        std::uint32_t pixelIndex = (bitmapWidth * row) + column;

                        // Check if we're breaking our vertical bounds, resize if needed
                        if (nextAtlasPosition.y + row >= atlasRows.size())
                        {
                            atlasRows.resize(nextAtlasPosition.y + row + 1);
                        }

                        atlasRows.at(nextAtlasPosition.y + row).data[nextAtlasPosition.x +
                                                                     column] = face->glyph->bitmap.buffer[pixelIndex];
                    }
                }

                typeCharacters[c] = TypeCharacter{
                        c,
                        ImageReference{0},
                        {bitmapWidth, bitmapHeight},
                        nextAtlasPosition,
                        {face->glyph->bitmap_left, face->glyph->bitmap_top},
                        static_cast<std::uint32_t>(face->glyph->advance.x)
                };

                // Move position over for processing of next glyph
                nextAtlasPosition.x += bitmapWidth;
            }
        }

        // Convert all glyph data into a 1D array
        std::uint8_t* atlasData = new std::uint8_t[atlasRows.size() * MAX_ATLAS_COLUMNS];

        for (std::uint32_t row = 0; row < atlasRows.size(); ++row)
        {
            for (std::uint32_t column = 0; column < MAX_ATLAS_COLUMNS; ++column)
            {
                atlasData[(row * MAX_ATLAS_COLUMNS) + column] = atlasRows.at(row).data[column];
            }
        }

        // Create the OpenGL resource of the glyph atlas data
        std::uint32_t texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(
                GL_TEXTURE_2D,
                0,
                GL_RED,
                MAX_ATLAS_COLUMNS,
                atlasRows.size(),
                0,
                GL_RED,
                GL_UNSIGNED_BYTE,
                atlasData
        );

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        delete[] atlasData;

        ImageReference imageReference{texture};
        imageReference.width = MAX_ATLAS_COLUMNS;
        imageReference.height = atlasRows.size();
        imageReference.requiresAlphaBlending = true;

        // Load the map with all the individual glyph data
        for (auto tChar: typeCharacters)
        {
            tChar.image = imageReference;
            loadedCharacters.insert(
                    std::pair<std::int8_t, TypeCharacter>{tChar.character, tChar}
            );
        }

        return success;
    }

    std::uint32_t OpenGLGfxApi::loadMesh(
            Vertex* vertexData,
            std::uint32_t vertexCount,
            std::vector<std::vector<BoneWeight>> meshBoneWeights) const
    {
        Mesh mesh{};

        std::vector<std::uint32_t> indexBoneIds{};
        std::vector<float> indexBoneWeights{};

        // 3 axis position, +3 axis normal, +2 axis UV coord
        mesh.stride = 3 + 3 + 2;

        std::vector<Vertex> uniqueVertices{};
        std::vector<std::uint32_t> indices{};

        std::uint32_t indexCounter = 0;

        // Filter out duplicate vertices, create EBO indices
        for (std::uint32_t i = 0; i < vertexCount; ++i)
        {
            std::int32_t index = findVertexInVector(vertexData[i], uniqueVertices);

            if (index < 0)
            {
                uniqueVertices.push_back(vertexData[i]);
                indices.push_back(indexCounter++);

                for (std::uint32_t j = 0; j < MAX_BONE_WEIGHTS; ++j)
                {
                    if (j < meshBoneWeights[i].size())
                    {
                        indexBoneIds.push_back(meshBoneWeights[i][j].boneId);
                        indexBoneWeights.push_back(meshBoneWeights[i][j].weight);
                    }
                    else
                    {
                        indexBoneIds.push_back(0);
                        indexBoneWeights.push_back(0.0f);
                    }
                }
            }
            else
            {
                indices.push_back(static_cast<std::uint32_t>(index));
            }
        }

        std::vector<float> vertexVBOData{};
        std::vector<float> normalVBOData{};
        std::vector<float> uvVBOData{};

        // Convert unique vertex vector to float array
        for (auto& v: uniqueVertices)
        {
            vertexVBOData.push_back(v.position.x);
            vertexVBOData.push_back(v.position.y);
            vertexVBOData.push_back(v.position.z);

            normalVBOData.push_back(v.normal.x);
            normalVBOData.push_back(v.normal.y);
            normalVBOData.push_back(v.normal.z);

            uvVBOData.push_back(v.uv.x);
            uvVBOData.push_back(v.uv.y);
        }

        // Create buffers
        glGenVertexArrays(1, &(mesh.VAO));

        glBindVertexArray(mesh.VAO);

        // Indices buffer
        glGenBuffers(1, &mesh.EBO);

        std::uint32_t eboBufferSize = static_cast<std::uint32_t>(sizeof(indices[0]) * indices.size());
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, eboBufferSize, &indices[0], GL_STATIC_DRAW);

        // Vertices VBO
        glGenBuffers(1, &mesh.vertexVBO);

        std::uint32_t vertexVboBufferSize = static_cast<std::uint32_t>(sizeof(vertexVBOData[0]) * vertexVBOData.size());
        glBindBuffer(GL_ARRAY_BUFFER, mesh.vertexVBO);
        glBufferData(GL_ARRAY_BUFFER, vertexVboBufferSize, &vertexVBOData[0], GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(vertexVBOData[0]), (void*) 0); // NOLINT(modernize-use-nullptr)
        glEnableVertexAttribArray(0);

        // Normals VBO
        glGenBuffers(1, &mesh.normalVBO);

        std::uint32_t normalVboBufferSize = static_cast<std::uint32_t>(sizeof(normalVBOData[0]) * normalVBOData.size());
        glBindBuffer(GL_ARRAY_BUFFER, mesh.normalVBO);
        glBufferData(GL_ARRAY_BUFFER, normalVboBufferSize, &normalVBOData[0], GL_STATIC_DRAW);

        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(normalVBOData[0]), (void*) 0); // NOLINT(modernize-use-nullptr)
        glEnableVertexAttribArray(1);

        // UVs VBO
        glGenBuffers(1, &mesh.uvVBO);

        std::uint32_t uvVboBufferSize = static_cast<std::uint32_t>(sizeof(uvVBOData[0]) * uvVBOData.size());
        glBindBuffer(GL_ARRAY_BUFFER, mesh.uvVBO);
        glBufferData(GL_ARRAY_BUFFER, uvVboBufferSize, &uvVBOData[0], GL_STATIC_DRAW);

        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(uvVBOData[0]), (void*) 0); // NOLINT(modernize-use-nullptr)
        glEnableVertexAttribArray(2);

        // Bone Indices VBO
        glGenBuffers(1, &mesh.boneIndexVBO);

        std::uint32_t boneIndexVBOBufferSize = static_cast<std::uint32_t>(sizeof(indexBoneIds[0]) * indexBoneIds.size());
        glBindBuffer(GL_ARRAY_BUFFER, mesh.boneIndexVBO);
        glBufferData(GL_ARRAY_BUFFER, boneIndexVBOBufferSize, &indexBoneIds[0], GL_STATIC_DRAW);

        glVertexAttribPointer(3, MAX_BONE_WEIGHTS, GL_INT, GL_FALSE, MAX_BONE_WEIGHTS * sizeof(indexBoneIds[0]), (void*) 0); // NOLINT(modernize-use-nullptr)
        glEnableVertexAttribArray(3);

        // Bone Weights VBO
        glGenBuffers(1, &mesh.boneWeightVBO);

        std::uint32_t boneWeightVBOBufferSize = static_cast<std::uint32_t>(sizeof(indexBoneWeights[0]) * indexBoneWeights.size());
        glBindBuffer(GL_ARRAY_BUFFER, mesh.boneWeightVBO);
        glBufferData(GL_ARRAY_BUFFER, boneWeightVBOBufferSize, &indexBoneWeights[0], GL_STATIC_DRAW);

        glVertexAttribPointer(4, MAX_BONE_WEIGHTS, GL_FLOAT, GL_FALSE, MAX_BONE_WEIGHTS * sizeof(indexBoneWeights[0]), (void*) 0); // NOLINT(modernize-use-nullptr)
        glEnableVertexAttribArray(4);

        // These could be unbound now, because glVertexAttribPointer registers the buffers already
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);

        // TODO: Hardcoded to only support EBO, revisit this? reason?
        mesh.drawCount = static_cast<std::int32_t>(indices.size());

        RENDER_REFERENCES.push_back(mesh);

        return RENDER_REFERENCES.size();
    }

    std::uint32_t OpenGLGfxApi::loadShader(const Shader& shader) const
    {
        SHADER_PROGRAM_REFERENCES.push_back(shader);

        return SHADER_PROGRAM_REFERENCES.size();
    }

    void OpenGLGfxApi::initializeUBORanges()
    {
        glGenBuffers(1, &UBO_);
        glBindBuffer(GL_UNIFORM_BUFFER, UBO_);

        constexpr std::uint32_t sizeOfTransforms = 3 * static_cast<std::uint32_t>(sizeof(mat4));
        // Pad to next offset if needed
        std::uint32_t lightCountOffset = std::max(sizeOfTransforms, minimumUBOOffset_);
        // Pad to next offset if needed
        std::uint32_t sizeOfLightCount = std::max(static_cast<std::uint32_t>(sizeof(std::uint32_t)), minimumUBOOffset_);
        std::uint32_t firstLightOffset = lightCountOffset + sizeOfLightCount;
        constexpr std::uint32_t sizeOfLights = 10 * (4 * sizeof(vec4));
        std::uint32_t bufferSize = firstLightOffset + sizeOfLights;
        // Create buffer of adequate size
        glBufferData(GL_UNIFORM_BUFFER, bufferSize, nullptr, GL_STATIC_DRAW);

        glBindBufferRange(GL_UNIFORM_BUFFER, 0, UBO_, 0, lightCountOffset);
        glBindBufferRange(GL_UNIFORM_BUFFER, 1, UBO_, lightCountOffset, sizeOfLightCount);
        glBindBufferRange(GL_UNIFORM_BUFFER, 2, UBO_, firstLightOffset, sizeOfLights);

        glBindBuffer(GL_UNIFORM_BUFFER, 0);
    }

    void OpenGLGfxApi::setTransformUBOData(mat4 view, mat4 projection, mat4 uiProjection) const
    {
        glBindBuffer(GL_UNIFORM_BUFFER, UBO_);

        constexpr std::uint32_t sizeOfMat4 = sizeof(mat4);
        //TODO: Used when lights are implemented
//		constexpr std::uint32_t sizeOfTransforms = 3 * sizeOfMat4;
//		std::uint32_t lightCountOffset = std::max(sizeOfTransforms, minimumUBOOffset_);
//		std::uint32_t sizeOfLightCount = std::max(static_cast<std::uint32_t>(sizeof(std::uint32_t)), minimumUBOOffset_);
//		std::uint32_t firstLightOffset = std::max(sizeOfTransforms + sizeOfLightCount, 2 * minimumUBOOffset_);
//		constexpr std::uint32_t sizeOfLight = 4 * sizeof(vec4);
//		constexpr std::uint32_t sizeOfLights = 10 * sizeOfLight;
//		std::uint32_t bufferSize = firstLightOffset + sizeOfLights;
        //                                    v-- Size of data
        //              v-- Type           v-- Offset (bytes)    v-- Data
        glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeOfMat4, &uiProjection[0][0]);
        glBufferSubData(GL_UNIFORM_BUFFER, sizeOfMat4, sizeOfMat4, &projection[0][0]);
        glBufferSubData(GL_UNIFORM_BUFFER, sizeOfMat4 * 2, sizeOfMat4, &view[0][0]);

        glBindBuffer(GL_UNIFORM_BUFFER, 0);
    }

    bool OpenGLGfxApi::initGfxDebug() const
    {
        GLint flags;
        glGetIntegerv(GL_CONTEXT_FLAGS, &flags);

        if (flags & GL_CONTEXT_FLAG_DEBUG_BIT)
        {
            glEnable(GL_DEBUG_OUTPUT);
            glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
#ifdef _WIN64
            glDebugMessageCallback(debug_callback, nullptr);
#endif
            //                    v-- source    v-- error type v-- severity
            glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);

            return true;
        }

        return false;
    }

    std::unique_ptr<IRenderComponent> OpenGLGfxApi::createRenderComponent() const
    {
        return std::make_unique<OpenglRenderComponent>();
    }
}