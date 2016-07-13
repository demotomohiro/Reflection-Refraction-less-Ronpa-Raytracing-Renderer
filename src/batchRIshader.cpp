#pragma warning( disable: 4503 4996)	//‚±‚Ìwarning‚Ípush,pop‚ÅˆÍ‚Þ‚Ædisable‚Å‚«‚È‚¢?
#pragma warning( push, 1 )

#include <cassert>
#include <iostream>
#include <vector>
#include <string>
#include <chrono>

#pragma warning( pop )

#include "gl_common.hpp"
#include "glcontext.hpp"
#include "shader.hpp"
#include "cstd_shader.hpp"
#include "frame_buffer_object.hpp"
#include "image_file.hpp"
#include "options.hpp"

using namespace std;

namespace
{
void APIENTRY gl_debug_callback(
	GLenum source, GLenum type, GLuint id,
	GLenum severity,
	GLsizei length, const char *message,
	const GLvoid * /*userParam*/)
{
	(source);
	(type);
	(id);
	(severity);
	(length);

	cerr << "Message from glDebugMessageCallback:\n";
	cerr <<
		"In " << get_gl_call_info().funcname << ", " <<
		get_gl_call_info().filename << ":" <<
		get_gl_call_info().line <<
		endl;

	cerr << message << endl;
}

void init_gl()
{
	GLint cntxt_flags;
	glGetIntegerv(GL_CONTEXT_FLAGS, &cntxt_flags);
	if(cntxt_flags & GL_CONTEXT_FLAG_DEBUG_BIT && glIsEnabled(GL_DEBUG_OUTPUT)==GL_TRUE)
	{
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
		GLuint ids = 0;
		glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, &ids, GL_TRUE);
		glDebugMessageCallback(&gl_debug_callback, NULL);
	}
}

void print_gl_info()
{
	GLint contextProfileMask;
	glGetIntegerv(GL_CONTEXT_PROFILE_MASK, &contextProfileMask);
	string contextProfileMaskStr;
	if(contextProfileMask & GL_CONTEXT_CORE_PROFILE_BIT)
		contextProfileMaskStr += "GL_CONTEXT_CORE_PROFILE_BIT ";
	if(contextProfileMask & GL_CONTEXT_COMPATIBILITY_PROFILE_BIT)
		contextProfileMaskStr += "CONTEXT_COMPATIBILITY_PROFILE_BIT ";

	GLint contextFlags;
	glGetIntegerv(GL_CONTEXT_FLAGS, &contextFlags);
	string contextFlagsStr;
	if(contextFlags & GL_CONTEXT_FLAG_DEBUG_BIT)
		contextFlagsStr += "GL_CONTEXT_FLAG_DEBUG_BIT ";
	if(contextFlags & GL_CONTEXT_FLAG_FORWARD_COMPATIBLE_BIT)
		contextFlagsStr += "GL_CONTEXT_FLAG_FORWARD_COMPATIBLE_BIT ";

	cout <<
		"\nGL_VERSION:\n\t" <<
			reinterpret_cast<const char*>(glGetString(GL_VERSION)) <<
		"\nGL_SHADING_LANGUAGE_VERSION:\n\t" <<
			reinterpret_cast<const char*>(glGetString(GL_SHADING_LANGUAGE_VERSION)) <<
		"\nGL_RENDERER:\n\t" <<
			reinterpret_cast<const char*>(glGetString(GL_RENDERER)) <<
		"\nGL_VENDOR:\n\t" <<
			reinterpret_cast<const char*>(glGetString(GL_VENDOR)) <<
		"\nGL_CONTEXT_PROFILE_MASK:\n\t" <<
			contextProfileMaskStr	<<
		"\nGL_CONTEXT_FLAGS:\n\t" <<
			contextFlagsStr <<
		endl
		;

	GLint major_ver;
	GLint minor_ver;
	glGetIntegerv(GL_MAJOR_VERSION, &major_ver);
	glGetIntegerv(GL_MINOR_VERSION, &minor_ver);
	cout << "OpenGL version:" << major_ver << "." << minor_ver << endl;

	GLfloat point_size_range[2];
	glGetFloatv(GL_POINT_SIZE_RANGE, point_size_range); 
	cout << "GL_POINT_SIZE_RANGE: " << point_size_range[0] << ", " << point_size_range[1] << endl;
}

const char* vert_shader_source =
"#version 430\n"
"\n"
"out float v;"
"void main()\n"
"{\n"
"	v = 3.3;\n"
"	if(gl_VertexID==0)\n"
"		gl_Position=vec4(-1.0, -1.0, 0.0, 1.0);\n"
"	else if(gl_VertexID==1)\n"
"		gl_Position=vec4(3.0, -1.0, 0.0, 1.0);\n"
"	else\n"
"		gl_Position=vec4(-1.0, 3.0, 0.0, 1.0);\n"
"}\n"
;

struct renderer
{
	renderer(const options& opts):
		ret(1),
		is_render(false)
	{
		if(!cntxt.get_is_success())
		{
			cerr << "Failed to initialize OpenGL context\n";
			return;
		}

		print_gl_info();
		init_gl();
		init_fullscreen_program(opts);
		init_particle_program(opts);

		GLuint vao;
		glGenVertexArrays(1, &vao);
		glBindVertexArray(vao);

		const render_info& ri = opts.rinfo;
		GL_CALL(glPixelStorei(GL_PACK_ALIGNMENT, 1));
		GL_CALL(glPixelStorei(GL_PACK_ROW_LENGTH, ri.output_w));

		ret = 0;
		is_render = true;
	}

	int get_ret() const
	{
		return ret;
	}

	bool get_is_render() const
	{
		return is_render;
	}

	GLuint get_program() const
	{
		assert(glIsProgram(program));
		return program;
	}

	GLuint get_particle_program() const
	{
		assert(glIsProgram(particle_program));
		return particle_program;
	}

private:

	void	init_fullscreen_program(const options& opts)
	{
		using namespace gl_util;

		bool status;
		scoped_shader vert_shader(load_shader(GL_VERTEX_SHADER, vert_shader_source, status));
		if(!status)
		{
			return ;
		}

		scoped_shader frag_shader(
			load_shader_from_file(
				GL_FRAGMENT_SHADER, opts.source_file, status));
		if(!status)
		{
			return;
		}

		program = link_program(vert_shader, frag_shader, status);
		if(!status)
		{
			return;
		}

		glUseProgram(program);

		const render_info& ri = opts.rinfo;
		const GLsizei spr_smpl_w = ri.get_super_sampling_width();

		GLint loc = glGetUniformLocation(program, "resolution");
		if(loc==-1)
		{
			cerr << "Warning, add & use \"uniform vec2 resolution;\" in your fragment shader.\n";
		}else
		{
			const float res[] = {(float)ri.output_w*spr_smpl_w, (float)ri.output_h*spr_smpl_w};
			glUniform2fv(loc, 1, res);
		}
	}

	void	init_particle_program(const options& opts)
	{
		using namespace gl_util;

		if(!opts.is_draw_particles)
		{
			return;
		}

		bool status;
		scoped_shader vert_shader(
			load_shader_from_file(
				GL_VERTEX_SHADER, opts.particle_vert_source_file, status));
		if(!status)
		{
			return ;
		}

		scoped_shader frag_shader(
			load_shader_from_file(
				GL_FRAGMENT_SHADER, opts.particle_frag_source_file, status));
		if(!status)
		{
			return;
		}

		particle_program = link_program(vert_shader, frag_shader, status);
		if(!status)
		{
			return;
		}

		glUseProgram(particle_program);
		glEnable(GL_PROGRAM_POINT_SIZE);

		const render_info& ri = opts.rinfo;
		{
			const GLint loc = glGetUniformLocation(particle_program, "viewport_scale");
			if(loc == -1)
			{
				cerr << "Warning, add & use \"uniform vec2 viewport_scale;\" in your particle vertex shader.\n";
			}
			glUniform2f(loc, (float)ri.num_tile_x, (float)ri.num_tile_y);
		}
		{
			const GLint loc = glGetUniformLocation(particle_program, "viewport_size");
			if(loc == -1)
			{
				cerr << "Warning, add & use \"uniform vec2 viewport_size;\" in your particle vertex shader.\n";
			}
			glUniform2f(loc, (GLfloat)ri.get_draw_w(), (GLfloat)ri.get_draw_h());
		}
		{
			const GLint loc = glGetUniformLocation(particle_program, "aspect_rate");
			if(loc == -1)
			{
				cerr << "Warning, add & use \"uniform float aspect_rate;\" in your particle vertex shader.\n";
			}
			const GLfloat aspect_rate = (GLfloat)ri.output_h/(GLfloat)ri.output_w;
			glUniform1f(loc, aspect_rate);
		}
	}

	int						ret;
	bool					is_render;
	gl_util::glcontext		cntxt;
	gl_util::scoped_program	program;
	gl_util::scoped_program	particle_program;
};

}


int main(int argc, char* argv[])
{
	using namespace gl_util;
	using namespace std::chrono;

	options opts(argc, argv);
	if(!opts.is_render)
	{
		return opts.ret_code;
	}

	renderer r(opts);
	if(!r.get_is_render())
	{
		return r.get_ret();
	}

	GLuint prog;
	glGetIntegerv(GL_CURRENT_PROGRAM, reinterpret_cast<GLint*>(&prog));
	assert(glIsProgram(prog)==GL_TRUE);

	const GLint coord_offset_loc = glGetUniformLocation(r.get_program(), "coord_offset");
	if(coord_offset_loc == -1)
	{
		cerr << "Warning, add & use \"uniform vec2 coord_offset;\" in your fullscreen fragment shader.\n";
	}

	const GLint viewport_offset_loc = 
		opts.is_draw_particles ?
		[&r](){
			GL_CALL(glUseProgram(r.get_particle_program()));
			const GLint loc = glGetUniformLocation(r.get_particle_program(), "viewport_offset");
			if(loc == -1)
			{
				cerr << "Warning, add & use \"uniform vec2 viewport_offset;\" in your particle vertex shader.\n";
			}
			return loc;
		}()
		:
		-1;

	const GLint vertexID_offset_loc = 
		opts.is_draw_particles ?
		[&r](){
			GL_CALL(glUseProgram(r.get_particle_program()));
			const GLint loc = glGetUniformLocation(r.get_particle_program(), "vertexID_offset");
			if(loc == -1)
			{
				cerr << "Warning, add & use \"uniform int vertexID_offset;\" in your particle vertex shader.\n";
			}
			return loc;
		}()
		:
		-1;

	const render_info& ri = opts.rinfo;

//	frame_buffer_object fbo
	frame_buffer_object_with_texture fbo
	(
		ri.get_draw_w(), ri.get_draw_h(), ri.super_sampling_level
	);

	if(!fbo.get_is_success())
	{
		return false;
	}

	std::vector<unsigned char> color_buf(ri.output_w*ri.output_h*3);

	cout << "Rendering ..." << endl;
	const steady_clock::time_point render_begin = steady_clock::now();

	const GLsizei tile_w = ri.get_tile_width();
	const GLsizei tile_h = ri.get_tile_height();
	for(GLsizei j=0; j<ri.num_tile_y; ++j)
	for(GLsizei i=0; i<ri.num_tile_x; ++i)
	{
		GL_CALL(glUseProgram(r.get_program()));
		if(coord_offset_loc != -1)
		{
			const float offset[] = {(float)i*ri.get_draw_w(), (float)j*ri.get_draw_h()};
			glUniform2fv(coord_offset_loc, 1, offset);
		}
		GL_CALL(glDrawArrays(GL_TRIANGLES, 0, 3));
		if(opts.is_draw_particles)
		{
			glEnable(GL_BLEND);
			glBlendFunc(GL_ONE, GL_ONE);
			GL_CALL(glUseProgram(r.get_particle_program()));
			if(viewport_offset_loc != -1)
			{
				glUniform2f
				(
					viewport_offset_loc,
					(float)((int)ri.num_tile_x-(int)i*2-1),
					(float)((int)ri.num_tile_y-(int)j*2-1)
				);
			}
			const GLsizei num_particles_per_draw = opts.num_particles / opts.num_div_particles;
			for(GLsizei k=0; k<opts.num_div_particles; ++k)
			{
				if(vertexID_offset_loc != -1)
					glUniform1i
					(
						vertexID_offset_loc,
						num_particles_per_draw*k
					);
				GL_CALL(glDrawArrays(GL_POINTS, 0, num_particles_per_draw));
				glFinish();	//TDR‘Îô.
			}
			glDisable(GL_BLEND);
		}

		glGenerateMipmap(GL_TEXTURE_2D);
		GL_CALL(glPixelStorei(GL_PACK_SKIP_PIXELS,	i*tile_w));
		GL_CALL(glPixelStorei(GL_PACK_SKIP_ROWS,	j*tile_h));

		GL_CALL
		(
			glReadPixels
			(
				0,		0,
				tile_w,	tile_h,
				GL_RGB, GL_UNSIGNED_BYTE, &color_buf[0]
			)
		);
	}

	const steady_clock::time_point render_end = steady_clock::now();
	cout	<< "Rendering completed in "
			<< duration_cast<duration<double>>(render_end - render_begin).count()
			<< " seconds." << endl;
	cout << "Writing to file" << endl;

//	cout << color_buf[0] << endl;
	if(!write_image(ri.output_w, ri.output_h, opts.output_file, color_buf.data()))
	{
		cerr << "Failed to write file!\n";
		return 1;
	}

	const steady_clock::time_point writing_end = steady_clock::now();
	cout	<< "completed in "
			<< duration_cast<duration<double>>(writing_end - render_end).count()
			<< " seconds." << endl;

	return 0;
}

