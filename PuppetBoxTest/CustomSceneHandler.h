#pragma once

#include <memory>

#include <PuppetBox.h>

class CustomSceneHandler : public PB::AbstractSceneHandler
{
public:
	void setUp()
	{
		std::unique_ptr<PB::BasicMessage> message = std::make_unique<PB::BasicMessage>(PB::Event::Type::ADD_TO_SCENE, "pyramid");
		publishEvent(&(*message));
		//addToScene("pyramid");
	};
	void update(float deltaTime)
	{

	};
};