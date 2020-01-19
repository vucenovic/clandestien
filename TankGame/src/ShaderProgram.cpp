#include "ShaderProgram.h"
#include <iostream>

void ShaderProgram::CreateAndLink(Shader * vertexShader, Shader * fragmentShader)
{
	if (vertexShader == nullptr || fragmentShader == nullptr)  throw std::invalid_argument("received nullptr");
	if (!vertexShader->compiledSuccessfully() || !fragmentShader->compiledSuccessfully())  throw std::invalid_argument("received an uncompiled shader");

	programHandle = glCreateProgram();

	glAttachShader(programHandle, vertexShader->getShaderID());
	glAttachShader(programHandle, fragmentShader->getShaderID());

	glLinkProgram(programHandle);

	if (!CheckLinkedStatus()) return;

	glDetachShader(programHandle, vertexShader->getShaderID());
	glDetachShader(programHandle, fragmentShader->getShaderID());
}

void ShaderProgram::CreateAndLink(Shader * vert, Shader * geom, Shader * frag)
{
	if (vert == nullptr || geom == nullptr || frag == nullptr)  throw std::invalid_argument("received nullptr");
	if (!vert->compiledSuccessfully() || !geom->compiledSuccessfully() || !frag->compiledSuccessfully())  throw std::invalid_argument("received an uncompiled shader");

	programHandle = glCreateProgram();

	glAttachShader(programHandle, vert->getShaderID());
	glAttachShader(programHandle, geom->getShaderID());
	glAttachShader(programHandle, frag->getShaderID());

	glLinkProgram(programHandle);

	if (!CheckLinkedStatus()) return;

	glDetachShader(programHandle, vert->getShaderID());
	glDetachShader(programHandle, geom->getShaderID());
	glDetachShader(programHandle, frag->getShaderID());
}

bool ShaderProgram::CheckLinkedStatus()
{
	GLint linked;
	glGetProgramiv(programHandle, GL_LINK_STATUS, (int *)&linked);
	if (linked == GL_FALSE)
	{
		GLint maxLength = 0;
		glGetProgramiv(programHandle, GL_INFO_LOG_LENGTH, &maxLength);

		GLchar* infoLog = new GLchar[maxLength];
		glGetProgramInfoLog(programHandle, maxLength, &maxLength, infoLog);

		glDeleteProgram(programHandle);
		programHandle = 0;

		std::string s(infoLog);
		std::cout << s << std::endl;

		delete[] infoLog;
		return false;
	}
	return true;
}

ShaderProgram::ShaderProgram(Shader* vertexShader, Shader* fragmentShader)
{
	CreateAndLink(vertexShader, fragmentShader);
}

ShaderProgram::ShaderProgram(Shader * vertexShader, Shader * geometryShader, Shader * fragmentShader)
{
	CreateAndLink(vertexShader, geometryShader, fragmentShader);
}

ShaderProgram::ShaderProgram(const std::string& vertSource, const std::string& fragSource)
{
	Shader* vert = new Shader(GL_VERTEX_SHADER, vertSource);
	Shader* frag = new Shader(GL_FRAGMENT_SHADER, fragSource);

	CreateAndLink(vert, frag);

	delete vert;
	delete frag;
}

ShaderProgram::ShaderProgram(const std::string& vertSource, const std::string& geomSource, const std::string& fragSource)
{
	Shader* vert = new Shader(GL_VERTEX_SHADER, vertSource);
	Shader* geom = new Shader(GL_GEOMETRY_SHADER, geomSource);
	Shader* frag = new Shader(GL_FRAGMENT_SHADER, fragSource);

	CreateAndLink(vert, geom, frag);

	delete vert;
	delete geom;
	delete frag;
}

std::unique_ptr<ShaderProgram> ShaderProgram::FromFile(const std::string& filePath)
{
	return FromFile(filePath + ".vert", filePath + ".frag");
}

std::unique_ptr<ShaderProgram> ShaderProgram::FromFile(const std::string & vertexShaderFilePath, const std::string & fragmentShaderFilePath)
{
	std::string vertSource = loadFileAsString(vertexShaderFilePath);
	std::string fragSource = loadFileAsString(fragmentShaderFilePath);
	return std::make_unique<ShaderProgram>(vertSource, fragSource);
}

std::unique_ptr<ShaderProgram> ShaderProgram::FromFile(const std::string & vertexShaderFilePath, const std::string & geometryShaderFilePath, const std::string & fragmentShaderFilePath)
{
	std::string vertSource = loadFileAsString(vertexShaderFilePath);
	std::string fragSource = loadFileAsString(fragmentShaderFilePath);
	std::string geomSource = loadFileAsString(geometryShaderFilePath);
	return std::make_unique<ShaderProgram>(vertSource, geomSource, fragSource);
}

std::unique_ptr<ShaderProgram> FromFile(const std::string& vertexShaderFilePath, const std::string& geometryShaderFilePath, const std::string& fragmentShaderFilePath)
{
	std::string vertSource = loadFileAsString(vertexShaderFilePath);
	std::string fragSource = loadFileAsString(fragmentShaderFilePath);
	std::string geomSource = loadFileAsString(geometryShaderFilePath);
	return std::make_unique<ShaderProgram>(vertSource, geomSource, fragSource);
}

ShaderProgram::~ShaderProgram()
{
	glDeleteProgram(programHandle);
}

GLuint ShaderProgram::GetProgramHandle() const
{
	return programHandle;
}

void ShaderProgram::SetUniformBlockBindingPort(const std::string & str, const GLuint port)
{
	glUniformBlockBinding(programHandle, glGetUniformBlockIndex(programHandle, str.c_str()), port);
}

GLuint ShaderProgram::GetUniformLocation(const std::string& name) const
{
	if (uniformLocationCache.find(name) != uniformLocationCache.end())
	{
		return uniformLocationCache[name];
	}
	else
	{
		GLint l = glGetUniformLocation(programHandle,name.c_str());
		if (l != -1) {
			uniformLocationCache[name] = l;
		}
		return l;
	}
}

void ShaderProgram::UseProgram()
{
	glUseProgram(programHandle);
}
