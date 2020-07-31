#version 450 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 velocity;
layout (location = 2) in vec4 color;
layout (location = 3) in float size;
layout (location = 4) in float rotation;
//possibly add animstate

layout (binding = 0, std140) uniform viewData
{
	mat4 projection;
	mat4 view;
};

uniform mat4 modelMatrix;
uniform mat4 modelNormalMatrix;

out ParticleDataGeom
{
    float size;
	vec2 upVector;

	vec4 color;
	vec2 velocityVector;
} particle;

void main()
{
	vec4 worldPos = modelMatrix*vec4(position,1);

    particle.size = size;
	particle.upVector.x = sin(rotation);
	particle.upVector.y = cos(rotation);

	particle.color = color;
	vec4 velRaw =  modelMatrix*vec4(velocity,0);
	particle.velocityVector =velRaw.xy;

	gl_Position = projection * view * worldPos;
}