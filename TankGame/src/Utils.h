#pragma once
#include <GL\glew.h>
#include <glm\glm.hpp>
#include <glm\gtx\quaternion.hpp>
#include <vector>

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
private:
	mutable bool dirtyBit;
	mutable glm::mat4 cachedMatrix;

	glm::vec3 position;
	glm::quat rotation;
	glm::vec3 scale;

	Transform * parent;
	std::vector<Transform *> children;
public:
	void setDirty() {
		if (!dirtyBit) {
			for (Transform * t : children)
			{
				t->setDirty();
			}
		}
		dirtyBit = true;
	}

	Transform();
	Transform(glm::vec3 pos, glm::vec3 rot, glm::vec3 scale);
	~Transform();

	inline void SetRotationDegrees(float x, float y, float z) {
		SetRotation(glm::vec3(glm::radians(x), glm::radians(y), glm::radians(z)));
	}
	inline void SetRotation(glm::vec3 r) {
		rotation = glm::quat(r);
		setDirty();
	}
	inline void SetPostion(glm::vec3 p) {
		position = p;
		setDirty();
	}
	inline void SetScale(glm::vec3 s) {
		scale = s;
		setDirty();
	}

	inline const glm::vec3 & GetPosition() const { return position; }
	inline const glm::vec3 & GetScale() const { return scale; }
	inline const glm::quat & GetRotation() const { return rotation; }
	inline const glm::vec3 GetRotationEuler() const { return glm::eulerAngles(rotation); }

	void Translate(const glm::vec3 & dp) {
		position += dp;
		setDirty();
	}
	void Rotate(const glm::vec3 & dr) {
		rotation *= glm::quat(dr);
		setDirty();
	}
	void Rotate(float rx,float ry,float rz) {
		Rotate(glm::vec3(rx, ry, rz));
	}

	const glm::mat4 & ToMatrix() const;
	const glm::mat4 ToInverseMatrix() const { return glm::inverse(ToMatrix()); }
	const glm::mat4 ToNormalMatrix() const { return glm::transpose(ToInverseMatrix()); }
	
	glm::vec3 GetForward() const {
		glm::vec4 dir = glm::vec4(0, 0, -scale.z, 0);
		dir = ToMatrix() * dir;
		return glm::vec3(dir.x, dir.y, dir.z);
	}
	glm::vec3 GetRight() const {
		return glm::cross(glm::vec3(0, 1, 0), GetForward());
	}

	void LookAt(const glm::vec3 eye, const glm::vec3 pos);
	void LookDir(const glm::vec3 dir);
};