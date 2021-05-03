#pragma once

#include <memory>
#include <string>
#include <vector>

#include "../include/puppetbox/AbstractSceneHandler.h"
#include "Logger.h"
#include "TypeDef.h"

namespace PB
{
	/**
	* \brief SceneGraph represents a single rendering scene within an application.  This could be a
	* title menu, or the game itself, allowing for switching between different scenes.
	*/
	class SceneGraph
	{
	public:
		/**
		* \brief Creates a new scene with a given name to be later referenced with.
		* 
		* \return The unique name this scene should be referenced by later.
		*/
		SceneGraph(std::string sceneName) : id_(sceneName) {};

		/**
		* \brief Sets a SceneHandler for this scene.
		* 
		* \param sceneHandler	The SceneHandler to be used for the scene.
		*/
		void setSceneHandler(AbstractSceneHandler* sceneHandler)
		{
			sceneHandler_ = sceneHandler;
			sceneHandler_->setUp();
		};

		/**
		* \brief Invokes the update() method of the sceneHandler, allowing updates for various scene
		* elements for the current frame.
		*
		* \param deltaTime	The amount of time passed since the last update invocation (in Milliseconds).
		*/
		void update(const float deltaTime)
		{
			sceneHandler_->update(deltaTime);
		}

		/**
		* \brief Invokes the render() method of the sceneHandler, rendering out any desired SceneObjects
		* for the current frame in the scene.
		*/
		void render() const
		{
			sceneHandler_->render();
		}
	private:
		std::string id_;
		AbstractSceneHandler* sceneHandler_ = nullptr;
	};
}