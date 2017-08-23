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
#include "frame_buffer_object.hpp"
#include "image_file.hpp"
#include "options.hpp"
#include "renderer.hpp"

using namespace std;

namespace
{
class progress_display
{
public:
    progress_display(int num_steps):
        num_steps(num_steps)
    {
    }

    void operator++()
    {
        std::cout << "Progress: " << ++i << '/' << num_steps << '\r' << std::flush;
    }

private:
    const int num_steps;
    int i = 0;
};

}

bool render_one_frame(
    const options& opts, const renderer& r, vector<unsigned char>& color_buf)
{
	using namespace std::chrono;

	const render_info& ri = opts.rinfo;

    progress_display progress(ri.num_tile_x * ri.num_tile_y * (1+(opts.is_draw_particles ? opts.num_div_particles : 0)));

	cout << "Rendering ..." << endl;
	const steady_clock::time_point render_begin = steady_clock::now();

	const GLsizei tile_w = ri.get_tile_width();
	const GLsizei tile_h = ri.get_tile_height();
	for(GLsizei j=0; j<ri.num_tile_y; ++j)
	for(GLsizei i=0; i<ri.num_tile_x; ++i)
	{
        r.partial_draw_fullscreen((float)i*ri.get_draw_w(), (float)j*ri.get_draw_h());
        ++progress;

		if(opts.is_draw_particles)
		{
            r.prepare_draw_particles(i, j, ri);
			const GLsizei num_particles_per_draw = opts.num_particles / opts.num_div_particles;
			for(GLsizei k=0; k<opts.num_div_particles; ++k)
			{
                r.partial_draw_particles(k, num_particles_per_draw);
                ++progress;
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
		return false;
	}

	const steady_clock::time_point writing_end = steady_clock::now();
	cout	<< "completed in "
			<< duration_cast<duration<double>>(writing_end - render_end).count()
			<< " seconds." << endl;

    return true;
}

int main(int argc, char* argv[])
{
	using namespace gl_util;

	options opts(argc, argv);
	if(!opts.is_render)
	{
		return opts.ret_code;
	}

    gl_util::glcontext cntxt;
    if(!cntxt.get_is_success())
    {
        cerr << "Failed to initialize OpenGL context\n";
        return 1;
    }

	renderer r(opts);
	if(!r.get_is_render())
	{
		return r.get_ret();
	}

	GLuint prog;
	glGetIntegerv(GL_CURRENT_PROGRAM, reinterpret_cast<GLint*>(&prog));
	assert(glIsProgram(prog)==GL_TRUE);

	const render_info& ri = opts.rinfo;

//	frame_buffer_object fbo
	frame_buffer_object_with_texture fbo
	(
		ri.get_draw_w(), ri.get_draw_h(), ri.super_sampling_level
	);

	if(!fbo.get_is_success())
	{
		return 1;
	}

	std::vector<unsigned char> color_buf(ri.output_w*ri.output_h*3);

    if(!render_one_frame(opts, r, color_buf))
    {
        return 1;
    }

	return 0;
}

