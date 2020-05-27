#pragma once

#include <gl/glew.h>

class FrameBuffer
{
protected:
	GLuint fboHandle;
public:
	FrameBuffer();
	//FrameBuffer(GLuint width, GLuint height);
	//FrameBuffer(const FrameBuffer &o) = delete; //TODO only allow move semantics
	~FrameBuffer();

	void Bind();

	static void Unbind();
	const GLuint GetHandle() const;
};

class ColorFrameBuffer : public FrameBuffer
{
public:
	GLuint color;

	ColorFrameBuffer(GLuint width, GLuint height);
	//FrameBuffer(const FrameBuffer &o) = delete; //TODO only allow move semantics
	~ColorFrameBuffer();
};

class ShadowMapPointFrameBuffer : public FrameBuffer
{	
	public:
		ShadowMapPointFrameBuffer(GLuint width, GLuint height);
		~ShadowMapPointFrameBuffer();
};

class ShadowMapSpotFrameBuffer : public FrameBuffer
{
	public:

		GLuint depthMap;

		ShadowMapSpotFrameBuffer(GLuint width, GLuint height);
		~ShadowMapSpotFrameBuffer();
};

class RenderFrameBuffer : public FrameBuffer
{
public:
	union
	{
		struct {
			GLuint color, depthStencil;
		};
		GLuint fboTex[2];
	};

	RenderFrameBuffer(GLuint width, GLuint height);
	//FrameBuffer(const FrameBuffer &o) = delete; //TODO only allow move semantics
	~RenderFrameBuffer();
};

class RenderFrameBufferMultisample : public FrameBuffer
{
private:
	GLuint rbos[2]; //change to renderbuffers
public:
	RenderFrameBufferMultisample(GLuint width, GLuint height);

	~RenderFrameBufferMultisample();
};