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
	// -position -> -rotation -> rotate by 180° -> targetRotation -> targetPosition
	return glm::mat4(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, posTarget.x, posTarget.y, posTarget.z, 1) * glm::toMat4(targetTransform.GetRotation()) * glm::mat4(-1, 0, 0, 0, 0, 1, 0, 0, 0, 0, -1, 0, 0, 0, 0, 1) * glm::toMat4(glm::inverse(transform.GetRotation())) * glm::mat4(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, pos.x, pos.y, pos.z, 1);
}

glm::vec4 Portal::getClipPlane() const
{
	targetTransform.GetRotation();
	return glm::vec4();
}
