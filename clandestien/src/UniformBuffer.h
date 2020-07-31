#pragma once
#include "ShaderProgram.h"
#include <vector>
#include <GL\glew.h>

class UniformBuffer
{
private:
	GLuint bufferHandle;
	GLint blockSize;
	std::vector<GLuint> variableOffsets;

	void Initialize(const ShaderProgram&  program, const std::string &blockName, const GLchar** blockVariables, const unsigned int blockVariableCount);
public:
	//Important blockVariables[] have to be prefixed with the name of the blockName (only if the block is named) ex. test.member
	//If the member is a struct the name has to be the first member of the struct and not the struct itself
	//If the member is an array the name has to be the first entry of the array
	UniformBuffer(const ShaderProgram & program, const std::string &blockName, const std::string blockVariables[], const unsigned int blockVariableCount);
	UniformBuffer(const ShaderProgram & program, const std::string &blockName, const GLchar** blockVariables, const unsigned int blockVariableCount);
	UniformBuffer(const ShaderProgram & program, const std::string &blockName, const std::initializer_list<std::string> blockVariables, const unsigned int blockVariableCount);
	~UniformBuffer();

	GLuint GetBlockSize();
	const std::vector<GLuint> & GetOffsets() const;
	const GLuint GetHandle() const { return bufferHandle; }

	void BindToPort(GLuint bufferPort);
};
