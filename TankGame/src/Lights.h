#pragma once
#include <GL\glew.h>
#include <glm\glm.hpp>

class PointLight
{
public:
	union {
		struct { //padded to vec4(s)
			struct alignas(16) { GLfloat x, y, z;} position;
			struct alignas(16) { GLfloat red, green, blue; } color;
			struct alignas(16) { GLfloat constant, linear, square, cutoffDistance; } falloff;
		};
		GLfloat val_ptr[12];
	};
public:
	PointLight(const PointLight &o);
	PointLight();
	~PointLight();

	void SetPosition(glm::vec3 pos);
	void SetColor(GLfloat r, GLfloat g, GLfloat b);
	void SetAttenuation(GLfloat constant, GLfloat linear, GLfloat squared, GLfloat cutoffDistance = -1);
};

class DirectionalLight
{
public:
	union {
		struct {
			struct alignas(16) { GLfloat x, y, z; } direction;
			struct alignas(16) { GLfloat red, green, blue; } color;
		};
		GLfloat val_ptr[8];
	};
public:
	DirectionalLight(const DirectionalLight &o);
	DirectionalLight();
	~DirectionalLight();

	void SetDirection(glm::vec3 dir);
	void SetColor(GLfloat r, GLfloat g, GLfloat b);
};

class SpotLight
{
public:
	union {
		struct {
			struct alignas(16) { GLfloat x, y, z, radialInner; } position;
			struct alignas(16) { GLfloat x, y, z, radialOuter; } direction;
			struct alignas(16) { GLfloat red, green, blue; } color;
			struct alignas(16) { GLfloat constant, linear, square, cutoffDistance; } falloff;
		};
		GLfloat val_ptr[16];
	};
public:
	SpotLight(const SpotLight &o);
	SpotLight();
	~SpotLight();

	void SetPosition(glm::vec3 pos);
	void SetDirection(glm::vec3 dir);
	void SetColor(GLfloat r, GLfloat g, GLfloat b);
	void SetAttenuation(GLfloat constant, GLfloat linear, GLfloat squared, GLfloat cutoffDistance = -1);
	void SetRadialFalloffDegrees(GLfloat inner, GLfloat outer);
};