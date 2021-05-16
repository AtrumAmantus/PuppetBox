#pragma once

#include <cstdint>

#include "puppetbox/DataStructures.h"

namespace PB::GfxMath
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
    mat4 Projection(std::uint32_t viewWidth, std::uint32_t viewHeight, SceneView::Mode viewMode);

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