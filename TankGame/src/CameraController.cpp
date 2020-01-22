#include "CameraController.h"

void CameraController::HandleInputs(const float &scrolloffset)
{
	double xd, yd;
	glfwGetCursorPos(window,&xd,&yd);
	float x = (float)xd, y = (float)yd;

	float dx = x - lastX, dy = y - lastY;

	//Handle Panning
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT)) {
		pivotYaw += dx * horizontalSensitivity;
		pivotPitch += dy * verticalSensitivity;
		
		if (pivotPitch >= 90) pivotPitch = 89.95f;
		else if (pivotPitch <= -90) pivotPitch = -89.95f;

		if (pivotYaw > 360) pivotYaw -= 360;
		else if (pivotYaw < -360) pivotYaw += 360;
	}
	//Set Rotation
	cameraTransform->SetRotationDegrees(pivotPitch, pivotYaw, 0);
	glm::mat3 rotmatrix = Transform::BuildRotationMatrix(cameraTransform->GetRotation());

	glm::vec3 forwardVector = rotmatrix * glm::vec3(0, 0, 1);
	glm::vec3 rightVector = rotmatrix * glm::vec3(1, 0, 0);
	glm::vec3 upVector = glm::cross(rightVector, forwardVector);

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

	strafeSpeed = -0.001f;
}

CameraController::~CameraController()
{
}
