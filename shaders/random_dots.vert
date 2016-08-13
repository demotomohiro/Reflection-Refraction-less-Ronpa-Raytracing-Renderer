#version 430

#include "particle.s"

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
    int yvid = vid >> 4;
    int new_vid = bitfieldInsert(yvid, bitfieldExtract(yvid, 0, 1), 1, 1);
    new_vid = bitfieldInsert(new_vid, bitfieldExtract(yvid, 1, 1), 0, 1);
    float x = float(vid&0xff) / width * 2.0 - 1.0;
    float y = float(new_vid&0xff) / width * 2.0 - 1.0;

    vec3 p = vec3(x, y, 0.0);
    brOutputPosition(p, pvm);
    brOutputPointSize(1.0/width, pvm);

    vary_color = vec4(vec3(1.0), 1.0);
}

