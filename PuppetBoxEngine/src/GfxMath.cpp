#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "GfxMath.h"

#define FLOAT_EQUALITY_THRESHOLD 0.0000001f
#define PI 3.1415926

namespace PB::GfxMath
{
    namespace
    {
        const float DEG_TO_RAD = PI / 180;

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
                    {m[0].x, m[0].y, m[0].z, m[0].w},
                    {m[1].x, m[1].y, m[1].z, m[1].w},
                    {m[2].x, m[2].y, m[2].z, m[2].w},
                    {m[3].x, m[3].y, m[3].z, m[3].w}
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

    mat4 Projection(std::uint32_t viewWidth, std::uint32_t viewHeight, std::uint32_t viewDepth, SceneView::Mode viewMode)
    {
        mat4 projection = mat4::eye();

        auto vWidth = static_cast<float>(viewWidth);
        auto vHeight = static_cast<float>(viewHeight);
        auto vDepth = static_cast<float>(viewDepth);

        // Using +/- (dimension / 2) will center the camera on 0,0
        float left = 0;
        float right = vWidth;
        float bottom = 0;
        float top = vHeight;
        float zNear = 0;
        float zFar = vDepth;

        if (viewMode == SceneView::Mode::ORTHO)
        {
            projection[0][0] = 2.0f / (right - left); // Normalize x
            projection[1][1] = 2.0f / (top - bottom); // Normalize y, not inverted so +y is "up"
            projection[2][2] = (2.0f / (zFar - zNear)); // Normalize z, -z is toward user
            projection[3][0] = -((right + left) / (right - left)); // Sets x:0 to left edge
            projection[3][1] = -((top + bottom) / (top - bottom)); // Sets y:0 to top edge
            projection[3][2] = -((zFar + zNear) / (zFar - zNear)); // Sets z:0 to screen
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
                m[0] * s.x,
                m[1] * s.y,
                m[2] * s.z,
                m[3]
        };

        return mat4{v};
    }

    mat4 Translate(mat4 m, vec3 t)
    {
        m[3] = vec4{
                m[0].x * t.x + m[1].x * t.y + m[2].x * t.z + m[3].x,
                m[0].y * t.x + m[1].y * t.y + m[2].y * t.z + m[3].y,
                m[0].z * t.x + m[1].z * t.y + m[2].z * t.z + m[3].z,
                m[3].w
        };

        return m;
    }

    mat4 Rotate(mat4 m, vec3 angles)
    {
        glm::mat4 gM{
                m[0].x, m[0].y, m[0].z, m[0].w,
                m[1].x, m[1].y, m[1].z, m[1].w,
                m[2].x, m[2].y, m[2].z, m[2].w,
                m[3].x, m[3].y, m[3].z, m[3].w
        };

        //TODO: Lets make our own later
        gM = glm::rotate(gM, angles.x * DEG_TO_RAD, {1.0f, 0.0f, 0.0f});
        gM = glm::rotate(gM, angles.y * DEG_TO_RAD, {0.0f, 1.0f, 0.0f});
        gM = glm::rotate(gM, angles.z * -DEG_TO_RAD, {0.0f, 0.0f, 1.0f}); // Rotate z the other way

        return glmToMat4(gM);
    }

    mat4 CreateTransformation(vec3 rotation, vec3 scale, vec3 position)
    {
        mat4 r = Rotate(mat4::eye(), rotation);

        mat4 p = Translate(mat4::eye(), position);

        mat4 s = Scale(mat4::eye(), scale);

        return p * (r * s);
    }
}