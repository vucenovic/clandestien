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

	static glm::mat4 Camera::GetObliqueProjection(float FOV, float aspect, float near, float far, glm::vec4 clipPlane);
	//Only works with projection matrix
	glm::mat4 Camera::GetObliqueProjection(glm::vec4 clipPlane);
	glm::vec4 toLocalClipplane(glm::vec4 clip) const { return glm::transpose(transform.ToMatrix()) * clip; };

	void UseCamera(const UniformBuffer & viewDataBuffer);
	static void SetViewParameters(const UniformBuffer & viewDataBuffer, const glm::mat4 & view, const glm::mat4 & projection);

	const glm::mat4 & getViewMatrix() const { return transform.ToInverseMatrix(); }
	const glm::mat4 & getProjectionMatrix() const { return projectionMatrix; }

	//virtual const GameObjectType & GetType() const override { return GameObjectType::Camera; }
};
