#include "Portal.h"

Portal::Portal()
{
}

Portal::~Portal()
{
}

glm::mat4 Portal::getOffsetMatrix() const
{
	glm::vec3 pos = -transform.GetPosition();
	glm::vec3 posTarget = targetTransform.GetPosition();
	// -position -> -rotation -> reflect -> targetRotation -> targetPosition
	return glm::mat4(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, posTarget.x, posTarget.y, posTarget.z, 1) * glm::toMat4(targetTransform.GetRotation()) * glm::mat4(-1, 0, 0, 0, 0, 1, 0, 0, 0, 0, -1, 0, 0, 0, 0, 1) * glm::toMat4(glm::inverse(transform.GetRotation())) * glm::mat4(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, pos.x, pos.y, pos.z, 1);
}

glm::vec4 Portal::getClipPlane() const
{
	glm::vec3 normal = targetTransform.GetRotation() * glm::vec3(0,0,1);
	float distance = glm::dot(normal, targetTransform.GetPosition());
	return glm::vec4(normal,distance);
}
