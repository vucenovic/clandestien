#pragma once
#include <GL\glew.h>
#include <string>

class Shader
{
private:
	GLuint shaderID;
	GLint compiled;
public:
	bool compiledSuccessfully();
	GLuint getShaderID();
	Shader(GLuint shaderType,const std::string& shaderSource);
	~Shader();
};