#version 430

#include "particle.s"
#include "philox.s"

#define ZNEAR	0.001
#define ZFAR	1.0
#define ZNEAR_W 0.001
#define ZNEAR_H	(ZNEAR_W*aspect_rate)

#define ROTATE_Y_MAT(theta)	mat3(		\
	cos(theta),	0.0,	-sin(theta),	\
	0.0,	    1.0, 	0.0,		    \
	sin(theta), 0.0,	cos(theta))

#ifndef NUM_ITER
#define NUM_ITER 64
#endif

#ifndef ROT_THETA
#define ROT_THETA 0.0
#endif

#ifndef SCAT_DAMP
#define SCAT_DAMP 0.9
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

float round_box(vec3 p, float size)
{
    vec3 p3 = pow(abs(p), vec3(4.0));
    return pow(p3.x+p3.y+p3.z, 1.0/4.0) - size;
}

float round_cross(vec3 p, float size)
{
    const float r = 4.0;
    vec3 p2 = pow(abs(p), vec3(r));

    float z = pow(p2.x+p2.y, 1.0/r) - size;
    float x = pow(p2.y+p2.z, 1.0/r) - size;
    float y = pow(p2.z+p2.x, 1.0/r) - size;

    return min(x, min(y, z));
}

float DF(vec3 p)
{
    vec3 p2 = p;
    const float size = 0.125;
    float d = max(round_box(p2, size), -round_cross(p2, size/3.0));

    p2 = p2/size * 0.5;
    float scale = 1.0/size * 0.5;

    for(int i=0; i<2; ++i)
    {
        p2 *= 3.0;
        p2 -= vec3(0.5);
        p2 = fract(p2) - vec3(0.5);
        scale *= 3.0;
        d = max(d, -round_cross(p2, 1.0/6.0)/scale);
    }

    return d;
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
    float distance0 = DF(star_pos);
    for(int i=0; i<NUM_ITER; ++i)
    {
        float distance = DF(star_pos);// * 0.25;
        if(i>NUM_ITER/2 && abs(distance) < 0.0002)
            break;
        vec3 dir = -normalize(grad(star_pos));
        star_pos += dir*distance;
        //Move star_pos randomly so that star_pos evenly distribute inside DF.
        vec3 scatter = uintToFloat(Philox4x32(uvec4(vid, i, 13, 17), uvec2(63429, 52632))).xyz;
        vec3 norm_scatter = (scatter*2.0 - vec3(1.0));
        float scat_radius = distance0;
        distance0 *= SCAT_DAMP;
        star_pos += norm_scatter * scat_radius;
    }

    //Following code visualize star_pos which went deep space as white points.
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

    const float change_frames = 60.;
    //Appear
    if(true){
        float t = max(1.0 - (CNT/change_frames) - (-star_pos.z+0.09)*4.0, 0.0);
        float x0 = (star_pos.x+0.25);
        star_pos.x += pow(t*8., x0*8.0+0.25)*3.0 + t*x0*8.0;
    //    star_pos.x = min(star_pos.x, 0.499);
    }

    //Disappear
    {
        vec3 rnorm = normalize(round(normal));
        float dot = dot(rnorm, star_pos);
        const float dmin = 0.03, dmax = 0.15;
        float d = (abs(dot) - dmin)/(dmax - dmin);
        float t = max((CNT - change_frames*3.0)/change_frames - (1.0 - d), 0.0);
        star_pos += /*normalize(star_pos)*/sign(dot)*rnorm*t*t*2.;
    }

    star_pos = ROTATE_Y_MAT(ROT_THETA) * star_pos;
    star_pos -= vec3(0.0, 0.0, 0.5);

    //Keep brightness constant after chaning a number of particles.
	float star_dim = ZNEAR_H*inversesqrt(BR_NUM_PARTICLESF/float(65536<<8));
	output_star(star_pos, star_dim);

	vary_color = vec4(abs(normal)*0.04, 0.05);
}

void main()
{
	gen_star();
}
