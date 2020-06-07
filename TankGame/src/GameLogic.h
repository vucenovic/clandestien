#pragma once

#include <GL\glew.h>
#include <glm\glm.hpp>
#include "Scene.h"
#include <GLFW\glfw3.h>
#include "CameraController.h"
#include "KeyMap.h"
#include <iostream>

#include <PhysX/PxPhysicsAPI.h>

using namespace physx;

struct ControllerParameters {
	float characterHeight;
	float characterRadius;
	float characterEyeHeight;
	float characterMoveSpeed;
};

enum ActiveGroup
{
	GROUP1 = (1 << 0),
	GROUP2 = (1 << 1),
	GROUP3 = (1 << 2),
	GROUP4 = (1 << 3),
};


class GameLogic
{
	private:
		Scene &ourScene;
		PxScene* ourPxScene;
		PxController* character;
		GLFWwindow* ourWindow;
		CameraController &ourCameraController;
		KeyMap keyBinds;
		float deltaTime;

		ControllerParameters cP = { 1.8f, 0.25f, 1.7f, 2.0f };
		PxRigidDynamic* ourGargoyleBox; //TODO: Aus szene auslesen

	public:
		GameLogic(Scene &scene, physx::PxScene *pxScene, PxController* c, GLFWwindow *window, CameraController &cameraController, KeyMap keyMap, float deltatime, PxRigidDynamic* gargoyleBox);
		void checkGameState(); // parent function calling all our logic
		void moveControllerCamera(); // moves character controller + adjusts camera movement to follow
		void raycastFilter(); // perfoms a raycast on each filter group and calls appropriate actions
		void moveDynamic(glm::vec3 viewVector); // moves dynamic objects
		void movePortal(); // moves the character from one portal to another adjusting the camera
		void initStaticColliders(); // initializes static colliders
		void updateDeltaTime(float newDelta);

		// helper functions

		glm::vec3 PxToGlmVec3(const PxExtendedVec3 v); // converts PxVec3 to glmVec3


};

