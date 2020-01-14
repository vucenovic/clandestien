#version 450 core

#define PI 3.1415926535897932384626433832795

layout (location = 0) out vec3 color;

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

uniform vec3 flatColor;
uniform vec4 material;

layout (binding = 1, std140) uniform LightData{
	vec4 ambientColor;
	uvec4 lightCounts;

	PointLight[5] pointLights;

	DirectionalLight[5] directionalLights;

	SpotLight[3] spotLights;
} lights;

in VertexData
{
	vec3 eyeDir;
	vec3 worldPos;
	vec2 texCoord;
	mat3 TBN;
} vertex;

vec3 reflectDir;
vec3 lightDiffuse;
vec3 lightSpecular;
vec3 eyeDir;
vec3 worldNormal;

//Declarations
void doPointLight(PointLight light);
void doDirectionalLight(DirectionalLight light);
void doSpotLight(SpotLight light);

float specularReflection(float dotHN, float dotHV, float dotNV, float dotLN);
float geometricAttenFactor(float dotHN, float dotNV, float dotLN, float dotHV);
float fresnelFactor(float cosTheta, float R0);
float beckmannFactor(float cosAlpha, float roughness);

void main()
{
	eyeDir = normalize(vertex.eyeDir);
	worldNormal = normalize(TBN[2]);

	lightDiffuse = vec3(0);
	lightSpecular = vec3(0);

	for(int i=0;i<lights.lightCounts.x;i++){ //do PointLights
		doPointLight(lights.pointLights[i]);
	}

	for(int i=0;i<lights.lightCounts.y;i++){ //do DirectionalLights
		doDirectionalLight(lights.directionalLights[i]);
	}

	for(int i=0;i<lights.lightCounts.z;i++){ //do SpotLight
		doSpotLight(lights.spotLights[i]);
	}
	
	color = flatColor * lightDiffuse*(1-material.y) + lightSpecular * mix(vec3(1,1,1), flatColor, material.y);
}

void doPointLight(PointLight light){
	vec3 lightDir = light.position.xyz - vertex.worldPos;
	float dotNL = max(0,dot(worldNormal,lightDir));
	if(dotNL > 0){
		float dist = length(lightDir);
		if(dist > light.falloff.w) return;
		lightDir /= dist;

		vec3 halfVec = normalize(lightDir+eyeDir);

		float falloff = 1/(light.falloff.x + light.falloff.y * dist + light.falloff.z * dist * dist);

		lightDiffuse += dotNL * light.color.xyz * falloff; //Diffuse
		lightSpecular += dotNL * (material.w + specularReflection(dot(halfVec,worldNormal),dot(halfVec,eyeDir),dot(worldNormal,eyeDir),dot(lightDir,worldNormal)) * (1-material.w)) * light.color.xyz * falloff; //Specular
	}
}

void doDirectionalLight(DirectionalLight light){
	vec3 lightDir = -light.direction.xyz;
	float dotNL = max(0,dot(worldNormal,lightDir));
	if(dotNL > 0){
		vec3 halfVec = normalize(lightDir+eyeDir);

		lightDiffuse += dotNL * light.color.xyz; //Diffuse
		lightSpecular += dotNL * (material.w + specularReflection(dot(halfVec,worldNormal),dot(halfVec,eyeDir),dot(worldNormal,eyeDir),dot(lightDir,worldNormal)) * (1-material.w)) * light.color.xyz; //Specular
	}
}

void doSpotLight(SpotLight light){
	vec3 lightDir = light.position.xyz - vertex.worldPos;
	float dotNL = max(0,dot(worldNormal,lightDir));
	if(dotNL > 0){
		float dist = length(lightDir);
		if(dist > light.falloff.w) return;
		lightDir /= dist;

		vec3 halfVec = normalize(lightDir+eyeDir);

		float radial = dot(-light.direction.xyz,lightDir);

		float falloff = 1/(light.falloff.x + light.falloff.y * dist + light.falloff.z * dist * dist);
		falloff *= smoothstep(light.direction.w,light.position.w,radial);

		lightDiffuse += dotNL * light.color.xyz * falloff; //Diffuse
		lightSpecular += dotNL * (material.w + specularReflection(dot(halfVec,worldNormal),dot(halfVec,eyeDir),dot(worldNormal,eyeDir),dot(lightDir,worldNormal)) * (1-material.w)) * light.color.xyz * falloff; //Specular
	}
}

float specularReflection(float dotHN, float dotHV, float dotNV, float dotLN){
	float denom = beckmannFactor(dotHN,material.x) * fresnelFactor(dotHV, material.z) * geometricAttenFactor(dotHN,dotNV,dotLN,dotHV);
	return denom / (PI * dotNV * dotLN);
}

float geometricAttenFactor(float dotHN, float dotNV, float dotLN, float dotHV){
	float t1 = (2*dotHN*dotNV)/dotHV;
	float t2 = (2*dotHN*dotLN)/dotHV;
	return min(1,min(t1,t2));
}

//cos theta = dot(HalfwayVector, EyeVector)
float fresnelFactor(float cosTheta, float R0){ //using schlick approx. //R0 = ( (1-IOR)/(1+IOR) )^2
	float c5 = (1-cosTheta);
	c5 = c5*c5*c5*c5*c5; //
	return R0 + (1-R0)*c5;
}

//cos alpha = dot(NormalVector, HalfwayVector)
float beckmannFactor(float cosAlpha, float roughness){
	float angleCosClamped = max(cosAlpha,0.0001);
	roughness = max(roughness,0.0001);
	float cos2 = angleCosClamped * angleCosClamped;
	float rough2 = roughness * roughness;

	return exp((cos2-1)/(cos2*rough2)) / (PI * rough2 * (cos2 * cos2));
}
