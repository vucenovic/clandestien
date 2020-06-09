#pragma once
#include "Interactable.h"
#include "GameLogic.h"

class GameLogic;

class Key :
	public Interactable
{
	private:
		GameObject& gObject;
		physx::PxShape* shape; // needed for deletion

	public:
		Key(GameObject& gObject);
		virtual void interact(physx::PxRigidBody* actor, physx::PxRigidBody* invoker, physx::PxRaycastBuffer& hit, GameLogic& gameLogic) override;
		void pushKeys(int keyCode);
};

