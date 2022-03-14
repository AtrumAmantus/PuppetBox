#include "puppetbox/Camera.h"
#include "GfxMath.h"

namespace PB
{
    void Camera::update(float deltaTime)
    {
        // Direction based movement
        // Move "forward" / "backward"
        position_ += back_ * directionalMoveVector_.z * panSpeed_ * deltaTime;
        // "Strafe" relative to facing direction
        position_ += right_ * directionalMoveVector_.x * panSpeed_ * deltaTime;
        // "Elevate" relative to up direction
        position_ += up_ * directionalMoveVector_.y * panSpeed_ * deltaTime;

        // Axis based movement
        position_ += moveVector_ * panSpeed_ * deltaTime;

        currentZoom_ = targetZoom_;
    }

    void Camera::move(vec3 vector)
    {
        moveVector_ = vector;
    }

    void Camera::directionalMove(vec3 vector)
    {
        directionalMoveVector_ = vector;
    }

    void Camera::moveTo(vec3 position)
    {
        position_ = position;
    }

    void Camera::moveNear(vec3 position, vec3 offset)
    {
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
        //TODO: Need to handle this in update(float) with different paths for mouse/keyboard

        // X is pitch
        // Y is yaw
        // Z is roll
        pyr_.x -= rotation.x;
        if (pyr_.x > 89.0f) pyr_.x = 89.0f;
        if (pyr_.x < -89.0f) pyr_.x = -89.0f;
        pyr_.y += rotation.y;

        back_.z = cos(pyr_.y * GfxMath::RADS_PER_DEGREE) * cos(pyr_.x * GfxMath::RADS_PER_DEGREE);
        back_.y = sin(pyr_.x * GfxMath::RADS_PER_DEGREE);
        back_.x = sin(pyr_.y * GfxMath::RADS_PER_DEGREE) * cos(pyr_.x * GfxMath::RADS_PER_DEGREE);

        back_ = GfxMath::Normalize(back_);

        // Using cross product on "absolute up" and "back" gets relative "right"
        right_ = GfxMath::Normalize(GfxMath::Cross({0.0f, 1.0f, 0.0f}, back_));
        // Using cross product on relative "back" and "right" gets relative "up"
        up_ = GfxMath::Cross(back_, right_);
    }

    void Camera::zoom(std::int8_t direction)
    {
        targetZoom_ += direction * zoomSpeed_;

        if (targetZoom_ > maxZoom_)
        {
            targetZoom_ = maxZoom_;
        }
        else if (targetZoom_ < minZoom_)
        {
            targetZoom_ = minZoom_;
        }
    }

    const mat4 Camera::calculateViewMatrix(RenderWindow renderWindow, SceneView::Mode mode) const
    {
        mat4 view;

        if (mode == SceneView::Mode::ORTHO)
        {
            float z = currentZoom_ / 100.0f;

            // Centers the camera on its own position for orthographic
            // Subtract position because moving the "camera" is really just
            // moving the "world" the opposite direction.
            view = mat4{
                    z, 0.0f, 0.0f, ((*renderWindow.width / 2)) - (position_.x * z),
                    0.0f, z, 0.0f, ((*renderWindow.height / 2)) - (position_.y * z),
                    0.0f, 0.0f, 1.0f, -position_.z,
                    0.0f, 0.0f, 0.0f, 1.0f
            };
        }
        else
        {
            view = GfxMath::LookAt(
                    position_,
                    position_ - back_,
                    up_
            );
        }

        return view;
    }

    const mat4 Camera::calculateInverseViewMatrix(RenderWindow renderWindow, SceneView::Mode viewMode) const
    {
        float z = 100.0f / currentZoom_;

        mat4 inverseView{};

        if (viewMode == SceneView::ORTHO)
        {
            inverseView = mat4{
                    z, 0.0f, 0.0f, position_.x - (((*renderWindow.width * z) / 2)),
                    0.0f, z, 0.0f, position_.y - (((*renderWindow.height * z) / 2)),
                    0.0f, 0.0f, 1.0f, position_.z,
                    0.0f, 0.0f, 0.0f, 1.0f
            };
        }
        else
        {
            //TODO: Implement inverse perspective view
        }

        return inverseView;
    }

    const vec3 Camera::getPosition() const
    {
        return position_;
    }

    void Camera::setPanSpeed(float panSpeed)
    {
        panSpeed_ = panSpeed;
    }

    void Camera::setZoomSpeed(float zoomSpeed)
    {
        zoomSpeed_ = zoomSpeed;
    }
}