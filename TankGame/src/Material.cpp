#include "Material.h"

Material::~Material()
{
}

void Material::Use()
{
	shader->UseProgram();
	for (std::pair<GLuint, std::shared_ptr<Texture>> tex : textures) {
		tex.second->Bind(tex.first);
	}
	for (const std::pair<const GLuint, std::unique_ptr<MaterialProperty>>& property : properties) {
		property.second.get()->Set(property.first);
	}
}

void Material::SetPropertyf(const std::string & name, GLfloat val)
{
	properties[shader->GetUniformLocation(name)] = std::make_unique<MaterialPropertyf>(val);
}

void Material::SetProperty3f(const std::string & name, glm::vec3 val)
{
	properties[shader->GetUniformLocation(name)] = std::make_unique<MaterialProperty3f>(val);
}

void Material::SetProperty4f(const std::string & name, glm::vec4 val)
{
	properties[shader->GetUniformLocation(name)] = std::make_unique<MaterialProperty4f>(val);
}

void Material::SetTexture(std::shared_ptr<Texture> texture, GLuint textureUnit)
{
	textures[textureUnit] = texture;
}

void MaterialPropertyf::Set(GLuint location)
{
	glUniform1f(location, val);
}

void MaterialProperty3f::Set(GLuint location)
{
	glUniform3fv(location, 1, glm::value_ptr(val));
}

void MaterialProperty4f::Set(GLuint location)
{
	glUniform4fv(location, 1, glm::value_ptr(val));
}

void MaterialInstance::Use()
{
	material.Use();
	for (std::pair<GLuint, std::shared_ptr<Texture>> tex : instanceTextures) {
		tex.second->Bind(tex.first);
	}
	for (const std::pair<const GLuint, std::unique_ptr<MaterialProperty>>& property : instanceProperties) {
		property.second.get()->Set(property.first);
	}
}
