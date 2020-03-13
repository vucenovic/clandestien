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
#include "FrameBuffer.h"
#include "Particles.h"
#include "Camera.h"

#include "Main.h"

#include <random>

#include <lua/lua.hpp>
//#define SOL_ALL_SAFETIES_ON 1
//#include <sol.hpp>

float scrollOffset = 0; // very ugly solution until I figure out something better Namely a proper inputmanager TODO
bool wireframeMode = false;
bool backfaceCulling = true;
bool debugDraw = false;
int debugDrawmode = 0;

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

	std::string window_title = "Clandestien";
	float FOV = 60;
	float nearPlane = 0.1;
	float farPlane = 100;

	//Heyo it works
	lua_State* L = luaL_newstate();

	luaL_dostring(L, "x=42");
	lua_getglobal(L, "x");
	lua_Number x = lua_tonumber(L, 1);
	std::cout << x << std::endl;
	lua_close(L);

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

	glViewport(0, 0, width, height);
	glClearColor(0, 0, 0, 1);
	glEnable(GL_CULL_FACE);
	glfwSwapInterval(vsync);

	glfwSetKeyCallback(window, MyKeyCallback);
	glfwSetScrollCallback(window, scroll_callback);

	{
		std::shared_ptr<ShaderProgram> standardShader,unifiedPBR, myShaderProgram, debugShader, particleShader, pp_demultAlpha, pp_gammaCorrect, pp_blur, pp_bloom;
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

		std::shared_ptr<Mesh> myCubeMesh = MeshBuilder::BoxFlatShaded(1.5f, 1.5f, 1.5f);
		std::shared_ptr<Mesh> myCylinderMesh = MeshBuilder::CylinderSplitShaded(1.3f, 1, 32);
		std::shared_ptr<Mesh> mySphereMesh = MeshBuilder::Sphere(1, 64, 32);
		std::shared_ptr<Mesh> myTestMesh = OBJLoader::LoadOBJ("res/models/monkey.obj");

		//Materials

		std::shared_ptr<Material> particlesMaterial = std::make_shared<Material>(particleShader);
		particlesMaterial->SetTexture(particleTex, 0);

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

		//Objects

		GameObject testobject = GameObject();
		testobject.mesh = myTestMesh.get();
		testobject.material = &tilesMaterial;
		testobject.GetTransform().SetPostion(glm::vec3(3, 1.5f, 0));

		GameObject sphere = GameObject();
		sphere.mesh = mySphereMesh.get();
		sphere.material = &tilesMaterial;
		sphere.GetTransform().SetPostion(glm::vec3(1.5f,-1,0));

		GameObject cube = GameObject();
		cube.mesh = myCubeMesh.get();
		cube.material = &woodMaterial;
		cube.GetTransform().SetPostion(glm::vec3(0, 1.5f, 0));

		GameObject cylinder = GameObject();
		cylinder.mesh = myCylinderMesh.get();
		cylinder.material = &tilesMaterial;
		cylinder.GetTransform().SetPostion(glm::vec3(-1.5f, -1, 0));

		//--------Uniform Buffers

		UniformBuffer viewDataBuffer = UniformBuffer(*standardShader, std::string("viewData"), { "projection","view" }, 2);
		viewDataBuffer.BindToPort(0);

		LightManager myLightManager = LightManager(*standardShader);
		myLightManager.BindToPort(1);

		{
			myLightManager.lightsUsed.point = 4;
			myLightManager.lightsUsed.directional = 1;
			myLightManager.lightsUsed.spot = 1;

			myLightManager.ambientLight = glm::vec3(1, 1, 1);

			myLightManager.pointLights[0].SetPosition(glm::vec3());
			myLightManager.pointLights[0].SetAttenuation(1, 0.4f, 0.1f);

			myLightManager.pointLights[3].SetPosition(glm::vec3(2.2f,1,0));
			myLightManager.pointLights[3].SetColor(50,0,0);
			myLightManager.pointLights[3].SetAttenuation(1, 1, 1);

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

		MasterRenderer myObjectRenderer = MasterRenderer();

		myObjectRenderer.AddObject(&sphere);
		myObjectRenderer.AddObject(&cube);
		myObjectRenderer.AddObject(&cylinder);
		myObjectRenderer.AddObject(&testobject);

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

		double lastFrameTime = 0;
		double nextSecond = 1;
		size_t frameCount = 0;

		//Render Loop
		while (!glfwWindowShouldClose(window))
		{
			double currentFrametime = glfwGetTime();
			float deltaTime = (float)(currentFrametime - lastFrameTime);
			lastFrameTime = currentFrametime;
			//DISPLAY FRAMES PER SECOND
			frameCount++;
			if (currentFrametime >= nextSecond) {
				std::cout << frameCount << "FPS" << std::endl;
				frameCount = 0;
				nextSecond = currentFrametime + 1;
			}

			// Handle Inputs
			glfwPollEvents();

			myCameraController.HandleInputs(scrollOffset);
			testobject.GetTransform().Rotate((glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS) * 0.01f,(glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS) * 0.01f,(glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS) * 0.01f);
			if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS)
				testobject.GetTransform().SetRotation(glm::vec3());

			//Set ViewProjectionMatrix
			camera.UseCamera(viewDataBuffer);

			renderFBO.Bind();
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			//glEnable(GL_MULTISAMPLE);
			glEnable(GL_DEPTH_TEST);
			
			{
				pSystem.Update(deltaTime);
				glBindBuffer(GL_ARRAY_BUFFER, pmmanager.GetBufferHandle());
				void * buf = glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
				glm::vec3 forward = camera.GetForward();
				pSystem.SortParticles(forward); // sort particles that use alpha blending instead of additive blending
				pSystem.WriteMesh((Particle*)buf, 0, 150);
				glUnmapBuffer(GL_ARRAY_BUFFER);
				glBindBuffer(GL_ARRAY_BUFFER, 0);
			}

			if (debugDraw) {
				debugMaterial.SetPropertyi("mode", debugDrawmode);
				myObjectRenderer.DrawOverrideMaterial(debugMaterial);
			}
			else {
				myObjectRenderer.Draw();
			}

			//Draw WorldPortals between Opaque phase and transparent phase
			//use stencil buffer for masking drawing and an oblique projection

			//render portal to stencil buffer
			//render screen quad with only stencil testing and write depth to be the far plane (to reset depth buffer in the portal)
			//rerender scene from other perspective with stencil and depth testing
			//?
			//profit

			/*
				//rough scematic
				renderScene(drawPortals){
					renderOpaque();
					if(drawPortals){
						renderPortal(){
							drawHoldoutToScreen();
							setViewParameters();
							renderScene(false);
						};
					}
					renderTransparents();
				}
			*/

			{ //TODO move into particle system class and particle sytemrenderer
				ParticleSystem::PrepareDraw();
				pmmanager.Bind();
				//pSystem.Draw();
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

				glActiveTexture(GL_TEXTURE0);
				glBindVertexArray(ssplaneVAO);

				//Demultipy alpha values
				ppFBO.Bind();
				pp_demultAlpha->UseProgram();
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
				glActiveTexture(GL_TEXTURE1);
				glBindTexture(GL_TEXTURE_2D, ppFBOb.color);
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, ppFBO.color);

				glUniform1i(pp_bloom->GetUniformLocation("combine"), 1);
				glUniform1f(pp_bloom->GetUniformLocation("exposure"), 1);
				glUniform1f(pp_bloom->GetUniformLocation("gamma"), gamma);
				glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
			}

			//Reset Renderstate
			if (wireframeMode) glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			else glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			if (backfaceCulling) glEnable(GL_CULL_FACE);
			else glDisable(GL_CULL_FACE);

			//Flip Buffers
			glfwSwapBuffers(window);
			glFlush();
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
		case GLFW_KEY_1:
			debugDrawmode = (debugDrawmode + 1) % 7;
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