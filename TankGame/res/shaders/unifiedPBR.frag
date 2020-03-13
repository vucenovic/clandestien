#version 450 core

#define PI 3.1415926535897932384626433832795

layout (location = 0) out vec4 color;

//LightSource Definitions
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

//Declarations
void doPointLight(PointLight light);
void doDirectionalLight(DirectionalLight light);
void doSpotLight(SpotLight light);

float specularReflection(float dotHN, float dotHV, float dotNV, float dotLN);
float geometricAttenFactor(float dotHN, float dotNV, float dotLN, float dotHV);
float fresnelFactor(float cosTheta, float R0);
float beckmannFactor(float cosAlpha, float roughness);

layout (binding = 0 ) uniform sampler2D albedoTex;
layout (binding = 1 ) uniform sampler2D materialTex;
layout (binding = 2 ) uniform sampler2D normalTex;
layout (binding = 3 ) uniform samplerCube cubemapTex;

layout (binding = 1, std140) uniform LightData{
	vec4 ambientColor;
	uvec4 lightCounts;

	PointLight[15] pointLights;

	DirectionalLight[3] directionalLights;

	SpotLight[5] spotLights;
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

float specularity = 1;
float roughness;
float IOR = 1.45f;

void main()
{
	eyeDir = normalize(vertex.eyeDir);
	worldNormal = normalize(vertex.TBN[2]);

	vec3 textureNormal = normalize(texture( normalTex, vertex.texCoord ).rgb * 2.0 - 1.0);
	worldNormal = vertex.TBN * textureNormal;

	reflectDir = reflect(eyeDir,worldNormal);

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

	vec4 albedo = texture(albedoTex, vertex.texCoord);
	vec4 material = texture(materialTex, vertex.texCoord);
	roughness = material.x;
	float metalness = material.y;
	metalness = 0;
	float emission = material.w;
	emission = 0;

	//color = vec4(albedo.rgb * (lights.ambientColor.xyz * material.x + lightDiffuse * material.y) + (lightSpecular + texture(cubemapTex, -reflectDir).xyz * flatColor.w) * material.z * texture(materialTex,vertex.texCoord).x,1);
	//TODO: add back support for cubemaps (maybe even parallax corrected Cubemaps, who knows)
	color = vec4(albedo.rgb * (lights.ambientColor.rgb + lightDiffuse * (1-metalness)) + (lightSpecular * mix(vec3(1,1,1), albedo.rgb, metalness)) + emission * albedo.rgb, 1);
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
		lightSpecular += dotNL * (specularity + specularReflection(dot(halfVec,worldNormal),dot(halfVec,eyeDir),dot(worldNormal,eyeDir),dot(lightDir,worldNormal)) * (1-specularity)) * light.color.xyz * falloff; //Specular
	}
}

void doDirectionalLight(DirectionalLight light){
	vec3 lightDir = -light.direction.xyz;
	float dotNL = max(0,dot(worldNormal,lightDir));
	if(dotNL > 0){
		vec3 halfVec = normalize(lightDir+eyeDir);

		lightDiffuse += dotNL * light.color.xyz; //Diffuse
		lightSpecular += dotNL * (specularity + specularReflection(dot(halfVec,worldNormal),dot(halfVec,eyeDir),dot(worldNormal,eyeDir),dot(lightDir,worldNormal)) * (1-specularity)) * light.color.xyz; //Specular
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
		lightSpecular += dotNL * (specularity + specularReflection(dot(halfVec,worldNormal),dot(halfVec,eyeDir),dot(worldNormal,eyeDir),dot(lightDir,worldNormal)) * (1-specularity)) * light.color.xyz * falloff; //Specular
	}
}

float specularReflection(float dotHN, float dotHV, float dotNV, float dotLN){
	float denom = beckmannFactor(dotHN,roughness) * fresnelFactor(dotHV, IOR) * geometricAttenFactor(dotHN,dotNV,dotLN,dotHV);
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
