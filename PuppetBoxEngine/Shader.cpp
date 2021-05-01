#include "pch.h"

#include <fstream>
#include <sstream>
#include <unordered_map>

#include <glad/glad.h>

#include "Logger.h"
#include "Shader.h"

namespace PB
{
    namespace
    {
        bool compileShaderProgram(uint32_t* programId, uint32_t* vShaderId, uint32_t* gShaderid, uint32_t* fShaderId)
        {
            int success;
            char infoLog[512];
            // shader Program
            *programId = glCreateProgram();

            if (vShaderId != 0)
            {
                glAttachShader(*programId, *vShaderId);
            }

            if (gShaderid != 0)
            {
                glAttachShader(*programId, *gShaderid);
            }

            if (fShaderId != 0)
            {
                glAttachShader(*programId, *fShaderId);
            }

            glLinkProgram(*programId);
            // print linking errors if any
            glGetProgramiv(*programId, GL_LINK_STATUS, &success);
            if (!success)
            {
                glGetProgramInfoLog(*programId, 512, NULL, infoLog);
                LOGGER_ERROR("Failed to link program'\n" + std::string(infoLog));
            }

            // delete the shaders as they're linked into our program now and no longer necessary
            glDeleteShader(*vShaderId);
            glDeleteShader(*gShaderid);
            glDeleteShader(*fShaderId);

            return success;
        }

        void checkForShaderCompileError(const uint32_t shaderId, const char* shaderName, bool* error)
        {
            int success;
            const int BUFF_SIZE = 1024;
            char infoLog[BUFF_SIZE];
            glGetShaderiv(shaderId, GL_COMPILE_STATUS, &success);

            if (!success)
            {
                *error = true;
                glGetShaderInfoLog(shaderId, BUFF_SIZE, NULL, infoLog);
                LOGGER_ERROR("Failed to load shader " + std::string(shaderName) + "\n" + std::string(infoLog));
            }
        }

        bool compileShader(uint32_t* shaderId, const int shaderType, const char* shaderSource, const char* shaderName)
        {
            bool error = false;
            *shaderId = glCreateShader(shaderType);
            glShaderSource(*shaderId, 1, &shaderSource, NULL);
            glCompileShader(*shaderId);
            checkForShaderCompileError(*shaderId, shaderName, &error);
            return !error;
        }
    }

    const uint32_t Shader::id() const
    {
        return programId_;
    }

    const std::string Shader::name() const
    {
        return shaderName_;
    }

    bool Shader::loadVertexShader(std::string shaderCode)
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

    bool Shader::loadGeometryShader(std::string shaderCode)
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

    bool Shader::loadFragmentShader(std::string shaderCode)
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
                //TODO: Add this later
                //glUniformBlockBinding(programId_, glGetUniformBlockIndex(programId_, "Transforms"), 0);
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

    void Shader::unuse() const
    {
        glUseProgram(0);
    }

    void Shader::setBool(const std::string& name, bool value) const
    {
        glUniform1i(glGetUniformLocation(programId_, name.c_str()), (int)value);
    }

    void Shader::setInt(const std::string& name, int value) const
    {
        glUniform1i(glGetUniformLocation(programId_, name.c_str()), value);
    }

    void Shader::setUInt(const std::string& name, uint32_t value) const
    {
        glUniform1ui(glGetUniformLocation(programId_, name.c_str()), value);
    }

    void Shader::setFloat(const std::string& name, float value) const
    {
        glUniform1f(glGetUniformLocation(programId_, name.c_str()), value);
    }

    void Shader::setVec2(const std::string& name, const vec2& value) const
    {
        //glUniform2fv(glGetUniformLocation(programId_, name.c_str()), 1, &value[0]);
    }

    void Shader::setVec2(const std::string& name, float x, float y) const
    {
        glUniform2f(glGetUniformLocation(programId_, name.c_str()), x, y);
    }

    void Shader::setVec3(const std::string& name, const vec3& value) const
    {
        //glUniform3fv(glGetUniformLocation(programId_, name.c_str()), 1, &value[0]);
    }
    void Shader::setVec3(const std::string& name, float x, float y, float z) const
    {
        glUniform3f(glGetUniformLocation(programId_, name.c_str()), x, y, z);
    }

    void Shader::setVec4(const std::string& name, const vec4& value) const
    {
        //glUniform4fv(glGetUniformLocation(programId_, name.c_str()), 1, &value[0]);
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
        //glUniformMatrix4fv(glGetUniformLocation(programId_, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }
}