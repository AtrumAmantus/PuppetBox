#pragma once

#include <stdint.h>

#include <memory>

#include "../PuppetBox.h"
#include "Constants.h"
#include "DataStructures.h"
#include "IBehavior.h"
#include "IModel.h"

namespace PB
{
	/**
	 * \brief The basic container for renderable objects in a scene.  Derived classes should be used to extend with additional needed attributes
	 * such as name, health, etc.
	 * 
	 * This class serves as a basic container to inject asset references into for later GFX API calls during rendering.
	 */
	class PUPPET_BOX_API SceneObject
	{
	public:

		/**
		* \brief World relative position of object.
		*/
		vec3 position{ 0.0f, 0.0f, 0.0f };

		/**
		* \brief World relative rotation of object.
		*/
		vec3 rotation{ 0.0f, 0.0f, 0.0f };

		/**
		* \brief World relative scale of object.
		*/
		vec3 scale{ 1.0f, 1.0f, 1.0f };

		/**
		* \brief The current move forces being applied to the object.
		*/
		vec3 velocity{ 0.0f, 0.0f, 0.0f };

		/**
		* \brief The max speed of the object.
		*/
		float speed = 64.0f;
	public:
		SceneObject() : id_(0), model_(nullptr) {};
		SceneObject(uint32_t id, vec3 baseScale, IModel* model) : id_(id), baseScale_(baseScale), model_(model) {};

		/**
		* \brief Returns the ID for the object.
		*
		* \return The ID of the object.
		*/
		const uint32_t id() const;

		/**
		* \brief Calls updates() and updates model matrices.
		*
		* \param deltaTime	The time passed (in Milliseconds) since the last update.
		*/
		void update(float deltaTime);

		/**
		* \brief Executes GFX API specific render calls on injected assets.
		*/
		void render();

		/**
		* \brief Set a predefined behavior on the object. IBehavior#init() is called
		* immediately after being added to the SceneObject.
		*
		* \param behavior	The predefined behavior to set.
		*/
		void setBehavior(AI::Behavior behavior);

		/**
		* \brief Set a custom derived behavior on the object. IBehavior#init() is never
		* called, and must be invoked manually.
		*
		* \param behavior	The custom derived behavior to set.
		*/
		void setBehavior(std::unique_ptr<IBehavior> behavior);

		/**
		* \brief Returns the actual scale of the object, which is the product of the
		* public scale vector and the internal baseScale vector.
		* 
		* \return The actual scale of the object.
		*/
		vec3 actualScale();
	protected:
		/**
		* \brief Update method for consumer defined update logic, called by update().
		*
		* \param deltaTime	The time passed (in Milliseconds) since the last update.
		*/
		virtual void updates(float deltaTime) {};
	private:
		uint32_t id_ = 0;
		vec3 baseScale_{ 1.0f, 1.0f, 1.0f };
		IModel* model_ = nullptr;
		std::unique_ptr<IBehavior> behavior_{ nullptr };
	};
}