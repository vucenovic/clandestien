#include "Lights.h"
#include <glm\gtc\matrix_transform.hpp>

PointLight::PointLight(const PointLight & o)
{
	memcpy(val_ptr, o.val_ptr, sizeof(PointLight));
}

PointLight::PointLight()
{
	position.x = 0;
	position.y = 0;
	position.z = 0;
	color.red = 1;
	color.green = 1;
	color.blue = 1;
	falloff.constant = 1;
	falloff.linear = 0;
	falloff.square = 1;
	falloff.cutoffDistance = 10;
}

PointLight::~PointLight()
{
}

void PointLight::SetPosition(glm::vec3 pos)
{
	position.x = pos.x;
	position.y = pos.y;
	position.z = pos.z;
}

void PointLight::SetColor(GLfloat r, GLfloat g, GLfloat b)
{
	color.red = r;
	color.green = g;
	color.blue = b;
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
	memcpy(val_ptr, o.val_ptr, sizeof(DirectionalLight));
}

DirectionalLight::DirectionalLight()
{
	direction.x = 0;
	direction.y = -1;
	direction.z = 0;
	color.red = 1;
	color.green = 1;
	color.blue = 1;
}

DirectionalLight::~DirectionalLight()
{
}

void DirectionalLight::SetDirection(glm::vec3 dir)
{
	glm::vec3 d = glm::normalize(dir);
	direction.x = d.x;
	direction.y = d.y;
	direction.z = d.z;
}

void DirectionalLight::SetColor(GLfloat r, GLfloat g, GLfloat b)
{
	color.red = r;
	color.green = g;
	color.blue = b;
}

SpotLight::SpotLight(const SpotLight & o)
{
	memcpy(val_ptr, o.val_ptr, sizeof(SpotLight));
}

SpotLight::SpotLight()
{
	position.x = 0;
	position.y = 0;
	position.z = 0;
	direction.x = 0;
	direction.y = -1;
	direction.z = 0;
	position.radialInner = 0.9;
	direction.radialOuter = 0.85;
	color.red = 1;
	color.green = 1;
	color.blue = 1;
	falloff.constant = 1;
	falloff.linear = 0;
	falloff.square = 1;
	falloff.cutoffDistance = 10;
}

SpotLight::~SpotLight()
{
}

void SpotLight::SetPosition(glm::vec3 pos)
{
	position.x = pos.x;
	position.y = pos.y;
	position.z = pos.z;
}

void SpotLight::SetDirection(glm::vec3 dir)
{
	glm::vec3 d = glm::normalize(dir);
	direction.x = d.x;
	direction.y = d.y;
	direction.z = d.z;
}

void SpotLight::SetColor(GLfloat r, GLfloat g, GLfloat b)
{
	color.red = r;
	color.green = g;
	color.blue = b;
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
	position.radialInner = glm::cos(glm::radians(inner));
	direction.radialOuter = glm::cos(glm::radians(outer));
}
