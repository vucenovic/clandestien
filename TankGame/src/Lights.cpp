#include "Lights.h"
#include <glm\gtc\matrix_transform.hpp>

PointLight::PointLight(const PointLight & o)
{
	memcpy(this, &o, sizeof(PointLight));
}

PointLight::~PointLight()
{
}

void PointLight::SetPosition(const glm::vec3 pos)
{
	position = pos;
}

void PointLight::SetColor(const glm::vec3 col)
{
	color = col;
}

void PointLight::SetAttenuation(GLfloat constant, GLfloat linear, GLfloat squared, GLfloat cutoffDistance)
{
	falloff.constant = constant;
	falloff.linear = linear;
	falloff.square = squared;
	falloff.cutoffDistance = cutoffDistance == -1 ? 100 : cutoffDistance;
}

DirectionalLight::DirectionalLight(const DirectionalLight & o)
{
	memcpy(this, &o, sizeof(DirectionalLight));
}

DirectionalLight::~DirectionalLight()
{
}

void DirectionalLight::SetDirection(glm::vec3 dir)
{
	direction = glm::normalize(dir);
}

void DirectionalLight::SetColor(const glm::vec3 col)
{
	color = col;
}

SpotLight::SpotLight(const SpotLight & o)
{
	memcpy(this, &o, sizeof(SpotLight));
}

SpotLight::~SpotLight()
{
}

void SpotLight::SetPosition(const glm::vec3 pos)
{
	position = pos;
}

void SpotLight::SetDirection(const glm::vec3 dir)
{
	direction = glm::normalize(dir);
}

void SpotLight::SetColor(const glm::vec3 col)
{
	color = col;
}

void SpotLight::SetAttenuation(GLfloat constant, GLfloat linear, GLfloat squared, GLfloat cutoffDistance)
{
	falloff.constant = constant;
	falloff.linear = linear;
	falloff.square = squared;
	falloff.cutoffDistance = cutoffDistance == -1 ? 100 : cutoffDistance;
}

void SpotLight::SetRadialFalloffDegrees(GLfloat inner, GLfloat outer)
{
	radialInner = glm::cos(glm::radians(inner));
	radialOuter = glm::cos(glm::radians(outer));
}
