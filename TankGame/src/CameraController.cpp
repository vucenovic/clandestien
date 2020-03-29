#include "CameraController.h"
#include <iostream>

void CameraController::HandleInputs(const float &scrolloffset, char forward, char backward, char left, char right, float frametime)
{
	double xd, yd;
	glfwGetCursorPos(window,&xd,&yd);
	float x = (float)xd, y = (float)yd;

	float dx = x - lastX, dy = y - lastY;

	horizontalAngle += horizontalSensitivity * frametime * float(800 / 2 - xd);
	verticalAngle += verticalSensitivity * frametime * float(800 / 2 - yd);

	glm::mat3 rotmatrix = glm::toMat4(cameraTransform->GetRotation());

	glm::vec3 forwardVector = rotmatrix * glm::vec3(0, 0, 1);
	glm::vec3 rightVector = rotmatrix * glm::vec3(1, 0, 0);
	glm::vec3 upVector = glm::cross(rightVector, forwardVector);

	// Move forward
	if (glfwGetKey(window, (int)forward) == GLFW_PRESS) {
		pivotPostion[2] += forwardVector[2] * frametime * moveSpeed;
		pivotPostion[0] += forwardVector[0] * frametime * moveSpeed;
	}
	// Move backward
	if (glfwGetKey(window, (int)backward) == GLFW_PRESS) {
		pivotPostion[2] -= forwardVector[2] * frametime * moveSpeed;
		pivotPostion[0] -= forwardVector[0] * frametime * moveSpeed;
	}
	// Strafe right
	if (glfwGetKey(window, (int)right) == GLFW_PRESS) {
		pivotPostion[2] -= rightVector[2] * frametime * moveSpeed;
		pivotPostion[0] -= rightVector[0] * frametime * moveSpeed;
	}
	// Strafe left
	if (glfwGetKey(window, (int)left) == GLFW_PRESS) {
		pivotPostion[2] += rightVector[2] * frametime * moveSpeed;
		pivotPostion[0] += rightVector[0] * frametime * moveSpeed;
	}

	pivotYaw += dx * horizontalSensitivity;
	pivotPitch += dy * verticalSensitivity;
		
	if (pivotPitch >= 90) pivotPitch = 89.95f;
	else if (pivotPitch <= -90) pivotPitch = -89.95f;

	if (pivotYaw > 360) pivotYaw -= 360;
	else if (pivotYaw < -360) pivotYaw += 360;
	//Set Rotation
	 cameraTransform->SetRotationDegrees(pivotPitch, pivotYaw, 0);

	//Handle Strafing
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT)) {
		pivotPostion += (upVector * dy + rightVector * dx) * strafeSpeed * pivotRadius;
	}

	pivotRadius += scrollSensitivity * scrolloffset;
	if (pivotRadius < 0) pivotRadius = 0;

	//set Combined Position
	cameraTransform->SetPostion(pivotPostion + forwardVector * pivotRadius);

	lastX = x;
	lastY = y;
}


CameraController::CameraController(Transform* cameraTransform, GLFWwindow* window)
{
	this->window = window;
	this->cameraTransform = cameraTransform;

	pivotRadius = 6;
	pivotYaw = 0;
	pivotPitch = 0;
	horizontalSensitivity = -0.25;
	verticalSensitivity = -0.25;
	scrollSensitivity = -0.25;

	horizontalAngle = 3.14f;
	verticalAngle = 0.0f;

	strafeSpeed = -0.008f;
	moveSpeed = -0.008f;
}

CameraController::~CameraController()
{
}
