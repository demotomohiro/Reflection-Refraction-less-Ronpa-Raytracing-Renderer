#version 430

#include "simplexnoise2d.s"

out vec3 out_color;

uniform vec2 coord_offset;
uniform vec2 resolution;
uniform int  iFrame;

float fbm(vec2 pos)
{
	float c = 0.0;
	float s = 2.0;
	for(int i=0; i<6; ++i)
	{
        vec2 deriv, deriv2;
        float v = smplxNoise2D(pos*s, deriv, deriv2, uvec2(0xabcdeeeeu + uint(iFrame), 0x1234u + i));
        v = v * 49.7 + 0.5;
        c += v/s;
		s *= 2.0;
	}

	return c;
}

void main()
{
	vec2 pos = ((gl_FragCoord.xy+coord_offset)*2.0 - resolution)/resolution.y;

	float scale = 1./(1. + dot(pos, pos)*16.0);
	out_color = vec3(fbm(pos))*scale;
    if(iFrame == 1)
    {
        out_color = vec3(1.0, 0.0, 0.0);
    }
}
