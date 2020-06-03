#pragma once
#include <GL\glew.h>
#include <glm\glm.hpp>

class PointLight
{
public:
	struct { //padded to vec4(s)
		alignas(16) glm::vec3 position;
		alignas(16) glm::vec3 color;
		union {
			struct alignas(16) { GLfloat constant, linear, square, cutoffDistance; } falloff;
			glm::vec4 attenuation;
		};
	};
public:
	PointLight(const PointLight &o);
	PointLight(const glm::vec3 pos = glm::vec3(0,0,0), const glm::vec3 col = glm::vec3(1,1,1), const glm::vec4 att = glm::vec4(1, 0, 1, 100)) : position(pos), color(col), attenuation(att) {};
	~PointLight();

	void SetPosition(const glm::vec3 pos);
	void SetColor(const glm::vec3 col);
	void SetAttenuation(GLfloat constant, GLfloat linear, GLfloat squared, GLfloat cutoffDistance = -1);
};

class DirectionalLight
{
public:
	struct {
		alignas(16) glm::vec3 direction;
		alignas(16) glm::vec3 color;
	};
public:
	DirectionalLight(const DirectionalLight &o);
	DirectionalLight(const glm::vec3 dir = glm::vec3(0, -1, 0), const glm::vec3 col = glm::vec3(1, 1, 1)) : direction(dir), color(col) {};
	~DirectionalLight();

	void SetDirection(const glm::vec3 dir);
	void SetColor(const glm::vec3 col);
};

class SpotLight
{
public:
	struct {
		struct alignas(16) { glm::vec3 position; GLfloat radialInner; };
		struct alignas(16) { glm::vec3 direction; GLfloat radialOuter; };
		alignas(16) glm::vec3 color;
		union {
			struct alignas(16) { GLfloat constant, linear, square, cutoffDistance; } falloff;
			glm::vec4 attenuation;
		};
	};
public:
	SpotLight(const SpotLight &o);
	SpotLight(const glm::vec3 pos = glm::vec3(0, 0, 0), const glm::vec3 dir = glm::vec3(0, -1, 0), const glm::vec2 radial = glm::vec2(0.9f, 0.85f), const glm::vec3 col = glm::vec3(1, 1, 1), const glm::vec4 att = glm::vec4(1, 0, 1, 100)) : position(pos), direction(dir), radialInner(radial.x), radialOuter(radial.y), color(col), attenuation(att) {};
	~SpotLight();

	void SetPosition(const glm::vec3 pos);
	void SetDirection(const glm::vec3 dir);
	void SetColor(const glm::vec3 col);
	void SetAttenuation(GLfloat constant, GLfloat linear, GLfloat squared, GLfloat cutoffDistance = -1);
	void SetRadialFalloffDegrees(GLfloat inner, GLfloat outer);
};