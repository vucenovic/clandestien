#include "Projector.h"

void Projector::interact(physx::PxRigidBody * actor, physx::PxRigidBody * invoker, physx::PxRaycastBuffer & hit, GameLogic & gameLogic)
{
	if (Inventory::instance().getInventory().filmreel == true) {
		//TODO: activate spotlight
	}
}
