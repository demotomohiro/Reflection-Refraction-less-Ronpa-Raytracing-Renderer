#version 430

#include "particle.s"
#include "philox.s"

uniform float aspect_rate;

//Parallel projection (-1, -1, -1)~(1, 1, 1)
mat4 projection =
mat4
(
	1.0,        0.0,	    0.0,		0.0,
	0.0,		1.0,	    0.0,		0.0,
	0.0,		0.0,		-1.0,		0.0,
	0.0,		0.0,		0.0,	    1.0
);

const mat4 pvm = projection;

out vec4 vary_color;

void main()
{
    const float width = 256.0;
    int vid = brGetVertexID();

    vec4 rp = uintToFloat(Philox4x32(uvec4(vid, 3, 7, 11), uvec2(1274053, 440525)));

    vec3 p = vec3(rp.xy, 0.0)*2.0 - 1.0;

    brOutputPosition(p, pvm);
    brOutputPointSize(1.0/width, pvm);

    vary_color = vec4(vec3(1.0), 1.0);
}

