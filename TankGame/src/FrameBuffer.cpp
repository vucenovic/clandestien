#include "FrameBuffer.h"
#include <iostream>

FrameBuffer::FrameBuffer()
{
	glGenFramebuffers(1, &fboHandle);
}

FrameBuffer::~FrameBuffer()
{
	glDeleteFramebuffers(1, &fboHandle);
}

void FrameBuffer::Bind()
{
	glBindFramebuffer(GL_FRAMEBUFFER, fboHandle);
}

void FrameBuffer::Unbind()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

const GLuint FrameBuffer::GetHandle() const
{
	return fboHandle;
}

RenderFrameBufferMultisample::RenderFrameBufferMultisample(GLuint width, GLuint height)
{
	Bind();

	glGenTextures(2, rbos);
	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, rbos[0]);

	glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, 4, GL_RGBA16F, width, height, false);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, rbos[0], 0);

	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, rbos[1]);

	glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, 4, GL_DEPTH24_STENCIL8, width, height, false);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D_MULTISAMPLE, rbos[1], 0);

	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;

	Unbind();
}

RenderFrameBufferMultisample::~RenderFrameBufferMultisample()
{
	glDeleteTextures(2, rbos);
}

RenderFrameBuffer::RenderFrameBuffer(GLuint width, GLuint height)
{
	glGenFramebuffers(1, &fboHandle);
	Bind();

	glGenTextures(2, fboTex);
	glBindTexture(GL_TEXTURE_2D, fboTex[0]);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, fboTex[0], 0);

	glBindTexture(GL_TEXTURE_2D, fboTex[1]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, width, height, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, fboTex[1], 0);

	glBindTexture(GL_TEXTURE_2D, 0);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;

	Unbind();
}

RenderFrameBuffer::~RenderFrameBuffer()
{
	glDeleteTextures(2, fboTex);
}

ColorFrameBuffer::ColorFrameBuffer(GLuint width, GLuint height)
{
	glGenFramebuffers(1, &fboHandle);
	Bind();

	glGenTextures(1, &color);
	glBindTexture(GL_TEXTURE_2D, color);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, color, 0);

	glBindTexture(GL_TEXTURE_2D, 0);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;

	Unbind();
}

ColorFrameBuffer::~ColorFrameBuffer()
{
	glDeleteTextures(1, &color);
}

ShadowMapPointFrameBuffer::ShadowMapPointFrameBuffer(GLuint width, GLuint height)
{
	//glGenFramebuffers(1, &fboHandle);
	//Bind();

	//glGenTextures(1, &fboHandle);
	//glBindTexture(GL_TEXTURE_CUBE_MAP, this->GetHandle);

	//for (unsigned int i = 0; i < 6; ++i)
	//	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT,
	//		width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	//glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	//glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	//glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	//glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	//glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	//glBindFramebuffer(GL_FRAMEBUFFER, &this->GetHandle);
	//glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, &this->GetHandle, 0);
	//glDrawBuffer(GL_NONE);
	//glReadBuffer(GL_NONE);
	//Bind();

}

ShadowMapPointFrameBuffer::~ShadowMapPointFrameBuffer()
{
}

ShadowMapSpotFrameBuffer::ShadowMapSpotFrameBuffer(GLuint width, GLuint height)
{
	glGenFramebuffers(1, &fboHandle);

	glGenTextures(1, &depthMap);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
		width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);


	Bind();
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	Unbind();
}

ShadowMapSpotFrameBuffer::~ShadowMapSpotFrameBuffer()
{
}
