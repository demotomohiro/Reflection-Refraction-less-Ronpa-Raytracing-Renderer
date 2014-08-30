#version 430

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

	float pointsize;
	if(gl_VertexID==0)
	{
		gl_Position = vec4(0.000125, 0.000125, -ZNEAR, 1.0);
		pointsize = ZNEAR_H*0.5;
	}else if(gl_VertexID==1)
	{
		gl_Position = vec4(0.000125, -0.000125, -ZNEAR*2.0, 1.0);
		pointsize = ZNEAR_H*0.5;
	}else
	{
		gl_Position = vec4(-0.00025, -0.00025, -ZNEAR*4.0, 1.0);
		pointsize = ZNEAR_H;
	}

	gl_Position = pvm * gl_Position;
	gl_Position.xy = gl_Position.xy * viewport_scale + viewport_offset*gl_Position.w;

	float ps = pvm[0][0] * pointsize * viewport_scale.x;
	gl_PointSize = viewport_size.x*0.5*ps / gl_Position.w;
}
