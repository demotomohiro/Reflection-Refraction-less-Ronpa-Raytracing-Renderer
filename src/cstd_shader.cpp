#pragma warning( disable: 4503 4996)	//このwarningはpush,popで囲むとdisableできない?
#pragma warning( push, 1 )

#include <fstream>
#include <iostream>
#include <string>

#pragma warning( pop )

#include "gl_common.hpp"
#include "cstd_shader.hpp"
#include "shader.hpp"
#define TOFU_OPENGL_GLSL_PREPROCESSOR_ENABLE_EMIT_LINE_DIRECTIVES 1
#include "glsl_preprocessor.hpp"

GLuint gl_util::load_shader(GLenum type, const std::string& source, const std::vector<std::string>& macro_definitions, bool& status)
{
	using namespace gl_util;
	using namespace std;

	status = false;

	cout << "Loading " <<
		(
		type == GL_VERTEX_SHADER	? "vertex" :
		type == GL_FRAGMENT_SHADER	? "fragment" :
		"nazo"
		)
		<<
		" shader ... ";

	const string pp_source = tofu::glsl::glsl_preprocessor(source, status, {"../shaders/include", "."}, macro_definitions);
    if(!status)
    {
        return 0;
    }

	GLuint shader = get_shader_obj(type, pp_source);
	if(shader == 0)
	{
		cerr << "\nFailed to create shader\n";
		return 0;
	}

	status = compile_shader(shader);
	cout << (status ? "Success!\n" : "Failed!\n");
	const string log = get_shader_info_log(shader);
	if(log.length() > 1)
	{
		cerr << "Arigatai message from GLSL compiler:\n";
		cerr << log << endl;
	}

	return shader;
}

GLuint gl_util::load_shader_from_file(GLenum type, const std::string& file, const std::vector<std::string>& macro_definitions, bool& status)
{
	using namespace std;

	status = false;
	string source;

    if(file != "-")
    {
        cout << "Loading " << file << endl;
        ifstream ifs(file);
        if(!ifs.good())
        {
            cerr << "Failed to read the file!\n";
            return 0;
        }
        source = string(
            std::istreambuf_iterator<char>(ifs.rdbuf()),
            std::istreambuf_iterator<char>());
    }else
    {
        cout << "Loading from stdin" << endl;
        //Load until '\0' or EOF so that subpross.Popen in Python can send multiple input.
        getline(cin, source, '\0');
    }

	return gl_util::load_shader(type, source, macro_definitions, status);
}

GLuint gl_util::link_program(GLuint vert_shader, GLuint frag_shader, bool& status)
{
	using namespace std;

	cout << "Linking shader objects to 1 program object ... ";
	const GLuint	program = get_program_obj(vert_shader, frag_shader);
	status = link_program(program);
	cout << (status ? "Success!\n" : "Failed!\n");
	const string log = get_program_info_log(program);
	if(log.length() > 1)
	{
		cerr << "Musidekinai message from GLSL linker:\n";
		cerr << log << endl;
	}

	return program;
}

