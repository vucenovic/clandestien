#pragma once

#include <vector>
#include <unordered_map>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "ShaderProgram.h"
#include "Texture.h"

//The material Property system does not support reading out of property types at the moment
class MaterialProperty {
public:
	virtual void Set(GLuint location) = 0;
};

class MaterialPropertyMatrix4f : public MaterialProperty {
	glm::mat4 val;

	public:
	MaterialPropertyMatrix4f(glm::mat4 v) : val(v) {};
	virtual void Set(GLuint location) override;
};

class MaterialPropertyf : public MaterialProperty {
	GLfloat val;
public:
	MaterialPropertyf(GLfloat v) : val(v) {};

	virtual void Set(GLuint location) override;
};

class MaterialPropertyi : public MaterialProperty {
	GLint val;
public:
	MaterialPropertyi(GLint v) : val(v) {};

	virtual void Set(GLuint location) override;
};

class MaterialProperty3f : public MaterialProperty {
	glm::vec3 val;
public:
	MaterialProperty3f(glm::vec3 v) : val(v) {};
	virtual void Set(GLuint location) override;
};

class MaterialProperty4f : public MaterialProperty {
	glm::vec4 val;
public:
	MaterialProperty4f(glm::vec4 v) : val(v) {};
	virtual void Set(GLuint location) override;
};

class Material
{
public:
	ShaderProgram * shader;
	std::unordered_map<GLuint, Texture *> textures;
	std::unordered_map<GLuint, std::unique_ptr<MaterialProperty>> properties;

	Material(ShaderProgram * shaderProg);
	~Material();

	virtual void Use() const;
	void UseShader() const;
	void ApplyProperties() const;

	void SetPropertyf(const std::string & name, GLfloat val);
	void SetProperty3f(const std::string & name, glm::vec3 val);
	void SetProperty4f(const std::string & name, glm::vec4 val);
	void SetPropertyMatrix4f(const std::string & name, glm::mat4 val);
	void SetPropertyi(const std::string & name, GLint val);

	void SetTexture(Texture * texture, GLuint textureUnit);
};

class StandardMaterial : public Material
{
private:
	GLuint matLoc, colLoc;
public:
	glm::vec4 material = glm::vec4(0.1f, 0.7f, 1, 8);
	glm::vec4 color = glm::vec4(1, 1, 1, 0);
	Texture2D * diffuse;
	Texture2D * specular;
	Texture2D * normal;
	Texture2D * emission;
	//TextureCubemap * cubeMap;

	StandardMaterial(ShaderProgram * shader);

	void Use() const override;
};