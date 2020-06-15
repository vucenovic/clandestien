#include "Door.h"
#include "GameLogic.h"

void Door::interact(physx::PxRigidBody* actor, physx::PxRigidBody* invoker, physx::PxRaycastBuffer& hit, GameLogic& gameLogic)
{
	using namespace physx;
	if (Inventory::instance().getInventory().key) {
		auto doorPos = actor->getGlobalPose();
		doorPos.q = doorPos.q * PxConv<PxQuat>(glm::vec3(0, -glm::pi<float>() / 2, 0));
		actor->setGlobalPose(doorPos);
		gObject.GetTransform().SetRotationDegrees(0, -90, 0);
		Inventory::instance().removeKey();
	}
}
