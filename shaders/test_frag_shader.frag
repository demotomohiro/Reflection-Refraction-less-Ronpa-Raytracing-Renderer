#version 430

#include "noise.s"

out vec3 out_color;

uniform vec2 coord_offset;
uniform vec2 resolution;

void main()
{
	out_color = vec3(0.0);
	vec2 frag_coord = gl_FragCoord.xy+coord_offset;
	out_color = vec3((frag_coord )/resolution, 0.0);
#if 1
	if((int(frag_coord.x)&1)==0)
	{
		out_color.b = 0.0;
	}else
	{
		out_color.b = 1.0;
	}
#endif
}

