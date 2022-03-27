#pragma once

#include <string>
#include <utility>

#include "puppetbox/DataStructures.h"

namespace PB
{
    /**
    * \brief Class used to build and reference a single shader program.
    */
    class Shader
    {
    public:
        /**
        * \brief Creates a reference to a single shader program, referencing the given shaders.  The
        * shader must first have required shaders compiled with loadXXShader() invocations, and program
        * compiled with `init()` before it can be used in a rendering process.
        * 
        * \param shaderName     The virtual asset path of the shader program asset.
        * \param vertexPath     The virtual asset path of the vertex shader asset.
        * \param geometryPath   The virtual asset path of the geometry shader asset.
        * \param fragmentPath   The virtual asset path of the fragment shader asset.
        */
        explicit Shader(
                std::string shaderName,
                std::string vertexPath = "",
                std::string geometryPath = "",
                std::string fragmentPath = ""
        ) :
                shaderName_(std::move(shaderName)),
                vertexPath_(std::move(vertexPath)),
                geometryPath_(std::move(geometryPath)),
                fragmentPath_(std::move(fragmentPath)) {};

        /**
        * \brief Gets the gfx API specific ID for the shader program.
        * 
        * \return The ID of the shader program.
        */
        std::uint32_t id() const;

        /**
        * \brief Gets the name for the shader program.
        *
        * \return The name of the shader program, represented by the it's virtual asset path.
        */
        std::string name() const;

        /**
        * \brief Builds a vertex shader with the given shader code to be used in the shader program.
        * 
        * \param shaderCode The respective code for the shader to be compiled.
        * 
        * \return True if the shader compiled successfully, False otherwise.
        */
        bool loadVertexShader(const std::string& shaderCode);

        /**
        * \brief Builds a geometry shader with the given shader code to be used in the shader program.
        *
        * \param shaderCode The respective code for the shader to be compiled.
        *
        * \return True if the shader compiled successfully, False otherwise.
        */
        bool loadGeometryShader(const std::string& shaderCode);

        /**
        * \brief Builds a fragment shader with the given shader code to be used in the shader program.
        *
        * \param shaderCode The respective code for the shader to be compiled.
        *
        * \return True if the shader compiled successfully, False otherwise.
        */
        bool loadFragmentShader(const std::string& shaderCode);

        /**
        * \brief Compiles the shader program using the previously compiled shaders
        *
        * \return True if the shader program compiled successfully, False otherwise.
        */
        bool init();

        /**
        * \brief Actives the shader program to be used for subsequent render calls.
        */
        void use() const;

        /**
        * \brief Disables the shader program from being used for subsequent render calls.
        */
        void unuse() const;

        /**
        * \brief Sets a Boolean uniform value in the shader.
        * 
        * \param name   The name of the uniform variable in the shader.
        * \param value  The desired value for the uniform variable in the shader.
        */
        void setBool(const std::string& name, bool value) const;

        /**
        * \brief Sets a Signed Integer uniform value in the shader.
        *
        * \param name   The name of the uniform variable in the shader.
        * \param value  The desired value for the uniform variable in the shader.
        */
        void setInt(const std::string& name, std::int32_t value) const;

        /**
        * \brief Sets a Unsigned Integer uniform value in the shader.
        *
        * \param name   The name of the uniform variable in the shader.
        * \param value  The desired value for the uniform variable in the shader.
        */
        void setUInt(const std::string& name, std::uint32_t value) const;

        /**
        * \brief Sets a Float uniform value in the shader.
        *
        * \param name   The name of the uniform variable in the shader.
        * \param value  The desired value for the uniform variable in the shader.
        */
        void setFloat(const std::string& name, float value) const;

        /**
         * \brief Sets an array of Float uniform values in the shader.
         *
         * \param name      The name of the uniform variable in the shader.
         * \param size      The size of the float array.
         * \param values    Pointer to the float array.
         */
        void setFloatArray(const std::string& name, std::uint32_t size, float* values) const;

        /**
        * \brief Sets a Vec2 uniform value in the shader.
        *
        * \param name   The name of the uniform variable in the shader.
        * \param value  The desired value for the uniform variable in the shader.
        */
        void setVec2(const std::string& name, const vec2& value) const;

        /**
        * \brief Sets a Vec2 uniform value in the shader.
        *
        * \param name   The name of the uniform variable in the shader.
        * \param x      The x value to use for the Vec2 uniform variable.
        * \param y      The y value to use for the Vec2 uniform variable.
        */
        void setVec2(const std::string& name, float x, float y) const;

        /**
        * \brief Sets a Vec3 uniform value in the shader.
        *
        * \param name   The name of the uniform variable in the shader.
        * \param value  The desired value for the uniform variable in the shader.
        */
        void setVec3(const std::string& name, const vec3& value) const;

        /**
        * \brief Sets a Vec3 uniform value in the shader.
        *
        * \param name   The name of the uniform variable in the shader.
        * \param x      The x value to use for the Vec3 uniform variable.
        * \param y      The y value to use for the Vec3 uniform variable.
        * \param z      The z value to use for the Vec3 uniform variable.
        */
        void setVec3(const std::string& name, float x, float y, float z) const;

        /**
        * \brief Sets a Vec4 uniform value in the shader.
        *
        * \param name   The name of the uniform variable in the shader.
        * \param value  The desired value for the uniform variable in the shader.
        */
        void setVec4(const std::string& name, const vec4& value) const;

        /**
        * \brief Sets a Vec4 uniform value in the shader.
        *
        * \param name   The name of the uniform variable in the shader.
        * \param x      The x value to use for the Vec4 uniform variable.
        * \param y      The y value to use for the Vec4 uniform variable.
        * \param z      The z value to use for the Vec4 uniform variable.
        * \param w      The w value to use for the Vec4 uniform variable.
        */
        void setVec4(const std::string& name, float x, float y, float z, float w) const;

        /**
        * \brief Sets a Mat2 uniform value in the shader.
        *
        * \param name   The name of the uniform variable in the shader.
        * \param value  The desired value for the uniform variable in the shader.
        */
        void setMat2(const std::string& name, const mat2& mat) const;

        /**
        * \brief Sets a Mat3 uniform value in the shader.
        *
        * \param name   The name of the uniform variable in the shader.
        * \param value  The desired value for the uniform variable in the shader.
        */
        void setMat3(const std::string& name, const mat3& mat) const;

        /**
        * \brief Sets a Mat4 uniform value in the shader.
        *
        * \param name   The name of the uniform variable in the shader.
        * \param value  The desired value for the uniform variable in the shader.
        */
        void setMat4(const std::string& name, const mat4& mat) const;

        void setMat4(const std::string& name, const std::uint8_t count, const mat4* mat) const;

        /**
         * \brief Cleans up all shader resources.
         */
        void destroy();

    private:
        std::string shaderName_;
        std::string vertexPath_;
        std::string geometryPath_;
        std::string fragmentPath_;
        std::uint32_t programId_ = 0;
        std::uint32_t vertexShaderId_ = 0;
        std::uint32_t geometryShaderId_ = 0;
        std::uint32_t fragmentShaderId_ = 0;
    };
}