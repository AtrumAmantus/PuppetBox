#include <vector>

#include "puppetbox/DataStructures.h"
#include "GfxMath.h"
#include "OpenGLGfxApi.h"

namespace PB
{
    namespace
    {
        /**
        * \brief Searches for the provided vertex in the provided vector.  Two ertices are considered equal if each of
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
            std::cout << "Build: Alpha 0.0.1" << std::endl;

            glEnable(GL_DEPTH_TEST);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

            // Store the card's minimum UBO offset value for later.
            std::int32_t value;
            glGetIntegerv(GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT, &value);
            minimumUBOOffset_ = static_cast<std::uint32_t>(value);
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
        glViewport(0, 0, static_cast<int32_t>(width_), static_cast<int32_t>(height_));
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    void OpenGLGfxApi::setRenderDimensions(std::uint32_t width, std::uint32_t height)
    {
        width_ = width;
        height_ = height;
    }

    std::uint32_t OpenGLGfxApi::getRenderWidth()
    {
        return width_;
    }

    std::uint32_t OpenGLGfxApi::getRenderHeight()
    {
        return height_;
    }

    ImageReference OpenGLGfxApi::loadImage(ImageData imageData, ImageOptions options) const
    {
        ImageReference imageReference{0};

        if (imageData.bufferData)
        {
            std::uint32_t openGLId;

            glGenTextures(1, &openGLId);
            glBindTexture(GL_TEXTURE_2D, openGLId);

            if (options.repeatMode == ImageOptions::Mode::CLAMP_TO_BORDER)
            {
                // Repeat texture
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

                glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, options.colour);
            }
            else
            {
                // Repeat texture
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
            }

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

            imageReference = ImageReference{openGLId};
        }

        return imageReference;
    }

    Mesh OpenGLGfxApi::loadMesh(Vertex* vertexData, std::uint32_t vertexCount) const
    {
        Mesh mesh{};

        std::uint32_t stride = 3 + 3 + 2 + (3 * vertexData[0].useColour);

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
            }
            else
            {
                indices.push_back(static_cast<std::uint32_t>(index));
            }
        }

        std::vector<float> vboData{};

        // Convert unique vertex vector to float array
        for (auto& v: uniqueVertices)
        {
            vboData.push_back(v.position.x);
            vboData.push_back(v.position.y);
            vboData.push_back(v.position.z);

            vboData.push_back(v.normal.x);
            vboData.push_back(v.normal.y);
            vboData.push_back(v.normal.z);

            vboData.push_back(v.uv.x);
            vboData.push_back(v.uv.y);

            if (vertexData[0].useColour)
            {
                vboData.push_back(v.colour.x);
                vboData.push_back(v.colour.y);
                vboData.push_back(v.colour.z);
            }
        }

        // Create buffers
        glGenVertexArrays(1, &(mesh.VAO));
        glGenBuffers(1, &mesh.VBO);
        glGenBuffers(1, &mesh.EBO);

        glBindVertexArray(mesh.VAO);

        glBindBuffer(GL_ARRAY_BUFFER, mesh.VBO);
        glBufferData(GL_ARRAY_BUFFER, static_cast<std::intmax_t>(sizeof(vboData[0]) * vboData.size()), &vboData[0],
                     GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, static_cast<std::intmax_t>(sizeof(indices[0]) * indices.size()),
                     &indices[0], GL_STATIC_DRAW);

        // position attribute
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, static_cast<std::int32_t>(stride * sizeof(float)),
                              (void*) 0); // NOLINT(modernize-use-nullptr)
        glEnableVertexAttribArray(0);

        // normal attribute
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, static_cast<std::int32_t>(stride * sizeof(float)),
                              (void*) (3 * sizeof(float)));
        glEnableVertexAttribArray(1);

        // texture attribute
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, static_cast<std::int32_t>(stride * sizeof(float)),
                              (void*) (6 * sizeof(float)));
        glEnableVertexAttribArray(2);

        if (vertexData[0].useColour)
        {
            // colour attribute
            glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, static_cast<std::int32_t>(stride * sizeof(float)),
                                  (void*) (8 * sizeof(float)));
            glEnableVertexAttribArray(3);
        }

        // These could be unbound now, because glVertexAttribPointer registers the buffers already
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);

        // TODO: Hardcoded to only support EBO, revisit this? reason?
        mesh.drawCount = static_cast<int32_t>(indices.size());

        return mesh;
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

    void OpenGLGfxApi::setTransformUBOData(mat4 view, mat4 projection) const
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
        //glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeOfMat4, &orthoProjection[0][0]);
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
            glDebugMessageCallback(debug_callback, nullptr);
            //                    v-- source    v-- error type v-- severity
            glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);

            return true;
        }

        return false;
    }
}