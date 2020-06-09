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
	bool first = false;
	bool second = false;
	bool third = false;
	bool fourth = false;

	while (!glfwGetKey(gameLogic.getWindow(), GLFW_KEY_SPACE) && GLFW_PRESS) {
		if (glfwGetKey(gameLogic.getWindow(), GLFW_KEY_Q) && GLFW_PRESS) {
			first = true;
		} if (glfwGetKey(gameLogic.getWindow(), GLFW_KEY_M) && GLFW_PRESS && first) {
			second = true;
		} if (glfwGetKey(gameLogic.getWindow(), GLFW_KEY_G) && GLFW_PRESS && first && second) {
			third = true;
		} if (glfwGetKey(gameLogic.getWindow(), GLFW_KEY_Y) && GLFW_PRESS && first && second && third) {
			fourth = true;
		}
	}

	if (first && second && third && fourth) {
		actor->detachShape(*shape);
		gameLogic.getPxScene()->removeActor(*actor);
		gameLogic.getScene().RemoveObject("old_key");
		//TODO: add key to character inventory
	}

	// leave view if failed or succed
	gameLogic.getScene().activeCamera = oldCamera;
	

}


