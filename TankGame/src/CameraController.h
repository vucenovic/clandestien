#pragma once
#include <GL\glew.h>
#include <GLFW\glfw3.h>
#include "Utils.h"

class CameraController
{
private:
	Transform* cameraTransform;
	GLFWwindow* window;

	float lastX, lastY;

public:
	glm::vec3 pivotPostion;
	float pivotRadius;
	float pivotYaw;
	float pivotPitch;
	float strafeSpeed;

	float horizontalSensitivity, verticalSensitivity, scrollSensitivity;

	void HandleInputs(const float &scrollOffset);

	CameraController(Transform* cameraTransform, GLFWwindow* window);
	~CameraController();
};