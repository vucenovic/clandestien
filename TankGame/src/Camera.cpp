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

glm::mat4 Camera::MakeOblique(glm::mat4 mat, glm::vec4 clipPlane)
{
	return mat;
}

glm::vec4 Camera::toLocalClipplane(glm::vec4 clip) const
{
	glm::vec3 worldNormal = glm::vec3(clip.x, clip.y, clip.z);
	glm::vec3 worldPos = worldNormal * clip.w;

	glm::mat4 mat = transform.ToInverseMatrix();

	glm::vec4 cm4 = (mat * glm::vec4(worldNormal, 0));
	glm::vec3 camNormal = glm::vec3(cm4.x, cm4.y, cm4.z);
	glm::vec4 cp4 = mat * glm::vec4(worldPos, 1);
	glm::vec3 camPos = glm::vec3(cp4.x, cp4.y, cp4.z);
	float camDistance = glm::dot(camPos, camNormal);

	return glm::vec4(camNormal.x, camNormal.y, camNormal.z, camDistance);
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
