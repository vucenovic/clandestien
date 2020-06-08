#include "GameLogic.h"

GameLogic::GameLogic(Scene &scene, physx::PxScene* pxScene, PxController* c, GLFWwindow* window, CameraController &cameraController, KeyMap keyMap, float deltatime, PxRigidDynamic* gargoyleBox)
	: ourScene(scene), ourPxScene(pxScene), character(c), ourWindow(window), ourCameraController(cameraController), keyBinds(keyMap), deltaTime(deltatime), ourGargoyleBox(gargoyleBox)
{
}

void GameLogic::checkGameState()
{
	moveControllerCamera();
	movePortal();
	raycastFilter();
}

void GameLogic::moveControllerCamera()
{
	ourCameraController.HandleInputs();

	glm::vec3 forwardVector = ourScene.activeCamera->GetTransform().GetForward();
	forwardVector.y = 0; //clamp movement to horizonal plane
	forwardVector = glm::normalize(forwardVector);
	glm::vec3 rightVector = glm::cross(forwardVector, glm::vec3(0, 1, 0));

	glm::vec3 moveDir = forwardVector * (float)((glfwGetKey(ourWindow, keyBinds.forward) == GLFW_PRESS) - (glfwGetKey(ourWindow, keyBinds.backward) == GLFW_PRESS)) +
		rightVector * (float)((glfwGetKey(ourWindow, keyBinds.right) == GLFW_PRESS) - (glfwGetKey(ourWindow, keyBinds.left) == GLFW_PRESS));
	moveDir *= cP.characterMoveSpeed * deltaTime; 
	moveDir.y = -0.01f;

	PxControllerFilters filters(NULL, NULL, NULL);
	PxControllerCollisionFlags collFlags = character->move(PxVec3(moveDir.x, moveDir.y, moveDir.z), 0.0, deltaTime, filters, NULL);

	PxControllerState state;
	character->getState(state);
	PxExtendedVec3 newPos = character->getFootPosition();
	ourCameraController.cameraTransform->SetPostion(glm::vec3(newPos[0], newPos[1] + cP.characterEyeHeight, newPos[2])); 
	
}

void GameLogic::raycastFilter()
{
	glm::vec3 camPos = ourScene.activeCamera->GetTransform().GetPosition();
	PxVec3 origin = PxVec3(camPos.x, camPos.y, camPos.z);            
	glm::vec3 viewVector = ourScene.activeCamera->GetTransform().GetForward();

	PxVec3 unitDir = PxVec3(viewVector.x, viewVector.y, viewVector.z);            
	PxReal maxDistance = 1.0;            
	PxRaycastBuffer hit;

	PxQueryFilterData filterData = PxQueryFilterData(); 
	filterData.data.word0 = GROUP1; 
	const PxHitFlags outputFlags = PxHitFlag::eDEFAULT | PxHitFlag::ePOSITION | PxHitFlag::eNORMAL;

	//Nutz das "Userdata" attribute von PxActor um eine 1:1 beziehung mit dem Gameobject oder was auch immer zu erzeugen.
	bool status = ourPxScene->raycast(origin, unitDir, maxDistance, hit, outputFlags, filterData);
	if (status && (glfwGetKey(ourWindow, keyBinds.interaction) == GLFW_PRESS)) {
		std::cout << hit.block.actor << std::endl;
		//gargoyle hit
		moveDynamic(viewVector);
		//	// TODO: 2 Rätsel Logik
		//	//checkPrerequisites();
		//	//disableParticles();
		//	//removeKeyFromScene();
		//}
		//	// TODO: Tür Logik
		//}
	}
}

void GameLogic::moveDynamic(glm::vec3 viewVector)
{
	auto &transform = ourScene.GetObject("gargoyle")->GetTransform();
	if (glfwGetKey(ourWindow, keyBinds.forward) == GLFW_PRESS) {
		auto currPos = ourGargoyleBox->getGlobalPose();
		auto newPos = PxTransform(viewVector.x * 2.0 * deltaTime, 0.0, viewVector.z * 2.0 * deltaTime).transform(currPos);
		ourGargoyleBox->setGlobalPose(newPos);
		//gargyoleBox->setKinematicTarget(PxTransform(viewVector.x * 2.0 * deltaTime, 0.0, viewVector.z * 2.0 * deltaTime));
		//gargyoleBox->addForce(PxVec3(0.001, 0.001, 0.001), PxForceMode::eFORCE);
		transform.SetPostion(PxToGlmVec3(PxExtendedVec3(ourGargoyleBox->getGlobalPose().p[0], ourGargoyleBox->getGlobalPose().p[1], ourGargoyleBox->getGlobalPose().p[2])));
	}
	else if (glfwGetKey(ourWindow, keyBinds.backward) == GLFW_PRESS) {
		auto currPos = ourGargoyleBox->getGlobalPose();
		auto newPos = PxTransform(viewVector.x * -2.0 * deltaTime, 0.0, viewVector.z * -2.0 * deltaTime).transform(currPos);
		ourGargoyleBox->setGlobalPose(newPos);
		//auto debug = ourGargoyleBox->getGlobalPose();
		//gargyoleBox->setKinematicTarget(PxTransform(viewVector.x * -2.0 * deltaTime, 0.0, viewVector.z * -2.0 * deltaTime));
		//gargyoleBox->addForce(PxVec3(viewVector.x * 2.0 * deltaTime, 0.0, viewVector.z * 2.0 * deltaTime), PxForceMode::eFORCE);
		//transform.SetPostion(glm::vec3(ourGargoyleBox->getGlobalPose().p[0], ourGargoyleBox->getGlobalPose().p[1], ourGargoyleBox->getGlobalPose().p[2]));
		transform.SetPostion(PxToGlmVec3(PxExtendedVec3(ourGargoyleBox->getGlobalPose().p[0], ourGargoyleBox->getGlobalPose().p[1], ourGargoyleBox->getGlobalPose().p[2])));
	}
}

void GameLogic::movePortal()
{
	glm::vec3 charPos = PxToGlmVec3(character->getFootPosition());
	for (const Portal & portal : ourScene.renderPortals) {
		if (glm::distance2(portal.transform.GetPosition(), charPos) < 2) {
			glm::vec4 portalPlane = glm::vec4(portal.transform.GetForward(), glm::dot(-portal.transform.GetForward(), portal.transform.GetPosition()));

			glm::vec4 pos = glm::vec4(charPos, 1);

			if (glm::dot(portalPlane, pos) > 0) {
				glm::vec3 newPos = portal.getOffsetMatrix() * pos;
				glm::mat4 m = portal.getOffsetMatrix();
				ourCameraController.yaw += glm::degrees(glm::atan(-m[0].z, m[0].x));
				ourScene.activeCamera->GetTransform().SetRotationDegrees(ourCameraController.pitch, ourCameraController.yaw, 0);
				ourScene.activeCamera->GetTransform().SetPostion(newPos + glm::vec3(0, cP.characterEyeHeight, 0));
				character->setFootPosition(PxExtendedVec3(newPos.x, newPos.y, newPos.z));
			}
		}
	}
	
}

void GameLogic::initStaticColliders()
{
}

void GameLogic::updateDeltaTime(float newDelta)
{
	this->deltaTime = newDelta;
}

glm::vec3 GameLogic::PxToGlmVec3(const PxExtendedVec3 v)
{
	return glm::vec3(v.x, v.y, v.z);
}
