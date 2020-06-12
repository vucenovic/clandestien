#include "Door.h"

void Door::interact(physx::PxRigidBody* actor, physx::PxRigidBody* invoker, physx::PxRaycastBuffer& hit, GameLogic& gameLogic)
{
	if (Inventory::instance().getInventory().key) {
		//
	}
}

void Door::LateUpdate()
{
	gObject.GetTransform().SetPostion(PxConv<glm::vec3>(actor->getGlobalPose().p));
}
