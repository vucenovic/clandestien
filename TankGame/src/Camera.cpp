#include "Camera.h"

#include <glm/gtc/type_ptr.hpp>

void Camera::SetProjectionMatrix(glm::mat4 mat)
{
	projectionMatrix = mat;
}

void Camera::SetPerspective(float FOV, float aspect, float near, float far)
{
	SetProjectionMatrix(glm::perspective(glm::radians(FOV), aspect, near, far));
}

void Camera::SetOrtho(float left, float right, float top, float bottom)
{
	SetProjectionMatrix(glm::ortho(left,right,top,bottom));
}

void Camera::UseCamera(const UniformBuffer & viewDataBuffer)
{
	glm::mat4 view = transform.ToInverseMatrix();
	SetViewParameters(viewDataBuffer, view, projectionMatrix);
}

void Camera::SetViewParameters(const UniformBuffer & viewDataBuffer, const glm::mat4 & view, const glm::mat4 & projection)
{
	glBindBuffer(GL_UNIFORM_BUFFER, viewDataBuffer.GetHandle());
	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(projection));
	glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(view));
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}
