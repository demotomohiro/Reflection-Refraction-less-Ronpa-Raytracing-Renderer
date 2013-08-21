#version 430

#include "plane.s"

out vec3 out_color;

uniform vec2 coord_offset;
uniform vec2 resolution;

void main()
{
	vec2 pos0 = ((gl_FragCoord.xy+coord_offset)*2.0 - resolution)/resolution.y;
	vec2 pos1 = (gl_FragCoord.xy+coord_offset)/resolution.y;

	out_color = color(pos0);
}
