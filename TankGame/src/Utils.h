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

class Transform //TODO maybe cache matrix
{
private:
	mutable bool dirtyBit;
	mutable glm::mat4 cachedMatrix;

	glm::vec3 position;
	glm::vec3 rotation;
	glm::vec3 scale;
public:

	Transform * parent;

	Transform();
	Transform(glm::vec3 pos, glm::vec3 rot, glm::vec3 scale);
	~Transform();

	inline void SetRotationDegrees(float x, float y, float z) {
		rotation.x = glm::radians(x);
		rotation.y = glm::radians(y);
		rotation.z = glm::radians(z);
		dirtyBit = true;
	}
	inline void SetRotation(glm::vec3 r) {
		rotation = r;
		dirtyBit = true;
	}
	inline void SetPostion(glm::vec3 p) {
		position = p;
		dirtyBit = true;
	}
	inline void SetScale(glm::vec3 s) {
		scale = s;
		dirtyBit = true;
	}

	inline const glm::vec3 & GetPosition() const { return position; }
	inline const glm::vec3 & GetScale() const { return scale; }
	inline const glm::vec3 & GetRotation() const { return rotation; }

	void Translate(glm::vec3 dp) {
		position += dp;
		dirtyBit = true;
	}
	void Rotate(glm::vec3 dr) {
		rotation += dr;
		dirtyBit = true;
	}
	void Rotate(float rx,float ry,float rz) {
		rotation.x += rx;
		rotation.y += ry;
		rotation.z += rz;
		dirtyBit = true;
	}

	static glm::mat4 BuildRotationMatrix(const glm::vec3 rot);
	const glm::mat4 & ToMatrix() const;
	const glm::mat4 ToInverseMatrix() const { return glm::inverse(ToMatrix()); }
	const glm::mat4 ToNormalMatrix() const { return glm::transpose(ToInverseMatrix()); }
};