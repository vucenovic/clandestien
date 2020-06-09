#include "Key.h"

void Key::interact(physx::PxRigidBody * actor, physx:: PxRigidBody * invoker, physx::PxRaycastBuffer & hit, GameLogic & gameLogic)
{
	// change camera to bird perspective
	Camera camera = Camera();
	glm::vec3 world_up(0.0f, 1.0f, 0.0f);
	glm::vec3 world_north(1.0f, 0.0f, 0.0f);
	glm::vec3 camPosition = world_up * 10.0f;
	float height = 10.0f;
	glm::vec3 camTraget = glm::vec3(actor->getGlobalPose().p[0], actor->getGlobalPose().p[1], actor->getGlobalPose().p[2]);
	glm::mat4 view = glm::lookAt(camPosition, camTraget, world_north);
	camera.SetViewParameters(*gameLogic.getScene().viewDataBuffer, view, gameLogic.getScene().activeCamera->getProjectionMatrix());
	Camera* oldCamera = gameLogic.getScene().activeCamera;
	gameLogic.getScene().activeCamera = &camera;

	// unhexing logic

}
