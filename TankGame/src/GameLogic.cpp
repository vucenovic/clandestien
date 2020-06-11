#include "GameLogic.h"
#include "GameSceneAggregateBuilder.h"
#include "ResourceManager.h"

GameLogic::GameLogic(Scene &scene, physx::PxScene* pxScene, GLFWwindow* window, PxPhysics* physX, CameraController &cameraController, KeyMap keyMap)
	: scene(scene), ourPxScene(pxScene), ourWindow(window), physX(physX), ourCameraController(cameraController), keyBinds(keyMap)
{
}

void GameLogic::Update(const float & deltaTime)
{
	this->deltaTime = deltaTime;
	moveControllerCamera();
	handlePortals();
	raycastFilter();
}

void GameLogic::LateUpdate()
{
	gargoyleController->LateUpdate();
}

void GameLogic::moveControllerCamera()
{
	using namespace physx;
	ourCameraController.HandleInputs();

	glm::vec3 forwardVector = scene.activeCamera->GetTransform().GetForward();
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
	ourCameraController.cameraTransform->SetPostion(glm::vec3(newPos[0], newPos[1] + cP.eyeHeight, newPos[2])); 
	
}

void GameLogic::raycastFilter()
{
	using namespace physx;
	glm::vec3 camPos = scene.activeCamera->GetTransform().GetPosition();
	PxVec3 origin = PxVec3(camPos.x, camPos.y, camPos.z);
	glm::vec3 viewVector = scene.activeCamera->GetTransform().GetForward();

	PxVec3 unitDir = PxVec3(viewVector.x, viewVector.y, viewVector.z);
	PxReal maxDistance = 1.5;
	PxRaycastBuffer hit;

	PxQueryFilterData filterData = PxQueryFilterData(); 
	filterData.data.word0 = GROUP1; 
	const PxHitFlags outputFlags = PxHitFlag::eDEFAULT | PxHitFlag::ePOSITION | PxHitFlag::eNORMAL;

	//Nutz das "Userdata" attribute von PxActor um eine 1:1 beziehung mit dem Gameobject oder was auch immer zu erzeugen.
	bool status = ourPxScene->raycast(origin, unitDir, maxDistance, hit, outputFlags, filterData);
	if (status && (glfwGetKey(ourWindow, keyBinds.interaction) == GLFW_PRESS)) {
		//gargoyle hit
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
		ourKey = addColliderToDynamic("Key", PxTransform(PxVec3(2,1,0)), PxBoxGeometry(0.35f, 0.1f, 0.35f));
		setCollisionGroup(ourKey, 2, PxFilterData(GROUP1, 0, 0, 0));
		ourKey->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, true);
	}
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
	resourceManager.AddMesh(OBJLoader::LoadOBJ("res/models/Door_Static.obj"), "Door_Static");
	resourceManager.AddMesh(OBJLoader::LoadOBJ("res/models/gargoyle.obj"), "gargoyle");
	resourceManager.AddMesh(OBJLoader::LoadOBJ("res/models/Door.obj"), "Door");
	resourceManager.AddMesh(OBJLoader::LoadOBJ("res/models/Portal.obj"), "Portal");
	resourceManager.AddMesh(OBJLoader::LoadOBJ("res/models/PortalWallCaps.obj"), "PortalWallCaps");
	resourceManager.AddMesh(OBJLoader::LoadOBJ("res/models/Key.obj"), "Key");

	//Load Textures
	resourceManager.AddTexture(std::make_unique<Texture2D>("res/textures/dev_diffuse"), "dev_diff");
	resourceManager.AddTexture(std::make_unique<Texture2D>("res/textures/dev_normal"), "dev_norm");
	resourceManager.AddTexture(std::make_unique<Texture2D>("res/textures/particle"), "particle");

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
	scene.AddObject(std::make_unique<GameObject>(Transform(glm::vec3(0), glm::vec3(), glm::vec3(1.26f, 1.0f, 1.0f)), resourceManager.GetMesh("Floors"), resourceManager.GetMaterial("dev"), "Floor"));
	scene.AddObject(std::make_unique<GameObject>(Transform(glm::vec3(0), glm::vec3(), glm::vec3(1.0f, 1.0f, 1.0f)), resourceManager.GetMesh("Walls"), resourceManager.GetMaterial("dev"), "Walls"));
	scene.AddObject(std::make_unique<GameObject>(Transform(glm::vec3(0), glm::vec3(), glm::vec3(1.0f, 1.0f, 1.0f)), resourceManager.GetMesh("Ceiling"), resourceManager.GetMaterial("dev"), "Ceiling"));
	scene.AddObject(std::make_unique<GameObject>(Transform(glm::vec3(0), glm::vec3(), glm::vec3(1.0f, 1.0f, 1.0f)), resourceManager.GetMesh("Cliff"), resourceManager.GetMaterial("dev"), "Cliff"));
	scene.AddObject(std::make_unique<GameObject>(Transform(glm::vec3(0, -16.874f, 23.398f), glm::vec3(), glm::vec3(1.0f, 1.0f, 1.0f)), resourceManager.GetMesh("Ocean"), resourceManager.GetMaterial("dev"), "Ocean"));
	scene.AddObject(std::make_unique<GameObject>(Transform(glm::vec3(0, 2.5f, -2.15f), glm::vec3(), glm::vec3(1.0f, 1.0f, 1.0f)), resourceManager.GetMesh("Railing"), resourceManager.GetMaterial("dev"), "Railing"));
	scene.AddObject(std::make_unique<GameObject>(Transform(glm::vec3(1.135f, 2.5f, 2.0f), glm::vec3(), glm::vec3(1.0f, 1.0f, 1.0f)), resourceManager.GetMesh("Girder"), resourceManager.GetMaterial("dev"), "Girder"));
	scene.AddObject(std::make_unique<GameObject>(Transform(glm::vec3(3.441f, 2.5f, 2.0f), glm::vec3(), glm::vec3(1.0f, 1.0f, 1.0f)), resourceManager.GetMesh("Girder"), resourceManager.GetMaterial("dev"), "Girder.001"));
	scene.AddObject(std::make_unique<GameObject>(Transform(glm::vec3(-1.185f, 2.5f, 2.0f), glm::vec3(), glm::vec3(1.0f, 1.0f, 1.0f)), resourceManager.GetMesh("Girder"), resourceManager.GetMaterial("dev"), "Girder.002"));
	scene.AddObject(std::make_unique<GameObject>(Transform(glm::vec3(-3.457f, 2.5f, 2.0f), glm::vec3(), glm::vec3(1.0f, 1.0f, 1.0f)), resourceManager.GetMesh("Girder"), resourceManager.GetMaterial("dev"), "Girder.003"));
	scene.AddObject(std::make_unique<GameObject>(Transform(glm::vec3(0, 0.502f, 2.2f), glm::vec3(), glm::vec3(1.0f, 1.0f, 1.0f)), resourceManager.GetMesh("Window"), resourceManager.GetMaterial("dev"), "WindowBank"));
	scene.AddObject(std::make_unique<GameObject>(Transform(glm::vec3(-2.286f, 0.502f, 2.2f), glm::vec3(), glm::vec3(1.0f, 1.0f, 1.0f)), resourceManager.GetMesh("Window"), resourceManager.GetMaterial("dev"), "WindowBank.001"));
	scene.AddObject(std::make_unique<GameObject>(Transform(glm::vec3(2.286f, 0.502f, 2.2f), glm::vec3(), glm::vec3(1.0f, 1.0f, 1.0f)), resourceManager.GetMesh("Window"), resourceManager.GetMaterial("dev"), "WindowBank.002"));
	scene.AddObject(std::make_unique<GameObject>(Transform(glm::vec3(0, 5.5f, -4.039f), glm::vec3(), glm::vec3(1.0f, 1.0f, 1.0f)), resourceManager.GetMesh("Lamp"), resourceManager.GetMaterial("dev"), "Lamp"));
	scene.AddObject(std::make_unique<GameObject>(Transform(glm::vec3(-2.5f, 5.5f, -4.039f), glm::vec3(), glm::vec3(1.0f, 1.0f, 1.0f)), resourceManager.GetMesh("Lamp"), resourceManager.GetMaterial("dev"), "Lamp.001"));
	scene.AddObject(std::make_unique<GameObject>(Transform(glm::vec3(2.5f, 5.5f, -4.039f), glm::vec3(), glm::vec3(1.0f, 1.0f, 1.0f)), resourceManager.GetMesh("Lamp"), resourceManager.GetMaterial("dev"), "Lamp.002"));
	scene.AddObject(std::make_unique<GameObject>(Transform(glm::vec3(2.0f, 0, 0), glm::vec3(), glm::vec3(1.0f, 1.0f, 1.0f)), resourceManager.GetMesh("Table"), resourceManager.GetMaterial("dev"), "Cube"));
	scene.AddObject(std::make_unique<GameObject>(Transform(glm::vec3(-2.615f, 0, -2.0f), glm::vec3(), glm::vec3(1.0f, 1.0f, 1.0f)), resourceManager.GetMesh("DoorFrame"), resourceManager.GetMaterial("dev"), "DoorFrame"));
	scene.AddObject(std::make_unique<GameObject>(Transform(glm::vec3(-0.024f, 2.493f, -2.863f), glm::vec3(), glm::vec3(1.0f, 1.0f, 1.0f)), resourceManager.GetMesh("Table"), resourceManager.GetMaterial("dev"), "Table.006"));
	scene.AddObject(std::make_unique<GameObject>(Transform(glm::vec3(1.995f, 1.0f, -0.009f), glm::vec3(3.12662f, 1.5708f, 0), glm::vec3(1.0f, 1.0f, 1.0f)), resourceManager.GetMesh("Projector"), resourceManager.GetMaterial("dev"), "Projector"));
	scene.AddObject(std::make_unique<GameObject>(Transform(glm::vec3(2.198f, 1.0f, 0.257f), glm::vec3(2.70931f, 0, -1.5708f), glm::vec3(1.131f, 1.131f, 1.131f)), resourceManager.GetMesh("FilmReel"), resourceManager.GetMaterial("dev"), "FilmReel"));
	scene.AddObject(std::make_unique<GameObject>(Transform(glm::vec3(0, 0, 0), glm::vec3(), glm::vec3(1.42f, 1.0f, 1.5f)), resourceManager.GetMesh("Maze"), resourceManager.GetMaterial("dev"), "Maze"));

	//GameObjects
	{
		scene.AddObject(std::make_unique<GameObject>(Transform(glm::vec3(-4.0f, 2.5f, -4.0f), glm::vec3(0, 1.5708f, 0), glm::vec3(1.0f, 1.0f, 1.0f)), resourceManager.GetMesh("Door_Static"), resourceManager.GetMaterial("dev"), "Door.001"));
		scene.AddObject(std::make_unique<GameObject>(Transform(glm::vec3(4.0f, 2.5f, -4.0f), glm::vec3(0, -1.5708f, 0), glm::vec3(1.0f, 1.0f, 1.0f)), resourceManager.GetMesh("Door_Static"), resourceManager.GetMaterial("dev"), "Door.002"));
		scene.AddObject(std::make_unique<GameObject>(Transform(glm::vec3(4.0f, 0, 0), glm::vec3(0, -1.5708f, 0), glm::vec3(1.0f, 1.0f, 1.0f)), resourceManager.GetMesh("Door_Static"), resourceManager.GetMaterial("dev"), "Door.003"));
		scene.AddObject(std::make_unique<GameObject>(Transform(glm::vec3(0.213f, 0.611f, -1.411f), glm::vec3(), glm::vec3(1.0f, 1.0f, 1.0f)), resourceManager.GetMesh("gargoyle"), resourceManager.GetMaterial("gargoyle"), "gargoyle"));
		scene.AddObject(std::make_unique<GameObject>(Transform(glm::vec3(-2.115f, 0, -2.0f), glm::vec3(), glm::vec3(1.0f, 1.0f, 1.0f)), resourceManager.GetMesh("Door"), resourceManager.GetMaterial("dev"), "DoorOpenable"));
		/*Blocks portals from view, must be removed later by gamelogic to open portals*///scene.AddObject(std::make_unique<GameObject>(Transform(glm::vec3(0), glm::vec3(), glm::vec3(1.0f, 1.0f, 1.0f)), resourceManager.GetMesh("PortalWallCaps"), resourceManager.GetMaterial("dev"), "PortalWallCaps"));
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
}
