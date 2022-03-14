#pragma once

#include <memory>

#include "Constants.h"
#include "DataStructures.h"
#include "RenderWindow.h"
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
         * \brief Update the current state of the camera based on current input.
         *
         * \param deltaTime The time (in seconds) since the last update().
         */
        void update(float deltaTime);

        /**
        * \brief Set the move vector for the camera that will be used to calculate position changes during
        * update() calls.  This calculates axis based movement along X, Y, and Z axis, without regard to
        * camera orientation.
        *
        * \param vector	A vector containing movement values along x, y, and z axis.
        */
        void move(vec3 vector);

        /**
        * \brief Set the move vector for the camera that will be used to calculate position changes during
        * update() calls.  This calculates direction based movement with X representing left / right,
         * Y representing up / down, and Z representing forward / backward.
        *
        * \param vector	A vector containing directional movement data.
        */
        void directionalMove(vec3 vector);

        /**
        * \brief Instantly set the current position of the camera.
        *
        * \param vector	A vector containing position values for x, y, and z axis.
        */
        void moveTo(vec3 position);

        /**
         * \brief Instantly set the current position of the camera to one "near" the target position.
         *
         * \param position The position the camera should be centered "near".
         * \param offset   The minimum distance the camera can be from the target position.
         */
        void moveNear(vec3 position, vec3 offset);

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
        const mat4 calculateViewMatrix(RenderWindow renderWindow, SceneView::Mode mode) const;

        /**
         * \brief Calculates the Inverse View Matrix for transforming window coordinates into
         * world coordinates.
         *
         * \param renderWindow  {\link PB::RenderWindow} instance containing render window dimensions.
         * \param mode          The current projection {\link PB::SceneView::Mode}.
         *
         * \return The Inverse View Matrix
         */
        const mat4 calculateInverseViewMatrix(RenderWindow renderWindow, SceneView::Mode mode) const;

        /**
         * Returns the current position of the camera in X, Y, and Z coordinate values.
         *
         * \return The position of the camera in X, Y, and Z coordinate values.
         */
        const vec3 getPosition() const;

        /**
         * Sets the speed at which the camera moves around the world space.
         *
         * \param panSpeed The speed at which the camera moves in units per second.
         */
        void setPanSpeed(float panSpeed);

        /**
         * Sets the speed at which the camera zooms in and out.
         *
         * \param zoomSpeed The speed at which the camera zooms per second
         */
        void setZoomSpeed(float zoomSpeed);

    private:
        vec3 position_{};
        vec3 pyr_{};
        vec3 back_{0, 0, 1};
        vec3 up_{0, 1, 0};
        vec3 right_{1, 0, 0};
        vec3 moveVector_{};
        vec3 directionalMoveVector_{};
        float panSpeed_ = 5.0f;
        float zoomSpeed_ = 1.0f;
        float currentZoom_ = 100.0f;
        float targetZoom_ = 100.0f;
        float minZoom_ = 50.0f;
        float maxZoom_ = 150.0f;
    };
}