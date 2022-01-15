#include "puppetbox/Camera.h"
#include "GfxMath.h"

namespace PB
{
    Camera::Camera() = default;

    Camera::Camera(vec3 position, vec3 direction) : position_(position), direction_(direction)
    {

    }

    void Camera::update(float deltaTime)
    {
        position_ += (moveVector_ * (cameraSpeed_ * deltaTime));
        currentZoom_ = targetZoom_;
    }

    void Camera::move(vec3 vector)
    {
        moveVector_ = vector;
    }

    void Camera::rotate(vec3 rotation)
    {
        //TODO: Implement rotate() method;
    }

    void Camera::zoom(std::int8_t direction)
    {
        targetZoom_ += direction;

        if (targetZoom_ > maxZoom_)
        {
            targetZoom_ = maxZoom_;
        }
        else if (targetZoom_ < minZoom_)
        {
            targetZoom_ = minZoom_;
        }
    }

    mat4 Camera::calculateViewMatrix(SceneView::Mode mode) const
    {
        if (mode == SceneView::Mode::ORTHO)
        {
            float z = currentZoom_ / 100.0f;

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