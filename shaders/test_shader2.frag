#version 430

#include "noise.s"

out vec3 out_color;

uniform vec2 coord_offset;
uniform vec2 resolution;

float perlin2D(vec3 pos)
{
	float c = 0.0;
	float s = 4.0;
	for(int i=0; i<6; ++i)
	{
		c += softnoise(vec3(pos)+vec3(1.0/float(i+2)), s)/s*2.0;
		s *= 2.0;
	}

	return c;
}

void main()
{
	out_color = vec3(0.0);
	vec2 pos0 = ((gl_FragCoord.xy+coord_offset)*2.0 - resolution)/resolution.y;
	vec2 pos1 = (gl_FragCoord.xy+coord_offset)/resolution.y;

	float rad = min(dot(pos0, pos0)+0.5, 1.0);
	float c = 0.0;
	float z = 0.0;
	int	num = 32;
	float dz = 1.0/float(num);
	for(int i=0; i<num; ++i)
	{
		c += perlin2D(vec3(pos1, z)) * rad;
		z += dz;
	}

	c *= dz*2.0;
	c -= 0.5;
//	s = 4.0*2.0*2.0;
//	c = softnoise(vec3(pos, 0.1), s)/s*2.0;
	out_color = vec3(c);
}

