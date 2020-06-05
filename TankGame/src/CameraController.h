#pragma once
#include <GL\glew.h>
#include <GLFW\glfw3.h>
#include "Utils.h"

class CameraController
{
private:
	float lastX = 0, lastY = 0;
public:
	GLFWwindow* window;
	Transform* cameraTransform;
	glm::vec3 position = glm::vec3();
	float yaw = 0;
	float pitch = 0;

	float horizontalSensitivity = -0.25f;
	float verticalSensitivity = -0.25f;

	void HandleInputs();

	CameraController(Transform* cameraTransform, GLFWwindow* window) : window(window), cameraTransform(cameraTransform) {};
	~CameraController() {};
};
