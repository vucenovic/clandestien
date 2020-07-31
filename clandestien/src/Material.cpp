#include "Material.h"
#include "ResourceManager.h"
#include <glm/gtc/type_ptr.hpp>

Material::Material(ShaderProgram * shaderProg) : shader(shaderProg)
{
}

Material::~Material()
{
}

void Material::Use() const
{
	UseShader();
	ApplyProperties();
}

void Material::UseShader() const
{
	shader->UseProgram();
}

void Material::ApplyProperties() const
{
	for (std::pair<GLuint, Texture *> tex : textures) {
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

void Material::SetPropertyMatrix4f(const std::string & name, glm::mat4 val)
{
	properties[shader->GetUniformLocation(name)] = std::make_unique<MaterialPropertyMatrix4f>(val);
}

void Material::SetPropertyi(const std::string & name, GLint val)
{
	properties[shader->GetUniformLocation(name)] = std::make_unique<MaterialPropertyi>(val);
}

void Material::SetTexture(Texture * texture, GLuint textureUnit)
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

void MaterialPropertyMatrix4f::Set(GLuint location)
{
	glUniformMatrix4fv(location, 1, false ,  glm::value_ptr(val));
}

void MaterialPropertyi::Set(GLuint location)
{
	glUniform1i(location, val);
}

StandardMaterial::StandardMaterial(ShaderProgram * shader) : Material(shader)
{
	ResourceManager & res = ResourceManager::GetInstance();
	diffuse = (Texture2D*)res.GetTexture("white");
	specular = (Texture2D*)res.GetTexture("black");
	normal = (Texture2D*)res.GetTexture("purple");
	emission = (Texture2D*)res.GetTexture("black");

	matLoc = shader->GetUniformLocation("material");
	colLoc = shader->GetUniformLocation("flatColor");
}

void StandardMaterial::Use() const
{
	diffuse->Bind(0);
	specular->Bind(1);
	normal->Bind(2);
	emission->Bind(3);

	glActiveTexture(GL_TEXTURE0 + 3);//TODO replace with actual cubemap loading
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

	UseShader();
	glUniform4fv(matLoc, 1, glm::value_ptr(material));
	glUniform4fv(colLoc, 1, glm::value_ptr(color));

	ApplyProperties();
}
