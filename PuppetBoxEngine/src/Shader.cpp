#include <unordered_map>

#include <glad/glad.h>

#include "Logger.h"
#include "Shader.h"

namespace PB
{
    namespace
    {
        bool compileShaderProgram(
                std::uint32_t* programId,
                const std::uint32_t* vShaderId,
                const std::uint32_t* gShaderId,
                const std::uint32_t* fShaderId)
        {
            int success;
            char infoLog[512];
            // shader Program
            *programId = glCreateProgram();

            if (*vShaderId != 0) // NOLINT(modernize-use-nullptr)
            {
                glAttachShader(*programId, *vShaderId);
            }

            if (*gShaderId != 0) // NOLINT(modernize-use-nullptr)
            {
                glAttachShader(*programId, *gShaderId);
            }

            if (*fShaderId != 0) // NOLINT(modernize-use-nullptr)
            {
                glAttachShader(*programId, *fShaderId);
            }

            glLinkProgram(*programId);
            // print linking errors if any
            glGetProgramiv(*programId, GL_LINK_STATUS, &success);
            if (!success)
            {
                glGetProgramInfoLog(*programId, 512, nullptr, infoLog);
                LOGGER_ERROR("Failed to link program'\n" + std::string(infoLog));
            }

            return success;
        }

        void checkForShaderCompileError(const std::uint32_t shaderId, const char* shaderName, bool* error)
        {
            int success;
            const int BUFF_SIZE = 1024;
            char infoLog[BUFF_SIZE];
            glGetShaderiv(shaderId, GL_COMPILE_STATUS, &success);

            if (!success)
            {
                *error = true;
                glGetShaderInfoLog(shaderId, BUFF_SIZE, nullptr, infoLog);
                LOGGER_ERROR("Failed to load shader " + std::string(shaderName) + "\n" + std::string(infoLog));
            }
        }

        bool
        compileShader(std::uint32_t* shaderId, const int shaderType, const char* shaderSource, const char* shaderName)
        {
            bool error = false;
            *shaderId = glCreateShader(shaderType);
            glShaderSource(*shaderId, 1, &shaderSource, nullptr);
            glCompileShader(*shaderId);
            checkForShaderCompileError(*shaderId, shaderName, &error);
            return !error;
        }
    }

    std::uint32_t Shader::id() const
    {
        return programId_;
    }

    std::string Shader::name() const
    {
        return shaderName_;
    }

    bool Shader::loadVertexShader(const std::string& shaderCode)
    {
        if (programId_ == 0 && vertexShaderId_ == 0)
        {
            if (!shaderCode.empty())
            {
                return compileShader(&vertexShaderId_, GL_VERTEX_SHADER, shaderCode.c_str(), "VERTEX");
            }
        }
        else
        {
            LOGGER_WARN("Vertex Shader has already been initialized for '" + shaderName_ + "'");
        }

        return true;
    }

    bool Shader::loadGeometryShader(const std::string& shaderCode)
    {
        if (programId_ == 0 && geometryShaderId_ == 0)
        {
            if (!shaderCode.empty())
            {
                return compileShader(&geometryShaderId_, GL_GEOMETRY_SHADER, shaderCode.c_str(), "GEOMETRY");
            }
        }
        else
        {
            LOGGER_WARN("Geometry Shader has already been initialized for '" + shaderName_ + "'");
        }

        return true;
    }

    bool Shader::loadFragmentShader(const std::string& shaderCode)
    {
        if (programId_ == 0 && fragmentShaderId_ == 0)
        {
            if (!shaderCode.empty())
            {
                return compileShader(&fragmentShaderId_, GL_FRAGMENT_SHADER, shaderCode.c_str(), "FRAGMENT");
            }
        }
        else
        {
            LOGGER_WARN("Fragment Shader has already been initialized for '" + shaderName_ + "'");
        }

        return true;
    }

    bool Shader::init()
    {
        if (programId_ == 0)
        {
            if (compileShaderProgram(&programId_, &vertexShaderId_, &geometryShaderId_, &fragmentShaderId_))
            {
                // Initialize UBO locations for later use.
                std::uint32_t loc = glGetUniformBlockIndex(programId_, "Transforms");

                if (loc != GL_INVALID_INDEX)
                {
                    glUniformBlockBinding(programId_, loc, 0);
                }

                loc = glGetUniformBlockIndex(programId_, "LightCounter");

                if (loc != GL_INVALID_INDEX)
                {
                    glUniformBlockBinding(programId_, loc, 1);
                }

                loc = glGetUniformBlockIndex(programId_, "LightData");

                if (loc != GL_INVALID_INDEX)
                {
                    glUniformBlockBinding(programId_, loc, 2);
                }

                return true;
            }
        }
        else
        {
            LOGGER_WARN("Shader '" + shaderName_ + "' has already been initialized");
        }

        return false;
    }

    void Shader::use() const
    {
        glUseProgram(programId_);
    }

    // Keeping this non static for consistency with Shader#use()
    void Shader::unuse() const // NOLINT(readability-convert-member-functions-to-static)
    {
        glUseProgram(0);
    }

    void Shader::setBool(const std::string& name, bool value) const
    {
        glUniform1i(glGetUniformLocation(programId_, name.c_str()), (int) value);
    }

    void Shader::setInt(const std::string& name, int value) const
    {
        glUniform1i(glGetUniformLocation(programId_, name.c_str()), value);
    }

    void Shader::setUInt(const std::string& name, std::uint32_t value) const
    {
        glUniform1ui(glGetUniformLocation(programId_, name.c_str()), value);
    }

    void Shader::setFloat(const std::string& name, float value) const
    {
        glUniform1f(glGetUniformLocation(programId_, name.c_str()), value);
    }

    void Shader::setFloatArray(const std::string& name, std::uint32_t count, float* values) const
    {
        glUniform1fv(glGetUniformLocation(programId_, name.c_str()), count, values);
    }

    void Shader::setVec2(const std::string& name, const vec2& value) const
    {
        glUniform2fv(glGetUniformLocation(programId_, name.c_str()), 1, &value[0]);
    }

    void Shader::setVec2(const std::string& name, float x, float y) const
    {
        glUniform2f(glGetUniformLocation(programId_, name.c_str()), x, y);
    }

    void Shader::setVec3(const std::string& name, const vec3& value) const
    {
        glUniform3fv(glGetUniformLocation(programId_, name.c_str()), 1, &value[0]);
    }

    void Shader::setVec3(const std::string& name, float x, float y, float z) const
    {
        glUniform3f(glGetUniformLocation(programId_, name.c_str()), x, y, z);
    }

    void Shader::setVec4(const std::string& name, const vec4& value) const
    {
        glUniform4fv(glGetUniformLocation(programId_, name.c_str()), 1, &value[0]);
    }

    void Shader::setVec4(const std::string& name, float x, float y, float z, float w) const
    {
        glUniform4f(glGetUniformLocation(programId_, name.c_str()), x, y, z, w);
    }

    void Shader::setMat2(const std::string& name, const mat2& mat) const
    {
        //glUniformMatrix2fv(glGetUniformLocation(programId_, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }

    void Shader::setMat3(const std::string& name, const mat3& mat) const
    {
        //glUniformMatrix3fv(glGetUniformLocation(programId_, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }

    void Shader::setMat4(const std::string& name, const mat4& mat) const
    {
        glUniformMatrix4fv(glGetUniformLocation(programId_, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }

    void Shader::destroy()
    {
        glDetachShader(programId_, vertexShaderId_);
        glDeleteShader(vertexShaderId_);
        glDetachShader(programId_, geometryShaderId_);
        glDeleteShader(geometryShaderId_);
        glDetachShader(programId_, fragmentShaderId_);
        glDeleteShader(fragmentShaderId_);

        glDeleteProgram(programId_);
        programId_ = 0;
        vertexShaderId_ = 0;
        geometryShaderId_ = 0;
        fragmentShaderId_ = 0;
    }
}