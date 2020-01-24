#version 450 core

layout (location = 0) in vec3 position;
layout (location = 1) in float size;
layout (location = 2) in float rotation;
//possibly add velocity and color and animstate

layout (binding = 0, std140) uniform viewData
{
	mat4 viewProjection;
	vec4 eyePos;
};

uniform mat4 modelMatrix;
uniform mat4 modelNormalMatrix;

out ParticleDataGeom
{
    float size;
	vec2 upVector;
} particle;

void main()
{
	vec4 worldPos = modelMatrix*vec4(position,1);

    particle.size = size;
	particle.upVector.x = sin(rotation);
	particle.upVector.y = cos(rotation);

	gl_Position = viewProjection*worldPos;
}