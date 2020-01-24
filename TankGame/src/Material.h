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
	std::shared_ptr<ShaderProgram> shader;
	std::unordered_map<GLuint, std::shared_ptr<Texture>> textures;
	std::unordered_map<GLuint, std::unique_ptr<MaterialProperty>> properties;

	Material(std::shared_ptr<ShaderProgram> shaderProg);
	~Material();

	void Use() const;
	void UseShader() const;
	void ApplyProperties() const;

	void SetPropertyf(const std::string & name, GLfloat val);
	void SetProperty3f(const std::string & name, glm::vec3 val);
	void SetProperty4f(const std::string & name, glm::vec4 val);
	void SetPropertyi(const std::string & name, GLint val);

	void SetTexture(std::shared_ptr<Texture> texture, GLuint textureUnit);
};

//Needs some work, currently not functional
class MaterialInstance
{
private:
	Material & material;
public:
	std::unordered_map<GLuint, std::shared_ptr<Texture>> instanceTextures;
	std::unordered_map<GLuint, std::unique_ptr<MaterialProperty>> instanceProperties;

	MaterialInstance(Material & mat) : material(mat) {};
	~MaterialInstance();

	virtual void Use();
};