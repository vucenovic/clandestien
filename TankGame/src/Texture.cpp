#include "Texture.h"

void Texture2D::SetDDS(const DDSImage & image)
{
	glGenTextures(1, &textureHandle);
	glBindTexture(GL_TEXTURE_2D, textureHandle);

	glCompressedTexImage2D(GL_TEXTURE_2D, 0, image.format, image.width, image.height, 0, image.size, image.data);
	glGenerateMipmap(GL_TEXTURE_2D);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
}

Texture2D::Texture2D(const std::string & filepath)
{
	SetDDS(DDSImage::loadDDS((filepath + ".dds").c_str()));
}

Texture2D::Texture2D(const DDSImage & image)
{
	SetDDS(image);
}

Texture2D::Texture2D(const glm::vec3 & color)
{
	glGenTextures(1, &textureHandle);
	glBindTexture(GL_TEXTURE_2D, textureHandle);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 1, 1, 0, GL_RGB, GL_FLOAT, &color.r);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
}

void Texture2D::Bind(GLuint textureUnit)
{
	glActiveTexture(GL_TEXTURE0 + textureUnit);
	glBindTexture(GL_TEXTURE_2D, textureHandle);
}

TextureCubemap::TextureCubemap()
{
}

TextureCubemap::TextureCubemap(const std::string & filepath)
{
	glGenTextures(1, &textureHandle);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureHandle);

	{
		DDSImage img = DDSImage::loadDDS((filepath + "negx.dds").c_str());
		glCompressedTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, img.format, img.width, img.height, 0, img.size, img.data);
	}

	{
		DDSImage img = DDSImage::loadDDS((filepath + "posx.dds").c_str());
		glCompressedTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, img.format, img.width, img.height, 0, img.size, img.data);
	}

	{
		DDSImage img = DDSImage::loadDDS((filepath + "negy.dds").c_str());
		glCompressedTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, img.format, img.width, img.height, 0, img.size, img.data);
	}

	{
		DDSImage img = DDSImage::loadDDS((filepath + "posy.dds").c_str());
		glCompressedTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, img.format, img.width, img.height, 0, img.size, img.data);
	}

	{
		DDSImage img = DDSImage::loadDDS((filepath + "negz.dds").c_str());
		glCompressedTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, img.format, img.width, img.height, 0, img.size, img.data);
	}

	{
		DDSImage img = DDSImage::loadDDS((filepath + "posz.dds").c_str());
		glCompressedTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, img.format, img.width, img.height, 0, img.size, img.data);
	}

	glGenerateMipmap(GL_TEXTURE_CUBE_MAP); //this is incredibly slow (cubemap mipmap generation seems to be done in software whereas Texture2D is done in hardware)

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
}

void TextureCubemap::Bind(GLuint textureUnit)
{
	glActiveTexture(GL_TEXTURE0 + textureUnit);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureHandle);
}

Texture::Texture() : textureHandle(0)
{
	
}

Texture::~Texture()
{
	glDeleteTextures(1, &textureHandle);
}

void Texture::Unbind(GLuint textureUnit)
{
	glActiveTexture(GL_TEXTURE0 + textureUnit);
	glBindTexture(GL_TEXTURE_2D, 0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}
