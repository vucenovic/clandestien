#include "Projector.h"

void Projector::interact(physx::PxRigidBody * actor, physx::PxRigidBody * invoker, physx::PxRaycastBuffer & hit, GameLogic & gameLogic)
{
	if (Inventory::instance().getInventory().filmreel) {
		ResourceManager & resourceManager = ResourceManager::GetInstance();
		auto& lightManager = gameLogic.getScene().getLightManager();
		lightManager.shadowLightUsed = true;
		lightManager.UpdateBuffer();
		gameLogic.getScene().AddObject(std::make_unique<GameObject>(Transform(glm::vec3(2.241f, 1.208f, -0.044f), glm::vec3(1.5708f, 0, 1.52319f), glm::vec3(1.131f, 1.131f, 1.131f)), resourceManager.GetMesh("FilmReel"), resourceManager.GetMaterial("ProjectorMaterial"), "FilmReelOnProjector"));
		Inventory::instance().removeFilmreel();
	}
}
