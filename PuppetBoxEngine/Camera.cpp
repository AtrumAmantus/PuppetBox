#include "pch.h"

#include "Camera.h"

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
			return mat4::eye();
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