#version 430

#include "noise.s"

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

vec3 Hue( float hue )
{
	vec3 rgb = fract(hue + vec3(0.0,2.0/3.0,1.0/3.0));

	rgb = abs(rgb*2.0-1.0);
		
	return clamp(rgb*3.0-1.0,0.0,1.0);
}

vec3 HSVtoRGB( vec3 hsv )
{
	return ((Hue(hsv.x)-1.0)*hsv.y+1.0) * hsv.z;
}

vec3 HSVtoRGB(float h, float s, float v)
{
	return HSVtoRGB(vec3(h, s, v));
}

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

vec3 rand3(float p)
{
	const vec3 x = vec3(0.01, 0.013, 0.051);
	const vec3 y = vec3(0.031, 0.027, 0.007);
	const vec3 z = vec3(0.023, 0.0115, 0.0213);

	return
		vec3(
			rand(fract(vec3(p)+vec3(x))), 
			rand(fract(vec3(p)+vec3(y))), 
			rand(fract(vec3(p)+vec3(z))));
}

void gen_star()
{
	float vid = gl_VertexID / 8000000.0;
	float size = ZFAR - ZNEAR;
	vec3 centor = vec3(0.0, 0.0, -(ZFAR + ZNEAR)*0.5);
	vec3 star_pos = (rand3(vid) - vec3(0.5)) * vec3(size) + centor;
	float star_dim = ZNEAR_H*2.;
	output_star(star_pos, star_dim);

	float h = rand(vec3(vid+0.03));
	float s = rand(vec3(vid+0.02))*0.6;
	float v = rand(vec3(vid+0.01))*0.2 + 0.8;
	vary_color = vec4(HSVtoRGB(h, s, v), 1.0);
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

//	test_star();
	gen_star();
}

