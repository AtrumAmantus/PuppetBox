#pragma once

#include "../include/puppetbox/IBehavior.h"
#include "../include/puppetbox/SceneObject.h"
#include "Math.h"
#include "Utilities.h"

namespace PB
{
	class WanderBehavior : public IBehavior
	{
	public:
		void init(SceneObject* sceneObject)
		{
			targetPosition_ = sceneObject->position;
		};

		void update(SceneObject* sceneObject, float deltaTime)
		{
			if (waitTime_ <= 0)
			{
				if (GfxMath::BasicallyEqual(sceneObject->position, targetPosition_))
				{
					waitTime_ = ((3 * RandomUtils::pseudoRand()) + 1);

					float randX = (128 * RandomUtils::pseudoRand()) - 64;
					float randY = (128 * RandomUtils::pseudoRand()) - 64;

					float dirX = randX == 0 ? 1 : (abs(randX) / randX);
					float dirY = randY == 0 ? 1 : (abs(randY) / randY);

					vec3 newPosOffset = vec3{
						randX + (dirX * 32),
						randY + (dirY * 32),
						0
					};

					targetPosition_ = sceneObject->position + newPosOffset;
				}
				else
				{
					vec3 positionDelta = targetPosition_ - sceneObject->position;
					float deltaSpeed = sceneObject->speed * deltaTime;

					if (GfxMath::Dot(positionDelta, positionDelta) < deltaSpeed)
					{
						sceneObject->position = targetPosition_;
					}
					else
					{
						vec3 deltaMove = GfxMath::Normalize(positionDelta) * deltaSpeed;
						sceneObject->position += deltaMove;
					}
				}
			}
			else
			{
				waitTime_ -= deltaTime;
			}
		}
	private:
		vec3 targetPosition_{};
		float waitTime_ = 0;
	};
}