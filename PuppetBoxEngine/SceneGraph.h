#pragma once

#include <memory>
#include <string>
#include <vector>

#include "../include/puppetbox/AbstractSceneHandler.h"
#include "Logger.h"
#include "TypeDef.h"

namespace PB
{
	class SceneGraph
	{
	public:
		SceneGraph(std::string sceneName) : id_(sceneName) {};
		void init()
		{
			// Nothing yet
		};
		void setSceneHandler(AbstractSceneHandler* sceneHandler)
		{
			sceneHandler_ = sceneHandler;
			sceneHandler_->setUp();
		};
		void update(const float deltaTime)
		{
			sceneHandler_->update(deltaTime);
		}
		void render() const
		{
			sceneHandler_->render();
		}
	private:
		std::string id_;
		AbstractSceneHandler* sceneHandler_ = nullptr;
	};
}