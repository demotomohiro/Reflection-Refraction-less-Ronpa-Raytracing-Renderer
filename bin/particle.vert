#version 430

#include "noise.s"

//1=1光年とする!

//#define WIDTH	1920.0
//#define HEIGHT	1080.0
#define ZNEAR	0.001
#define ZFAR	1.0
#define ZNEAR_W 0.001
#define ZNEAR_H	(ZNEAR_W*aspect_rate)

uniform vec2 viewport_scale;
uniform vec2 viewport_offset;
uniform vec2 viewport_size;
uniform float aspect_rate;
uniform int	 vertexID_offset;

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

vec3 star_pos_mod(vec3 pos)
{
	const float size = 10.0;

	vec3 p = pos;
	vec3 scaled		= p*vec3(size);
#if 0
	vec3 floored	= floor(scaled) / vec3(size);
	vec3 fr			= fract(scaled);
	if(fr.x > fr.y && fr.x > fr.z)
	{
		p.y = floored.y;
		p.z = floored.z;
	}else if(fr.y > fr.z)
	{
		p.x = floored.x;
		p.z = floored.z;
	}else
	{
		p.x = floored.x;
		p.y = floored.y;
	}
#else
	vec3 floored	= floor(scaled+vec3(0.5)) / vec3(size);
	vec3 fr			= abs(fract(scaled) - vec3(0.5));
	if(fr.z < fr.x && fr.z < fr.y)
	{
		p.x = floored.x;
		p.y = floored.y;
	}else if(fr.x < fr.y)
	{
		p.y = floored.y;
		p.z = floored.z;
	}else
	{
		p.z = floored.z;
		p.x = floored.x;
	}
#endif

	p.x -= p.y;
	p.z -= p.x;
	return mix(p, pos, 0.04);
}

vec3 star_pos_rand3(float vid)
{
	vec3 centor = vec3(0.0, 0.0, -0.5);
	return rand3(vid) - vec3(0.5) + centor;
}

vec3 star_pos_rand2(float vid)
{
	vec2 uv = rand3(vid).yx;
	float theta = uv.x * 17.0;

#if 0
//	return vec3(cos(theta), sin(theta), uv.y - 1.0)*vec2(0.45/(theta+8.0), 1.0).xxy;
	float rad = 1.0/(theta+12.0)*12.0*0.14;
//	float rad = 0.14;
	vec3 spread = rand3(vid)*rad*0.1;
//	return vec3(rad*cos(theta), sqrt(uv.y)*0.04-0.08, rad*sin(theta)-0.32) + spread;
#else
	float rad = 1.0/(theta+6.0)*6.0*0.07;
	vec3 spread = rand3(vid)*rad*0.8;
	float width = (uv.y*uv.y)*0.07;
	rad += width * (1.0 - uv.x);
	float height = -width * uv.x * uv.x * 2.0;
	return vec3(rad*cos(theta), -0.07+theta*0.005+height, rad*sin(theta)-0.32) + spread;
#endif
}

void gen_star()
{
	float vid = (gl_VertexID + vertexID_offset) / 8000000.0;
	vec3 star_pos = vid < 0.5 ? star_pos_rand3(vid) : star_pos_rand2(vid);
//	star_pos = star_pos_mod(star_pos);

	float star_dim = ZNEAR_H*2.;
	output_star(star_pos, star_dim);

	float h = rand(vec3(vid+0.03));
	float s = rand(vec3(vid+0.02))*0.6;
	float v = rand(vec3(vid+0.01))*0.2 + 0.8;
	float attenuation = min(1.0/dot(star_pos, star_pos), 1.0/(0.35*0.35));
	vary_color = vec4(HSVtoRGB(h, s, v), attenuation);
//	vary_color = vec4(HSVtoRGB(-star_pos.z, 1.0, 1.0), 1.0);
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

