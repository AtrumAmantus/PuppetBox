#pragma once

#include <cassert>
#include <cstdint>

#include "../include/puppetbox/Constants.h"

namespace PB
{
    struct vec2
    {
        union { float x, r, s = 0; };
        union { float y, g, t = 0; };

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
        union { float x, r, s = 0; };
        union { float y, g, t = 0; };
        union { float z, b, p = 0; };

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
        }

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

        vec3 operator*(const float& rhv) const
        {
            return {
                    this->x * rhv,
                    this->y * rhv,
                    this->z * rhv
            };
        };
    };

    struct vec4
    {
        union { float x, r, s = 0; };
        union { float y, g, t = 0; };
        union { float z, b, p = 0; };
        union { float w, a, q = 0; };
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
        vec4 operator+(vec4 v) const
        {
            return vec4{
                    this->x + v.x,
                    this->y + v.y,
                    this->z + v.z,
                    this->w + v.w
            };
        };
    };

    struct mat2
    {

    };

    struct mat3
    {
        explicit mat3(vec3 v[3])
        {
            values_[0] = v[0];
            values_[1] = v[1];
            values_[2] = v[2];
        };
        mat3(
            float const& x1, float const& y1, float const& z1,
            float const& x2, float const& y2, float const& z2,
            float const& x3, float const& y3, float const& z3
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
            return mat3(
                1, 0, 0,
                0, 1, 0,
                0, 0, 1
            );
        }
    private:
        vec3 values_[3];
    };

    struct mat4
    {
        explicit mat4(vec4 v[4])
        {
            values_[0] = v[0];
            values_[1] = v[1];
            values_[2] = v[2];
            values_[3] = v[3];
        };
        mat4(
            float const& x1, float const& y1, float const& z1, float const& w1,
            float const& x2, float const& y2, float const& z2, float const& w2,
            float const& x3, float const& y3, float const& z3, float const& w3,
            float const& x4, float const& y4, float const& z4, float const& w4
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

        static mat4 eye()
        {
            return mat4(
                1, 0, 0, 0,
                0, 1, 0, 0,
                0, 0, 1, 0,
                0, 0, 0, 1
            );
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
        vec3 colour{ 0.0f, 0.0f, 0.0f };
        bool useColour = false;
    };
}