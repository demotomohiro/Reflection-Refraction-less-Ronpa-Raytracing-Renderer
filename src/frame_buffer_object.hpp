#pragma once

#include "gl_common.hpp"

namespace gl_util
{

struct frame_buffer_object
{
	frame_buffer_object(GLsizei width, GLsizei height);

	~frame_buffer_object()
	{
		glDeleteRenderbuffers(1, &renderColorBuffer);
		glDeleteFramebuffers(1, &frame_buf_obj);
	}

	bool get_is_success() const
	{
		return is_success;
	}

	bool	is_success;
	GLuint	frame_buf_obj;
	GLuint	renderColorBuffer;
};

struct frame_buffer_object_with_texture
{
	frame_buffer_object_with_texture(GLsizei width, GLsizei height, GLuint read_level);

	~frame_buffer_object_with_texture()
	{
		glDeleteTextures(1, &texture_obj);
		glDeleteFramebuffers(1, &read_frame_buf_obj);
		glDeleteFramebuffers(1, &draw_frame_buf_obj);
	}

	bool get_is_success() const
	{
		return is_success;
	}

	bool	is_success;
	GLuint	draw_frame_buf_obj;
	GLuint	read_frame_buf_obj;
	GLuint	texture_obj;
};

}

