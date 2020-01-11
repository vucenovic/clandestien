#include "LightManager.h"
#include <glm/gtc/type_ptr.hpp>

LightManager::LightManager(const ShaderProgram & program)
{
	lightDataBuffer = new UniformBuffer(program, std::string("LightData"), { "LightData.ambientColor", "LightData.lightCounts","LightData.pointLights[0].position","LightData.directionalLights[0].direction","LightData.spotLights[0].position" }, 5);

	const std::vector<GLuint> & offsets = lightDataBuffer->GetOffsets();

	lightMaxes.point = (offsets[3] - offsets[2]) / sizeof(PointLight);
	lightMaxes.directional = (offsets[4] - offsets[3]) / sizeof(DirectionalLight);
	lightMaxes.spot = (lightDataBuffer->GetBlockSize() - offsets[4]) / sizeof(SpotLight);
	ambientLight = glm::vec3();
	pointLights = new PointLight[lightMaxes.point];
	directionalLights = new DirectionalLight[lightMaxes.directional];
	spotLights = new SpotLight[lightMaxes.spot];
}

LightManager::~LightManager()
{
	delete[] pointLights;
	delete[] directionalLights;
	delete[] spotLights;
	delete lightDataBuffer;
}

void LightManager::BindToPort(GLuint port)
{
	lightDataBuffer->BindToPort(port);
}

void LightManager::UpdateBuffer()
{
	const std::vector<GLuint> & offsets = lightDataBuffer->GetOffsets();

	glBindBuffer(GL_UNIFORM_BUFFER, lightDataBuffer->GetHandle());
	glBufferSubData(GL_UNIFORM_BUFFER, offsets[0], sizeof(glm::vec3), glm::value_ptr(ambientLight));
	glBufferSubData(GL_UNIFORM_BUFFER, offsets[1], sizeof(LightManager::LightCounts), &lightsUsed);

	glBufferSubData(GL_UNIFORM_BUFFER, offsets[2], sizeof(PointLight) * lightsUsed.point, (GLfloat *)pointLights);
	glBufferSubData(GL_UNIFORM_BUFFER, offsets[3], sizeof(DirectionalLight) * lightsUsed.directional, (GLfloat *)directionalLights);
	glBufferSubData(GL_UNIFORM_BUFFER, offsets[4], sizeof(SpotLight) * lightsUsed.spot, (GLfloat *)spotLights);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

const LightManager::LightCounts & LightManager::getMaximums() const
{
	return lightMaxes;
}
