#include "Filmreel.h"

void Filmreel::interact(physx::PxRigidBody * actor, physx::PxRigidBody * invoker, physx::PxRaycastBuffer & hit, GameLogic & gameLogic)
{
	gameLogic.getScene().RemoveObject("Filmreel");
	gameLogic.getPxScene()->removeActor(*actor);
	Inventory::instance().addKey();
}
