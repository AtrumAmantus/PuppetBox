#pragma once

#include <stdint.h>

#include "../PuppetBox.h"
#include "../../PuppetBoxEngine/Math.h"
#include "../../PuppetBoxEngine/IModel.h"

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
	public:
		SceneObject() : id_(0), model_(nullptr) {};
		SceneObject(uint32_t id, IModel* model) : id_(id), model_(model) {};
		const uint32_t id() const;
		
		/**
		* \brief Calls updates() and updates model matrices.
		* 
		* \param deltaTime	The time passed (in Milliseconds) since the last update.
		*/
		void update(float deltaTime)
		{
			updates(deltaTime);
			vec3 transform[3] = { position, rotation, scale };
			model_->updateModelMatrix(transform);
		};

		/**
		* \brief Executes GFX API specific render calls on injected assets.
		*/
		void render() {
			model_->render();
		};
	protected:
		/**
		* \brief Update method for consumer defined update logic, called by update().
		* 
		* \param deltaTime	The time passed (in Milliseconds) since the last update.
		*/
		virtual void updates(float deltaTime) {};
	private:
		uint32_t id_ = 0;
		IModel* model_;
	};
}