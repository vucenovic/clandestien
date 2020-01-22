#include "Utils.h"
#include <sstream>
#include <fstream>
#include <glm/gtc/type_ptr.hpp>
#include <limits>

std::string loadFileAsString(std::string filepath)
{
	std::ifstream t(filepath);
	std::stringstream buffer;
	buffer << t.rdbuf();
	return buffer.str();
}

Transform::Transform() : position(glm::vec3(0, 0, 0)), rotation(glm::vec3(0, 0, 0)), scale(glm::vec3(1, 1, 1)), parent(nullptr)
{
}

Transform::Transform(glm::vec3 pos, glm::vec3 rot, glm::vec3 scl) : position(pos), rotation(rot), scale(scl)
{
}

Transform::~Transform()
{
}

glm::mat4 Transform::BuildRotationMatrix(const glm::vec3 rot)
{
	float cx = glm::cos(rot.x);
	float sx = glm::sin(rot.x);
	float cy = glm::cos(-rot.y);
	float sy = glm::sin(-rot.y);
	float cz = glm::cos(rot.z);
	float sz = glm::sin(rot.z);
	return glm::mat4(
		cz * cy + sx * sz * sy, -sz * cx, cz * sy - sx * sz * cy, 0,
		cy * sz - sy * sx * cz, cz*cx, (cz * sx * cy + sz * sy), 0,
		-cx * sy, -sx, cx * cy, 0,
		0, 0, 0, 1
	);
}

const glm::mat4 & Transform::ToMatrix() const
{
	if (dirtyBit) {
		float cx = glm::cos(rotation.x);
		float sx = glm::sin(rotation.x);
		float cy = glm::cos(-rotation.y);
		float sy = glm::sin(-rotation.y);
		float cz = glm::cos(rotation.z);
		float sz = glm::sin(rotation.z);
		cachedMatrix = glm::mat4(
			(cz*cy + sx * sz * sy) * scale.x,		-sz * cx * scale.x,		(cz * sy - sx * sz * cy) * scale.x, 0,
			(cy * sz - sy * sx * cz) * scale.y,		cz*cx * scale.y,		(cz * sx * cy + sz * sy) * scale.y, 0,
			-cx * sy * scale.z,						-sx * scale.z,			cx * cy * scale.z, 0,
			position.x, position.y, position.z, 1
		);
		dirtyBit = false;
	}
	if (parent == nullptr) {
		return cachedMatrix;
	}
	else {
		return parent->ToMatrix() * cachedMatrix; //need a way to message dirtying to allow caching of combined matrices
	}
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
