#version 430

#include "particle.s"
#include "philox.s"

#define ZNEAR	0.001
#define ZFAR	1.0
#define ZNEAR_W 0.001
#define ZNEAR_H	(ZNEAR_W*aspect_rate)

#ifndef TURBULENCE
#define TURBULENCE 2.0
#endif

#ifndef NUM_ITER
#define NUM_ITER 32
#endif

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

float DF(vec3 p)
{
    vec3 p2 = p;
    return max(length(p2) - 0.125, -(length(p2.yz)-0.03));
}

vec3 grad(vec3 p)
{
    const vec2 delta = vec2(0.0005, 0.0);
    return
    vec3(
        (DF(p+delta.xyy) - DF(p-delta.xyy)),
        (DF(p+delta.yxy) - DF(p-delta.yxy)),
        (DF(p+delta.yyx) - DF(p-delta.yyx)));
}

void gen_star()
{
	int vid = brGetVertexID();

    vec3 star_pos = uintToFloat(Philox4x32(uvec4(vid, 3, 7, 11), uvec2(1274053, 440525))).xyz;
    star_pos = star_pos * 0.5 - vec3(0.25);
    for(int i=0; i<NUM_ITER; ++i)
    {
        float distance = DF(star_pos) * 0.25;
        vec3 dir = -normalize(grad(star_pos));
        star_pos += dir*distance;
        //Move star_pos randomly so that star_pos evenly distribute on the surface of DF.
        //But don't move randomly to the "dir" direction in order to prevent star_pos go away from DF = 0 places.
        vec3 scatter = uintToFloat(Philox4x32(uvec4(vid, i, 13, 17), uvec2(63429, 52632))).xyz;
        vec3 norm_scatter = (scatter*2.0 - vec3(1.0));
        norm_scatter -= dot(norm_scatter, dir)*dir;
        star_pos += norm_scatter * distance*TURBULENCE;
    }

    //Following code visualize star_pos which went deep space as white points.
    //TURBULENCE >= 4 makes such star_pos.
#if 0
    if(star_pos.x < -1.0 || star_pos.x > 1.0 || star_pos.y < -1.0 || star_pos.y > 1.0 || star_pos.z < -1.0 || star_pos.z > 1.0)
    {
        star_pos = uintToFloat(Philox4x32(uvec4(vid, 3, 7, 71), uvec2(14053, 525))).xyz;
        star_pos.xy = (star_pos.xy - 0.5)*0.5;
        star_pos.z = -0.5;
        vary_color = vec4(1.0, 1.0, 1.0, 1.0);
        float star_dim = ZNEAR_H*4.0;
        output_star(star_pos, star_dim);
        return;
    }
#endif

    vec3 normal = normalize(grad(star_pos));
    star_pos -= vec3(0.0, 0.0, 0.5);

	float star_dim = ZNEAR_H*2.0;
	output_star(star_pos, star_dim);

//	float attenuation = min(0.0625/dot(star_pos, star_pos), 0.25/(0.35*0.35));
	float attenuation = 0.5;
	vary_color = vec4(abs(normal), attenuation);
}

void main()
{
	gen_star();
}
