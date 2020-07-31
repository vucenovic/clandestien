#pragma once
#include "Interactable.h"
#include "GameLogic.h"
#include "Inventory.h"

class GameLogic;

class Key :
	public Interactable
{
	private:
		GameObject& gObject;
		physx::PxRigidBody* actor;
		physx::PxShape* shape; // needed for deletion TODO: move into gameLogic

	public:
		Key(GameObject& gObject, physx::PxRigidBody* actor) : gObject(gObject), actor(actor) {};
		virtual void interact(physx::PxRigidBody* actor, physx::PxRigidBody* invoker, physx::PxRaycastBuffer& hit, GameLogic& gameLogic) override;
};

