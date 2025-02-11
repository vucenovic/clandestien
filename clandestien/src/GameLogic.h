#pragma once

#include <GL\glew.h>
#include <glm\glm.hpp>
#include "Scene.h"
#include <GLFW\glfw3.h>
#include "CameraController.h"
#include "KeyMap.h"
#include <iostream>
class Gargoyle; //forward declaration
class Key; 
class Projector;
class Filmreel;
class Door;
#include "Projector.h"
#include "Door.h"
#include "Filmreel.h"
#include "Key.h"
#include "Gargoyle.h"
#include "Interactable.h"
#include "InputManager.h"
#include "KeyRiddle.h"

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


//Converts between Px and glm
template <typename T, typename O> static T PxConv(const O & o) = delete;

template<>
static physx::PxQuat PxConv(const glm::vec3 & e)
{
	glm::quat q = glm::quat(e);
	return physx::PxQuat(q.x, q.y, q.z, q.w);
}

template<>
static physx::PxVec3 PxConv(const glm::vec3 & v) {
	return physx::PxVec3(v.x, v.y, v.z);
}

template<>
static glm::vec3 PxConv(const physx::PxExtendedVec3 & v) {
	return glm::vec3(v.x, v.y, v.z);
}

template<>
static glm::vec3 PxConv(const physx::PxVec3 & v) {
	return glm::vec3(v.x, v.y, v.z);
}

template<>
static physx::PxQuat PxConv(const glm::quat & q) {
	return physx::PxQuat(q.x, q.y, q.z, q.w);
}

class GameLogic
{
private:
	Scene &scene;
	physx::PxScene* ourPxScene;
	physx::PxController* character;
	GLFWwindow* ourWindow;
	physx::PxPhysics* physX;
	CameraController &ourCameraController;
	KeyMap keyBinds;
	float deltaTime = 0;

	CharacterDef cP = { 1.8f, 0.25f, 1.7f, 2.0f };
	physx::PxRigidDynamic* gargoyleRigidbody; //TODO: Aus szene auslesen
	KeyRiddle keyRiddle = "QMGK";
	std::unique_ptr<Gargoyle> gargoyleController;
	physx::PxRigidDynamic* ourKey;
	std::unique_ptr<Key> ourKeyController;
	bool checkKey = true;
	bool checkGarg = true;
	physx::PxRigidDynamic* projector;
	physx::PxRigidDynamic* filmReel;
	physx::PxRigidDynamic* door;
	physx::PxRigidDynamic* portalCap;
	std::unique_ptr<Projector> projectorController;
	std::unique_ptr<Filmreel> filmreelController;
	std::unique_ptr<Door> doorController;

	int cameraState = 0; //0 default state, 1 for key interaction 
	Camera * alternativeCamera = nullptr;

public:
	GameLogic(Scene &scene, physx::PxScene *pxScene, GLFWwindow *window, physx::PxPhysics* physX, CameraController &cameraController, KeyMap keyMap);
	void Update(const float & newDelta); //Called before the physics simulation step each frame
	void LateUpdate(); //Called after the physics each frame
	void PhysicsUpdate(const float & physTimeStep); //Called after each physics simulation step (can be called multiple times or not called on a frame!)
	void moveControllerCamera(); // moves character controller + adjusts camera movement to follow
	void raycastFilter(); // perfoms a raycast on each filter group and calls appropriate actions
	void moveDynamic(glm::vec3 viewVector); // moves dynamic objects
	void handlePortals(); // moves the character from one portal to another adjusting the camera
	void switchCameraState(); // checks key interaction state and handles cameras switching
	void setupKeyCallbacks(); // sets up various key callbacks 
	void updateKeyRiddleLogic(); // controls key riddle logic 
	void updateGargoyleRiddleLogic(); // controls gargoyle riddle logic
	void checkWinState(); //checks if player won the game

	void SetupScene(); //do setup
	static void SetupResources(); //Load resources to the resource Manager

	Scene& getScene() { return scene; };
	physx::PxScene* getPxScene() { return ourPxScene; };
	GLFWwindow* getWindow() { return ourWindow; };
	KeyMap getKeyBinds() { return keyBinds;};
	float getDelta() { return deltaTime; };
	void setCameraState(int state);

private:
	static void setCollisionGroup(physx::PxRigidActor * actor, const physx::PxU16 grp, const physx::PxFilterData & grpenum);
	physx::PxRigidDynamic * addColliderToDynamic(const std::string & name, const physx::PxTransform & localTransform, const physx::PxBoxGeometry & scale);
	void initStaticColliders(); // initializes static colliders
	void initGameObjects();

	static void DefineMaterials();
};
