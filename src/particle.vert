#version 430

//1=1光年とする!

//#define WIDTH	1920.0
//#define HEIGHT	1080.0
#define ZNEAR	0.001
#define ZFAR	1.0
#define ZNEAR_H	0.0005
//#define ZNEAR_W (ZNEAR_H*WIDTH/HEIGHT)
#define ZNEAR_W (ZNEAR_H/aspect_rate)

uniform vec2 viewport_scale;
uniform vec2 viewport_offset;
uniform vec2 viewport_size;
uniform float aspect_rate;

mat4 projection =
mat4
(
	2.0*ZNEAR/ZNEAR_W,	0.0,				0.0,							0.0,
	0.0,				2.0*ZNEAR/ZNEAR_H,	0.0,							0.0,
	0.0,				0.0,				-(ZFAR+ZNEAR)/(ZFAR-ZNEAR),		-1.0,
	0.0,				0.0,				-2.0*ZFAR*ZNEAR/(ZFAR-ZNEAR),	0.0
);

const mat4 pvm = projection;

out vec4 vary_color;

void output_star(vec3 star_pos, float star_dim)
{
	gl_Position = pvm * vec4(star_pos, 1.);
	gl_Position.xy = gl_Position.xy * viewport_scale + viewport_offset*gl_Position.w;

	float ps = pvm[0][0] * star_dim * viewport_scale.x;
	gl_PointSize = viewport_size.x*0.5*ps / gl_Position.w;
}

void test_star()
{
	float star_dim;
	vec3 star_pos;
	if(gl_VertexID==0)
	{
		star_pos = vec3(0.000125, 0.000125, -ZNEAR);
		star_dim = ZNEAR_H*0.5;
	}else if(gl_VertexID==1)
	{
		star_pos = vec3(0.000125, -0.000125, -ZNEAR*2.0);
		star_dim = ZNEAR_H*0.5;
	}else
	{
		float z = gl_VertexID * 0.001;
		star_pos = vec3(-0.0005, -0.0005, -ZNEAR*2.0-z);
		star_dim = ZNEAR_H*0.2;
	}

	output_star(star_pos, star_dim);
}

void main()
{
#if 0
	if(gl_VertexID==0)
		gl_Position = vec4(0.5, 0.0, 0.0, 1.0);
	else if(gl_VertexID==1)
		gl_Position = vec4(1.0, 0.0, 0.0, 1.0);
	else
	{
		float x = gl_VertexID / 10.0;
		gl_Position = vec4(x, x, 0.0, 1.0);
	}
#endif
//	gl_Position = vec4(0.00025, 0.00025, -gl_VertexID*0.0001-0.001, 1.0);

	test_star();
}

