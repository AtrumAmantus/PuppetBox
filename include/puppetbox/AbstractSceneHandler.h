#pragma once

#include <vector>

#include "../PuppetBox.h"

namespace PB
{
	/**
	* \brief Base class to create derived SceneHandlers for guiding events within a scene.
	*/
	class PUPPET_BOX_API AbstractSceneHandler
	{
	public:
		/**
		* \brief Runs only once, when the scene first loads
		*/
		virtual void setUp() = 0;

		/**
		* \brief Runs once per frame, after input processing, but before rendering.
		*/
		virtual void update(const float deltaTime) = 0;

		/**
		* \brief Runs once per frame, after updates
		*/
		virtual void render() = 0;
	};
}