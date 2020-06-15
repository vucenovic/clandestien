#include "Projector.h"

void Projector::interact(physx::PxRigidBody * actor, physx::PxRigidBody * invoker, physx::PxRaycastBuffer & hit, GameLogic & gameLogic)
{
	if (Inventory::instance().getInventory().filmreel) {
		auto& lightManager = gameLogic.getScene().getLightManager();
		lightManager.shadowLightUsed = true;
		lightManager.UpdateBuffer();
		Inventory::instance().removeFilmreel();
	}
}
