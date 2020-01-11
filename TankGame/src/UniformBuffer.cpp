#include "UniformBuffer.h"

void UniformBuffer::Initialize(const ShaderProgram & program, const std::string & blockName, const GLchar ** blockVariables, const unsigned int blockVariableCount)
{
	GLuint programHandle = program.GetProgramHandle();
	GLuint blockIndex = glGetUniformBlockIndex(programHandle, blockName.c_str());

	glGetActiveUniformBlockiv(programHandle, blockIndex, GL_UNIFORM_BLOCK_DATA_SIZE, &blockSize);

	GLuint* indices = new GLuint[blockVariableCount];
	GLint* offsets = new GLint[blockVariableCount];

	glGetUniformIndices(programHandle, blockVariableCount, blockVariables, indices);
	glGetActiveUniformsiv(programHandle, blockVariableCount, indices, GL_UNIFORM_OFFSET, offsets);

	for (size_t i = 0; i < blockVariableCount; i++)
	{
		variableOffsets.push_back(offsets[i]);
	}

	glGenBuffers(1, &bufferHandle);

	glBindBuffer(GL_UNIFORM_BUFFER, bufferHandle);
	glBufferData(GL_UNIFORM_BUFFER, blockSize, nullptr, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	delete[] indices;
	delete[] offsets;
}

UniformBuffer::UniformBuffer(const ShaderProgram & program, const std::string & blockName, const std::string blockVariables[], const unsigned int blockVariableCount)
{
	const GLchar ** bvn = new const GLchar*[blockVariableCount];
	for (size_t i = 0; i < blockVariableCount; i++)
	{
		bvn[i] = blockVariables[i].c_str();
	}

	Initialize(program, blockName, bvn, blockVariableCount);

	delete[] bvn;
}

UniformBuffer::UniformBuffer(const ShaderProgram & program, const std::string & blockName, const GLchar ** blockVariables, const unsigned int blockVariableCount)
{
	Initialize(program, blockName, blockVariables, blockVariableCount);
}

UniformBuffer::UniformBuffer(const ShaderProgram & program, const std::string & blockName, const std::initializer_list<std::string> blockVariables, const unsigned int blockVariableCount)
{
	const GLchar ** bvn = new const GLchar*[blockVariables.size()];
	std::string * tmpStringStore = new std::string[blockVariables.size()]; //because the c_str(s) are freed as soon as the loop iterates

	size_t index = 0;
	for (std::string str : blockVariables)
	{
		tmpStringStore[index] = str;
		bvn[index++] = tmpStringStore[index].c_str();
	}

	Initialize(program, blockName, bvn, blockVariableCount);

	delete[] bvn;
	delete[] tmpStringStore;
}

UniformBuffer::~UniformBuffer()
{
	glDeleteBuffers(1, &bufferHandle);
}

GLuint UniformBuffer::GetBlockSize()
{
	return blockSize;
}

const std::vector<GLuint> & UniformBuffer::GetOffsets() const
{
	return variableOffsets;
}

GLuint UniformBuffer::GetHandle()
{
	return bufferHandle;
}

void UniformBuffer::BindToPort(GLuint bufferPort)
{
	glBindBufferBase(GL_UNIFORM_BUFFER, bufferPort, bufferHandle);
}
