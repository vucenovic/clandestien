#include "Gargoyle.h"

void Gargoyle::interact(physx::PxRigidBody* actor, physx::PxRigidBody* invoker, physx::PxRaycastBuffer& hit, GameLogic& gameLogic)
{
	using namespace physx;
	glm::vec3 viewVector = gameLogic.getScene().activeCamera->GetTransform().GetForward();
	float factor = (float)(glfwGetKey(gameLogic.getWindow(), gameLogic.getKeyBinds().forward) == GLFW_PRESS) - (glfwGetKey(gameLogic.getWindow(), gameLogic.getKeyBinds().backward) == GLFW_PRESS);
	if (factor != 0) {
		auto currPos = actor->getGlobalPose();
		viewVector.y = 0;
		glm::vec3 moveDir = glm::normalize(viewVector) * factor * 2.0f * gameLogic.getDelta();
		auto newPos = PxTransform(moveDir.x, moveDir.y, moveDir.z).transform(currPos);
		actor->setGlobalPose(newPos);
	}
}

void Gargoyle::LateUpdate()
{
	gObject.GetTransform().SetPostion(PxConv<glm::vec3>(actor->getGlobalPose().p));
}
