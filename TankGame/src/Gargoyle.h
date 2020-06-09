#pragma once
#include "Interactable.h"
#include "GameLogic.h"

class GameLogic;

class Gargoyle :
	public Interactable
{
	private:
		GameObject& gObject;

	public:
		Gargoyle(GameObject& gObject);
		virtual void interact(physx::PxRigidBody* actor, physx::PxRigidBody* invoker, physx::PxRaycastBuffer& hit, GameLogic& gameLogic) override;

};

