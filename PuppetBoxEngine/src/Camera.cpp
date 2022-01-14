#include "puppetbox/Camera.h"
#include "GfxMath.h"

namespace PB
{
    Camera::Camera() = default;

    Camera::Camera(vec3 position, vec3 direction) : position_(position), direction_(direction)
    {

    }

    void Camera::move(vec3 vector)
    {
        moveVector_ += vector;
    }

    void Camera::rotate(vec3 rotation)
    {
        //TODO: Implement rotate() method;
    }

    void Camera::zoom(std::uint8_t direction)
    {
        zoom_ += direction;

        if (zoom_ > maxZoom_)
        {
            zoom_ = maxZoom_;
        }
        else if (zoom_ < minZoom_)
        {
            zoom_ = minZoom_;
        }
    }

    mat4 Camera::calculateViewMatrix(SceneView::Mode mode)
    {
        if (mode == SceneView::Mode::ORTHO)
        {
            float z = (float)zoom_ / 10;

            return mat4{
                    z, 0.0f, 0.0f, position_.x,
                    0.0f, z, 0.0f, position_.y,
                    0.0f, 0.0f, 1.0f, position_.z,
                    0.0f, 0.0f, 0.0f, 1.0f
            };
        }
        else
        {
            return GfxMath::LookAt(
                    position_,
                    position_ + direction_,
                    up_
            );
        }
    }
}