#pragma once

namespace gl_util
{
GLuint load_shader(GLenum type, const std::string& source, bool& status);
GLuint load_shader_from_file(GLenum type, const std::string& file, bool& status);
GLuint link_program(GLuint vert_shader, GLuint frag_shader, bool& status);
}


