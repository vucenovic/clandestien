#include "LightManager.h"
#include <glm/gtc/type_ptr.hpp>
#include <algorithm>

LightManager::LightManager(const ShaderProgram & program)
{
	lightDataBuffer = new UniformBuffer(program, std::string("LightData"), { "LightData.ambientColor", "LightData.lightCounts","LightData.pointLights[0].position","LightData.directionalLights[0].direction","LightData.spotLights[0].position","LightData.shadowLight.position", "LightData.shadowMatrix"}, 7);

	const std::vector<GLuint> & offsets = lightDataBuffer->GetOffsets();

	lightMaxes.point = (offsets[3] - offsets[2]) / sizeof(PointLight);
	lightMaxes.directional = (offsets[4] - offsets[3]) / sizeof(DirectionalLight);
	lightMaxes.spot = (offsets[5] - offsets[4]) / sizeof(SpotLight);
	lightMaxes.shadow = (lightDataBuffer->GetBlockSize() - offsets[5]) / sizeof(SpotLight);
	ambientLight = glm::vec3();
}

LightManager::~LightManager()
{
	delete lightDataBuffer;
}

void LightManager::BindToPort(GLuint port)
{
	lightDataBuffer->BindToPort(port);
}

void LightManager::UpdateBuffer()
{
	const std::vector<GLuint> & offsets = lightDataBuffer->GetOffsets();
	LightManager::LightCounts counts;
	counts.point = std::min((GLuint)pointLights.size(), lightMaxes.point);
	counts.directional = std::min((GLuint)directionalLights.size(), lightMaxes.directional);
	counts.spot = std::min((GLuint)spotLights.size(), lightMaxes.spot);
	counts.shadow = std::min((GLuint)shadowLightUsed, lightMaxes.shadow);

	glBindBuffer(GL_UNIFORM_BUFFER, lightDataBuffer->GetHandle());
	glBufferSubData(GL_UNIFORM_BUFFER, offsets[0], sizeof(glm::vec3), glm::value_ptr(ambientLight));
	glBufferSubData(GL_UNIFORM_BUFFER, offsets[1], sizeof(LightManager::LightCounts), &counts);

	glBufferSubData(GL_UNIFORM_BUFFER, offsets[2], sizeof(PointLight) * counts.point, (GLfloat *)pointLights.data());
	glBufferSubData(GL_UNIFORM_BUFFER, offsets[3], sizeof(DirectionalLight) * counts.directional, (GLfloat *)directionalLights.data());
	glBufferSubData(GL_UNIFORM_BUFFER, offsets[4], sizeof(SpotLight) * counts.spot, (GLfloat *)spotLights.data());
	glBufferSubData(GL_UNIFORM_BUFFER, offsets[5], sizeof(SpotLight) * counts.shadow, &shadowLight);
	glBufferSubData(GL_UNIFORM_BUFFER, offsets[6], sizeof(glm::mat4) * counts.shadow, glm::value_ptr(shadowProjection() * shadowView()));
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

const LightManager::LightCounts & LightManager::getMaximums() const
{
	return lightMaxes;
}
