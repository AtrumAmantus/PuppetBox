#pragma once

#include "Constants.h"
#include "DataStructures.h"
#include "TypeDef.h"

namespace PB
{
    /**
    * \brief Acts as the point of reference within the scene for render calls.
    */
    class PUPPET_BOX_API Camera
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
         * \brief Update the current state of the camera based on current input.
         *
         * \param deltaTime The time (in milliseconds) since the last update().
         */
        void update(float deltaTime);

        /**
        * \brief Set the move vector for the camera that will be used to calculate position changes during
        * update() calls.
        *
        * \param vector	A vector containing movement values along x, y, and z axis.
        */
        void move(vec3 vector);

        /**
        * \brief Instantly set the current position of the camera.
        *
        * \param vector	A vector containing position values for x, y, and z axis.
        */
        void setPosition(vec3 vector);

        /**
        * \brief Set the rotation of the camera.
        *
        * \param rotation	A vector containing rotation values around x, y, and z axis.
        */
        void rotate(vec3 rotation);

        /**
         * \brief Zoom in with the camera in the given direction.  Input should be positive or negative
         * integers.
         *
         * \param direction The direction to scroll in (negative/out or positive/in).
         */
        void zoom(std::int8_t direction);

        /**
        * \brief Calculates a View Matrix from the camera's attributes.
        *
        * \param mode	The current projection mode for the camera (Perspective / Ortho).
        *
        * \return The calculated mat4 view matrix.
        */
        mat4 calculateViewMatrix(SceneView::Mode mode) const;

    private:
        vec3 position_{};
        vec3 direction_{};
        vec3 up_{};
        vec3 right_{};
        vec3 moveVector_{};
        float currentZoom_ = 100.0f;
        float targetZoom_ = 100.0f;
        float minZoom_ = 50.0f;
        float maxZoom_ = 150.0f;
        float cameraSpeed_ = 100.0f;
    };
}