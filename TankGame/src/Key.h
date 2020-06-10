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
		physx::PxShape* shape; // needed for deletion

		//keys
		bool first = false;
		bool second = false;
		bool third = false;
		bool fourth = false;

	public:
		Key(GameObject& gObject);
		virtual void interact(physx::PxRigidBody* actor, physx::PxRigidBody* invoker, physx::PxRaycastBuffer& hit, GameLogic& gameLogic) override;
		void pushKeys(int keyCode);
};

