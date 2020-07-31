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

glm::mat4 Camera::GetObliqueProjection(float FOV, float aspect, float near, float far, glm::vec4 C)
{
	glm::mat4 proj = glm::perspective(glm::radians(FOV), aspect, near, far);
	glm::vec4 Q = glm::inverse(proj) * glm::vec4(glm::sign(C.x), glm::sign(C.y), 1, 1);
	glm::vec4 M3 = ((-2 * Q.z) / glm::dot(C, Q))*C + glm::vec4(0, 0, 1, 0);
	proj = glm::transpose(proj);
	proj[2] = M3;
	return glm::transpose(proj);
}

glm::mat4 Camera::GetObliqueProjection(glm::vec4 C)
{
	glm::mat4 proj = projectionMatrix;
	glm::vec4 Q = glm::inverse(proj) * glm::vec4(glm::sign(C.x), glm::sign(C.y), 1, 1);
	glm::vec4 M3 = ((-2 * Q.z) / glm::dot(C, Q))*C + glm::vec4(0, 0, 1, 0);
	proj = glm::transpose(proj);
	proj[2] = M3;
	return glm::transpose(proj);
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
