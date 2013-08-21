#pragma once

#include <cassert>

namespace gl_util
{

class scoped_shader
{
public:

	scoped_shader(GLuint shader=0):shader(shader)
	{
	}

	~scoped_shader()
	{
		delete_shader();
	}

	operator GLuint() const
	{
		return shader;
	}

	void operator=(GLuint shader)
	{
		if(this->shader == shader)
			return;

		delete_shader();
		this->shader = shader;
	}

	bool operator!() const
	{
		return this->shader == 0;
	}

private:

	void delete_shader();

	GLuint shader;
};

class scoped_program
{
public:

	scoped_program(GLuint program=0):program(program)
	{
		assert(glIsProgram(program) || program == 0);
	}

	~scoped_program();

	operator GLuint() const
	{
		return program;
	}

	scoped_program& operator = (GLuint program)
	{
		assert(glIsProgram(program) == GL_TRUE || program == 0);
		assert(glIsProgram(this->program) == GL_TRUE || this->program == 0);

		if(this->program == program)
			return *this;

		glDeleteProgram(this->program);
		this->program = program;

		return *this;
	}

private:

	GLuint program;
};

void		set_shader_source(GLuint shader, const char* source);
GLuint		get_shader_obj(GLenum type, const char* source);
GLuint		get_shader_obj(GLenum type, const std::string& source);

bool		compile_shader(GLuint shader);

std::string	get_shader_info_log(GLuint shader);
GLuint		get_program_obj(GLuint shader0, GLuint shader1);
bool		link_program(GLuint program);
std::string	get_program_info_log(GLuint program);

}

