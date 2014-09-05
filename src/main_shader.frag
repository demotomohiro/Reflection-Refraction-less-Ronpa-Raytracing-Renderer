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
	vec2 pos0 = ((gl_FragCoord.xy+coord_offset)*2.0 - resolution)/resolution.y;
	vec2 pos1 = (gl_FragCoord.xy+coord_offset)/resolution.y;

	float scale = 1./(1. + dot(pos0, pos0)*16.0);
	out_color = vec3(perlin2D(vec3(pos1, 0.7)))*scale;
}
