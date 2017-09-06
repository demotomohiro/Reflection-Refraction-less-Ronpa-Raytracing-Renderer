#version 430

#include "particle.s"
#include "HSV.s"
#include "philox.s"
#include "simplexnoise3d.s"

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

uniform float iTime;
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

vec3 smplxNoiseMove(vec3 pos)
{
    vec3 p1 = pos;
    float scaling = 12.0;

    for(int i=0; i<4; ++i)
    {
        vec3 deriv;
        float c = smplxNoise3D(p1*scaling, deriv, uvec2(135, 984));
        deriv *= scaling;
        if(length(deriv) < 0.0001)
            return p1;
        vec3 delta = c*deriv/dot(deriv, deriv);
        p1 = p1 - delta;
    }
    return p1;
}

void gen_star()
{
	int vid = brGetVertexID();

    vec3 star_pos = uintToFloat(Philox4x32(uvec4(vid, 3, 7, 11), uvec2(1274053, 440525))).xyz;
	vec3 centor = vec3(0.0, 0.0, -0.5);
    star_pos = star_pos - vec3(0.5) + centor;
    star_pos = mix(star_pos, smplxNoiseMove(star_pos), smoothstep(0.0, 1.0, min(iTime*0.5, 1.0)));
    star_pos.z = -fract(star_pos.z - iTime*iTime*0.25);

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

    //Keep brightness constant after chaning a number of particles.
	float star_dim = ZNEAR_H*inversesqrt(BR_NUM_PARTICLESF/float(65536<<8));
	output_star(star_pos, star_dim);

    vec4 randColorBase = uintToFloat(Philox4x32(uvec4(vid, 3, 7, 11), uvec2(2253, 440525)));
	float h = randColorBase.x;
	float s = randColorBase.y*0.6;
	float v = randColorBase.z*0.2 + 0.8;
	float attenuation = min(0.5/dot(star_pos, star_pos), 0.5/(0.35*0.35));
	vary_color = vec4(HSVtoRGB(h, s, v), attenuation);
//	vary_color = vec4(abs(normal)*0.04, 0.05);
}

void main()
{
	gen_star();
}
