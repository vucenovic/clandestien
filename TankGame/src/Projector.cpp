#include "Projector.h"

void Projector::interact(physx::PxRigidBody * actor, physx::PxRigidBody * invoker, physx::PxRaycastBuffer & hit, GameLogic & gameLogic)
{
	if (Inventory::instance().getInventory().filmreel == true) {
		auto& lightManager = gameLogic.getScene().getLightManager();
		lightManager.shadowLightUsed = true;
	}
}
