#pragma once

#include <string>

#include "Math.h"

namespace PB
{
    class Shader
    {
    public:
        Shader(
            const std::string shaderName,
            const std::string vertexPath = "",
            const std::string geometryPath = "",
            const std::string fragmentPath = ""
        ) :
            shaderName_(shaderName),
            vertexPath_(vertexPath),
            geometryPath_(geometryPath),
            fragmentPath_(fragmentPath) {};
        const uint32_t id() const;
        const std::string name() const;
        bool loadVertexShader(std::string shaderCode);
        bool loadGeometryShader(std::string shaderCode);
        bool loadFragmentShader(std::string shaderCode);
        bool init();
        void use() const;
        void unuse() const;
        void setBool(const std::string& name, bool value) const;
        void setInt(const std::string& name, int32_t value) const;
        void setUInt(const std::string& name, uint32_t value) const;
        void setFloat(const std::string& name, float value) const;
        void setVec2(const std::string& name, const vec2& value) const;
        void setVec2(const std::string& name, float x, float y) const;
        void setVec3(const std::string& name, const vec3& value) const;
        void setVec3(const std::string& name, float x, float y, float z) const;
        void setVec4(const std::string& name, const vec4& value) const;
        void setVec4(const std::string& name, float x, float y, float z, float w) const;
        void setMat2(const std::string& name, const mat2& mat) const;
        void setMat3(const std::string& name, const mat3& mat) const;
        void setMat4(const std::string& name, const mat4& mat) const;
    private:
        std::string shaderName_;
        std::string vertexPath_;
        std::string geometryPath_;
        std::string fragmentPath_;
        uint32_t programId_ = 0;
        uint32_t vertexShaderId_ = 0;
        uint32_t geometryShaderId_ = 0;
        uint32_t fragmentShaderId_ = 0;
    };
}