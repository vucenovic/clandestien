#pragma once

#include <gl/glew.h>

class FrameBuffer //Increase abstraction of framebuffer baseclass to make using different types of framebuffers easier TODO
{
private:
	GLuint fboHandle;
public:
	GLuint fboTex[2];//increase abstraction TODO

	FrameBuffer(GLuint width, GLuint height);
	//FrameBuffer(const FrameBuffer &o) = delete; //TODO only allow move semantics
	~FrameBuffer();

	void Bind();

	static void Unbind();
};