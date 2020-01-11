#include "MyUtils.h"
#include <sstream>
#include <fstream>
#include <glm/gtc/type_ptr.hpp>

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

void Transform::SetRotationDegrees(float x, float y, float z)
{
	rotation.x = glm::radians(x);
	rotation.y = glm::radians(y);
	rotation.z = glm::radians(z);
}

glm::mat4 Transform::BuildRotationMatrix(const glm::vec3 rot)
{
	float cx = glm::cos(rot.x);
	float sx = glm::sin(rot.x);
	float cy = glm::cos(-rot.y);
	float sy = glm::sin(-rot.y);
	float cz = glm::cos(rot.z);
	float sz = glm::sin(rot.z);
	float szx = sx * sz;
	float czsx = cz * sx;
	return glm::mat4(
		cz*cy - szx * sy, sz*cx, szx*cy + cz * sy, 0,
		-szx - czsx * sy, cz*cx, czsx*cy - sz * sy, 0,
		-cx * sy, -sx, cx*cy, 0,
		0, 0, 0, 1
	);
}

glm::mat4 Transform::ToMatrix() const
{
	float cx = glm::cos(rotation.x);
	float sx = glm::sin(rotation.x);
	float cy = glm::cos(-rotation.y);
	float sy = glm::sin(-rotation.y);
	float cz = glm::cos(rotation.z);
	float sz = glm::sin(rotation.z);
	float szx = sx * sz;
	float czsx = cz * sx;
	glm::mat4 ret = glm::mat4(
		(cz*cy - szx * sy) * scale.x, sz*cx * scale.x, (szx*cy + cz * sy) * scale.x, 0,
		(-szx - czsx * sy) * scale.y, cz*cx * scale.y, (czsx*cy - sz * sy) * scale.y, 0,
		-cx * sy * scale.z, -sx * scale.z, cx * cy * scale.z, 0,
		position.x, position.y, position.z, 1
	);
	if (parent == nullptr) {
		return ret;
	}
	else {
		return parent->ToMatrix() * ret;
	}
}

glm::mat4 Transform::ToInverseMatrix() const
{
	return glm::inverse(ToMatrix()); //TODO: Replace with algebraic solution
}

glm::mat4 Transform::ToNormalMatrix() const
{
	return glm::transpose(ToInverseMatrix()); //TODO: Replace with algebraic solution
}

const GLfloat * Transform::ToMatrixBuffer()
{
	return glm::value_ptr(ToMatrix());
}
