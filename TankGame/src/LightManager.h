#pragma once
#include "Lights.h"
#include <GL/glew.h>
#include "UniformBuffer.h"
#include "ShaderProgram.h"
#include <glm/glm.hpp>

//specific to the current implementation
class LightManager
{
public:
	struct LightCounts { GLuint point, directional, spot; };
private:
	LightCounts lightMaxes;
	
	UniformBuffer * lightDataBuffer;
public:
	LightManager(const ShaderProgram & program);
	~LightManager();

	void BindToPort(GLuint port);

	void UpdateBuffer();

	LightCounts lightsUsed;
	glm::vec3 ambientLight;
	PointLight * pointLights;
	DirectionalLight * directionalLights;
	SpotLight * spotLights;

	const LightCounts & getMaximums() const;
};