#pragma once

#include <GL\glew.h>

#include "Utils.h"
#include "UniformBuffer.h"

class Camera
{
private:
	Transform transform;
	glm::mat4 projectionMatrix;

public:
	Transform & GetTransform() { return transform; }
	void SetProjectionMatrix(glm::mat4 mat);
	
	void SetPerspective(float FOV, float aspect, float near, float far);
	void SetOrtho(float left, float right, float top, float bottom);

	glm::vec3 GetForward() const { return transform.ToMatrix()*glm::vec4(0, 0, 1, 0); }

	void UseCamera(const UniformBuffer & viewDataBuffer);

	//virtual const GameObjectType & GetType() const override { return GameObjectType::Camera; }
};
