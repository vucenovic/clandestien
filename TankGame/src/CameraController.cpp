#include "CameraController.h"
#include <iostream>

void CameraController::HandleInputs(const float &scrolloffset, char forward, char backward, char left, char right, float frametime)
{
	double xd, yd;
	glfwGetCursorPos(window,&xd,&yd);
	float x = (float)xd, y = (float)yd;

	glm::vec3 position = glm::vec3(0.0, 0.0, 0.0);

	float dx = x - lastX, dy = y - lastY;

	float horizontalAngle = 3.14f;
	float verticalAngle = 0.0f;

	horizontalAngle += horizontalSensitivity * frametime * float(800 / 2 - xd);
	verticalAngle += verticalSensitivity * frametime * float(800 / 2 - yd);

	glm::vec3 direction(
		cos(verticalAngle) * sin(horizontalAngle),
		sin(verticalAngle),
		cos(verticalAngle) * cos(horizontalAngle)
	);

	glm::vec3 r = glm::vec3(
		sin(horizontalAngle - 3.14f / 2.0f),
		0,
		cos(horizontalAngle - 3.14f / 2.0f)
	);

	glm::vec3 up = glm::cross(r, direction);
	// Move forward
	if (glfwGetKey(window, MapKeys(forward)) == GLFW_PRESS) {
		std::cout << "W pressed";
		position += direction * frametime * moveSpeed;
	}
	// Move backward
	if (glfwGetKey(window, MapKeys(backward)) == GLFW_PRESS) {
		position -= direction * frametime * moveSpeed;
	}
	// Strafe right
	if (glfwGetKey(window, MapKeys(right)) == GLFW_PRESS) {
		position += r * frametime * strafeSpeed;
	}
	// Strafe left
	if (glfwGetKey(window, MapKeys(left)) == GLFW_PRESS) {
		position -= r * frametime * strafeSpeed;
	}

	cameraTransform->Translate(position);

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
	glm::mat3 rotmatrix = glm::toMat4(cameraTransform->GetRotation());

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

int CameraController::MapKeys(char key)
{
	switch (key) {
		case 'A':
			return GLFW_KEY_A;
		case 'B':
			return GLFW_KEY_B;
		case 'C':
			return GLFW_KEY_C;
		case 'D':
			return GLFW_KEY_D;
		case 'E':
			return GLFW_KEY_E;
		case 'F':
			return GLFW_KEY_F;
		case 'G':
			return GLFW_KEY_G;
		case 'H':
			return GLFW_KEY_H;
		case 'I':
			return GLFW_KEY_I;
		case 'J':
			return GLFW_KEY_K;
		case 'L':
			return GLFW_KEY_L;
		case 'M':
			return GLFW_KEY_M;
		case 'N':
			return GLFW_KEY_N;
		case 'O':
			return GLFW_KEY_O;
		case 'P':
			return GLFW_KEY_P;
		case 'Q':
			return GLFW_KEY_Q;
		case 'R':
			return GLFW_KEY_R;
		case 'S':
			return GLFW_KEY_T;
		case 'U':
			return GLFW_KEY_V;
		case 'W':
			return GLFW_KEY_W;
		case 'X':
			return GLFW_KEY_X;
		case 'Y':
			return GLFW_KEY_Y;
		case 'Z':
			return GLFW_KEY_Z;
		case 'UP':
			return GLFW_KEY_UP;
		case 'DOWN':
			return GLFW_KEY_DOWN;
	}
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
	moveSpeed = -0.01f;
}

CameraController::~CameraController()
{
}
