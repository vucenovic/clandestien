#version 450 core

layout (location = 0) out vec4 color;

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

uniform vec4 flatColor;
uniform vec4 material;

layout (binding = 1, std140) uniform LightData{
	vec4 ambientColor;
	uvec4 lightCounts;

	PointLight[5] pointLights;

	DirectionalLight[5] directionalLights;

	SpotLight[3] spotLights;

	SpotLight shadowLight;
	mat4 shadowMatrix;
} lights;

in VertexData
{
	vec3 eyeDir;
	vec3 worldPos;
	vec2 texCoord;
	mat3 TBN;
    vec4 shadowPos;
} vertex;

vec3 reflectDir;
vec3 lightDiffuse;
vec3 lightSpecular;
vec3 eyeDir;
vec3 worldNormal;

vec3 mod289(vec3 x) {
    return x - floor(x * (1.0 / 289.0)) * 289.0;
}

vec4 mod289(vec4 x) {
    return x - floor(x * (1.0 / 289.0)) * 289.0;
}

vec4 permute(vec4 x) {
    return mod289(((x*34.0)+1.0)*x);
}

void main()
{
    vec3 n = mod289( vec3(vertex.texCoord, 1.0));
    float r = fract( 20 * n.z * n.x );
    r *= 20 * ( 1.0 - r );
    float l = sin(pow( r, 1.05 ) + vertex.texCoord.x);
    vec3 c = mix( vec3(1.0, 1.0, 1.0), vec3(0.0, 0.0, 0.0), l);
    color = permute(vec4( c, 1.0 ));
}