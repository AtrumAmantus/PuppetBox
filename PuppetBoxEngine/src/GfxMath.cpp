#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "GfxMath.h"

#define FLOAT_EQUALITY_THRESHOLD 0.0000001f

namespace PB::GfxMath
{
    namespace
    {
        /**
        * \brief Helper function to convert from GLM's mat4 to local mat4 struct
        *
        * \param m	The glm::mat4 object populated with data
        *
        * \return The local mat4 representation of the given glm::mat4 object
        */
        mat4 glmToMat4(glm::mat4 m)
        {
            vec4 v[4] = {
                    {m[0][0], m[0][1], m[0][2], m[0][3]},
                    {m[1][0], m[1][1], m[1][2], m[1][3]},
                    {m[2][0], m[2][1], m[2][2], m[2][3]},
                    {m[3][0], m[3][1], m[3][2], m[3][3]}
            };

            return mat4{v};
        }
    }

    bool BasicallyEqual(vec2 v1, vec2 v2)
    {
        return abs(v1.x - v2.x) < FLOAT_EQUALITY_THRESHOLD
               && abs(v1.y - v2.y) < FLOAT_EQUALITY_THRESHOLD;
    }

    bool BasicallyEqual(vec3 v1, vec3 v2)
    {
        return abs(v1.x - v2.x) < FLOAT_EQUALITY_THRESHOLD
               && abs(v1.y - v2.y) < FLOAT_EQUALITY_THRESHOLD
               && abs(v1.z - v2.z) < FLOAT_EQUALITY_THRESHOLD;
    }

    bool BasicallyEqual(Vertex v1, Vertex v2)
    {
        return BasicallyEqual(v1.position, v2.position)
               && BasicallyEqual(v1.normal, v2.normal)
               //&& BasicallyEqual(v1.colour, v2.colour)
               && BasicallyEqual(v1.uv, v2.uv);
    }

    float Dot(vec3 v1, vec3 v2)
    {
        return (v1.x * v2.x) + (v1.y * v2.y) + (v1.z * v2.z);
    }

    vec3 Normalize(vec3 v)
    {
        /**
        * Finds the Dot product of v and itself, which is it's magnitude squared, then
        * multiplies the inverse square of that value on the vector, normalizing it.
        */
        return v * (1.0f / sqrt(Dot(v, v)));
    }

    mat4 LookAt(vec3 origin, vec3 target, vec3 up)
    {
        return glmToMat4(glm::lookAt(glm::vec3{origin.x, origin.y, origin.z}, {target.x, target.y, target.z},
                                     {up.x, up.y, up.z}));
    }

    mat4 Projection(std::uint32_t viewWidth, std::uint32_t viewHeight, SceneView::Mode viewMode)
    {
        mat4 projection = mat4::eye();

        auto vWidth = static_cast<float>(viewWidth);
        auto vHeight = static_cast<float>(viewHeight);

        // Using +/- (dimension / 2) will center the camera on 0,0
        float left = 0;
        float right = vWidth;
        float bottom = 0;
        float top = vHeight;

        float zNear = -1.0f;
        float zFar = 1.0f;

        if (viewMode == SceneView::Mode::ORTHO)
        {
            projection[0][0] = 2.0f / (right - left); // Normalize x, world center on [0,0]
            projection[1][1] = 2.0f / (top - bottom); // Normalize y, world center on [0,0], not inverted so +y is "up"
            projection[2][2] = -(2.0f / (zFar - zNear)); // Normalize z, inverted so +z is toward user
            projection[3][0] = -((right + left) / (right - left)); // Sets x:0 to left edge
            projection[3][1] = -((top + bottom) / (top - bottom)); // Sets y:0 to top edge
            projection[3][2] = 0; //-((zFar + zNear) / (zFar - zNear)); //TODO: What are these for?
        }
        else
        {
            //TODO: Implement perspective projection logic
            //projection = glm::perspective(glm::radians(45.0f), static_cast<float>(viewWidth) / static_cast<float>(viewHeight), 0.1f, 100.0f);
        }

        return projection;
    }

    mat4 Scale(mat4 m, vec3 s)
    {
        vec4 v[4] = {
                m[0] * s[0],
                m[1] * s[1],
                m[2] * s[2],
                m[3]
        };

        return mat4{v};
    }

    mat4 Translate(mat4 m, vec3 t)
    {
        m[3] = m[0] * t[0] + m[1] * t[1] + m[2] * t[2] + m[3];
        return m;
    }

    mat4 Rotate(mat4 m, float angle, vec3 axies)
    {
        glm::mat4 gM{
                m[0][0], m[0][1], m[0][2], m[0][3],
                m[1][0], m[1][1], m[1][2], m[1][3],
                m[2][0], m[2][1], m[2][2], m[2][3],
                m[3][0], m[3][1], m[3][2], m[3][3]
        };

        glm::mat4 gMat4 = glm::rotate(gM, angle, {axies.x, axies.y, axies.z});

        return glmToMat4(gMat4);
    }

    mat4 Rotate(mat4 m, vec3 angles)
    {
        m = Rotate(m, angles.x, {1.0f, 0.0f, 0.0f});
        m = Rotate(m, angles.y, {0.0f, 1.0f, 0.0f});
        m = Rotate(m, angles.z, {0.0f, 0.0f, 1.0f});

        return m;
    }
}