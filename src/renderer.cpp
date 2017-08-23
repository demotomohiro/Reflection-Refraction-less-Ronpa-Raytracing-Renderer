#include "renderer.hpp"
#include "gl_common.hpp"
#include "options.hpp"
#include "GLUtil/shader.hpp"
#include "GLUtil/cStdShader.hpp"
#include "GLUtil/contextUtil.hpp"

#include <cassert>
#include <iostream>

using namespace std;

namespace
{
void print_gl_info()
{
    GLUtil::print_context_info();

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

}

renderer::renderer(const options& opts):
    ret(1),
    is_render(false)
{
    if(!opts.hide_gl_info)
    {
        print_gl_info();
    }

    GLUtil::enable_debug_message_cerr_out_if_available();
    if(!init_fullscreen_program(opts))
    {
        cerr << "Failed to initialize fullscreen program\n";
        return;
    }
    if(!init_particle_program(opts))
    {
        cerr << "Failed to initialize particle program\n";
        return;
    }

    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    const render_info& ri = opts.rinfo;
    GL_CALL(glPixelStorei(GL_PACK_ALIGNMENT, 1));
    GL_CALL(glPixelStorei(GL_PACK_ROW_LENGTH, ri.output_w));

    ret = 0;
    is_render = true;
}

void renderer::set_iFrame(const int iFrame)
{
    if(fullscreen_iFrame_loc != -1)
    {
        assert(glIsProgram(program));
        glUseProgram(program);

        glUniform1i(fullscreen_iFrame_loc, iFrame);
    }
}

void renderer::partial_draw_fullscreen(
    const float coord_offset_x, const float coord_offset_y) const
{
    assert(glIsProgram(program));
    glUseProgram(program);
    if(coord_offset_loc != -1)
    {
        const float offset[] = {coord_offset_x, coord_offset_y};
        glUniform2fv(coord_offset_loc, 1, offset);
    }
    GL_CALL(glDrawArrays(GL_TRIANGLES, 0, 3));
}

void renderer::prepare_draw_particles(
    GLsizei i, GLsizei j, const render_info& ri) const
{
    assert(glIsProgram(particle_program));

    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE);
    glUseProgram(particle_program);
    if(viewport_offset_loc != -1)
    {
        glUniform2f
        (
            viewport_offset_loc,
            (float)((int)ri.num_tile_x-(int)i*2-1),
            (float)((int)ri.num_tile_y-(int)j*2-1)
        );
    }
}

void renderer::partial_draw_particles(
    GLsizei k, const GLsizei num_particles_per_draw) const
{
    if(vertexID_offset_loc != -1)
        glUniform1i
        (
            vertexID_offset_loc,
            num_particles_per_draw*k
        );
    GL_CALL(glDrawArrays(GL_POINTS, 0, num_particles_per_draw));
    glFinish();	//Prevent TDR(Timeout Detection and Recovery)
}

bool renderer::init_fullscreen_program(const options& opts)
{
    using namespace GLUtil;

    bool status;
    scoped_shader vert_shader(load_shader(GL_VERTEX_SHADER, vert_shader_source, status, opts.macro_definitions));
    if(!status)
    {
        return false;
    }

    scoped_shader frag_shader(
        load_shader_from_file(
            GL_FRAGMENT_SHADER, opts.source_file, status, opts.macro_definitions));
    if(!status)
    {
        return false;
    }

    program = link_program(vert_shader, frag_shader, status);
    if(!status)
    {
        return false;
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

    coord_offset_loc = glGetUniformLocation(program, "coord_offset");
    if(coord_offset_loc == -1)
    {
        cerr << "Warning, add & use \"uniform vec2 coord_offset;\" in your fullscreen fragment shader.\n";
    }

    fullscreen_iFrame_loc = glGetUniformLocation(program, "iFrame");

    return true;
}

bool renderer::init_particle_program(const options& opts)
{
    using namespace GLUtil;

    if(!opts.is_draw_particles)
    {
        return true;
    }

    bool status;
    scoped_shader vert_shader(
        load_shader_from_file(
            GL_VERTEX_SHADER, opts.particle_vert_source_file, status, opts.macro_definitions));
    if(!status)
    {
        return false;
    }

    scoped_shader frag_shader(
        load_shader_from_file(
            GL_FRAGMENT_SHADER, opts.particle_frag_source_file, status, opts.macro_definitions));
    if(!status)
    {
        return false;
    }

    particle_program = link_program(vert_shader, frag_shader, status);
    if(!status)
    {
        return false;
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

    viewport_offset_loc = glGetUniformLocation(particle_program, "viewport_offset");
    if(viewport_offset_loc == -1)
    {
        cerr << "Warning, add & use \"uniform vec2 viewport_offset;\" in your particle vertex shader.\n";
    }

    vertexID_offset_loc = glGetUniformLocation(particle_program, "vertexID_offset");
    if(vertexID_offset_loc == -1)
    {
        cerr << "Warning, add & use \"uniform int vertexID_offset;\" in your particle vertex shader.\n";
    }

    return true;
}

