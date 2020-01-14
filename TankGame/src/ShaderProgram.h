#pragma once
#include <map>
#include <GL\glew.h>
#include "Shader.h"
#include "Utils.h"

class ShaderProgram
{
private:
	GLuint programHandle;
	mutable std::map<std::string, GLuint> uniformLocationCache;
	void CreateAndLink(Shader* vert, Shader* frag);
public:
	ShaderProgram(Shader* vertexShader, Shader* fragmentShader);
	ShaderProgram(const std::string&, const std::string&);
	static ShaderProgram* FromFile(const std::string& filePath);
	static ShaderProgram* FromFile(const std::string& vertexShaderFilePath, const std::string& fragmentShaderFilePath);
	~ShaderProgram();

	GLuint GetProgramHandle() const;

	void SetUniformBlockBindingPort(const std::string &str, const GLuint port);
	GLuint GetUniformLocation(const std::string& name) const;

	void UseProgram();
};

