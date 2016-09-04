#version 430

#include "particle.s"
#include "HSV.s"
#include "philox.s"

#define ZNEAR	0.001
#define ZFAR	1.0
#define ZNEAR_W 0.001
#define ZNEAR_H	(ZNEAR_W*aspect_rate)

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

void output_star(vec3 star_pos, float star_dim)
{
    brOutputPosition(star_pos, pvm);
    brOutputPointSize(star_dim, pvm);
}

void gen_star()
{
	int vid = brGetVertexID();

    vec3 star_pos = uintToFloat(Philox4x32(uvec4(vid, 3, 7, 11), uvec2(1274053, 440525))).xyz;
    star_pos -= vec3(0.5, 0.5, 1.0);

	float star_dim = ZNEAR_H;
	output_star(star_pos, star_dim);

    vec3 hsv = uintToFloat(Philox4x32(uvec4(vid, 23, 41, 11), uvec2(36253, 3546492))).xyz;
	hsv.y *= 0.6;
	hsv.z = hsv.z*0.2 + 0.8;
	float attenuation = min(0.0625/dot(star_pos, star_pos), 0.25/(0.35*0.35));
	vary_color = vec4(HSVtoRGB(hsv), attenuation);
}

void main()
{
	gen_star();
}
