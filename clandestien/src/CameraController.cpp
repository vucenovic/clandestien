#include "CameraController.h"
#include <iostream>

void CameraController::HandleInputs()
{
	double xd, yd;
	glfwGetCursorPos(window,&xd,&yd);
	float x = (float)xd, y = (float)yd;

	float dx = x - lastX, dy = y - lastY;

	// TODO: first person instead of orbit
	yaw += dx * horizontalSensitivity;
	pitch += dy * verticalSensitivity;
		
	if (pitch >= 90) pitch = 89.95f;
	else if (pitch <= -90) pitch = -89.95f;

	if (yaw > 360) yaw -= 360;
	else if (yaw < -360) yaw += 360;
	//Set Rotation
	cameraTransform.GetTransform().SetRotationDegrees(pitch, yaw, 0);

	lastX = x;
	lastY = y;
}