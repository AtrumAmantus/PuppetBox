#pragma once

namespace PB
{
	class SceneObject;

	class IBehavior
	{
	public:
		/**
		* \brief Initializes the behavior configurations.
		* 
		* \param sceneObject	The SceneObject the behavior is attached to.
		*/
		virtual void init(SceneObject* sceneObject) = 0;

		/**
		* \brief Runs the update logic for the behavior.
		* 
		* \param sceneObject	The SceneObject the behavior is attached to.
		* \param deltaTime		The amount of time (in Milliseconds) that have elapsed since the last update.
		*/
		virtual void update(SceneObject* sceneObject, float deltaTime) = 0;
	};
}