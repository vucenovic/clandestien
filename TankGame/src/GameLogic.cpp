#include "GameLogic.h"
#include "GameSceneAggregateBuilder.h"
#include "ResourceManager.h"

GameLogic::GameLogic(Scene &scene, physx::PxScene* pxScene, GLFWwindow* window, physx::PxPhysics* physX, CameraController &cameraController, KeyMap keyMap)
	: scene(scene), ourPxScene(pxScene), ourWindow(window), physX(physX), ourCameraController(cameraController), keyBinds(keyMap)
{
}

void GameLogic::Update(const float & deltaTime)
{
	this->deltaTime = deltaTime;
	if (cameraState == 0) {
		moveControllerCamera();
	}
	handlePortals();
	raycastFilter();
	setupKeyCallbacks();
	switchCameraState();
	if (checkKey) {
		updateKeyRiddleLogic();
	}
	if (checkGarg) {
		updateGargoyleRiddleLogic();
	}
	
	
}

void GameLogic::LateUpdate()
{
	
}

void GameLogic::PhysicsUpdate(const float & physTimeStep)
{
	gargoyleController->LateUpdate();
}

void GameLogic::moveControllerCamera()
{
	using namespace physx;
	ourCameraController.HandleInputs();

	glm::vec3 forwardVector = ourCameraController.cameraTransform.GetTransform().GetForward();
	forwardVector.y = 0; //clamp movement to horizonal plane
	forwardVector = glm::normalize(forwardVector);
	glm::vec3 rightVector = glm::cross(forwardVector, glm::vec3(0, 1, 0));

	glm::vec3 moveDir = forwardVector * (float)((glfwGetKey(ourWindow, keyBinds.forward) == GLFW_PRESS) - (glfwGetKey(ourWindow, keyBinds.backward) == GLFW_PRESS)) +
		rightVector * (float)((glfwGetKey(ourWindow, keyBinds.right) == GLFW_PRESS) - (glfwGetKey(ourWindow, keyBinds.left) == GLFW_PRESS));
	moveDir *= cP.moveSpeed * deltaTime; 
	moveDir.y = -0.01f;

	PxControllerFilters filters(NULL, NULL, NULL);
	PxControllerCollisionFlags collFlags = character->move(PxVec3(moveDir.x, moveDir.y, moveDir.z), 0.0, deltaTime, filters, NULL);

	PxControllerState state;
	character->getState(state);
	PxExtendedVec3 newPos = character->getFootPosition();
	ourCameraController.cameraTransform.GetTransform().SetPostion(glm::vec3(newPos[0], newPos[1] + cP.eyeHeight, newPos[2])); 
}

void GameLogic::raycastFilter()
{
	using namespace physx;
	glm::vec3 camPos = ourCameraController.cameraTransform.GetTransform().GetPosition();
	PxVec3 origin = PxVec3(camPos.x, camPos.y, camPos.z);
	glm::vec3 viewVector = ourCameraController.cameraTransform.GetTransform().GetForward();

	PxVec3 unitDir = PxVec3(viewVector.x, viewVector.y, viewVector.z);
	PxReal maxDistance = 2;
	PxRaycastBuffer hit;

	PxQueryFilterData filterData = PxQueryFilterData(); 
	filterData.data.word0 = GROUP1; 
	const PxHitFlags outputFlags = PxHitFlag::eDEFAULT | PxHitFlag::ePOSITION | PxHitFlag::eNORMAL;

	//Nutz das "Userdata" attribute von PxActor um eine 1:1 beziehung mit dem Gameobject oder was auch immer zu erzeugen.
	bool status = ourPxScene->raycast(origin, unitDir, maxDistance, hit, outputFlags, filterData);
	if (status && (glfwGetKey(ourWindow, keyBinds.interaction) == GLFW_PRESS)) {
		void* p = hit.block.actor->userData;
		if (p != nullptr) {
			((Interactable*) p)->interact((PxRigidBody*) hit.block.actor, (PxRigidBody*) character->getActor(), hit, *this);
		}
	}
}

void GameLogic::handlePortals()
{
	using namespace physx;
	glm::vec3 charPos = PxConv<glm::vec3>(character->getFootPosition());
	for (const Portal & portal : scene.renderPortals) {
		if (glm::distance2(portal.transform.GetPosition(), charPos) < 2) {
			glm::vec4 portalPlane = glm::vec4(portal.transform.GetForward(), glm::dot(-portal.transform.GetForward(), portal.transform.GetPosition()));

			glm::vec4 pos = glm::vec4(charPos, 1);

			if (glm::dot(portalPlane, pos) > 0) {
				glm::vec3 newPos = portal.getOffsetMatrix() * pos;
				glm::mat4 m = portal.getOffsetMatrix();
				ourCameraController.yaw += glm::degrees(glm::atan(-m[0].z, m[0].x));
				scene.activeCamera->GetTransform().SetRotationDegrees(ourCameraController.pitch, ourCameraController.yaw, 0);
				scene.activeCamera->GetTransform().SetPostion(newPos + glm::vec3(0, cP.eyeHeight, 0));
				character->setFootPosition(PxExtendedVec3(newPos.x, newPos.y, newPos.z));
			}
		}
	}
	
}

void GameLogic::switchCameraState()
{
	if (this->cameraState == 1) {
		if (glfwGetKey(ourWindow, GLFW_KEY_R) == GLFW_PRESS) {
			setCameraState(0);
		}
	}
}

void GameLogic::characterCallback(GLFWwindow * window, int key, int scancode, int action, int mods)
{
	if (action == GLFW_PRESS) {
		if (keyBinds.forward == key) {
			//.. TODO:
		}
	}
}

void GameLogic::setupKeyCallbacks()
{
	//inputManager.registerCallback(characterCallback);
	InputManager::instance().setglfwcallback(ourWindow);
}

void GameLogic::updateKeyRiddleLogic()
{
	if (keyRiddle.getSolved()) {
		//ourKey->detachShape(*ourKey); TODO: lets see if we need this
		scene.RemoveObject("Key");
		ourPxScene->removeActor(*ourKey);
		Inventory::instance().addKey();
		checkKey = false;
	}
}

void GameLogic::updateGargoyleRiddleLogic()
{
	auto debugD = glm::distance2(glm::vec3(gargoyleRigidbody->getGlobalPose().p[0], gargoyleRigidbody->getGlobalPose().p[1], gargoyleRigidbody->getGlobalPose().p[2]), glm::vec3(2.0, 0.0, 0.0));
	auto gargoylePosDebug = glm::vec3(gargoyleRigidbody->getGlobalPose().p[0], gargoyleRigidbody->getGlobalPose().p[1], gargoyleRigidbody->getGlobalPose().p[2]);
	if (scene.getLightManager().shadowLightUsed) {
		if (glm::distance2(glm::vec3(gargoyleRigidbody->getGlobalPose().p[0], gargoyleRigidbody->getGlobalPose().p[1], gargoyleRigidbody->getGlobalPose().p[2]), glm::vec3(-2.0, 0.5, 0.0)) < 2) {
			scene.RemoveObject("PortalWallCaps");
			ourPxScene->removeActor(*portalCap);
			checkGarg = false;
		}
	}
}

void GameLogic::setCameraState(int state)
{
	switch (state)
	{
	case 1:
		glm::vec3 camTarget = glm::vec3(scene.GetObject("Key")->GetTransform().GetPosition()); // set key as target
		alternativeCamera->SetProjectionMatrix(scene.activeCamera->getProjectionMatrix());
		{
			Transform & t = alternativeCamera->GetTransform();
			t.SetPostion(camTarget + glm::vec3(0, 1, 0));
			t.SetRotationDegrees(-90, 180, 0);
		}
		scene.activeCamera = alternativeCamera;
		InputManager::instance().registerCallback(1, [this](GLFWwindow * window, int key, int scancode, int action, int mods) { keyRiddle.keyInput(window, key, scancode, action, mods); });
		break;
	case 0:
		InputManager::instance().unregisterCallback(1);
		scene.activeCamera = &ourCameraController.cameraTransform;
		break;
	default:
		break;
	}
	cameraState = state;
}

void GameLogic::setCollisionGroup(physx::PxRigidActor * actor, const physx::PxU16 grp, const physx::PxFilterData & grpenum)
{
	using namespace physx;
	PxSetGroup(*actor, grp);
	PxU32 bufSize = actor->getNbShapes();
	PxShape ** shapes = new PxShape*[bufSize];
	actor->getShapes(shapes, bufSize);
	for (PxU32 i = 0; i < bufSize; i++) {
		shapes[0]->setQueryFilterData(grpenum);
	}
	delete[] shapes;
}

physx::PxRigidDynamic * GameLogic::addColliderToDynamic(const std::string & name, const physx::PxTransform & localTransform, const PxBoxGeometry & scale)
{
	GameObject & obj = *scene.GetObject(name);
	Transform & trans = obj.GetTransform();
	PxTransform objTransform = PxTransform(PxConv<PxVec3>(trans.GetPosition()),PxConv<PxQuat>(trans.GetRotation()));
	PxMaterial* boxMaterial = physX->createMaterial(0.5f, 0.5f, 0.6f);
	PxRigidDynamic* actor = physX->createRigidDynamic(objTransform);
	PxShape* shape = PxRigidActorExt::createExclusiveShape(*actor, scale, *boxMaterial);
	shape->setLocalPose(localTransform);
	ourPxScene->addActor(*actor);
	return actor;
}

void GameLogic::SetupScene()
{
	using namespace physx;
	initStaticColliders();
	initGameObjects();

	PxControllerManager* manager = PxCreateControllerManager(*ourPxScene);
	PxCapsuleControllerDesc desc;
	PxMaterial* controllerMaterial = physX->createMaterial(0.8f, 0.8f, 0.9f);
	desc.stepOffset = 0.1f;
	desc.contactOffset = 0.05;
	desc.material = controllerMaterial;
	desc.density = 10.0;
	desc.isValid();
	desc.scaleCoeff = 0.95;
	desc.volumeGrowth = 1.5f;
	desc.position = PxExtendedVec3(1.0, cP.height / 2, 0.0);
	desc.radius = cP.radius;
	desc.height = cP.height - cP.radius * 2; //height = distance between sphere centers on capsule
	desc.climbingMode = PxCapsuleClimbingMode::eLAST;
	character = manager->createController(desc);
	manager->setOverlapRecoveryModule(true);

	PxSetGroup(*character->getActor(), 1);

	//Gargoyle
	{
		gargoyleRigidbody = addColliderToDynamic("gargoyle", PxTransform(0.001f, 0.143f, 0.029f), PxBoxGeometry(0.75f, 0.75f, 0.6f));
		setCollisionGroup(gargoyleRigidbody, 2, PxFilterData(GROUP1, 0, 0, 0));
		gargoyleRigidbody->setRigidDynamicLockFlags(PxRigidDynamicLockFlag::eLOCK_ANGULAR_X | PxRigidDynamicLockFlag::eLOCK_ANGULAR_Y | PxRigidDynamicLockFlag::eLOCK_ANGULAR_Z);
		gargoyleController = std::make_unique<Gargoyle>(*scene.GetObject("gargoyle"),gargoyleRigidbody);
		gargoyleRigidbody->userData = gargoyleController.get();
	}

	//Key
	{
		ourKey = addColliderToDynamic("Key", PxTransform(PxVec3(0,0,0)), PxBoxGeometry(0.35f, 0.1f, 0.35f));
		ourKey->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, true);
		setCollisionGroup(ourKey, 2, PxFilterData(GROUP1, 0, 0, 0));
		ourKeyController = std::make_unique<Key>(*scene.GetObject("Key"), ourKey);
		ourKey->userData = ourKeyController.get();
	}
	//Projector
	{
		projector = addColliderToDynamic("Projector", PxTransform(PxVec3(-0.094f, 0.397f, 0), PxConv<PxQuat>(glm::vec3(-3.14159f, 1.5708f, 0))), PxBoxGeometry(0.135f, 0.424f, 0.417f));
		setCollisionGroup(projector, 2, PxFilterData(GROUP1, 0, 0, 0));
		projectorController = std::make_unique<Projector>(*scene.GetObject("Projector"), projector);
		projector->userData = projectorController.get();

	}
	//Filmreel
	{
		filmReel = addColliderToDynamic("FilmReel", PxTransform(PxVec3(0, 0.032f, 0)), PxBoxGeometry(0.143f, 0.032f, 0.13f));
		filmReel->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, true);
		setCollisionGroup(filmReel, 2, PxFilterData(GROUP1, 0, 0, 0));
		filmreelController = std::make_unique<Filmreel>(*scene.GetObject("FilmReel"), filmReel);
		filmReel->userData = filmreelController.get();
	}
	//PortalCap
	{
		portalCap = addColliderToDynamic("PortalWallCaps", PxTransform(PxVec3(-4.5f, 1.0f, 0)), PxBoxGeometry(0.5f, 1.0f, 0.5f));
		portalCap->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, true);
		setCollisionGroup(portalCap, 2, PxFilterData(GROUP1, 0, 0, 0));
	}
	//Door
	{
		door = addColliderToDynamic("DoorOpenable", PxTransform(PxVec3(-0.507f, 0.989f, -0.024f)), PxBoxGeometry(0.512f, 1.018f, 0.039f));
		door->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, true);
		setCollisionGroup(door, 2, PxFilterData(GROUP1, 0, 0, 0));
		doorController = std::make_unique<Door>(*scene.GetObject("DoorOpenable"), door);
		door->userData = doorController.get();
	}
	alternativeCamera = new Camera();
}

void GameLogic::SetupResources()
{
	ResourceManager & resourceManager = ResourceManager::GetInstance();

	//Load Meshes
	resourceManager.AddMesh(OBJLoader::LoadOBJ("res/models/Floors.obj"), "Floors");
	resourceManager.AddMesh(OBJLoader::LoadOBJ("res/models/Walls.obj"), "Walls");
	resourceManager.AddMesh(OBJLoader::LoadOBJ("res/models/Ceiling.obj"), "Ceiling");
	resourceManager.AddMesh(OBJLoader::LoadOBJ("res/models/Cliff.obj"), "Cliff");
	resourceManager.AddMesh(OBJLoader::LoadOBJ("res/models/Ocean.obj"), "Ocean");
	resourceManager.AddMesh(OBJLoader::LoadOBJ("res/models/Railing.obj"), "Railing");
	resourceManager.AddMesh(OBJLoader::LoadOBJ("res/models/Girder.obj"), "Girder");
	resourceManager.AddMesh(OBJLoader::LoadOBJ("res/models/Window.obj"), "Window");
	resourceManager.AddMesh(OBJLoader::LoadOBJ("res/models/Lamp.obj"), "Lamp");
	resourceManager.AddMesh(OBJLoader::LoadOBJ("res/models/Table.obj"), "Table");
	resourceManager.AddMesh(OBJLoader::LoadOBJ("res/models/DoorFrame.obj"), "DoorFrame");
	resourceManager.AddMesh(OBJLoader::LoadOBJ("res/models/Projector.obj"), "Projector");
	resourceManager.AddMesh(OBJLoader::LoadOBJ("res/models/FilmReel.obj"), "FilmReel");
	resourceManager.AddMesh(OBJLoader::LoadOBJ("res/models/Maze.obj"), "Maze");
	resourceManager.AddMesh(OBJLoader::LoadOBJ("res/models/Poster1.obj"), "Poster1");
	resourceManager.AddMesh(OBJLoader::LoadOBJ("res/models/Poster2.obj"), "Poster2");
	resourceManager.AddMesh(OBJLoader::LoadOBJ("res/models/DoorStatic.obj"), "DoorStatic");
	resourceManager.AddMesh(OBJLoader::LoadOBJ("res/models/Gargoyle.obj"), "Gargoyle");
	resourceManager.AddMesh(OBJLoader::LoadOBJ("res/models/Door.obj"), "Door");
	resourceManager.AddMesh(OBJLoader::LoadOBJ("res/models/PortalWallCaps.obj"), "PortalWallCaps");
	resourceManager.AddMesh(OBJLoader::LoadOBJ("res/models/Key.obj"), "Key");
	resourceManager.AddMesh(OBJLoader::LoadOBJ("res/models/Portal.obj"), "Portal");

	//Load Textures
	resourceManager.AddTexture(std::make_unique<Texture2D>("res/textures/dev_diffuse"), "dev_diff");
	resourceManager.AddTexture(std::make_unique<Texture2D>("res/textures/dev_normal"), "dev_norm");
	resourceManager.AddTexture(std::make_unique<Texture2D>("res/textures/particle"), "particle");

	resourceManager.AddTexture(std::make_unique<Texture2D>("res/textures/BricksDiff"), "BricksDiff");
	resourceManager.AddTexture(std::make_unique<Texture2D>("res/textures/BricksNorm"), "BricksNorm");
	resourceManager.AddTexture(std::make_unique<Texture2D>("res/textures/CeilingDiff"), "CeilingDiff");
	resourceManager.AddTexture(std::make_unique<Texture2D>("res/textures/CeilingNorm"), "CeilingNorm");
	resourceManager.AddTexture(std::make_unique<Texture2D>("res/textures/CeilingSpec"), "CeilingSpec");
	resourceManager.AddTexture(std::make_unique<Texture2D>("res/textures/DoorDiff"), "DoorDiff");
	resourceManager.AddTexture(std::make_unique<Texture2D>("res/textures/DoorNorm"), "DoorNorm");
	resourceManager.AddTexture(std::make_unique<Texture2D>("res/textures/FloorDiff"), "FloorDiff");
	resourceManager.AddTexture(std::make_unique<Texture2D>("res/textures/FloorNorm"), "FloorNorm");
	resourceManager.AddTexture(std::make_unique<Texture2D>("res/textures/FloorSpec"), "FloorSpec");
	resourceManager.AddTexture(std::make_unique<Texture2D>("res/textures/glyphs_alphabet"), "glyphs_alphabet");
	resourceManager.AddTexture(std::make_unique<Texture2D>("res/textures/glyphs_solving"), "glyphs_solving");
	resourceManager.AddTexture(std::make_unique<Texture2D>("res/textures/Railing"), "Railing");

	DefineMaterials();
}

void GameLogic::initStaticColliders()
{
	using namespace physx;
	//StaticColliders
	PxU32 staticColliderCount = 64;
	GameSceneAggregateBuilder agg = GameSceneAggregateBuilder(staticColliderCount, false, physX);
	{
		agg.addStaticBox(PxTransform(PxVec3(0, 1.25f, 2.5f)), PxBoxGeometry(4.0f, 1.25f, 0.5f));
		agg.addStaticBox(PxTransform(PxVec3(4.5f, 2.75f, 0)), PxBoxGeometry(0.5f, 2.75f, 2.4f));
		agg.addStaticBox(PxTransform(PxVec3(-4.5f, 4.0f, -2.0f)), PxBoxGeometry(0.5f, 1.5f, 4.4f));
		agg.addStaticBox(PxTransform(PxVec3(-4.5f, 1.25f, -1.5f)), PxBoxGeometry(0.5f, 1.25f, 1.0f));
		agg.addStaticBox(PxTransform(PxVec3(-4.5f, 1.25f, 1.75f)), PxBoxGeometry(0.5f, 1.25f, 1.25f));
		agg.addStaticBox(PxTransform(PxVec3(0, -0.5f, -7.501f)), PxBoxGeometry(10.272f, 0.5f, 9.946f));
		agg.addStaticBox(PxTransform(PxVec3(-4.5f, 2.25f, 0)), PxBoxGeometry(0.5f, 0.25f, 0.5f));
		agg.addStaticBox(PxTransform(PxVec3(-2.25f, 4.0f, -6.5f)), PxBoxGeometry(1.75f, 1.5f, 0.5f));
		agg.addStaticBox(PxTransform(PxVec3(2.25f, 4.0f, -6.5f)), PxBoxGeometry(1.75f, 1.5f, 0.5f));
		agg.addStaticBox(PxTransform(PxVec3(0, 5.0f, -6.5f)), PxBoxGeometry(0.5f, 0.5f, 0.5f));
		agg.addStaticBox(PxTransform(PxVec3(0, 4.3f, 0.8f), PxConv<PxQuat>(glm::vec3(-0.7854f, 0, 0))), PxBoxGeometry(4.0f, 3.0f, 0.45f));
		agg.addStaticBox(PxTransform(PxVec3(0, 6.0f, -3.5f)), PxBoxGeometry(4.0f, 0.5f, 2.5f));
		agg.addStaticBox(PxTransform(PxVec3(0, 3.061f, -2.15f)), PxBoxGeometry(4.227f, 0.549f, 0.13f));
		agg.addStaticBox(PxTransform(PxVec3(2.0f, 0.89f, 0)), PxBoxGeometry(0.49f, 0.118f, 0.49f));
		agg.addStaticBox(PxTransform(PxVec3(2.0f, 0.441f, 0)), PxBoxGeometry(0.222f, 0.345f, 0.222f));
		agg.addStaticBox(PxTransform(PxVec3(2.0f, 0.021f, 0)), PxBoxGeometry(0.416f, 0.061f, 0.416f));
		agg.addStaticBox(PxTransform(PxVec3(0, 2.375f, -4.5f)), PxBoxGeometry(4.0f, 0.125f, 2.5f));
		agg.addStaticBox(PxTransform(PxVec3(4.5f, 4.0f, -4.2f)), PxBoxGeometry(0.5f, 1.7f, 2.2f));
		agg.addStaticBox(PxTransform(PxVec3(1.085f, 1.25f, -2.25f)), PxBoxGeometry(3.181f, 1.25f, 0.25f));
		agg.addStaticBox(PxTransform(PxVec3(-3.569f, 1.25f, -2.25f)), PxBoxGeometry(0.436f, 1.25f, 0.25f));
		agg.addStaticBox(PxTransform(PxVec3(-0.024f, 3.516f, -2.863f)), PxBoxGeometry(0.49f, 0.118f, 0.49f));
		agg.addStaticBox(PxTransform(PxVec3(-0.024f, 3.066f, -2.863f)), PxBoxGeometry(0.222f, 0.345f, 0.222f));
		agg.addStaticBox(PxTransform(PxVec3(-0.024f, 2.646f, -2.863f)), PxBoxGeometry(0.416f, 0.061f, 0.416f));
		agg.addStaticBox(PxTransform(PxVec3(-4.587f, 1.188f, -8.48f)), PxBoxGeometry(3.0f, 1.2f, 0.1f));
		agg.addStaticBox(PxTransform(PxVec3(-2.587f, 1.188f, -10.48f)), PxBoxGeometry(1.0f, 1.2f, 0.1f));
		agg.addStaticBox(PxTransform(PxVec3(-6.587f, 1.188f, -10.48f)), PxBoxGeometry(1.0f, 1.2f, 0.1f));
		agg.addStaticBox(PxTransform(PxVec3(-5.587f, 1.188f, -11.48f)), PxBoxGeometry(0.1f, 1.2f, 1.0f));
		agg.addStaticBox(PxTransform(PxVec3(-1.587f, 1.188f, -10.48f)), PxBoxGeometry(0.1f, 1.2f, 2.0f));
		agg.addStaticBox(PxTransform(PxVec3(0.413f, 1.188f, -8.48f)), PxBoxGeometry(0.1f, 1.2f, 2.0f));
		agg.addStaticBox(PxTransform(PxVec3(4.413f, 1.188f, -9.48f)), PxBoxGeometry(0.1f, 1.2f, 3.0f));
		agg.addStaticBox(PxTransform(PxVec3(8.413f, 1.188f, -8.48f)), PxBoxGeometry(0.1f, 1.2f, 4.0f));
		agg.addStaticBox(PxTransform(PxVec3(-9.587f, 1.188f, -8.48f)), PxBoxGeometry(0.1f, 1.2f, 4.0f));
		agg.addStaticBox(PxTransform(PxVec3(1.413f, 1.188f, -10.48f)), PxBoxGeometry(1.0f, 1.2f, 0.1f));
		agg.addStaticBox(PxTransform(PxVec3(5.413f, 1.188f, -8.48f)), PxBoxGeometry(1.0f, 1.2f, 0.1f));
		agg.addStaticBox(PxTransform(PxVec3(3.413f, 1.188f, -6.48f)), PxBoxGeometry(1.0f, 1.2f, 0.1f));
		agg.addStaticBox(PxTransform(PxVec3(-6.587f, 1.188f, -6.48f)), PxBoxGeometry(1.0f, 1.2f, 0.1f));
		agg.addStaticBox(PxTransform(PxVec3(-5.587f, 1.188f, -5.48f)), PxBoxGeometry(0.1f, 1.2f, 1.0f));
		agg.addStaticBox(PxTransform(PxVec3(2.413f, 1.188f, -7.48f)), PxBoxGeometry(0.1f, 1.2f, 1.0f));
		agg.addStaticBox(PxTransform(PxVec3(6.413f, 1.188f, -5.48f)), PxBoxGeometry(0.1f, 1.2f, 1.0f));
		agg.addStaticBox(PxTransform(PxVec3(-3.587f, 1.188f, -7.48f)), PxBoxGeometry(0.1f, 1.2f, 1.0f));
		agg.addStaticBox(PxTransform(PxVec3(-5.587f, 1.188f, -12.48f)), PxBoxGeometry(4.0f, 1.2f, 0.1f));
		agg.addStaticBox(PxTransform(PxVec3(4.413f, 1.188f, -12.48f)), PxBoxGeometry(4.0f, 1.2f, 0.1f));
		agg.addStaticBox(PxTransform(PxVec3(-3.587f, 1.188f, -3.48f)), PxBoxGeometry(0.1f, 1.2f, 1.0f));
		agg.addStaticBox(PxTransform(PxVec3(-1.587f, 1.188f, -3.48f)), PxBoxGeometry(0.1f, 1.2f, 1.0f));
		agg.addStaticBox(PxTransform(PxVec3(-6.587f, 1.188f, -4.48f)), PxBoxGeometry(3.0f, 1.2f, 0.1f));
		agg.addStaticBox(PxTransform(PxVec3(-1.587f, 1.188f, -6.48f)), PxBoxGeometry(2.0f, 1.2f, 0.1f));
		agg.addStaticBox(PxTransform(PxVec3(3.413f, 1.188f, -4.48f)), PxBoxGeometry(5.0f, 1.2f, 0.1f));
	}
	ourPxScene->addAggregate(*agg.gameSceneAggregate);
}

void GameLogic::initGameObjects()
{
	ResourceManager & resourceManager = ResourceManager::GetInstance();

	//StaticMeshes
	scene.AddObject(std::make_unique<GameObject>(Transform(glm::vec3(0), glm::vec3(), glm::vec3(1.26f, 1.0f, 1.0f)), resourceManager.GetMesh("Floors"), resourceManager.GetMaterial("Floor"), "Floor"));
	scene.AddObject(std::make_unique<GameObject>(Transform(glm::vec3(0), glm::vec3(), glm::vec3(1.0f, 1.0f, 1.0f)), resourceManager.GetMesh("Walls"), resourceManager.GetMaterial("Wall"), "Walls"));
	scene.AddObject(std::make_unique<GameObject>(Transform(glm::vec3(0), glm::vec3(), glm::vec3(1.0f, 1.0f, 1.0f)), resourceManager.GetMesh("Ceiling"), resourceManager.GetMaterial("Ceiling"), "Ceiling"));
	scene.AddObject(std::make_unique<GameObject>(Transform(glm::vec3(0), glm::vec3(), glm::vec3(1.0f, 1.0f, 1.0f)), resourceManager.GetMesh("Cliff"), resourceManager.GetMaterial("dev"), "Cliff"));
	scene.AddObject(std::make_unique<GameObject>(Transform(glm::vec3(0, -16.874f, 23.398f), glm::vec3(), glm::vec3(1.0f, 1.0f, 1.0f)), resourceManager.GetMesh("Ocean"), resourceManager.GetMaterial("dev"), "Ocean"));
	scene.AddObject(std::make_unique<GameObject>(Transform(glm::vec3(0, 2.5f, -2.15f), glm::vec3(), glm::vec3(1.0f, 1.0f, 1.0f)), resourceManager.GetMesh("Railing"), resourceManager.GetMaterial("Railing"), "Railing"));
	scene.AddObject(std::make_unique<GameObject>(Transform(glm::vec3(1.135f, 2.5f, 2.0f), glm::vec3(), glm::vec3(1.0f, 1.0f, 1.0f)), resourceManager.GetMesh("Girder"), resourceManager.GetMaterial("Ceiling"), "Girder"));
	scene.AddObject(std::make_unique<GameObject>(Transform(glm::vec3(3.441f, 2.5f, 2.0f), glm::vec3(), glm::vec3(1.0f, 1.0f, 1.0f)), resourceManager.GetMesh("Girder"), resourceManager.GetMaterial("Ceiling"), "Girder.001"));
	scene.AddObject(std::make_unique<GameObject>(Transform(glm::vec3(-1.185f, 2.5f, 2.0f), glm::vec3(), glm::vec3(1.0f, 1.0f, 1.0f)), resourceManager.GetMesh("Girder"), resourceManager.GetMaterial("Ceiling"), "Girder.002"));
	scene.AddObject(std::make_unique<GameObject>(Transform(glm::vec3(-3.457f, 2.5f, 2.0f), glm::vec3(), glm::vec3(1.0f, 1.0f, 1.0f)), resourceManager.GetMesh("Girder"), resourceManager.GetMaterial("Ceiling"), "Girder.003"));
	scene.AddObject(std::make_unique<GameObject>(Transform(glm::vec3(0, 0.502f, 2.2f), glm::vec3(), glm::vec3(1.0f, 1.0f, 1.0f)), resourceManager.GetMesh("Window"), resourceManager.GetMaterial("Railing"), "WindowBank"));
	scene.AddObject(std::make_unique<GameObject>(Transform(glm::vec3(-2.286f, 0.502f, 2.2f), glm::vec3(), glm::vec3(1.0f, 1.0f, 1.0f)), resourceManager.GetMesh("Window"), resourceManager.GetMaterial("Railing"), "WindowBank.001"));
	scene.AddObject(std::make_unique<GameObject>(Transform(glm::vec3(2.286f, 0.502f, 2.2f), glm::vec3(), glm::vec3(1.0f, 1.0f, 1.0f)), resourceManager.GetMesh("Window"), resourceManager.GetMaterial("Railing"), "WindowBank.002"));
	scene.AddObject(std::make_unique<GameObject>(Transform(glm::vec3(0, 5.5f, -4.039f), glm::vec3(), glm::vec3(1.0f, 1.0f, 1.0f)), resourceManager.GetMesh("Lamp"), resourceManager.GetMaterial("dev"), "Lamp"));
	scene.AddObject(std::make_unique<GameObject>(Transform(glm::vec3(-2.5f, 5.5f, -4.039f), glm::vec3(), glm::vec3(1.0f, 1.0f, 1.0f)), resourceManager.GetMesh("Lamp"), resourceManager.GetMaterial("dev"), "Lamp.001"));
	scene.AddObject(std::make_unique<GameObject>(Transform(glm::vec3(2.5f, 5.5f, -4.039f), glm::vec3(), glm::vec3(1.0f, 1.0f, 1.0f)), resourceManager.GetMesh("Lamp"), resourceManager.GetMaterial("dev"), "Lamp.002"));
	scene.AddObject(std::make_unique<GameObject>(Transform(glm::vec3(2.0f, 0, 0), glm::vec3(), glm::vec3(1.0f, 1.0f, 1.0f)), resourceManager.GetMesh("Table"), resourceManager.GetMaterial("dev"), "Table1"));
	scene.AddObject(std::make_unique<GameObject>(Transform(glm::vec3(-2.615f, 0, -2.0f), glm::vec3(), glm::vec3(1.0f, 1.0f, 1.0f)), resourceManager.GetMesh("DoorFrame"), resourceManager.GetMaterial("Door"), "DoorFrame"));
	scene.AddObject(std::make_unique<GameObject>(Transform(glm::vec3(-0.024f, 2.493f, -2.863f), glm::vec3(), glm::vec3(1.0f, 1.0f, 1.0f)), resourceManager.GetMesh("Table"), resourceManager.GetMaterial("dev"), "Table2"));
	scene.AddObject(std::make_unique<GameObject>(Transform(glm::vec3(1.995f, 1.0f, -0.009f), glm::vec3(), glm::vec3(1.0f, 1.0f, 1.0f)), resourceManager.GetMesh("Projector"), resourceManager.GetMaterial("dev"), "Projector"));
	scene.AddObject(std::make_unique<GameObject>(Transform(glm::vec3(3.371f, 0, 1.245f), glm::vec3(0, 0.73946f, 0), glm::vec3(1.131f, 1.131f, 1.131f)), resourceManager.GetMesh("FilmReel"), resourceManager.GetMaterial("dev"), "FilmReel"));
	scene.AddObject(std::make_unique<GameObject>(Transform(glm::vec3(0, 0, 0), glm::vec3(), glm::vec3(1.42f, 1.0f, 1.5f)), resourceManager.GetMesh("Maze"), resourceManager.GetMaterial("Wall"), "Maze"));
	scene.AddObject(std::make_unique<GameObject>(Transform(glm::vec3(2.734f, 1.389f, -2.0f), glm::vec3(1.5708f, 0, 0.15863f), glm::vec3(1.0f, 1.0f, 1.0f)), resourceManager.GetMesh("Poster1"), resourceManager.GetMaterial("Poster1"), "Poster1"));
	scene.AddObject(std::make_unique<GameObject>(Transform(glm::vec3(0.211f, 3.493f, -3.132f), glm::vec3(0, 2.83547f, 0), glm::vec3(1.0f, 1.0f, 1.0f)), resourceManager.GetMesh("Poster2"), resourceManager.GetMaterial("Poster2"), "Poster1.001"));

	//GameObjects
	{
		scene.AddObject(std::make_unique<GameObject>(Transform(glm::vec3(-4.0f, 2.5f, -4.0f), glm::vec3(0, 1.5708f, 0), glm::vec3(1.0f, 1.0f, 1.0f)), resourceManager.GetMesh("DoorStatic"), resourceManager.GetMaterial("Door"), "Door.001"));
		scene.AddObject(std::make_unique<GameObject>(Transform(glm::vec3(4.0f, 2.5f, -4.0f), glm::vec3(0, -1.5708f, 0), glm::vec3(1.0f, 1.0f, 1.0f)), resourceManager.GetMesh("DoorStatic"), resourceManager.GetMaterial("Door"), "Door.002"));
		scene.AddObject(std::make_unique<GameObject>(Transform(glm::vec3(4.0f, 0, 0), glm::vec3(0, -1.5708f, 0), glm::vec3(1.0f, 1.0f, 1.0f)), resourceManager.GetMesh("DoorStatic"), resourceManager.GetMaterial("Door"), "Door.003"));
		scene.AddObject(std::make_unique<GameObject>(Transform(glm::vec3(0.213f, 0.611f, -1.411f), glm::vec3(), glm::vec3(1.0f, 1.0f, 1.0f)), resourceManager.GetMesh("Gargoyle"), resourceManager.GetMaterial("gargoyle"), "gargoyle"));
		scene.AddObject(std::make_unique<GameObject>(Transform(glm::vec3(-2.115f, 0, -2.0f), glm::vec3(), glm::vec3(1.0f, 1.0f, 1.0f)), resourceManager.GetMesh("Door"), resourceManager.GetMaterial("Door"), "DoorOpenable"));
		scene.AddObject(std::make_unique<GameObject>(Transform(glm::vec3(0), glm::vec3(), glm::vec3(1.0f, 1.0f, 1.0f)), resourceManager.GetMesh("PortalWallCaps"), resourceManager.GetMaterial("Wall"), "PortalWallCaps"));
		scene.AddObject(std::make_unique<GameObject>(Transform(glm::vec3(0, 3.499f, -2.902f), glm::vec3(0, 0.7854f, 0), glm::vec3(1.0f, 1.0f, 1.0f)), resourceManager.GetMesh("Key"), resourceManager.GetMaterial("dev"), "Key"));
	}
}

void GameLogic::DefineMaterials()
{
	ResourceManager & resourceManager = ResourceManager::GetInstance();
	ShaderProgram * standardShader = resourceManager.GetShader("standard");
	
	//TODO define all the materials
	{
		std::unique_ptr<Material> material = std::make_unique<StandardMaterial>(standardShader);
		StandardMaterial * mat = (StandardMaterial*)material.get();
		mat->diffuse = (Texture2D*)resourceManager.GetTexture("dev_diff");
		mat->normal = (Texture2D*)resourceManager.GetTexture("dev_norm");
		resourceManager.AddMaterial(material, "gargoyle");
	}

	{
		std::unique_ptr<Material> material = std::make_unique<StandardMaterial>(standardShader);
		StandardMaterial * mat = (StandardMaterial*)material.get();
		mat->material = glm::vec4(0.05f, 0.5f, 1, 8);
		mat->color = glm::vec4(0.7f, 0.7f, 0.7f, 2);
		mat->diffuse = (Texture2D*)resourceManager.GetTexture("dev_diff");
		mat->normal = (Texture2D*)resourceManager.GetTexture("dev_norm");
		resourceManager.AddMaterial(material, "dev");
	}

	{//Wall
		std::unique_ptr<Material> material = std::make_unique<StandardMaterial>(standardShader);
		StandardMaterial * mat = (StandardMaterial*)material.get();
		mat->material = glm::vec4(0.05f, 0.5f, 0.2f, 2);
		mat->diffuse = (Texture2D*)resourceManager.GetTexture("BricksDiff");
		mat->normal = (Texture2D*)resourceManager.GetTexture("BricksNorm");
		resourceManager.AddMaterial(material, "Wall");
	}

	{//Ceiling
		std::unique_ptr<Material> material = std::make_unique<StandardMaterial>(standardShader);
		StandardMaterial * mat = (StandardMaterial*)material.get();
		mat->material = glm::vec4(0.05f, 0.1f, 0.1f, 8);
		mat->diffuse = (Texture2D*)resourceManager.GetTexture("CeilingDiff");
		mat->specular = (Texture2D*)resourceManager.GetTexture("CeilingSpec");
		mat->normal = (Texture2D*)resourceManager.GetTexture("CeilingNorm");
		resourceManager.AddMaterial(material, "Ceiling");
	}

	{//Floor
		std::unique_ptr<Material> material = std::make_unique<StandardMaterial>(standardShader);
		StandardMaterial * mat = (StandardMaterial*)material.get();
		mat->material = glm::vec4(0.05f, 0.5f, 0.5f, 8);
		mat->diffuse = (Texture2D*)resourceManager.GetTexture("FloorDiff");
		mat->specular = (Texture2D*)resourceManager.GetTexture("FloorSpec");
		mat->normal = (Texture2D*)resourceManager.GetTexture("FloorNorm");
		resourceManager.AddMaterial(material, "Floor");
	}

	{//Railing
		std::unique_ptr<Material> material = std::make_unique<StandardMaterial>(standardShader);
		StandardMaterial * mat = (StandardMaterial*)material.get();
		mat->material = glm::vec4(0.05f, 0.5f, 0.2f, 8);
		mat->diffuse = (Texture2D*)resourceManager.GetTexture("Railing");
		resourceManager.AddMaterial(material, "Railing");
	}

	{//Door
		std::unique_ptr<Material> material = std::make_unique<StandardMaterial>(standardShader);
		StandardMaterial * mat = (StandardMaterial*)material.get();
		mat->material = glm::vec4(0.05f, 0.5f, 0.5f, 8);
		mat->diffuse = (Texture2D*)resourceManager.GetTexture("DoorDiff");
		mat->normal = (Texture2D*)resourceManager.GetTexture("DoorNorm");
		resourceManager.AddMaterial(material, "Door");
	}

	{//Poster1
		std::unique_ptr<Material> material = std::make_unique<StandardMaterial>(standardShader);
		StandardMaterial * mat = (StandardMaterial*)material.get();
		mat->material = glm::vec4(0.05f, 1, 0, 8);
		mat->diffuse = (Texture2D*)resourceManager.GetTexture("glyphs_alphabet");
		resourceManager.AddMaterial(material, "Poster1");
	}

	{//Poster2
		std::unique_ptr<Material> material = std::make_unique<StandardMaterial>(standardShader);
		StandardMaterial * mat = (StandardMaterial*)material.get();
		mat->material = glm::vec4(0.05f, 1, 0, 8);
		mat->diffuse = (Texture2D*)resourceManager.GetTexture("glyphs_solving");
		resourceManager.AddMaterial(material, "Poster2");
	}
}
