#pragma once
#include "Interactable.h"
#include "GameLogic.h"

class GameLogic;

class Gargoyle :
	public Interactable
{
	private:
		GameObject& gObject;
		physx::PxRigidBody* actor;

	public:
		Gargoyle(GameObject& gObject, physx::PxRigidBody* actor) : gObject(gObject), actor(actor) {};
		virtual void interact(physx::PxRigidBody* actor, physx::PxRigidBody* invoker, physx::PxRaycastBuffer& hit, GameLogic& gameLogic) override;

		void LateUpdate();
};
