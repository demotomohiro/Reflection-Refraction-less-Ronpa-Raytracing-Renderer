#pragma once

#include <string>
#include <vector>

namespace gl_util
{
GLuint load_shader(GLenum type, const std::string& source, const std::vector<std::string>& macro_definitions, bool& status);
GLuint load_shader_from_file(GLenum type, const std::string& file, const std::vector<std::string>& macro_definitions, bool& status);
GLuint link_program(GLuint vert_shader, GLuint frag_shader, bool& status);
}


