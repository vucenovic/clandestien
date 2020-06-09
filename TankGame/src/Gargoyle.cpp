#include "Gargoyle.h"

Gargoyle::Gargoyle(GameObject & gObject)
	:gObject(gObject)
{
}

void Gargoyle::interact(PxRigidBody* actor, PxRigidBody* invoker, PxRaycastBuffer& hit, GameLogic& gameLogic)
{
	using namespace physx;
	glm::vec3 viewVector = gameLogic.getScene().activeCamera->GetTransform().GetForward();
	auto &transform = gObject.GetTransform();
	float factor = (float)(glfwGetKey(gameLogic.getWindow(), gameLogic.getKeyBinds().forward) == GLFW_PRESS) - (glfwGetKey(gameLogic.getWindow(), gameLogic.getKeyBinds().backward) == GLFW_PRESS);
	if (factor != 0) {
		auto currPos = actor->getGlobalPose();
		viewVector.y = 0;
		glm::vec3 moveDir = glm::normalize(viewVector) * factor * 2.0f * gameLogic.getDelta();
		auto newPos = PxTransform(moveDir.x, moveDir.y, moveDir.z).transform(currPos);
		transform.SetPostion(PxToGlmVec3(PxExtendedVec3(actor->getGlobalPose().p[0], actor->getGlobalPose().p[1], actor->getGlobalPose().p[2])));
		actor->setGlobalPose(newPos);
	}
}
