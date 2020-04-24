#include "Utils.h"
#include <glm/gtc/type_ptr.hpp>
#include <limits>

Transform::Transform() : position(glm::vec3(0, 0, 0)), rotation(glm::vec3(0, 0, 0)), scale(glm::vec3(1, 1, 1)), parent(nullptr), dirtyBit(true)
{
}

Transform::Transform(glm::vec3 pos, glm::vec3 rot, glm::vec3 scl) : position(pos), rotation(rot), scale(scl), dirtyBit(true)
{
}

Transform::~Transform()
{
}

const glm::mat4 & Transform::ToMatrix() const
{
	if (dirtyBit) {
		cachedMatrix = glm::mat4(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, position.x, position.y, position.z, 1) * glm::toMat4(rotation) * glm::mat4(scale.x, 0, 0, 0, 0, scale.y, 0, 0, 0, 0, scale.z, 0, 0, 0, 0, 1);
		if (parent) {
			cachedMatrix = parent->ToMatrix() * cachedMatrix;
		}
		dirtyBit = false;
	}
	return cachedMatrix;
}

BoundingBox::BoundingBox(const GLfloat * points, const size_t & count, size_t stride) : min(std::numeric_limits<float>::max()), max(std::numeric_limits<float>::lowest())
{
	size_t len = count * stride;
	for (size_t i = 0; i < len; i += stride) {
		glm::vec3 * p = (glm::vec3*)(points + i);
		min.x = min.x > p->x ? p->x : min.x;
		min.y = min.y > p->y ? p->y : min.y;
		min.z = min.z > p->z ? p->z : min.z;

		max.x = max.x < p->x ? p->x : max.x;
		max.y = max.y < p->y ? p->y : max.y;
		max.z = max.z < p->z ? p->z : max.z;
	}
}

std::pair<glm::vec3, glm::vec3> BoundingBox::AlongAxis(glm::vec3 axis)
{
	glm::vec3 low, high;
	low.x = axis.x >= 0 ? max.x : min.x;
	low.y = axis.y >= 0 ? max.y : min.y;
	low.z = axis.z >= 0 ? max.z : min.z;

	high.x = axis.x < 0 ? max.x : min.x;
	high.y = axis.y < 0 ? max.y : min.y;
	high.z = axis.z < 0 ? max.z : min.z;
	return std::pair<glm::vec3, glm::vec3>(low,high);
}
