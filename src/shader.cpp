#include <string>
#include <vector>

#include "gl_common.hpp"
#include "shader.hpp"

using namespace std;
using namespace gl_util;

void gl_util::scoped_shader::delete_shader()
{
	glDeleteShader(shader);
}

gl_util::scoped_program::~scoped_program()
{
	glDeleteProgram(program);
}

void gl_util::set_shader_source(GLuint shader, const char* source)
{
	const char* string[] = {source};
	glShaderSource(shader, 1, string, NULL);
}

GLuint gl_util::get_shader_obj(GLenum type, const char* source)
{
	GLuint shader = glCreateShader(type);
	if(shader==0)
	{
		return 0;
	}

	set_shader_source(shader, source);
	return shader;
}

GLuint gl_util::get_shader_obj(GLenum type, const std::string& source)
{
	return get_shader_obj(type, source.c_str());
}

bool gl_util::compile_shader(GLuint shader)
{
	glCompileShader(shader);

	GLint success;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
	return success == GL_TRUE;
}

string gl_util::get_shader_info_log(GLuint shader)
{
	GLint log_len;
	glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &log_len);

	if(log_len==0)
		return string();

	vector<char> log(log_len);
	glGetShaderInfoLog(shader, log_len, NULL, &log[0]);

	return string(log.begin(), log.end());
}

GLuint gl_util::get_program_obj(GLuint shader0, GLuint shader1)
{
	GLuint program = glCreateProgram();
	if(program == 0)
	{
		return 0;
	}

	glAttachShader(program, shader0);
	glAttachShader(program, shader1);

	return program;
}

bool gl_util::link_program(GLuint program)
{
	glLinkProgram(program);

	GLint success;
	glGetProgramiv(program, GL_LINK_STATUS, &success);
	return success==GL_TRUE;
}

string gl_util::get_program_info_log(GLuint program)
{
	GLint log_len;
	glGetProgramiv(program, GL_INFO_LOG_LENGTH, &log_len);

	if(log_len==0)
		return string();

	vector<char> log(log_len);
	glGetProgramInfoLog(program, log_len, NULL, &log[0]);

	return string(log.begin(), log.end());
}

