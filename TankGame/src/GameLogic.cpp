#include "GameLogic.h"
#include "GameSceneAggregateBuilder.h"

GameLogic::GameLogic(Scene &scene, physx::PxScene* pxScene, GLFWwindow* window, PxPhysics* physX, CameraController &cameraController, KeyMap keyMap)
	: ourScene(scene), ourPxScene(pxScene), ourWindow(window), physX(physX), ourCameraController(cameraController), keyBinds(keyMap)
{
}

void GameLogic::Update(const float & deltaTime)
{
	this->deltaTime = deltaTime;
	moveControllerCamera();
	handlePortals();
	raycastFilter();
}

void GameLogic::moveControllerCamera()
{
	using namespace physx;
	ourCameraController.HandleInputs();

	glm::vec3 forwardVector = ourScene.activeCamera->GetTransform().GetForward();
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
	glm::vec3 camPos = ourScene.activeCamera->GetTransform().GetPosition();
	PxVec3 origin = PxVec3(camPos.x, camPos.y, camPos.z);
	glm::vec3 viewVector = ourScene.activeCamera->GetTransform().GetForward();

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
				ourScene.activeCamera->GetTransform().SetPostion(newPos + glm::vec3(0, cP.eyeHeight, 0));
				character->setFootPosition(PxExtendedVec3(newPos.x, newPos.y, newPos.z));
			}
		}
	}
	
}

void GameLogic::initStaticColliders()
{
	using namespace physx;
	//StaticColliders
	const PxU32 staticColliderCount = 32;
	GameSceneAggregateBuilder agg = GameSceneAggregateBuilder(staticColliderCount, false, physX);
	{
		agg.addStaticBox(PxTransform(0, 1.25f, 2.5f), PxBoxGeometry(4.0f, 1.25f, 0.5f));
		agg.addStaticBox(PxTransform(4.5f, 2.75f, 0), PxBoxGeometry(0.5f, 2.75f, 2.4f));
		agg.addStaticBox(PxTransform(-4.5f, 4.0f, -2.0f), PxBoxGeometry(0.5f, 1.5f, 4.4f));
		agg.addStaticBox(PxTransform(-4.5f, 1.25f, -1.5f), PxBoxGeometry(0.5f, 1.25f, 1.0f));
		agg.addStaticBox(PxTransform(-4.5f, 1.25f, 1.75f), PxBoxGeometry(0.5f, 1.25f, 1.25f));
		agg.addStaticBox(PxTransform(0, -0.5f, -5.5f), PxBoxGeometry(8.0f, 0.5f, 8.0f));
		agg.addStaticBox(PxTransform(-4.5f, 2.25f, 0), PxBoxGeometry(0.5f, 0.25f, 0.5f));
		agg.addStaticBox(PxTransform(-2.25f, 4.0f, -6.5f), PxBoxGeometry(1.75f, 1.5f, 0.5f));
		agg.addStaticBox(PxTransform(2.25f, 4.0f, -6.5f), PxBoxGeometry(1.75f, 1.5f, 0.5f));
		agg.addStaticBox(PxTransform(0, 5.0f, -6.5f), PxBoxGeometry(0.5f, 0.5f, 0.5f));
		agg.addStaticBox(PxTransform(PxVec3(0, 4.3f, 0.8f), fromEuler(glm::vec3(-0.7854f, 0, 0))), PxBoxGeometry(4.0f, 3.0f, 0.45f));
		agg.addStaticBox(PxTransform(0, 6.0f, -3.5f), PxBoxGeometry(4.0f, 0.5f, 2.5f));
		agg.addStaticBox(PxTransform(0, 3.061f, -2.15f), PxBoxGeometry(4.227f, 0.549f, 0.13f));
		agg.addStaticBox(PxTransform(2.0f, 0.89f, 0), PxBoxGeometry(0.49f, 0.118f, 0.49f));
		agg.addStaticBox(PxTransform(2.0f, 0.441f, 0), PxBoxGeometry(0.222f, 0.345f, 0.222f));
		agg.addStaticBox(PxTransform(2.0f, 0.021f, 0), PxBoxGeometry(0.416f, 0.061f, 0.416f));
		agg.addStaticBox(PxTransform(0, 2.375f, -4.5f), PxBoxGeometry(4.0f, 0.125f, 2.5f));
		agg.addStaticBox(PxTransform(0, 1.25f, -2.25f), PxBoxGeometry(4.0f, 1.25f, 0.25f));
		agg.addStaticBox(PxTransform(4.5f, 4.0f, -4.2f), PxBoxGeometry(0.5f, 1.7f, 2.2f));
		agg.addStaticBox(PxTransform(1.085f, 1.25f, -2.25f), PxBoxGeometry(3.181f, 1.25f, 0.25f));
		agg.addStaticBox(PxTransform(-3.569f, 1.25f, -2.25f), PxBoxGeometry(0.436f, 1.25f, 0.25f));
		agg.addStaticBox(PxTransform(0.01f, 3.388f, -2.902f), PxBoxGeometry(0.49f, 0.118f, 0.49f));
		agg.addStaticBox(PxTransform(0.01f, 2.939f, -2.902f), PxBoxGeometry(0.222f, 0.345f, 0.222f));
		agg.addStaticBox(PxTransform(0.01f, 2.519f, -2.902f), PxBoxGeometry(0.416f, 0.061f, 0.416f));
	}
	ourPxScene->addAggregate(*agg.gameSceneAggregate);
}

void GameLogic::SetupScene()
{
	using namespace physx;
	initStaticColliders();

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

	PxMaterial* gargoyleMat = physX->createMaterial(0.5f, 0.5f, 0.6f);
	auto &transform = ourScene.GetObject("gargoyle")->GetTransform();
	auto &gargPos = transform.GetPosition();
	ourGargoyleBox = physX->createRigidDynamic(PxTransform(gargPos[0], gargPos[1], gargPos[2]));
	PxShape* gargoyleBoxShape = PxRigidActorExt::createExclusiveShape(*ourGargoyleBox, PxBoxGeometry(0.75, 0.6, 0.75), *gargoyleMat);
	ourPxScene->addActor(*ourGargoyleBox);
	PxSetGroup(*ourGargoyleBox, 2);
	gargoyleBoxShape->setQueryFilterData(PxFilterData(GROUP1, 0, 0, 0));
	ourGargoyleBox->setRigidDynamicLockFlags(PxRigidDynamicLockFlag::eLOCK_ANGULAR_X | PxRigidDynamicLockFlag::eLOCK_ANGULAR_Y | PxRigidDynamicLockFlag::eLOCK_ANGULAR_Z);
	ourGargoyleBox->userData = new Gargoyle(*ourScene.GetObject("gargoyle"));

	//Key


}
