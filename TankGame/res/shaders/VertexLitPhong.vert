#version 450 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;

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

layout (binding = 0, std140) uniform viewData
{
	mat4 viewProjection;
	vec4 eyePos;
};

uniform mat4 modelMatrix;
uniform mat4 modelNormalMatrix;
uniform vec4 material;

layout (binding = 1, std140) uniform LightData{
	vec4 ambientColor;
	uvec4 lightCounts;

	PointLight[5] pointLights;

	DirectionalLight[5] directionalLights;

	SpotLight spotLights[5];
} lights;

uniform vec3 flatColor;
out vec3 finalColor;

//Declarations
void doPointLight(PointLight light);
void doDirectionalLight(DirectionalLight light);
void doSpotLight(SpotLight light);

vec3 eyeDir;
vec3 reflectDir;
vec3 worldPos;
vec3 worldNormal;
vec3 lightDiffuse;
vec3 lightSpecular;

//Definitions
void main()
{
	vec4 worldPosRaw = (modelMatrix*vec4(position,1));
	worldNormal = normalize((modelNormalMatrix*vec4(normal,0)).xyz);
	worldPos = worldPosRaw.xyz;

	gl_Position = viewProjection*worldPosRaw;

	lightDiffuse =  vec3(0);
	lightSpecular = vec3(0);

	eyeDir = normalize(eyePos.xyz - worldPos);
	reflectDir = reflect(eyeDir,worldNormal);

	for(int i=0;i<lights.lightCounts.x;i++){ //do PointLights
		doPointLight(lights.pointLights[i]);
	}

	for(int i=0;i<lights.lightCounts.y;i++){ //do DirectionalLights
		doDirectionalLight(lights.directionalLights[i]);
	}

	for(int i=0;i<lights.lightCounts.z;i++){ //do SpotLight
		doSpotLight(lights.spotLights[i]);
	}

	finalColor = flatColor * (lightDiffuse * material.y + lights.ambientColor.xyz * material.x) + lightSpecular * material.z;
}

void doPointLight(PointLight light){
	vec3 lightDir = light.position.xyz - worldPos;
	float dist = length(lightDir);
	if(dist > light.falloff.w) return;
	lightDir /= dist;

	float falloff = 1/(light.falloff.x + light.falloff.y * dist + light.falloff.z * dist * dist);

	float facDiff = max(dot(worldNormal,lightDir),0);
	float facSpec = facDiff>0 ? max(-dot(reflectDir,lightDir),0) : 0;

	lightDiffuse += facDiff * light.color.xyz * falloff; //Diffuse
	lightSpecular +=	pow(facSpec,material.w) * light.color.xyz * falloff; //Specular
}

void doDirectionalLight(DirectionalLight light){
	float facDiff = max(dot(worldNormal,-light.direction.xyz),0);
	float facSpec = facDiff>0 ? max(-dot(reflectDir,-light.direction.xyz),0) : 0;

	lightDiffuse += facDiff * light.color.xyz; //Diffuse
	lightSpecular +=	pow(facSpec,material.w) * light.color.xyz; //Specular
}

void doSpotLight(SpotLight light){
	vec3 lightDir = light.position.xyz - worldPos;
	float dist = length(lightDir);
	if(dist > light.falloff.w) return;
	lightDir /= dist;

	float radial = dot(-light.direction.xyz,lightDir);

	float falloff = 1/(light.falloff.x + light.falloff.y * dist + light.falloff.z * dist * dist);
	falloff *= smoothstep(light.direction.w,light.position.w,radial);

	float facDiff = max(dot(worldNormal,lightDir),0);
	float facSpec = facDiff>0 ? max(-dot(reflectDir,lightDir),0) : 0;

	lightDiffuse += facDiff * light.color.xyz * falloff; //Diffuse
	lightSpecular += pow(facSpec,material.w) * light.color.xyz * falloff; //Specular
}