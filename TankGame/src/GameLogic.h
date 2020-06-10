#pragma once

#include <GL\glew.h>
#include <glm\glm.hpp>
#include "Scene.h"
#include <GLFW\glfw3.h>
#include "CameraController.h"
#include "KeyMap.h"
#include <iostream>
#include "Gargoyle.h"
#include "Interactable.h"

#include <PhysX/PxPhysicsAPI.h>

struct CharacterDef {
	float height;
	float radius;
	float eyeHeight;
	float moveSpeed;
};

enum ActiveGroup
{
	GROUP1 = (1 << 0),
	GROUP2 = (1 << 1),
	GROUP3 = (1 << 2),
	GROUP4 = (1 << 3),
};

static physx::PxQuat fromEuler(const glm::vec3 & e) {
	glm::quat q = glm::quat(e);
	return physx::PxQuat(q.x, q.y, q.z, q.w);
}

static glm::vec3 PxToGlmVec3(const physx::PxExtendedVec3 & v) {
	return glm::vec3(v.x, v.y, v.z);
}

class GameLogic
{
private:
	Scene &ourScene;
	physx::PxScene* ourPxScene;
	physx::PxController* character;
	GLFWwindow* ourWindow;
	physx::PxPhysics* physX;
	CameraController &ourCameraController;
	KeyMap keyBinds;
	float deltaTime = 0;

	CharacterDef cP = { 1.8f, 0.25f, 1.7f, 2.0f };
	physx::PxRigidDynamic* ourGargoyleBox; //TODO: Aus szene auslesen
	physx::PxRigidDynamic* ourKey;

public:
	GameLogic(Scene &scene, physx::PxScene *pxScene, GLFWwindow *window, physx::PxPhysics* physX, CameraController &cameraController, KeyMap keyMap);
	void Update(const float & newDelta); // parent function calling all our logic
	void moveControllerCamera(); // moves character controller + adjusts camera movement to follow
	void raycastFilter(); // perfoms a raycast on each filter group and calls appropriate actions
	void moveDynamic(glm::vec3 viewVector); // moves dynamic objects
	void handlePortals(); // moves the character from one portal to another adjusting the camera

	void SetupScene();

	Scene& getScene() { return ourScene; };
	physx::PxScene* getPxScene() { return ourPxScene; };
	GLFWwindow* getWindow() { return ourWindow; };
	KeyMap getKeyBinds() { return keyBinds;};
	float getDelta() { return deltaTime; };

private:
	void initStaticColliders(); // initializes static colliders
};

