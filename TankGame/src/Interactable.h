#pragma once
#include <PhysX/PxPhysicsAPI.h>
#include "GameObject.h"

class GameLogic;

class Interactable
{
	public:
		virtual void interact(physx::PxRigidBody* actor, physx::PxRigidBody* invoker, physx::PxRaycastBuffer& hit, GameLogic& gameLogic) = 0; // parent function for interactables interaction
};

