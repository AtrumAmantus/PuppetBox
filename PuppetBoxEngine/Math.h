#pragma once

#include <assert.h>
#include <stdint.h>

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

        vec3& operator+=(vec3 const& rhv)
        {
            this->x += rhv.x;
            this->y += rhv.y;
            this->z += rhv.z;
            return *this;
        };

        vec3 operator+(const vec3& rhv)
        {
            return {
                    this->x + rhv.x,
                    this->y + rhv.y,
                    this->z + rhv.z
            };
        };

        vec3 operator-(const vec3& rhv)
        {
            return {
                    this->x - rhv.x,
                    this->y - rhv.y,
                    this->z - rhv.z
            };
        };

        vec3 operator*(const float& rhv)
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
        vec4 operator*(float scalar)
        {
            return vec4{
                    this->x * scalar,
                    this->y * scalar,
                    this->z * scalar,
                    this->w * scalar
            };
        };
        vec4 operator+(vec4 v)
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
        mat3(vec3 v[3])
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
        mat4(vec4 v[4])
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

	namespace GfxMath
	{
        /**
        * \brief Compares two vec2 objects to see if they are "basically" equal.  Comparisons use a
        * defined threshold for equality.
        * 
        * \param v1 The first vec2 to compare.
        * \param v2 The second vec2 to compare.
        * 
        * \return True if they are "basically" equal, False otherwise.
        */
        bool BasicallyEqual(vec2 v1, vec2 v2);

        /**
        * \brief Compares two vec3 objects to see if they are "basically" equal.  Comparisons use a
        * defined threshold for equality.
        *
        * \param v1 The first vec3 to compare.
        * \param v2 The second vec3 to compare.
        *
        * \return True if they are "basically" equal, False otherwise.
        */
        bool BasicallyEqual(vec3 v1, vec3 v2);

        /**
        * \brief Compares two vertex objects to see if they are "basically" equal.  Comparisons use a
        * defined threshold for equality.
        *
        * \param v1 The first vertex to compare.
        * \param v2 The second vertex to compare.
        *
        * \return True if they are "basically" equal, False otherwise.
        */
        bool BasicallyEqual(Vertex v1, Vertex v2);

        /**
        * \brief Finds the dot product of two vectors.
        * 
        * \param v  The first vector.
        * \param v  The second vector.
        * 
        * \return The dot product of the two vectors.
        */
        float Dot(vec3 v1, vec3 v2);

        /**
        * \brief Normalizes the vector.
        * 
        * \param v  The original vector.
        * 
        * \return The normalized vector.
        */
        vec3 Normalize(vec3 v);

		/**
		* \brief Create a transformation matrix using a starting position, target looked at, and relative up.  Used
		* to create a "view matrix".
		* 
		* \param origin	The position of the originating point.
		* \param target	The position of the targeted point.
		* \param up		The relative "up" vector for the originating point.
		* 
		* \return The transformation matrix (mat4) for the given references.
		*/
		mat4 LookAt(vec3 origin, vec3 target, vec3 up);

		/**
		* \brief Create a projection transformation matrix.
		*
		* \param viewWidth	The width of the render window area.
		* \param viewHeight	The height of the render window area.
		* \param viewMode	The view mode to use (Perspective/Ortho).
		*
		* \return The projection transformation matrix (mat4) for the given references.
		*/
		mat4 Projection(uint32_t viewWidth, uint32_t viewHeight, SceneView::Mode viewMode);

        /**
        * \brief Scales the matrix by the given values.
        * 
        * \param m  The matrix of values to scale.
        * \param s  The values by which to scale the matrix value.
        * 
        * \return The matrix after it has been scaled.
        */
        mat4 Scale(mat4 m, vec3 s);

        /**
        * \brief Translates the matrix by the given values.
        *
        * \param m  The matrix of values to translate.
        * \param s  The values by which to translate the matrix value.
        *
        * \return The matrix after it has been translated.
        */
        mat4 Translate(mat4 m, vec3 t);

        /**
        * \brief Rotates the matrix by the given values.
        *
        * \param m      The matrix of values to rotate.
        * \param angle  The angle to rotate by.
        * \param axies  The axis weight to apply to the rotation value.
        *
        * \return The matrix after it has been rotated.
        */
        mat4 Rotate(mat4 m, float angle, vec3 axies);

        /**
        * \brief Rotates the matrix on each axis by the respective given value in order: x, y, z.
        *
        * \param m      The matrix of values to rotate.
        * \param angles The angles by which to rotate each axis.
        *
        * \return The matrix after it has been rotated.
        */
        mat4 Rotate(mat4 m, vec3 angles);
	}
}