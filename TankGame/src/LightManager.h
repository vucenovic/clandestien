#pragma once
#include "Lights.h"
#include <GL/glew.h>
#include "UniformBuffer.h"
#include "ShaderProgram.h"
#include <glm/glm.hpp>
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
	bool shadowLightUsed = false;

	const LightCounts & getMaximums() const;
};