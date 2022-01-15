#pragma once

#include <cassert>
#include <cstdint>
#include <string>

#include "Constants.h"

namespace PB
{
    struct vec2
    {
        union
        {
            float x, r, s = 0;
        };
        union
        {
            float y, g, t = 0;
        };

        float& operator[](uint32_t i)
        {
            assert(i < 2);
            return (&x)[i];
        };

        const float& operator[](uint32_t i) const
        {
            assert(i < 2);
            return (&x)[i];
        };
    };

    struct vec3
    {
        union
        {
            float x, r, s = 0;
        };
        union
        {
            float y, g, t = 0;
        };
        union
        {
            float z, b, p = 0;
        };

        float& operator[](uint32_t i)
        {
            assert(i < 3);
            return (&x)[i];
        };

        const float& operator[](uint32_t i) const
        {
            assert(i < 3);
            return (&x)[i];
        };

        vec3& operator=(const vec2& rhv)
        {
            this->x = rhv.x;
            this->y = rhv.y;
            return *this;
        };

        vec3& operator+=(vec3 const& rhv)
        {
            this->x += rhv.x;
            this->y += rhv.y;
            this->z += rhv.z;
            return *this;
        };

        vec3& operator+=(vec2 const& rhv)
        {
            this->x += rhv.x;
            this->y += rhv.y;
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

        vec3 operator-(const vec3& rhv) const
        {
            return {
                    this->x - rhv.x,
                    this->y - rhv.y,
                    this->z - rhv.z
            };
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
            float x, r, s = 0;
        };
        union
        {
            float y, g, t = 0;
        };
        union
        {
            float z, b, p = 0;
        };
        union
        {
            float w, a, q = 0;
        };

        float& operator[](uint32_t i)
        {
            assert(i < 4);
            return (&x)[i];
        };

        const float& operator[](uint32_t i) const
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

        vec4& operator+=(vec4 const& rhv)
        {
            this->x += rhv.x;
            this->y += rhv.y;
            this->z += rhv.z;
            this->w += rhv.w;
            return *this;
        };

        vec4& operator+=(vec3 const& rhv)
        {
            this->x += rhv.x;
            this->y += rhv.y;
            this->z += rhv.z;
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

        const vec3& operator[](uint32_t i) const
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

        explicit mat4(vec4 v[4])
        {
            values_[0].x = v[0].x;
            values_[0].y = v[0].y;
            values_[0].z = v[0].z;
            values_[0].w = v[0].w;

            values_[1].x = v[1].x;
            values_[1].y = v[1].y;
            values_[1].z = v[1].z;
            values_[1].w = v[1].w;

            values_[2].x = v[2].x;
            values_[2].y = v[2].y;
            values_[2].z = v[2].z;
            values_[2].w = v[2].w;

            values_[3].x = v[3].x;
            values_[3].y = v[3].y;
            values_[3].z = v[3].z;
            values_[3].w = v[3].w;
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

        vec4& operator[](uint32_t i)
        {
            assert(i < 4);
            return this->values_[i];
        };

        const vec4& operator[](uint32_t i) const
        {
            assert(i < 4);
            return this->values_[i];
        };

        mat4& operator*=(mat4 const& rhv)
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

    /**
    * \brief Struct defining data for a single vertex.
    */
    struct Vertex
    {
        vec3 position;
        vec3 normal;
        vec2 uv;
        vec3 colour{0.0f, 0.0f, 0.0f};
        bool useColour = false;
    };

    struct Bone
    {
        Bone() {};

        Bone(vec4 offset, vec4 scale, vec4 rotation) : offset(offset), scale(scale), rotation(rotation) {};

        union
        {
            mat4 translation{};
            struct
            {
                vec4 rotation;
                vec4 scale;
                vec4 unused;
                union
                {
                    vec4 position, offset;
                };
            };
        };
    };

    struct BoneMap
    {
        std::string name;
        std::string parent;
        std::uint32_t depth;
        Bone bone{};
    };

    template<typename T>
    struct Result
    {
        T result;
        bool hasResult;
    };
}