#pragma once
#include <GL\glew.h>
#include <glm\glm.hpp>
#include <string>

std::string loadFileAsString(std::string filepath);

template <typename T>
inline T sgn(T x) {	return (x > 0) - (x < 0); }

class BoundingBox
{
public:
	glm::vec3 min;
	glm::vec3 max;

	BoundingBox() : min(0), max(0) {};
	BoundingBox(glm::vec3 min, glm::vec3 max) : min(min), max(max) {};
	BoundingBox(const GLfloat * points, const size_t & count, size_t stride = 3);

	std::pair<glm::vec3,glm::vec3> AlongAxis(glm::vec3 axis);
};

class Transform
{
public:
	glm::vec3 position;
	glm::vec3 rotation;
	glm::vec3 scale;

	Transform * parent;

	Transform();
	Transform(glm::vec3 pos, glm::vec3 rot, glm::vec3 scale);
	~Transform();

	void SetRotationDegrees(float x, float y, float z);

	static glm::mat4 BuildRotationMatrix(const glm::vec3 rot);
	glm::mat4 ToMatrix() const;
	glm::mat4 ToInverseMatrix() const;
	glm::mat4 ToNormalMatrix() const;
};