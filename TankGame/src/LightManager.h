#pragma once
#include "Lights.h"
#include <GL/glew.h>
#include "UniformBuffer.h"
#include "ShaderProgram.h"
#include <glm/glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <memory>
#include <vector>

//specific to the current implementation
class LightManager
{
public:
	struct LightCounts { GLuint point, directional, spot, shadow; };
private:
	LightCounts lightMaxes;
	
	UniformBuffer * lightDataBuffer;
public:
	LightManager(const ShaderProgram & program);
	~LightManager();

	void BindToPort(GLuint port);

	void UpdateBuffer();

	glm::vec3 ambientLight;
	std::vector<PointLight> pointLights;
	std::vector<DirectionalLight> directionalLights;
	std::vector<SpotLight> spotLights;
	SpotLight shadowLight;
	glm::mat4 shadowView() {
		return glm::lookAt(shadowLight.position, shadowLight.position + shadowLight.direction, glm::vec3(0, 1, 0));
	};
	glm::mat4 shadowProjection() {
		const glm::mat4 biasMatrix(
			1, 0.0, 0.0, 0.0,
			0.0, 1, 0.0, 0.0,
			0.0, 0.0, 1, 0.0,
			0.05, 0.05, 0.05, 1.0
		);
		return biasMatrix * glm::perspective<float>(glm::radians(45.0f), 1.0f, 2.0f, 50.0f);
	};
	bool shadowLightUsed = false;

	const LightCounts & getMaximums() const;
};