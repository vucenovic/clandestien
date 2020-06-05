#include <string>
#include <sstream>
#include <memory>

#include <iostream>
#include <GL\glew.h>
#include <GLFW\glfw3.h>
#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "INIReader.h"

#include <PhysX/PxPhysicsAPI.h>

#include "ShaderProgram.h"
#include "CameraController.h"
#include "Mesh.h"
#include "UniformBuffer.h"
#include "Lights.h"
#include "LightManager.h"
#include "Texture.h"
#include "Scene.h"
#include "GameObject.h"
#include "FrameBuffer.h"
#include "Particles.h"
#include "Camera.h"
#include "GameSceneAggregateBuilder.h"
#include "HUD.h"

#include "Main.h"

#include <random>

/*
#define SOL_ALL_SAFETIES_ON 1
#include <lua/lua.hpp>
#include <sol.hpp>
*/

float scrollOffset = 0; // very ugly solution until I figure out something better Namely a proper inputmanager TODO
bool wireframeMode = false;
bool backfaceCulling = true;
bool drawBloom = true;
bool drawDepth = false;
bool debugDraw = false;
int debugDrawmode = 0;
bool actionKey = false;

//TODO move somewhere sane -> group with remaining physX stuff
physx::PxQuat fromEuler(glm::vec3 e) {
	glm::quat q = glm::quat(e);
	return physx::PxQuat(q.x,q.y,q.z,q.w);
}

int main(int argc, char** argv)
{
	INIReader reader("res/settings.ini");
	
	int width = reader.Get<int>("gfx", "width", 800);
	int height = reader.Get<int>("gfx", "height", 800);
	int targetFPS = reader.Get<int>("gfx", "refresh_rate", 60);
	bool fpsCapped = targetFPS != 0;
	bool fullscreen = reader.Get<bool>("gfx", "fullscreen", false);
	int vsync = reader.Get<int>("gfx", "vsync", 0);
	float gamma = reader.Get<float>("gfx", "gamma", 1);
	char forward = reader.Get<char>("controls", "forwards", 'W');
	char backward = reader.Get<char>("controls", "backwards", 'S');
	char left = reader.Get<char>("controls", "left", 'A');
	char right = reader.Get<char>("controls", "right", 'D');
	char interaction = reader.Get<char>("controls", "interaction", 'E');

	std::string window_title = "Clandestien";
	float FOV = 60;
	float nearPlane = 0.05f;
	float farPlane = 100;

	if (!glfwInit()) {
		std::cerr << "Failed to initialize GLFW";
		return 1;
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
	glfwWindowHint(GLFW_DOUBLEBUFFER, GLFW_FALSE);
	//glfwWindowHint(GLFW_SAMPLES, 4);

	#if _DEBUG
		glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
	#endif

	//create and initialize Context
	GLFWwindow* window = glfwCreateWindow(width, height, window_title.c_str(), fullscreen ? glfwGetPrimaryMonitor() : NULL, NULL);
	if (!window) {
		glfwTerminate();
		std::cerr << "Failed to init Window";
		return 1;
	}
	glfwMakeContextCurrent(window);

	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK) {
		glfwTerminate();
		std::cerr << "Failed to init GLEW";
		return 1;
	}

	//Pre scene initialization
	#if _DEBUG
		glDebugMessageCallback(DebugCallback, NULL);
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
	#endif

	glClearColor(0, 0, 0, 1);
	glEnable(GL_CULL_FACE);
	glfwSwapInterval(vsync);

	glfwSetKeyCallback(window, MyKeyCallback);
	glfwSetScrollCallback(window, scroll_callback);

	// initialize PhysX engine as documented by Nvidia PhysX SDK Documentary
	PxDefaultAllocator		gAllocator;
	PxDefaultErrorCallback	gErrorCallback;
	PxFoundation* gFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, gAllocator, gErrorCallback);
	if (!gFoundation) {
		glfwTerminate();
		std::cerr << "Failed to initialize PhysX Foundation";
		return 1;
	}

	{
		// initialize physics and scene
		PxPhysics* gPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *gFoundation, PxTolerancesScale(), true, NULL);

		PxSceneDesc sceneDesc(gPhysics->getTolerancesScale());
		sceneDesc.gravity = PxVec3(0.0f, -9.81f, 0.0f);
		PxDefaultCpuDispatcher* gDispatcher = PxDefaultCpuDispatcherCreate(2);
		sceneDesc.cpuDispatcher = gDispatcher;
		//https://docs.nvidia.com/gameworks/content/gameworkslibrary/physx/apireference/files/group__extensions.html#g587ba12f90f77543c3e4452abeb0f22f
		//https://github.com/NVIDIAGameWorks/PhysX/blob/4.1/physx/source/physxextensions/src/ExtDefaultSimulationFilterShader.cpp
		sceneDesc.filterShader = PxDefaultSimulationFilterShader;
		PxScene* gScene = gPhysics->createScene(sceneDesc);

		PxPvdSceneClient* pvdClient = gScene->getScenePvdClient();
		if (pvdClient)
		{
			pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONSTRAINTS, true);
			pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONTACTS, true);
			pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_SCENEQUERIES, true);
		}

		// initialize aggregate for gamescene for optimization

		//StaticColliders
		const PxU32 staticColliderCount = 32;
		GameSceneAggregateBuilder agg = GameSceneAggregateBuilder(staticColliderCount, false, gPhysics);
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
		}
		gScene->addAggregate(*agg.gameSceneAggregate);

		// add kinematic capsule character controller (experimental parameters)

		float characterHeight = 1.8f;
		float characterRadius = 0.25f;
		float characterEyeHeight = 1.7f;
		float characterMoveSpeed = 2.0f;

		PxControllerManager* manager = PxCreateControllerManager(*gScene);
		PxCapsuleControllerDesc desc;
		PxMaterial* controllerMaterial = gPhysics->createMaterial(0.8f, 0.8f, 0.9f);
		desc.stepOffset = 0.1f;
		desc.contactOffset = 0.05;
		desc.material = controllerMaterial;
		desc.density = 10.0;
		desc.isValid();
		desc.scaleCoeff = 0.95;
		desc.volumeGrowth = 1.5f;
		desc.position = PxExtendedVec3(1.0, characterHeight / 2, 0.0);
		desc.radius = characterRadius;
		desc.height = characterHeight - characterRadius * 2; //height = distance between sphere centers on capsule
		desc.climbingMode = PxCapsuleClimbingMode::eLAST;
		PxController* c = manager->createController(desc);
		manager->setOverlapRecoveryModule(true);
		//https://docs.nvidia.com/gameworks/content/gameworkslibrary/physx/apireference/files/structPxFilterData.html

		PxSetGroup(*c->getActor(), 1);

		// add debug visualization parameters

		enum ActiveGroup
		{
			GROUP1 = (1 << 0),
			GROUP2 = (1 << 1),
			GROUP3 = (1 << 2),
			GROUP4 = (1 << 3),
		};

		gScene->setVisualizationParameter(PxVisualizationParameter::eSCALE, 1.0f);
		gScene->setVisualizationParameter(PxVisualizationParameter::eCOLLISION_AABBS, 1.0);
		gScene->setVisualizationParameter(PxVisualizationParameter::eCOLLISION_SHAPES, 1.0);

		std::shared_ptr<ShaderProgram> standardShader,unifiedPBR, myShaderProgram, debugShader, particleShader, pp_demultAlpha, pp_gammaCorrect, pp_blur, pp_bloom, unlitShader, SSDepthReset, DebugDepthSS, GargoyleShader, DepthShader;
		try
		{
			myShaderProgram = std::shared_ptr<ShaderProgram>(ShaderProgram::FromFile("res/shaders/common.vert", "res/shaders/LitPhong.frag"));
			unifiedPBR = std::shared_ptr<ShaderProgram>(ShaderProgram::FromFile("res/shaders/common.vert", "res/shaders/unifiedPBR.frag"));
			particleShader = std::shared_ptr<ShaderProgram>(ShaderProgram::FromFile("res/shaders/particle.vert", "res/shaders/particle.geom", "res/shaders/particle.frag"));
			standardShader = std::shared_ptr<ShaderProgram>(ShaderProgram::FromFile("res/shaders/common.vert", "res/shaders/TexturedLitPhong.frag"));
			debugShader = std::shared_ptr<ShaderProgram>(ShaderProgram::FromFile("res/shaders/common.vert", "res/shaders/Debug.frag"));
			pp_demultAlpha = std::shared_ptr<ShaderProgram>(ShaderProgram::FromFile("res/shaders/screenspace.vert", "res/shaders/DemultAlpha.frag"));
			pp_gammaCorrect = std::shared_ptr<ShaderProgram>(ShaderProgram::FromFile("res/shaders/screenspace.vert", "res/shaders/GammaCorrect.frag"));
			pp_blur = std::shared_ptr<ShaderProgram>(ShaderProgram::FromFile("res/shaders/screenspace.vert", "res/shaders/Blur.frag"));
			pp_bloom = std::shared_ptr<ShaderProgram>(ShaderProgram::FromFile("res/shaders/screenspace.vert", "res/shaders/Bloom.frag"));
			unlitShader = std::shared_ptr<ShaderProgram>(ShaderProgram::FromFile("res/shaders/common.vert", "res/shaders/Unlit.frag"));
			SSDepthReset = std::shared_ptr<ShaderProgram>(ShaderProgram::FromFile("res/shaders/screenspace.vert", "res/shaders/DepthReset.frag"));
			DebugDepthSS = std::shared_ptr<ShaderProgram>(ShaderProgram::FromFile("res/shaders/screenspace.vert", "res/shaders/depthToColor.frag"));
			GargoyleShader = std::shared_ptr<ShaderProgram>(ShaderProgram::FromFile("res/shaders/gargoyle.vert", "res/shaders/gargoyle.frag"));
			//DepthShader = std::shared_ptr<ShaderProgram>(ShaderProgram::FromFile("res/shaders/depth.vert", "res/shaders/depth.frag"));
		}
		catch (const std::invalid_argument&)
		{
			std::cerr << "Shader failed to compile" << std::endl;
			goto _shaderCompileError; //jumps to context cleanup section
		}
		std::shared_ptr<Texture2D> tilesDiff = std::make_shared<Texture2D>("res/textures/tiles_diffuse");
		std::shared_ptr<Texture2D> tilesSpec = std::make_shared<Texture2D>("res/textures/tiles_specular");
		std::shared_ptr<Texture2D> tilesNorm = std::make_shared<Texture2D>("res/textures/tiles_normal");
		std::shared_ptr<Texture2D> woodDiff = std::make_shared<Texture2D>("res/textures/wood_texture");
		std::shared_ptr<Texture2D> devDiff = std::make_shared<Texture2D>("res/textures/dev_diffuse");
		std::shared_ptr<Texture2D> devNorm = std::make_shared<Texture2D>("res/textures/dev_normal");
		std::shared_ptr<TextureCubemap> cubeMap = std::make_shared<TextureCubemap>("res/textures/cubemap/");
		std::shared_ptr<Texture2D> whiteTex = std::make_shared<Texture2D>(glm::vec3(1));
		std::shared_ptr<Texture2D> blackTex = std::make_shared<Texture2D>(glm::vec3(0));
		std::shared_ptr<Texture2D> purpleTex = std::make_shared<Texture2D>(glm::vec3(0.5f,0.5f,1));

		std::shared_ptr<Texture2D> particleTex = std::make_shared<Texture2D>("res/textures/particle");

		//--------Camera

		Camera camera = Camera();
		camera.SetPerspective(FOV, (float)width / (float)height, nearPlane, farPlane);

		CameraController myCameraController(&camera.GetTransform(),window);

		//Meshes
		
		std::shared_ptr<Mesh> gameStageMesh = OBJLoader::LoadOBJ("res/models/GameScene.obj");
		std::shared_ptr<Mesh> gargoyleMesh = OBJLoader::LoadOBJ("res/models/Gargoyle.obj");
		std::shared_ptr<Mesh> myTestMesh = OBJLoader::LoadOBJ("res/models/monkey.obj");
		std::shared_ptr<Mesh> myPortalTestMesh = OBJLoader::LoadOBJ("res/models/Portal.obj");

		//Materials

		std::shared_ptr<Material> particlesMaterial = std::make_shared<Material>(particleShader);
		particlesMaterial->SetTexture(particleTex, 0);

		Material debugMaterial = Material(debugShader);
		debugMaterial.SetPropertyi("mode",2);

		Material depthMaterial = Material(unlitShader);

		Material gargoyleMaterial = Material(GargoyleShader);
		gargoyleMaterial.SetProperty4f("material", glm::vec4(0.1f, 0.7f, 1, 8));
		gargoyleMaterial.SetProperty4f("flatColor", glm::vec4(1, 1, 1, 0.15f));
		gargoyleMaterial.SetTexture(devDiff, 0);
		gargoyleMaterial.SetTexture(whiteTex, 1);
		gargoyleMaterial.SetTexture(devNorm, 2);
		gargoyleMaterial.SetTexture(blackTex, 3);
	

		Material tilesMaterial = Material(GargoyleShader);
		tilesMaterial.SetProperty4f("material", glm::vec4(0.1f, 0.7f, 1, 8));
		tilesMaterial.SetProperty4f("flatColor", glm::vec4(1, 1, 1, 0.15f));
		tilesMaterial.SetTexture(tilesDiff, 0);
		tilesMaterial.SetTexture(tilesSpec, 1);
		tilesMaterial.SetTexture(tilesNorm, 2);
		tilesMaterial.SetTexture(cubeMap, 3);

		Material devMaterial = Material(GargoyleShader);
		devMaterial.SetProperty4f("material", glm::vec4(0.05f,0.5f,1,8));
		devMaterial.SetProperty4f("flatColor", glm::vec4(0.7f, 0.7f, 0.7f, 2));
		devMaterial.SetTexture(devDiff, 0);
		devMaterial.SetTexture(whiteTex, 1);
		devMaterial.SetTexture(devNorm, 2);
		devMaterial.SetTexture(blackTex, 3);

		Material woodMaterial = Material(GargoyleShader);
		woodMaterial.SetProperty4f("material", glm::vec4(0.1f, 0.7f, 0.1f, 2));
		woodMaterial.SetProperty4f("flatColor", glm::vec4(1, 1, 1, 0.15f));
		woodMaterial.SetTexture(woodDiff, 0);
		woodMaterial.SetTexture(whiteTex, 1);
		woodMaterial.SetTexture(purpleTex, 2);
		woodMaterial.SetTexture(blackTex, 3);

		//Objects

		std::unique_ptr<GameObject> gargoyle = std::make_unique<GameObject>();
		gargoyle->mesh = gargoyleMesh.get();
		gargoyle->material = &devMaterial;
		gargoyle->GetTransform().SetPostion(glm::vec3(0.0, 0.0, 0.0));
		gargoyle->name = "gargoyle";


		std::unique_ptr<GameObject> gameStage = std::make_unique<GameObject>();
		gameStage->mesh = gameStageMesh.get();
		gameStage->material = &devMaterial;
		gameStage->GetTransform().SetPostion(glm::vec3(0, 0, 0));
		gameStage->name = "gameStage";

		//--------Uniform Buffers

		UniformBuffer viewDataBuffer = UniformBuffer(*standardShader, std::string("viewData"), { "projection","view" }, 2);
		viewDataBuffer.BindToPort(0);

		LightManager myLightManager = LightManager(*standardShader);
		myLightManager.BindToPort(1);

		{
			myLightManager.ambientLight = glm::vec3(1, 1, 1);

			myLightManager.pointLights.push_back(PointLight(glm::vec3(), glm::vec3(1, 1, 1), glm::vec4(1, 0.4f, 0.1f, 100)));
			myLightManager.pointLights.push_back(PointLight(glm::vec3(2.2f, 1, 0), glm::vec3(50, 0, 0), glm::vec4(1, 1, 1, 100)));
			myLightManager.pointLights.push_back(PointLight(glm::vec3(-5, 1, 0), glm::vec3(1, 1, 1), glm::vec4(1, 0.4f, 0.3f, 100)));
			myLightManager.pointLights.push_back(PointLight(glm::vec3(5, 1, 0), glm::vec3(1, 1, 1), glm::vec4(1, 0.4f, 0.3f, 100)));

			myLightManager.directionalLights.push_back(DirectionalLight(glm::vec3(0, -1, 0), glm::vec3(0.1f, 0.1f, 0.1f)));
			//myLightManager.directionalLights.push_back(DirectionalLight(glm::vec3(0, -1, 1), glm::vec3(0.1f, 0.1f, 0.1f)));

			myLightManager.shadowLight = SpotLight(
				glm::vec3(2.0, 1.0, -1.0),
				glm::vec3(-1.0f, 0.0f, 0.0f),
				glm::vec2(glm::cos(glm::radians(5.0)), glm::cos(glm::radians(15.0))),
				glm::vec3(1, 1, 1),
				glm::vec4(0.2f, 0.01f, 0.05f, 100)
			);
			myLightManager.shadowLightUsed = true;

			//myLightManager.spotLights[0].SetPosition(glm::vec3(4.6f,-2.4f,-4.3f));
			//myLightManager.spotLights[0].SetDirection(glm::vec3(-0.7f, 0.15f, 0.62f));
			//myLightManager.spotLights[0].SetAttenuation(0.2f, 0.01f, 0.05f);
			//myLightManager.spotLights[0].SetRadialFalloffDegrees(5, 15);
		}

		myLightManager.UpdateBuffer();

		Scene myScene = Scene();

		myScene.AddObject(gargoyle);
		myScene.AddObject(gameStage);

		// Dynamics

		//gargoyle
		PxMaterial* gargoyleMat = gPhysics->createMaterial(0.5f, 0.5f, 0.6f);
		auto &transform = myScene.GetObject("gargoyle")->GetTransform();
		auto &gargPos = transform.GetPosition();
		PxRigidDynamic* gargyoleBox = gPhysics->createRigidDynamic(PxTransform(gargPos[0] - 0.75, gargPos[1]+0.7,gargPos[2] - 1.2));
		PxShape* gargoyleBoxShape = PxRigidActorExt::createExclusiveShape(*gargyoleBox, PxBoxGeometry(0.75, 0.7, 0.6), *gargoyleMat);
		gScene->addActor(*gargyoleBox);
		PxSetGroup(*gargyoleBox, 2);
		gargoyleBoxShape->setQueryFilterData(PxFilterData(GROUP1, 0, 0, 0));

		//Particles

		using namespace Particles;

		ParticleSystemMeshManager pmmanager(150);
		ParticleSystemDefinition pSystemDef;
		{
			pSystemDef.lifeTime = 5;
			pSystemDef.maxParticles = 150;
			pSystemDef.material = particlesMaterial;
		}
		ParticleSystem pSystem(pSystemDef);
		pSystem.CreateDebugParticles();
		
		//Post processing

		GLuint ssplaneVAO; //create screenspace Rect for all screenspace effects
		{
			glGenVertexArrays(1, &ssplaneVAO);
			glBindVertexArray(ssplaneVAO);
			GLuint buffers;
			glGenBuffers(1, &buffers);

			static const GLfloat data[4 * 4 * 2] = { -1,-1,0,0, 1,-1,1,0, -1,1,0,1, 1,1,1,1 };

			glBindBuffer(GL_ARRAY_BUFFER, buffers);
			glBufferData(GL_ARRAY_BUFFER, 2 * 4 * 4 * sizeof(GLfloat), data, GL_STATIC_DRAW);

			glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * 2 * sizeof(GLfloat), 0);
			glEnableVertexAttribArray(0);

			glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * 2 * sizeof(GLfloat), (const void*)(2 * sizeof(GLfloat)));
			glEnableVertexAttribArray(1);

			glBindVertexArray(0);
			glDeleteBuffers(1, &buffers);
		}

		RenderFrameBuffer renderFBO = RenderFrameBuffer(width, height);

		ColorFrameBuffer ppFBO = ColorFrameBuffer(width, height);
		ColorFrameBuffer ppFBO2 = ColorFrameBuffer(width, height);

		const float bloomScale = 4;
		ColorFrameBuffer ppFBOb = ColorFrameBuffer(width / bloomScale, height / bloomScale); //smaller scale frame buffers *just for the bloom* I don't really like it
		ColorFrameBuffer ppFBOb2 = ColorFrameBuffer(width / bloomScale, height / bloomScale); //but this was the obvious solution to the sampling problem

		/* Shadow Map Frame Buffer for Spotlights */

		DepthFrameBuffer shadowspotFBO = DepthFrameBuffer(256, 256);

		double lastFrameTime = 0;
		double nextSecond = 1;
		size_t frameCount = 0;

		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

		myScene.activeCamera = &camera;
		myScene.viewDataBuffer = &viewDataBuffer;
		myScene.SSrectVAOId = ssplaneVAO;
		myScene.portalHoldoutShader = unlitShader;
		myScene.depthResetSS = SSDepthReset;

		Portal myTestPortal = Portal();
		myTestPortal.portalMesh = myPortalTestMesh;
		myTestPortal.transform.SetRotationDegrees(0,90,0);
		myTestPortal.transform.SetPostion(glm::vec3(-3.999f, 1, 0));
		myTestPortal.targetTransform.SetRotationDegrees(0, 0, 0);
		myTestPortal.targetTransform.SetPostion(glm::vec3(0, 3.5f, -5.999f));
		myScene.renderPortals.push_back(myTestPortal);

		Portal myTestPortal2 = Portal();
		myTestPortal2.portalMesh = myPortalTestMesh;
		myTestPortal2.transform.SetRotationDegrees(0, 0, 0);
		myTestPortal2.transform.SetPostion(glm::vec3(0, 3.5f, -5.999f));
		myTestPortal2.targetTransform.SetRotationDegrees(0, 90, 0);
		myTestPortal2.targetTransform.SetPostion(glm::vec3(-3.999f, 1, 0));
		myScene.renderPortals.push_back(myTestPortal2);

		const float physTimeStep = 1.0f / 60.0f;
		float physTimeAccumulator = 0;

		//Render Loop
		while (!glfwWindowShouldClose(window))
		{
			double currentFrametime = glfwGetTime();
			float deltaTime = (float)(currentFrametime - lastFrameTime);
			physTimeAccumulator += deltaTime;
			lastFrameTime = currentFrametime;
			//DISPLAY FRAMES PER SECOND
			frameCount++;
			if (currentFrametime >= nextSecond) {
				std::cout << frameCount << "FPS" << std::endl;
				frameCount = 0;
				nextSecond = currentFrametime + 1;
			}

			//Poll
			glfwPollEvents();

			//Do Physics steps
			while (physTimeAccumulator > physTimeStep) {
				gScene->simulate(physTimeStep);
				gScene->fetchResults(true);
				physTimeAccumulator -= physTimeStep;
			}

			// Move character and camera
			{
				myCameraController.HandleInputs();

				glm::vec3 forwardVector = camera.GetTransform().GetForward();
				forwardVector.y = 0; //clamp movement to horizonal plane
				forwardVector = glm::normalize(forwardVector);
				glm::vec3 rightVector = glm::cross(forwardVector, glm::vec3(0, 1, 0));

				glm::vec3 moveDir = forwardVector * (float)((glfwGetKey(window, (int)forward) == GLFW_PRESS) - (glfwGetKey(window, (int)backward) == GLFW_PRESS)) +
					rightVector * (float)((glfwGetKey(window, (int)right) == GLFW_PRESS) - (glfwGetKey(window, (int)left) == GLFW_PRESS));
				moveDir *= characterMoveSpeed * deltaTime;
				moveDir.y = -0.01f;

				PxControllerFilters filters(NULL, NULL, NULL);
				PxControllerCollisionFlags collFlags = c->move(PxVec3(moveDir.x, moveDir.y, moveDir.z), 0.0, deltaTime, filters, NULL);

				PxControllerState state;
				c->getState(state);
				PxExtendedVec3 newPos = c->getFootPosition();
				myCameraController.cameraTransform->SetPostion(glm::vec3(newPos[0], newPos[1] + characterEyeHeight, newPos[2]));
			}

			/* PHYSX */
			{
				glm::vec3 camPos = camera.GetTransform().GetPosition();
				PxVec3 origin = PxVec3(camPos.x, camPos.y, camPos.z);            // [in] Ray origin
				glm::vec3 viewVector = camera.GetTransform().GetForward();
				viewVector.y = 0;
				viewVector = glm::normalize(viewVector);

				PxVec3 unitDir = PxVec3(viewVector.x ,viewVector.y, viewVector.z);             // [in] Normalized ray direction
				PxReal maxDistance = 1.0;            // [in] Raycast max distance
				PxRaycastBuffer hit;
				gargyoleBox->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, false); //TODO dont set in loop

				PxQueryFilterData filterData = PxQueryFilterData(); //TODO dont set in loop
				filterData.data.word0 = GROUP1; //TODO dont set in loop
				const PxHitFlags outputFlags = PxHitFlag::eDEFAULT | PxHitFlag::ePOSITION | PxHitFlag::eNORMAL;

				//Nutz das "Userdata" attribute von PxActor um eine 1:1 beziehung mit dem Gameobject oder was auch immer zu erzeugen.
				bool status = gScene->raycast(origin, unitDir, maxDistance, hit, outputFlags, filterData);
				if (status && (glfwGetKey(window, (int)interaction) == GLFW_PRESS)) {
					gargyoleBox->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, true); //TODO dont set in loop
					std::cout << hit.block.actor << std::endl;
					auto &transform = myScene.GetObject("gargoyle")->GetTransform();
					if (glfwGetKey(window, (int)forward) == GLFW_PRESS) {
						auto &transform = myScene.GetObject("gargoyle")->GetTransform();
						gargyoleBox->setGlobalPose(PxTransform(viewVector.x * 2.0 * deltaTime, 0.0, viewVector.z * 2.0 * deltaTime));// NO IDEA WHY i have to subtract, position just doesnt fit without 
						gargyoleBox->setKinematicTarget(PxTransform(viewVector.x * 2.0 * deltaTime, 0.0, viewVector.z * 2.0 * deltaTime));
						//gargyoleBox->addForce(PxVec3(0.001, 0.001, 0.001), PxForceMode::eFORCE);
						transform.SetPostion(glm::vec3(gargyoleBox->getGlobalPose().p[0], gargyoleBox->getGlobalPose().p[1], gargyoleBox->getGlobalPose().p[2]));
					}
					else if (glfwGetKey(window, (int)backward) == GLFW_PRESS) {
						auto &transform = myScene.GetObject("gargoyle")->GetTransform();
						gargyoleBox->setGlobalPose(PxTransform(viewVector.x * -2.0 * deltaTime, 0.0, viewVector.z * -2.0 * deltaTime)); // NO IDEA WHY i have to subtract, position just doesnt fit without 
						gargyoleBox->setKinematicTarget(PxTransform(viewVector.x * -2.0 * deltaTime, 0.0, viewVector.z * -2.0 * deltaTime));
						//gargyoleBox->addForce(PxVec3(viewVector.x * 2.0 * deltaTime, 0.0, viewVector.z * 2.0 * deltaTime), PxForceMode::eFORCE);
						transform.SetPostion(glm::vec3(gargyoleBox->getGlobalPose().p[0], gargyoleBox->getGlobalPose().p[1], gargyoleBox->getGlobalPose().p[2]));
					}
					
				}
			}

			// SHADOW MAPS: render depth 
			{
				// change view-projection matrix according to spotlight parameters
				Camera::SetViewParameters(viewDataBuffer, myLightManager.shadowView(), myLightManager.shadowProjection());

				shadowspotFBO.Bind();
				glViewport(0, 0, 256, 256);
				glClear(GL_DEPTH_BUFFER_BIT);

				glEnable(GL_DEPTH_TEST);
				glCullFace(GL_FRONT);
				myScene.DrawOpaqueObjects(depthMaterial);
				glCullFace(GL_BACK);

				glActiveTexture(GL_TEXTURE4);
				glBindTexture(GL_TEXTURE_2D, shadowspotFBO.depthMap);
			}

			//Render Scene
			renderFBO.Bind();
			glViewport(0, 0, width, height);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			//glEnable(GL_MULTISAMPLE);
			glEnable(GL_DEPTH_TEST);

			{
				pSystem.Update(deltaTime);
				glBindBuffer(GL_ARRAY_BUFFER, pmmanager.GetBufferHandle());
				void * buf = glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
				glm::vec3 forward = camera.GetTransform().GetForward();
				pSystem.SortParticles(forward); // sort particles that use alpha blending instead of additive blending
				pSystem.WriteMesh((Particle*)buf, 0, 150);
				glUnmapBuffer(GL_ARRAY_BUFFER);
				glBindBuffer(GL_ARRAY_BUFFER, 0);
			}

			if (debugDraw) {
				debugMaterial.SetPropertyi("mode", debugDrawmode);
				camera.UseCamera(viewDataBuffer);
				myScene.DrawOpaqueObjects(debugMaterial);
			}
			else {
				myScene.DrawScene(true);
			}

			{ //TODO move into particle system class and particle sytemrenderer
				ParticleSystem::PrepareDraw();
				pmmanager.Bind();
				pSystem.Draw();
				ParticleSystem::FinishDraw();
			}

			/* multisample stuffs 
			glBindFramebuffer(GL_DRAW_FRAMEBUFFER, ppFBO.GetHandle());
			glBindFramebuffer(GL_READ_FRAMEBUFFER, renderFBO.GetHandle());
			//glDrawBuffer(GL_BACK);
			glBlitFramebuffer(0, 0, width, height, 0, 0, width, height, GL_COLOR_BUFFER_BIT, GL_LINEAR);
			glDisable(GL_MULTISAMPLE);
			*/

			//Apply post processing
			//TODO: Abstract individual commands into a postprocessing stack container
			{
				glDisable(GL_DEPTH_TEST);
				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

				glBindVertexArray(ssplaneVAO);

				if (drawDepth) { //draw depthbuffer to screen for debug purposes
					FrameBuffer::Unbind();
					glActiveTexture(GL_TEXTURE0);
					glBindTexture(GL_TEXTURE_2D, renderFBO.depthStencil);
					DebugDepthSS->UseProgram();
					//glUniform1f(0, 1);
					glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
				}
				else
				{
					//Demultipy alpha values
					ppFBO.Bind();
					pp_demultAlpha->UseProgram();
					glActiveTexture(GL_TEXTURE0);
					glBindTexture(GL_TEXTURE_2D, renderFBO.color);
					glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

					//Filter Highlights out of image
					ppFBOb.Bind();
					glViewport(0, 0, width / bloomScale, height / bloomScale);
					pp_bloom->UseProgram();
					glBindTexture(GL_TEXTURE_2D, ppFBO.color);
					glUniform1i(pp_bloom->GetUniformLocation("combine"), 0);
					glUniform1f(pp_bloom->GetUniformLocation("cutoff"), 1);
					glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
				
					//Blur Highlights
					ppFBOb2.Bind();
					pp_blur->UseProgram();
					glBindTexture(GL_TEXTURE_2D, ppFBOb.color);
					glUniform1f(pp_blur->GetUniformLocation("scale"), 1.0f);
					glUniform1i(pp_blur->GetUniformLocation("horizontal"), 0);
					glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

					ppFBOb.Bind();
					glBindTexture(GL_TEXTURE_2D, ppFBOb2.color);
					glUniform1i(pp_blur->GetUniformLocation("horizontal"), 1);
					glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

					glViewport(0, 0, width, height);
					//Combine Blurred Highlights with base Image
					FrameBuffer::Unbind();
					pp_bloom->UseProgram();

					if (drawBloom) {
						glActiveTexture(GL_TEXTURE1);
						glBindTexture(GL_TEXTURE_2D, ppFBOb.color);
					}
					else {
						blackTex->Bind(1);
					}

					glActiveTexture(GL_TEXTURE0);
					glBindTexture(GL_TEXTURE_2D, ppFBO.color);

					glUniform1i(pp_bloom->GetUniformLocation("combine"), 1);
					glUniform1f(pp_bloom->GetUniformLocation("exposure"), 1);
					glUniform1f(pp_bloom->GetUniformLocation("gamma"), gamma);
					glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
				}

			}

			// draw HUD

			// hudManager.drawHUDquad(width, height);

			//Reset Renderstate
			if (wireframeMode) glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			else glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			if (backfaceCulling) glEnable(GL_CULL_FACE);
			else glDisable(GL_CULL_FACE);

			//Flip Buffers
			glfwSwapBuffers(window);//To enforce Vsync
			glFlush();//To actually draw to screen
			scrollOffset = 0;
		}

		//clean up before leaving scope
		glUseProgram(0);
		glDeleteVertexArrays(1, &ssplaneVAO);

	}
	_shaderCompileError:

	glfwTerminate(); //Cleanup GLFW

	return 0;
}

static void MyKeyCallback(GLFWwindow * window, int key, int scancode, int action, int mods)
{
	if (action == GLFW_PRESS) {
		switch (key)
		{
		case GLFW_KEY_ESCAPE:
			glfwSetWindowShouldClose(window, GLFW_TRUE);
			break;
		case GLFW_KEY_F1:
			wireframeMode = !wireframeMode;
			break;
		case GLFW_KEY_F2:
			backfaceCulling = !backfaceCulling;
			break;
		case GLFW_KEY_F3:
			debugDraw = !debugDraw;
			break;
		case GLFW_KEY_F4:
			drawDepth = !drawDepth;
			break;
		case GLFW_KEY_1:
			debugDrawmode = (debugDrawmode + 1) % 7;
			break;
		case GLFW_KEY_2:
			drawBloom = !drawBloom;
			break;
		default:
			break;
		}
	}
}


static void APIENTRY DebugCallback(GLenum source, GLenum type, GLuint id, GLenum severity,	GLsizei length, const GLchar* message, const GLvoid* userParam)
{
	if (id == 131185 || id == 131218) return;

	std::string error = FormatDebugOutput(source, type, id, severity, message);
	std::cout << error << std::endl;
}

static std::string FormatDebugOutput(GLenum source, GLenum type, GLuint id, GLenum severity, const char* msg) {
	std::stringstream stringStream;
	std::string sourceString;
	std::string typeString;
	std::string severityString;

	switch (source) {
	case GL_DEBUG_SOURCE_API: {
		sourceString = "API";
		break;
	}
	case GL_DEBUG_SOURCE_APPLICATION: {
		sourceString = "Application";
		break;
	}
	case GL_DEBUG_SOURCE_WINDOW_SYSTEM: {
		sourceString = "Window System";
		break;
	}
	case GL_DEBUG_SOURCE_SHADER_COMPILER: {
		sourceString = "Shader Compiler";
		break;
	}
	case GL_DEBUG_SOURCE_THIRD_PARTY: {
		sourceString = "Third Party";
		break;
	}
	case GL_DEBUG_SOURCE_OTHER: {
		sourceString = "Other";
		break;
	}
	default: {
		sourceString = "Unknown";
		break;
	}
	}

	switch (type) {
	case GL_DEBUG_TYPE_ERROR: {
		typeString = "Error";
		break;
	}
	case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: {
		typeString = "Deprecated Behavior";
		break;
	}
	case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR: {
		typeString = "Undefined Behavior";
		break;
	}
	case GL_DEBUG_TYPE_PORTABILITY_ARB: {
		typeString = "Portability";
		break;
	}
	case GL_DEBUG_TYPE_PERFORMANCE: {
		typeString = "Performance";
		break;
	}
	case GL_DEBUG_TYPE_OTHER: {
		typeString = "Other";
		break;
	}
	default: {
		typeString = "Unknown";
		break;
	}
	}

	switch (severity) {
	case GL_DEBUG_SEVERITY_HIGH: {
		severityString = "High";
		break;
	}
	case GL_DEBUG_SEVERITY_MEDIUM: {
		severityString = "Medium";
		break;
	}
	case GL_DEBUG_SEVERITY_LOW: {
		severityString = "Low";
		break;
	}
	default: {
		severityString = "Unknown";
		break;
	}
	}

	stringStream << "OpenGL Error: " << msg;
	stringStream << " [Source = " << sourceString;
	stringStream << ", Type = " << typeString;
	stringStream << ", Severity = " << severityString;
	stringStream << ", ID = " << id << "]";

	return stringStream.str();
}

void scroll_callback(GLFWwindow * window, double xoffset, double yoffset)
{
	scrollOffset = (float)yoffset;
}