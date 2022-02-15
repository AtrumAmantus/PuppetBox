#include "puppetbox/Camera.h"
#include "GfxMath.h"

namespace PB
{
    Camera::Camera(std::unique_ptr<IRenderWindow> renderWindow) : renderWindow_(std::move(renderWindow))
    {

    }

    void Camera::update(float deltaTime)
    {
        position_ += moveVector_ * deltaTime;
        currentZoom_ = targetZoom_;
    }

    void Camera::move(vec3 vector)
    {
        moveVector_ = vector;
    }

    void Camera::centerOn(vec3 position)
    {
        vec2 dimensions = renderWindow_->getDimensions();

        position_ = position - vec3(dimensions.x / 2.0f, dimensions.y / 2.0f, 0.0f);
    }

    void Camera::centerNear(vec3 position, vec3 offset)
    {
        vec2 dimensions = renderWindow_->getDimensions();
        position -= vec3(dimensions.x / 2.0f, dimensions.y / 2.0f, 0.0f);

        if (position_.x < position.x - offset.x)
        {
            position_.x = position.x - offset.x;
        }
        else if (position_.x > position.x + offset.x)
        {
            position_.x = position.x + offset.x;
        }

        if (position_.y < position.y - offset.y)
        {
            position_.y = position.y - offset.y;
        }
        else if (position_.y > position.y + offset.y)
        {
            position_.y = position.y + offset.y;
        }

        if (position_.z < position.z - offset.z)
        {
            position_.z = position.z - offset.z;
        }
        else if (position_.z > position.z + offset.z)
        {
            position_.z = position.z + offset.z;
        }
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

            // Negative coords because moving the "camera" is really just
            // moving the "world" the opposite direction.
            return mat4{
                    z, 0.0f, 0.0f, -position_.x,
                    0.0f, z, 0.0f, -position_.y,
                    0.0f, 0.0f, 1.0f, -position_.z,
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