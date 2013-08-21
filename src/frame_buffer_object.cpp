#include <iostream>

#include "gl_common.hpp"
#include "frame_buffer_object.hpp"

using namespace std;

gl_util::frame_buffer_object::frame_buffer_object(GLsizei width, GLsizei height):
	is_success(false), frame_buf_obj(0), renderColorBuffer(0)
{
	glGenFramebuffers(1, &frame_buf_obj);
	glBindFramebuffer(GL_FRAMEBUFFER, frame_buf_obj);

	glGenRenderbuffers(1, &renderColorBuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, renderColorBuffer);

	glRenderbufferStorageMultisample(GL_RENDERBUFFER, 0, GL_RGB8, width, height);

	glFramebufferRenderbuffer
	(
		GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
		GL_RENDERBUFFER, renderColorBuffer
	);

	glFramebufferRenderbuffer
	(
		GL_READ_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
		GL_RENDERBUFFER, renderColorBuffer
	);

	GLenum completeness = glCheckFramebufferStatus(GL_DRAW_FRAMEBUFFER);
	if(completeness != GL_FRAMEBUFFER_COMPLETE)
	{
		cerr << "GL_DRAW_FRAMEBUFFER is incomplete" << endl;
		return;
	}
	completeness = glCheckFramebufferStatus(GL_READ_FRAMEBUFFER);
	if(completeness != GL_FRAMEBUFFER_COMPLETE)
	{
		cerr << "GL_READ_FRAMEBUFFER is incomplete" << endl;
		return;
	}

	glViewport(0, 0, width, height);

	is_success = true;
}

gl_util::frame_buffer_object_with_texture::frame_buffer_object_with_texture
(
	GLsizei width, GLsizei height, GLuint read_level
):
	is_success(false), draw_frame_buf_obj(0), read_frame_buf_obj(0), texture_obj(0)
{
	glGenFramebuffers(1, &draw_frame_buf_obj);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, draw_frame_buf_obj);

	glGenFramebuffers(1, &read_frame_buf_obj);
	glBindFramebuffer(GL_READ_FRAMEBUFFER, read_frame_buf_obj);

	glGenTextures(1, &texture_obj);
	glBindTexture(GL_TEXTURE_2D, texture_obj);

	GL_CALL(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width, height, 0, GL_RGBA, GL_FLOAT, NULL));
	glGenerateMipmap(GL_TEXTURE_2D);

	glFramebufferTexture2D
	(
		GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture_obj, 0
	);
	glFramebufferTexture2D
	(
		GL_READ_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture_obj, read_level
	);

	GLenum completeness = glCheckFramebufferStatus(GL_DRAW_FRAMEBUFFER);
	if(completeness != GL_FRAMEBUFFER_COMPLETE)
	{
		cerr << "GL_DRAW_FRAMEBUFFER is incomplete" << endl;
		return;
	}
	completeness = glCheckFramebufferStatus(GL_READ_FRAMEBUFFER);
	if(completeness != GL_FRAMEBUFFER_COMPLETE)
	{
		cerr << "GL_READ_FRAMEBUFFER is incomplete" << endl;
		return;
	}

	glViewport(0, 0, width, height);

	is_success = true;
}

