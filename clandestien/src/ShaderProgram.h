#pragma once
#include <map>
#include <GL\glew.h>
#include "Shader.h"
#include <memory>

class ShaderProgram
{
private:
	GLuint programHandle;
	mutable std::map<std::string, GLuint> uniformLocationCache;
	void CreateAndLink(Shader* vert, Shader* frag);
	void CreateAndLink(Shader* vert, Shader* geom, Shader* frag);

	bool CheckLinkedStatus();
public:
	ShaderProgram(Shader* vertexShader, Shader* fragmentShader);
	ShaderProgram(Shader* vertexShader, Shader* geometryShader, Shader* fragmentShader);

	ShaderProgram(const std::string& vert, const std::string& frag);
	ShaderProgram(const std::string& vert, const std::string& geom, const std::string& frag);

	static std::unique_ptr<ShaderProgram> FromFile(const std::string& vertexShaderFilePath, const std::string& fragmentShaderFilePath);
	static std::unique_ptr<ShaderProgram> FromFile(const std::string& vertexShaderFilePath, const std::string& geometryShaderFilePath, const std::string& fragmentShaderFilePath);

	~ShaderProgram();

	static std::unique_ptr<ShaderProgram> FromFile(const std::string& filePath);

	GLuint GetProgramHandle() const;

	void SetUniformBlockBindingPort(const std::string &str, const GLuint port);
	GLuint GetUniformLocation(const std::string& name) const;

	void UseProgram();
};

