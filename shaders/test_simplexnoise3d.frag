#version 430

#include "simplexnoise3d.s"

out vec3 out_color;

uniform vec2 coord_offset;
uniform vec2 resolution;

void main()
{
	vec2 pos = ((gl_FragCoord.xy+coord_offset)*2.0 - resolution)/resolution.y;
    vec3 deriv;
    float c = smplxNoise3D(vec3(pos*2.0, 0.0), deriv, uvec2(0xdeadbeefu, 0xfeedaceu));
    float cn = c*54.0 + 0.5;
	out_color = vec3(cn);
}
