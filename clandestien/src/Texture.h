#pragma once
#include <gl/glew.h>
#include <glm/glm.hpp>
#include <string>
#include "DDSImage.h"

class Texture
{
protected:
	GLuint textureHandle;
public:
	Texture();
	~Texture();

	virtual void Bind(GLuint textureUnit) = 0;
	static void Unbind(GLuint textureUnit);
};

class Texture2D : public Texture {
private:
	void SetDDS(const DDSImage & image);
public:
	Texture2D(const std::string & filepath);
	Texture2D(const DDSImage & image);
	Texture2D(const glm::vec3 & color);
	
	// Inherited via Texture
	virtual void Bind(GLuint textureUnit) override;

};

class TextureCubemap : public Texture {
public:
	TextureCubemap();
	TextureCubemap(const std::string & filepath);

	// Inherited via Texture
	virtual void Bind(GLuint textureUnit) override;

};