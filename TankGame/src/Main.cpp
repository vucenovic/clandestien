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

#include "ShaderProgram.h"
#include "CameraController.h"
#include "Mesh.h"
#include "UniformBuffer.h"
#include "Lights.h"
#include "LightManager.h"
#include "Texture.h"
#include "Renderer.h"

#include "Main.h"

float scrollOffset = 0; // very ugly solution until I figure out something better Namely a proper inputmanager TODO
bool wireframeMode = false;
bool backfaceCulling = true;

int main(int argc, char** argv)
{
	INIReader reader("res/settings.ini");

	int width = reader.Get<int>("window", "width", 800);
	int height = reader.Get<int>("window", "height", 800);
	std::string window_title = reader.Get<std::string>("window", "title", "Tank Game");
	float FOV = reader.Get<float>("camera", "fov", 60.0);
	float nearPlane = reader.Get<float>("camera", "near", 0.1);
	float farPlane = reader.Get<float>("camera", "far", 100.0);

	if (!glfwInit()) {
		std::cerr << "Failed to initialize GLFW";
		return 1;
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
	glfwWindowHint(GLFW_SAMPLES, 4);

	#if _DEBUG
		glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
	#endif

	//create and initialize Context
	GLFWwindow* window = glfwCreateWindow(width, height, window_title.c_str(), NULL, NULL);
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

	glViewport(0, 0, width, height);
	glClearColor(1, 1, 1, 1);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glEnable(GL_MULTISAMPLE);
	glfwSwapInterval(1); // vsync

	glfwSetKeyCallback(window, MyKeyCallback);
	glfwSetScrollCallback(window, scroll_callback);

	{
		std::shared_ptr<ShaderProgram> standardShader,LitCookTorrance, myShaderProgram, debugShader, particleShader;
		try
		{
			myShaderProgram = std::shared_ptr<ShaderProgram>(ShaderProgram::FromFile("res/shaders/common.vert", "res/shaders/LitPhong.frag"));
			//LitCookTorrance = std::shared_ptr<ShaderProgram>(ShaderProgram::FromFile("res/shaders/common.vert", "res/shaders/LitCookTorrance.frag"));
			particleShader = std::shared_ptr<ShaderProgram>(ShaderProgram::FromFile("res/shaders/particle.vert", "res/shaders/particle.geom", "res/shaders/particle.frag"));
			standardShader = std::shared_ptr<ShaderProgram>(ShaderProgram::FromFile("res/shaders/common.vert", "res/shaders/TexturedLitPhong.frag"));
			debugShader = std::shared_ptr<ShaderProgram>(ShaderProgram::FromFile("res/shaders/common.vert", "res/shaders/Debug.frag"));
		}
		catch (const std::invalid_argument&)
		{
			std::cerr << "Shader failed to compile" << std::endl;
			goto _shaderCompileError;
		}
		std::shared_ptr<Texture2D> tilesDiff = std::make_shared<Texture2D>("res/textures/tiles_diffuse");
		std::shared_ptr<Texture2D> tilesSpec = std::make_shared<Texture2D>("res/textures/tiles_specular");
		std::shared_ptr<Texture2D> tilesNorm = std::make_shared<Texture2D>("res/textures/tiles_normal");
		std::shared_ptr<Texture2D> woodDiff = std::make_shared<Texture2D>("res/textures/wood_texture");
		std::shared_ptr<TextureCubemap> cubeMap = std::make_shared<TextureCubemap>("res/textures/cubemap/");
		std::shared_ptr<Texture2D> whiteTex = std::make_shared<Texture2D>(glm::vec3(1));
		std::shared_ptr<Texture2D> blackTex = std::make_shared<Texture2D>(glm::vec3(0));
		std::shared_ptr<Texture2D> purpleTex = std::make_shared<Texture2D>(glm::vec3(0.5f,0.5f,1));

		//--------Camera

		//setup identity matrix and perspective transformmatrix
		glm::mat4 perspective = glm::perspective(glm::radians(FOV), (float)width / (float)height, nearPlane, farPlane);

		Transform myCameraTransform = Transform();

		CameraController myCameraController(&myCameraTransform,window);

		//--------Meshes

		std::shared_ptr<Mesh> myCubeMesh = MeshBuilder::BoxFlatShaded(1.5f, 1.5f, 1.5f);
		std::shared_ptr<Mesh> myCylinderMesh = MeshBuilder::CylinderSplitShaded(1.3f, 1, 32);
		std::shared_ptr<Mesh> mySphereMesh = MeshBuilder::Sphere(1, 64, 32);
		std::shared_ptr<Mesh> myTestMesh = OBJLoader::LoadOBJ("res/models/monkey.obj");

		Material debugMaterial = Material(debugShader);
		debugMaterial.SetPropertyi("mode",2);

		Material tilesMaterial = Material(standardShader);
		tilesMaterial.SetProperty4f("material", glm::vec4(0.1f,0.7f,1,8));
		tilesMaterial.SetProperty4f("flatColor", glm::vec4(1, 1, 1, 0.15f));
		tilesMaterial.SetTexture(tilesDiff, 0);
		tilesMaterial.SetTexture(tilesSpec, 1);
		tilesMaterial.SetTexture(tilesNorm, 2);
		tilesMaterial.SetTexture(cubeMap, 3);

		Material woodMaterial = Material(standardShader);
		woodMaterial.SetProperty4f("material", glm::vec4(0.1f, 0.7f, 0.1f, 2));
		woodMaterial.SetProperty4f("flatColor", glm::vec4(1, 1, 1, 0.15f));
		woodMaterial.SetTexture(woodDiff, 0);
		woodMaterial.SetTexture(whiteTex, 1);
		woodMaterial.SetTexture(purpleTex, 2);
		woodMaterial.SetTexture(blackTex, 3);

		//--------Objects

		GameObject testobject = GameObject();
		testobject.mesh = myTestMesh.get();
		testobject.material = &tilesMaterial;
		testobject.GetTransform().position = glm::vec3(3, 1.5f, 0);

		GameObject sphere = GameObject();
		sphere.mesh = mySphereMesh.get();
		sphere.material = &tilesMaterial;
		sphere.GetTransform().position = glm::vec3(1.5f,-1,0);

		GameObject cube = GameObject();
		cube.mesh = myCubeMesh.get();
		cube.material = &woodMaterial;
		cube.GetTransform().position = glm::vec3(0, 1.5f, 0);

		GameObject cylinder = GameObject();
		cylinder.mesh = myCylinderMesh.get();
		cylinder.material = &tilesMaterial;
		cylinder.GetTransform().position = glm::vec3(-1.5f, -1, 0);

		//--------Uniform Buffers

		UniformBuffer viewDataBuffer = UniformBuffer(*standardShader, std::string("viewData"), { "viewProjection","eyePos" }, 2);
		viewDataBuffer.BindToPort(0);

		LightManager myLightManager = LightManager(*standardShader);
		myLightManager.BindToPort(1);

		{
			myLightManager.lightsUsed.point = 3;
			myLightManager.lightsUsed.directional = 1;
			myLightManager.lightsUsed.spot = 1;

			myLightManager.ambientLight = glm::vec3(1, 1, 1);

			myLightManager.pointLights[0].SetPosition(glm::vec3());
			myLightManager.pointLights[0].SetAttenuation(1, 0.4f, 0.1f);

			myLightManager.pointLights[1].SetPosition(glm::vec3(-5,1,0));
			myLightManager.pointLights[1].SetAttenuation(1, 0.4f, 0.3f);

			myLightManager.pointLights[2].SetPosition(glm::vec3(5, 1, 0));
			myLightManager.pointLights[2].SetAttenuation(1, 0.4f, 0.3f);

			myLightManager.directionalLights[0].SetDirection(glm::vec3(0,-1,-1));
			myLightManager.directionalLights[0].SetColor(0.8f,0.8f,0.8f);

			myLightManager.directionalLights[1].SetDirection(glm::vec3(0, -1, 1));
			myLightManager.directionalLights[1].SetColor(0.1f, 0.1f, 0.1f);

			myLightManager.spotLights[0].SetPosition(glm::vec3(4.6f,-2.4f,-4.3f));
			myLightManager.spotLights[0].SetDirection(glm::vec3(-0.7f, 0.15f, 0.62f));
			myLightManager.spotLights[0].SetAttenuation(0.2f, 0.01f, 0.05f);
			myLightManager.spotLights[0].SetRadialFalloffDegrees(5, 15);
		}

		myLightManager.UpdateBuffer();

		ObjectRenderer myObjectRenderer = ObjectRenderer();

		myObjectRenderer.AddObject(&sphere);
		myObjectRenderer.AddObject(&cube);
		myObjectRenderer.AddObject(&cylinder);
		myObjectRenderer.AddObject(&testobject);

		int debugmode = -1;
		bool debugpressedLastFrame = false;

		//Render Loop
		while (!glfwWindowShouldClose(window))
		{
			// Handle Inputs
			glfwPollEvents();

			myCameraController.HandleInputs(scrollOffset);
			if(glfwGetKey(window,GLFW_KEY_J) == GLFW_PRESS)
				testobject.GetTransform().rotation.y += 0.01f;
			if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS)
				testobject.GetTransform().rotation.x += 0.01f;
			if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS)
				testobject.GetTransform().rotation.z += 0.01f;
			if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS)
				testobject.GetTransform().rotation = glm::vec3();

			bool debugPressed = glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS; //getto solution for now
			if (debugPressed && !debugpressedLastFrame) {
				debugmode = (debugmode + 1) % 7;
				debugMaterial.SetPropertyi("mode", debugmode);
			}
			debugpressedLastFrame = debugPressed;

			//Clear
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			//Set ViewProjectionMatrix
			{
				glm::mat4 viewPerspective = perspective * myCameraTransform.ToInverseMatrix();
				glm::vec4 eyePos = glm::vec4(myCameraTransform.position,1);

				glBindBuffer(GL_UNIFORM_BUFFER, viewDataBuffer.GetHandle());
				glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(viewPerspective));
				glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::vec4), glm::value_ptr(eyePos));
				glBindBuffer(GL_UNIFORM_BUFFER, 0);
			}

			myObjectRenderer.Draw();

			//Flip Buffers
			glfwSwapBuffers(window);
			scrollOffset = 0;
		}

		//clean up before leaving scope
		glUseProgram(0);
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
			if (wireframeMode) glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			else glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			wireframeMode = !wireframeMode;
			break;
		case GLFW_KEY_F2:
			if (backfaceCulling) glDisable(GL_CULL_FACE);
			else glEnable(GL_CULL_FACE);
			backfaceCulling = !backfaceCulling;
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