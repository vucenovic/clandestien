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
	float moveSpeed;
	float horizontalAngle;
	float verticalAngle;

	float horizontalSensitivity, verticalSensitivity, scrollSensitivity;

	void HandleInputs(const float &scrollOffset, char forward, char backward, char left, char right, float frametime);
	int MapKeys(char key);

	CameraController(Transform* cameraTransform, GLFWwindow* window);
	~CameraController();
};