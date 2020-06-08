#version 450 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoord;
layout (location = 3) in vec4 tangent;

layout (binding = 0, std140) uniform viewData
{
	mat4 projection;
	mat4 view;
};

struct PointLight{
	vec4 position;
	vec4 color;
	vec4 falloff;
};

struct DirectionalLight{
	vec4 direction;
	vec4 color;
};

struct SpotLight{
	vec4 position;
	vec4 direction;
	vec4 color;
	vec4 falloff;
};

layout (binding = 1, std140) uniform LightData{
	vec4 ambientColor;
	uvec4 lightCounts;

	PointLight[5] pointLights;

	DirectionalLight[5] directionalLights;

	SpotLight[3] spotLights;

	SpotLight shadowLight;
	mat4 shadowMatrix;
} lights;

uniform mat4 modelMatrix;
uniform mat4 modelNormalMatrix;

out VertexData
{
	vec3 eyeDir;
	vec3 worldPos;
	vec2 texCoord;
	mat3 TBN;
    vec4 shadowPos;
} vertex;

//Definitions
void main()
{
	vec4 worldPos = modelMatrix*vec4(position,1);

	vec3 worldNormal = (modelNormalMatrix*vec4(normal,0)).xyz;
	vec3 worldTangent = (modelNormalMatrix*tangent).xyz;
	
	vertex.TBN = mat3(worldTangent,cross(worldTangent,worldNormal) * tangent.w ,normalize(worldNormal));

	vec4 eyePos = (inverse(view) * vec4(0,0,0,1));
	vertex.eyeDir = (eyePos - worldPos).xyz;
	vertex.worldPos = worldPos.xyz;

	gl_Position = projection * view * worldPos;
	vertex.texCoord = texCoord;

	vertex.shadowPos = lights.shadowMatrix * worldPos;
	vertex.texCoord = texCoord;
}