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

uniform mat4 modelMatrix;
uniform mat4 modelNormalMatrix;
uniform mat4 DepthBiasMatrix;

out VertexData
{
	vec3 eyeDir;
	vec3 worldPos;
	vec2 texCoord;
	mat3 TBN;
    vec4 shadow_Position;
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

    vertex.shadow_Position = DepthBiasMatrix * worldPos;
	vertex.texCoord = texCoord;
}