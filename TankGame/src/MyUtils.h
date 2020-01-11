#pragma once
#include <GL\glew.h>
#include <glm\glm.hpp>
#include <string>

std::string loadFileAsString(std::string filepath);
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
	const GLfloat * ToMatrixBuffer();//doesn't work right now as the matrix goes out of scope and the pointer goes bad
};