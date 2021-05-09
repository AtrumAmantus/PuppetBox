#pragma once

#include "../include/puppetbox/Constants.h"
#include "../include/puppetbox/DataStructures.h"

namespace PB
{
	/**
	* \brief Acts as the point of reference within the scene for render calls.
	*/
	class Camera
	{
	public:
		/**
		* \brief Create a default instance.
		*/
		Camera();

		/**
		* \brief Create an instance with initial position & direction.
		* 
		* \param position	The initial position of the camera.
		* \param direction	The initial direction of the camera.
		*/
		Camera(vec3 position, vec3 direction);

		/**
		* \brief Set the move vector for the camera.
		*
		* \param vector	A vector containing movement values along x, y, and z axis.
		*/
		void move(vec3 vector);

		/**
		* \brief Set the rotation of the camera.
		*
		* \param rotation	A vector containing rotation values around x, y, and z axis.
		*/
		void rotate(vec3 rotation);

		/**
		* \brief Calculates a View Matrix from the camera's attributes.
		*
		* \param mode	The current projection mode for the camera (Perspective / Ortho).
		* 
		* \return The calculated mat4 view matrix.
		*/
		mat4 calculateViewMatrix(SceneView::Mode mode);
	private:
		vec3 position_{};
		vec3 direction_{};
		vec3 up_{};
		vec3 right_{};
		vec3 moveVector_{};
		float cameraSpeed_ = 1.0f;
	};
}