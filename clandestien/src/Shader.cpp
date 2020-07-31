#include "Shader.h"
#include <iostream>

bool Shader::compiledSuccessfully()
{
	return compiled == GL_TRUE;
}

GLuint Shader::getShaderID()
{
	return shaderID;
}

Shader::Shader(GLuint shaderType, const std::string& shaderSource)
{
	shaderID = glCreateShader(shaderType);
	
	const GLchar *source = (const GLchar *)shaderSource.c_str();
	glShaderSource(shaderID, 1, &source, 0);

	glCompileShader(shaderID);

	glGetShaderiv(shaderID, GL_COMPILE_STATUS, &compiled);
	if (compiled == GL_FALSE)
	{
		GLint maxLength = 0;
		glGetShaderiv(shaderID, GL_INFO_LOG_LENGTH, &maxLength);

		GLchar* infoLog = new GLchar[maxLength];
		glGetShaderInfoLog(shaderID, maxLength, &maxLength, &infoLog[0]);

		glDeleteShader(shaderID);
		shaderID = NULL;

		std::string s(infoLog);
		
		std::cout << s << std::endl;

		delete[] infoLog;
		return;
	}

}


Shader::~Shader()
{
	if (compiledSuccessfully()) {
		glDeleteShader(shaderID);
	}
}
