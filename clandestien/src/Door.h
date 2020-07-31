#pragma once
#include "Interactable.h"
#include "GameLogic.h"
#include "Inventory.h"

#include <PhysX/PxPhysicsAPI.h>

class GameLogic;

class Door :
	public Interactable
{
private:
	GameObject& gObject;
	physx::PxRigidBody* actor;

public:
	Door(GameObject& gObject, physx::PxRigidBody* actor) : gObject(gObject), actor(actor) {};
	virtual void interact(physx::PxRigidBody* actor, physx::PxRigidBody* invoker, physx::PxRaycastBuffer& hit, GameLogic& gameLogic) override;
};

