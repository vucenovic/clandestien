#pragma once
#include "Interactable.h"
#include <PhysX/PxPhysicsAPI.h>
#include "GameObject.h"
#include "GameLogic.h"
#include <GL\glew.h>
#include <glm\glm.hpp>


using namespace physx;

class Gargoyle :
	public Interactable
{
	private:
		GameObject& gObject;

	public:
		Gargoyle::Gargoyle(GameObject& gObject);
		virtual void interact(PxRigidBody* actor, PxRigidBody* invoker, PxRaycastBuffer& hit, GameLogic& gameLogic) override;

};

