#include "Camera.h"
#include "GfxMath.h"

namespace PB
{
	Camera::Camera()
	{

	}

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

	mat4 Camera::calculateViewMatrix(SceneView::Mode mode)
	{
		if (mode == SceneView::Mode::ORTHO)
		{
			return mat4{
				1.0f, 0.0f, 0.0f, position_.x,
				0.0f, 1.0f, 0.0f, position_.y,
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