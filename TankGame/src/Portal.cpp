#include "Portal.h"

Portal::Portal()
{
}

Portal::~Portal()
{
}

glm::mat4 Portal::getOffsetMatrix() const
{
	return glm::mat4();
}

glm::vec4 Portal::getClipPlane() const
{
	return glm::vec4();
}
