#pragma once
#include "Interactable.h"


class Key :
	public Interactable
{
	private:
		GameObject& gObject;

	public:
		Key::Key(GameObject& gObject);
		virtual void interact(PxRigidBody* actor, PxRigidBody* invoker, PxRaycastBuffer& hit, GameLogic& gameLogic) override;
};

