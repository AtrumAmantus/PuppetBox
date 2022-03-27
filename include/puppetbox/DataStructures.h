#pragma once

#include <cassert>
#include <condition_variable>
#include <cstdint>
#include <functional>
#include <mutex>
#include <ostream>
#include <queue>
#include <string>
#include <unordered_map>

#include "Constants.h"

namespace PB
{

    /**
     * \brief Similar to Java's "Optional" class, this is used to indicate if the
     * response had a result.
     *
     * \tparam T The type of data being stored in the result.
     */
    template<typename T>
    struct Result
    {
        T result;
        bool hasResult = false;

        /**
         * \brief Return an optional default value if the result was empty.
         *
         * \param defaultValue The default value to return.
         * \return The result if it was not empty, otherwise returns the given defaulValue.
         */
        T orElse(T defaultValue) const
        {
            if (hasResult)
            {
                return result;
            }
            else
            {
                return defaultValue;
            }
        };

        Result<T>& ifPresent(std::function<void(T&)> consumer)
        {
            if (hasResult)
            {
                consumer(this->result);
            }

            return *this;
        }
    };

    struct ivec2
    {
        union
        {
            std::int32_t x, r, s = 0;
        };
        union
        {
            std::int32_t y, g, t = 0;
        };

        std::int32_t& operator[](std::uint32_t i)
        {
            assert(i < 2);
            return (&x)[i];
        }

        const std::int32_t& operator[](std::uint32_t i) const
        {
            assert(i < 2);
            return (&x)[i];
        }

        ivec2 operator*(const std::int32_t& scalar) const
        {
            return {
                    this->x * scalar,
                    this->y * scalar
            };
        };

        ivec2 operator-(const ivec2& rhv) const
        {
            return {
                    this->x - rhv.x,
                    this->y - rhv.y
            };
        };
    };

    struct uivec2
    {
        union
        {
            std::uint32_t x, r, s = 0;
        };
        union
        {
            std::uint32_t y, g, t = 0;
        };

        std::uint32_t& operator[](std::uint32_t i)
        {
            assert(i < 2);
            return (&x)[i];
        }

        const std::uint32_t& operator[](std::uint32_t i) const
        {
            assert(i < 2);
            return (&x)[i];
        }
    };

    struct vec2
    {
        vec2()
        {
            this->x = 0;
            this->y = 0;
        }

        vec2(float x, float y)
        {
            this->x = x;
            this->y = y;
        }

        union
        {
            float x, r, s;
        };

        union
        {
            float y, g, t;
        };

        float& operator[](std::uint32_t i)
        {
            assert(i < 2);
            return (&x)[i];
        };

        const float& operator[](std::uint32_t i) const
        {
            assert(i < 2);
            return (&x)[i];
        };

        vec2 operator/(const float& scalar) const
        {
            return {
                    this->x / scalar,
                    this->y / scalar
            };
        };

        vec2 operator*(const float& scalar) const
        {
            return {
                    this->x * scalar,
                    this->y * scalar
            };
        };
    };

    struct ivec3
    {
        union
        {
            std::int32_t x, r, s = 0;
        };
        union
        {
            std::int32_t y, g, t = 0;
        };
        union
        {
            std::int32_t z, b, p = 0;
        };

        std::int32_t& operator[](std::uint32_t i)
        {
            assert(i < 3);
            return (&x)[i];
        }

        const std::int32_t& operator[](std::uint32_t i) const
        {
            assert(i < 3);
            return (&x)[i];
        }
    };

    struct uivec3
    {
        union
        {
            std::uint32_t x, r, s;
        };
        union
        {
            std::uint32_t y, g, t;
        };
        union
        {
            std::uint32_t z, b, p;
        };

        uivec3()
        {
            this->x = 0;
            this->y = 0;
            this->z = 0;
        }

        uivec3(std::uint32_t x, std::uint32_t y, std::uint32_t z)
        {
            this->x = x;
            this->y = y;
            this->z = z;
        }

        std::uint32_t& operator[](std::uint32_t i)
        {
            assert(i < 3);
            return (&x)[i];
        }

        const std::uint32_t& operator[](std::uint32_t i) const
        {
            assert(i < 3);
            return (&x)[i];
        }
    };

    struct vec3
    {
        union
        {
            float x, r, s;
        };
        union
        {
            float y, g, t;
        };
        union
        {
            float z, b, p;
        };

        vec3()
        {
            this->x = 0.0f;
            this->y = 0.0f;
            this->z = 0.0f;
        }

        vec3(float x, float y, float z)
        {
            this->x = x;
            this->y = y;
            this->z = z;
        }

        float& operator[](std::uint32_t i)
        {
            assert(i < 3);
            return (&x)[i];
        };

        const float& operator[](std::uint32_t i) const
        {
            assert(i < 3);
            return (&x)[i];
        };

        vec3& operator+=(const vec3& rhv)
        {
            this->x += rhv.x;
            this->y += rhv.y;
            this->z += rhv.z;
            return *this;
        };

        vec3 operator+(const vec3& rhv) const
        {
            return {
                    this->x + rhv.x,
                    this->y + rhv.y,
                    this->z + rhv.z
            };
        };

        vec3& operator-=(const vec3& rhv)
        {
            this->x -= rhv.x;
            this->y -= rhv.y;
            this->z -= rhv.z;
            return *this;
        };

        vec3 operator-(const vec3& rhv) const
        {
            return {
                    this->x - rhv.x,
                    this->y - rhv.y,
                    this->z - rhv.z
            };
        };

        vec3& operator*=(const float& scalar)
        {
            this->x *= scalar;
            this->y *= scalar;
            this->z *= scalar;
            return *this;
        };

        vec3 operator*(const float& scalar) const
        {
            return {
                    this->x * scalar,
                    this->y * scalar,
                    this->z * scalar
            };
        };

        vec3 operator/(const float& scalar) const
        {
            return {
                    this->x / scalar,
                    this->y / scalar,
                    this->z / scalar
            };
        };

        bool operator==(const vec3& rhs) const
        {
            return this->x == rhs.x
                   && this->y == rhs.y
                   && this->z == rhs.z;
        };
    };

    struct vec4
    {
        union
        {
            float x, r, s;
        };
        union
        {
            float y, g, t;
        };
        union
        {
            float z, b, p;
        };
        union
        {
            float w, a, q;
        };

        vec4()
        {
            this->x = 0.0f;
            this->y = 0.0f;
            this->z = 0.0f;
            this->w = 0.0f;
        }

        vec4(float x, float y, float z, float w)
        {
            this->x = x;
            this->y = y;
            this->z = z;
            this->w = w;
        }

        vec4(vec3 v, float w)
        {
            this->x = v[0];
            this->y = v[1];
            this->z = v[2];
            this->w = w;
        }

        vec3 vec3() const
        {
            return {this->x, this->y, this->z};
        }

        float& operator[](std::uint32_t i)
        {
            assert(i < 4);
            return (&x)[i];
        };

        const float& operator[](std::uint32_t i) const
        {
            assert(i < 4);
            return (&x)[i];
        };

        vec4 operator*(float scalar) const
        {
            return vec4{
                    this->x * scalar,
                    this->y * scalar,
                    this->z * scalar,
                    this->w * scalar
            };
        };

        vec4 operator+(vec4 rhv) const
        {
            return vec4{
                    this->x + rhv.x,
                    this->y + rhv.y,
                    this->z + rhv.z,
                    this->w + rhv.w
            };
        };

        vec4 operator-(const vec4& rhv) const
        {
            return {
                    this->x - rhv.x,
                    this->y - rhv.y,
                    this->z - rhv.z,
                    this->w - rhv.w
            };
        };

        vec4 operator/(const float& scalar) const
        {
            return {
                    this->x / scalar,
                    this->y / scalar,
                    this->z / scalar,
                    this->w / scalar
            };
        };

        vec4& operator+=(const vec4& rhv)
        {
            this->x += rhv.x;
            this->y += rhv.y;
            this->z += rhv.z;
            this->w += rhv.w;
            return *this;
        };

        bool operator==(const vec4& rhs) const
        {
            return this->x == rhs.x
                   && this->y == rhs.y
                   && this->z == rhs.z
                   && this->w == rhs.w;
        };
    };

    struct Vec4
    {
        union
        {
            Result<float> x, r, s;
        };

        union
        {
            Result<float> y, g, t;
        };

        union
        {
            Result<float> z, b, p;
        };

        union
        {
            Result<float> w, a, q;
        };

        Vec4()
        {
            x = {0.0f, false};
            y = {0.0f, false};
            z = {0.0f, false};
            w = {0.0f, false};
        }

        Vec4(float x, float y, float z, float w)
        {
            this->x = {x, true};
            this->y = {y, true};
            this->z = {z, true};
            this->w = {w, true};
        }

        Vec4(Result<float> x, Result<float> y, Result<float> z, Result<float> w)
        {
            this->x = x;
            this->y = y;
            this->z = z;
            this->w = w;
        }

        Vec4& operator=(const vec4& rhv)
        {
            this->x = {rhv.x, true};
            this->y = {rhv.y, true};
            this->z = {rhv.z, true};
            this->w = {rhv.w, true};
            return *this;
        };
    };

    struct mat2
    {

    };

    struct mat3
    {
        mat3() = default;

        explicit mat3(vec3 v[3])
        {
            values_[0] = v[0];
            values_[1] = v[1];
            values_[2] = v[2];
        };

        mat3(
                const float& x1, const float& y1, const float& z1,
                const float& x2, const float& y2, const float& z2,
                const float& x3, const float& y3, const float& z3
        )
        {
            values_[0][0] = x1;
            values_[0][1] = y1;
            values_[0][2] = z1;

            values_[1][0] = x2;
            values_[1][1] = y2;
            values_[1][2] = z2;

            values_[2][0] = x3;
            values_[2][1] = y3;
            values_[2][2] = z3;
        };

        mat3(vec3 t, vec3 r, vec3 s)
        {
            values_[0] = t;
            values_[1] = r;
            values_[2] = s;
        }

        vec3& operator[](uint32_t i)
        {
            assert(i < 3);
            return this->values_[i];
        };

        const vec3& operator[](std::uint32_t i) const
        {
            assert(i < 3);
            return this->values_[i];
        };

        static mat3 eye()
        {
            return {
                    1, 0, 0,
                    0, 1, 0,
                    0, 0, 1
            };
        }

    private:
        vec3 values_[3];
    };

    struct mat4
    {
        mat4() = default;

        explicit mat4(vec4 v1, vec4 v2, vec4 v3, vec4 v4)
        {
            values_[0].x = v1.x;
            values_[0].y = v1.y;
            values_[0].z = v1.z;
            values_[0].w = v1.w;

            values_[1].x = v2.x;
            values_[1].y = v2.y;
            values_[1].z = v2.z;
            values_[1].w = v2.w;

            values_[2].x = v3.x;
            values_[2].y = v3.y;
            values_[2].z = v3.z;
            values_[2].w = v3.w;

            values_[3].x = v4.x;
            values_[3].y = v4.y;
            values_[3].z = v4.z;
            values_[3].w = v4.w;
        }

        explicit mat4(vec4 v[4]) : mat4(v[0], v[1], v[2], v[3])
        {

        };

        explicit mat4(const float* mat4)
        {
            for (std::uint8_t i = 0; i < 4; ++i)
            {
                for (std::uint8_t j = 0; j < 4; ++j)
                {
                    values_[i][j] = mat4[(i * 4) + j];
                }
            }
        }

        mat4(
                const float& x1, const float& x2, const float& x3, const float& x4,
                const float& y1, const float& y2, const float& y3, const float& y4,
                const float& z1, const float& z2, const float& z3, const float& z4,
                const float& w1, const float& w2, const float& w3, const float& w4
        )
        {
            values_[0][0] = x1;
            values_[0][1] = y1;
            values_[0][2] = z1;
            values_[0][3] = w1;

            values_[1][0] = x2;
            values_[1][1] = y2;
            values_[1][2] = z2;
            values_[1][3] = w2;

            values_[2][0] = x3;
            values_[2][1] = y3;
            values_[2][2] = z3;
            values_[2][3] = w3;

            values_[3][0] = x4;
            values_[3][1] = y4;
            values_[3][2] = z4;
            values_[3][3] = w4;
        };

        vec4& operator[](std::uint32_t i)
        {
            assert(i < 4);
            return this->values_[i];
        };

        const vec4& operator[](std::uint32_t i) const
        {
            assert(i < 4);
            return this->values_[i];
        };

        vec4 operator*(const vec4& rhv) const
        {
            return {
                    (this->values_[0].x * rhv.x) + (this->values_[1].x * rhv.y) + (this->values_[2].x * rhv.z)
                    + (this->values_[3].x * rhv.w),
                    (this->values_[0].y * rhv.x) + (this->values_[1].y * rhv.y) + (this->values_[2].y * rhv.z)
                    + (this->values_[3].y * rhv.w),
                    (this->values_[0].z * rhv.x) + (this->values_[1].z * rhv.y) + (this->values_[2].z * rhv.z)
                    + (this->values_[3].z * rhv.w),
                    (this->values_[0].w * rhv.x) + (this->values_[1].w * rhv.y) + (this->values_[2].w * rhv.z)
                    + (this->values_[3].w * rhv.w)
            };
        };

        mat4 operator*(const mat4& rhv) const
        {
            mat4 newMatrix{};

            newMatrix[0].x = (this->values_[0].x * rhv.values_[0].x) + (this->values_[1].x * rhv.values_[0].y) +
                             (this->values_[2].x * rhv.values_[0].z) + (this->values_[3].x * rhv.values_[0].w);
            newMatrix[1].x = (this->values_[0].x * rhv.values_[1].x) + (this->values_[1].x * rhv.values_[1].y) +
                             (this->values_[2].x * rhv.values_[1].z) + (this->values_[3].x * rhv.values_[1].w);
            newMatrix[2].x = (this->values_[0].x * rhv.values_[2].x) + (this->values_[1].x * rhv.values_[2].y) +
                             (this->values_[2].x * rhv.values_[2].z) + (this->values_[3].x * rhv.values_[2].w);
            newMatrix[3].x = (this->values_[0].x * rhv.values_[3].x) + (this->values_[1].x * rhv.values_[3].y) +
                             (this->values_[2].x * rhv.values_[3].z) + (this->values_[3].x * rhv.values_[3].w);

            newMatrix[0].y = (this->values_[0].y * rhv.values_[0].x) + (this->values_[1].y * rhv.values_[0].y) +
                             (this->values_[2].y * rhv.values_[0].z) + (this->values_[3].y * rhv.values_[0].w);
            newMatrix[1].y = (this->values_[0].y * rhv.values_[1].x) + (this->values_[1].y * rhv.values_[1].y) +
                             (this->values_[2].y * rhv.values_[1].z) + (this->values_[3].y * rhv.values_[1].w);
            newMatrix[2].y = (this->values_[0].y * rhv.values_[2].x) + (this->values_[1].y * rhv.values_[2].y) +
                             (this->values_[2].y * rhv.values_[2].z) + (this->values_[3].y * rhv.values_[2].w);
            newMatrix[3].y = (this->values_[0].y * rhv.values_[3].x) + (this->values_[1].y * rhv.values_[3].y) +
                             (this->values_[2].y * rhv.values_[3].z) + (this->values_[3].y * rhv.values_[3].w);

            newMatrix[0].z = (this->values_[0].z * rhv.values_[0].x) + (this->values_[1].z * rhv.values_[0].y) +
                             (this->values_[2].z * rhv.values_[0].z) + (this->values_[3].z * rhv.values_[0].w);
            newMatrix[1].z = (this->values_[0].z * rhv.values_[1].x) + (this->values_[1].z * rhv.values_[1].y) +
                             (this->values_[2].z * rhv.values_[1].z) + (this->values_[3].z * rhv.values_[1].w);
            newMatrix[2].z = (this->values_[0].z * rhv.values_[2].x) + (this->values_[1].z * rhv.values_[2].y) +
                             (this->values_[2].z * rhv.values_[2].z) + (this->values_[3].z * rhv.values_[2].w);
            newMatrix[3].z = (this->values_[0].z * rhv.values_[3].x) + (this->values_[1].z * rhv.values_[3].y) +
                             (this->values_[2].z * rhv.values_[3].z) + (this->values_[3].z * rhv.values_[3].w);

            //TODO: Should this be hardcoded to just {0, 0, 0, 1}?
            newMatrix[0].w = (this->values_[0].w * rhv.values_[0].x) + (this->values_[1].w * rhv.values_[0].y) +
                             (this->values_[2].w * rhv.values_[0].z) + (this->values_[3].w * rhv.values_[0].w);
            newMatrix[1].w = (this->values_[0].w * rhv.values_[1].x) + (this->values_[1].w * rhv.values_[1].y) +
                             (this->values_[2].w * rhv.values_[1].z) + (this->values_[3].w * rhv.values_[1].w);
            newMatrix[2].w = (this->values_[0].w * rhv.values_[2].x) + (this->values_[1].w * rhv.values_[2].y) +
                             (this->values_[2].w * rhv.values_[2].z) + (this->values_[3].w * rhv.values_[2].w);
            newMatrix[3].w = (this->values_[0].w * rhv.values_[3].x) + (this->values_[1].w * rhv.values_[3].y) +
                             (this->values_[2].w * rhv.values_[3].z) + (this->values_[3].w * rhv.values_[3].w);

            return newMatrix;
        };

        mat4& operator*=(const mat4& rhv)
        {
            mat4 newMatrix = *this * rhv;

            this->values_[0].x = newMatrix[0].x;
            this->values_[1].x = newMatrix[1].x;
            this->values_[2].x = newMatrix[2].x;
            this->values_[3].x = newMatrix[3].x;

            this->values_[0].y = newMatrix[0].y;
            this->values_[1].y = newMatrix[1].y;
            this->values_[2].y = newMatrix[2].y;
            this->values_[3].y = newMatrix[3].y;

            this->values_[0].z = newMatrix[0].z;
            this->values_[1].z = newMatrix[1].z;
            this->values_[2].z = newMatrix[2].z;
            this->values_[3].z = newMatrix[3].z;

            return *this;
        };

        static mat4 eye()
        {
            return {
                    1, 0, 0, 0,
                    0, 1, 0, 0,
                    0, 0, 1, 0,
                    0, 0, 0, 1
            };
        }

    private:
        vec4 values_[4];
    };

    struct Transform
    {
        vec3 position;
        vec3 rotation;
        vec3 scale;
    };

    /**
    * \brief Struct defining data for a single vertex.
    */
    struct Vertex
    {
        vec3 position;
        vec3 normal;
        vec2 uv;
    };

    struct Bone
    {
        Bone() {};

        Bone(vec4 offset, vec4 scale, vec4 rotation) : offset(offset), scale(scale), rotation(rotation) {};

        //TODO: This structure seems confusing, revisit
        mat4 transform{};
        vec4 rotation{};
        vec4 scale{};
        vec4 unused{};
        union
        {
            vec4 position, offset;
        };
    };

    struct BoneNode
    {
        std::string name;
        std::uint32_t id;
        std::string parent;
        std::uint32_t parentId;
        Bone bone{};
    };

    class BoneMap
    {
    public:
        BoneMap() = default;

        BoneMap(const BoneMap& boneMap)
        {
            for (auto& entry : boneMap.boneMap_)
            {
                boneIds_.insert(
                        std::pair<std::string, std::uint32_t>{entry.second.name, entry.first}
                );
            }

            boneMap_.insert(boneMap.boneMap_.begin(), boneMap.boneMap_.end());
        };

        std::uint32_t addBone(const std::string& name, const std::string& parent, const Bone& bone)
        {
            //TODO: Need a better hashing algo, account for bone hierarchy to protect against duplicate bone names?
            std::uint32_t boneId = std::hash<std::string>()(name);

            boneIds_.insert(
                    std::pair<std::string, std::uint32_t>{name, boneId}
            );

            std::uint32_t parentId = getBoneId(parent);

            boneMap_.insert(
                    std::pair<std::uint32_t, BoneNode>{boneId, BoneNode{name, boneId, parent, parentId, bone}}
            );

            return boneId;
        };

        std::uint32_t getBoneId(const std::string& boneName) const
        {
            std::uint32_t boneId;

            auto itr = boneIds_.find(boneName);

            if (itr != boneIds_.end())
            {
                boneId = itr->second;
            }
            else
            {
                boneId = 0;
            }

            return boneId;
        };

        Result<const BoneNode*> getBone(std::uint32_t boneId) const
        {
            Result<const BoneNode*> result{};

            auto itr = boneMap_.find(boneId);

            if (itr != boneMap_.end())
            {
                result.hasResult = true;
                result.result = &itr->second;
            }

            return result;
        };

        const std::unordered_map<std::uint32_t, BoneNode>& getAllBones() const
        {
            return boneMap_;
        }

        std::unordered_map<std::uint32_t, BoneNode>& getAllBones()
        {
            return boneMap_;
        };
    private:
        std::unordered_map<std::string, std::uint32_t> boneIds_{};
        std::unordered_map<std::uint32_t, BoneNode> boneMap_{};
    };

    struct UUID
    {
        std::uint32_t bytes[4];

        UUID()
        {
            bytes[0] = 0;
            bytes[1] = 0;
            bytes[2] = 0;
            bytes[3] = 0;
        };

        UUID(std::uint32_t bytes0, std::uint32_t bytes1, std::uint32_t bytes2, std::uint32_t bytes3)
        {
            bytes[0] = bytes0;
            bytes[1] = bytes1;
            bytes[2] = bytes2;
            bytes[3] = bytes3;
        };

        UUID(const UUID& o)
        {
            this->bytes[0] = o.bytes[0];
            this->bytes[1] = o.bytes[1];
            this->bytes[2] = o.bytes[2];
            this->bytes[3] = o.bytes[3];
        };

        static UUID nullUUID()
        {
            return UUID(0, 0, 0, 0);
        }

        bool operator==(const UUID& rhs) const
        {
            return this->bytes[0] == rhs.bytes[0]
                   && this->bytes[1] == rhs.bytes[1]
                   && this->bytes[2] == rhs.bytes[2]
                   && this->bytes[3] == rhs.bytes[3];
        };

        bool operator!=(const UUID& rhs) const
        {
            return this->bytes[0] != rhs.bytes[0]
                   || this->bytes[1] != rhs.bytes[1]
                   || this->bytes[2] != rhs.bytes[2]
                   || this->bytes[3] != rhs.bytes[3];
        };

        friend std::ostream& operator<<(std::ostream& output, const UUID& uuid)
        {
            output << "UUID: {"
                   << uuid.bytes[0] << ", "
                   << uuid.bytes[1] << ", "
                   << uuid.bytes[2] << ", "
                   << uuid.bytes[3] << "}";
            return output;
        }

        bool operator<(const UUID& rhs) const
        {
            return this->bytes[0] < rhs.bytes[0]
                   || (this->bytes[0] == rhs.bytes[0] && this->bytes[1] < rhs.bytes[1])
                   || (this->bytes[0] == rhs.bytes[0]
                       && this->bytes[1] == rhs.bytes[1]
                       && this->bytes[2] < rhs.bytes[2])
                   || (this->bytes[0] == rhs.bytes[0]
                       && this->bytes[1] == rhs.bytes[1]
                       && this->bytes[2] == rhs.bytes[2]
                       && this->bytes[3] < rhs.bytes[3]);
        };
    };

    /**
     * \brief Simple struct to pass an array and it's size in one object.
     *
     * \tparam T The type of data held in the array.
     */
    template<typename T>
    struct SizedArray
    {
        T* array = nullptr;
        std::uint32_t length = 0;
    };

    namespace Concurrent
    {
        namespace NonBlocking
        {
            template<typename T>
            class Queue
            {
            public:
                void push(const T& item)
                {
                    std::unique_lock<std::mutex> mlock(mutex_);
                    queue_.push(item);
                };

                Result<T> pop()
                {
                    std::unique_lock<std::mutex> mlock(mutex_);
                    Result<T> item{};

                    if (!queue_.empty())
                    {
                        item.result = queue_.front();
                        item.hasResult = true;
                        queue_.pop();
                    }
                    else
                    {
                        item.hasResult = false;
                    }

                    return item;
                };

            private:
                std::queue<T> queue_{};
                std::mutex mutex_;
            };
        }

        namespace Blocking
        {
            template<typename T>
            class Queue
            {
            public:
                void push(const T& item)
                {
                    std::unique_lock<std::mutex> mlock(mutex_);
                    queue_.push(item);
                    mlock.unlock();
                    cond_.notify_one();
                };

                T pop()
                {
                    std::unique_lock<std::mutex> mlock(mutex_);
                    while (queue_.empty())
                    {
                        cond_.wait(mlock);
                    }
                    auto item = queue_.front();
                    queue_.pop();
                    return item;
                };

            private:
                std::queue<T> queue_{};
                std::mutex mutex_;
                std::condition_variable cond_;
            };
        }
    }
}

namespace std
{
    /**
     * \brief Define hashing operation for {\link PB::UUID} objects.
     */
    template<>
    struct hash<PB::UUID>
    {
        std::size_t operator()(const PB::UUID& value) const noexcept
        {
            return (((((value.bytes[0] * 37) + value.bytes[1]) * 37) + value.bytes[2]) * 37) + value.bytes[3];
        }
    };

    inline string to_string(const PB::UUID& uuid)
    {
        std::ostringstream ss;
        ss << uuid;
        return ss.str();
    }
}