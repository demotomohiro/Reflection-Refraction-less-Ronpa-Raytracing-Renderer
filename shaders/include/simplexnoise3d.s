#ifndef SIMPLEXNOISE3D_S
#define SIMPLEXNOISE3D_S

#include "constants.s"
#include "philox.s"
#include "simplexnoise_common.s"

void randThetaPhi(vec3 id[4], uvec2 randKey, out vec2 tp[4])
{
    for(int i=0; i<4; ++i)
    {
        //According to 5.4.1 Conversion and Scalar Constructors in The OpenGL Shading Language Version 4.5, 
        //it is undefined to convert a negative floating-point value to an uint.
        //The constructor uint(int) preserves the bit pattern in the argument, which will change its value if it is negative.
        vec4 rand = uintToFloat(Philox4x32(uvec4(ivec3(id[i]), 0), randKey));

        //Spherical coordinate of a point which is uniformly distributed on sphere.
        //http://mathworld.wolfram.com/SpherePointPicking.html
        tp[i] = vec2(
            2.0*PI*rand.x,
            acos(2.0*rand.y - 1.0)
        );
    }
}

vec3 smplxNoise3DDeriv(vec3 x, float m, vec3 g)
{
    vec3 dmdxy = max(vec3(0.5) - dot(x, x), 0.0);
    dmdxy = -8.0*x*dmdxy*dmdxy*dmdxy;
    return dmdxy*dot(x, g) + m*g;
}

float smplxNoise3D(vec3 p, out vec3 deriv, uvec2 randKey)
{
    vec3 id[4];
    id[0] = floor(p + vec3( (p.x + p.y + p.z)*skewF(3.0) ));
    float unskew = unskewG(3.0);
    vec3 x[4];
    x[0] = p - (id[0] + vec3( (id[0].x + id[0].y + id[0].z)*unskew ));

    vec3 cmp1 = step(vec3(0.0), x[0] - x[0].zxy);
    vec3 cmp2 = vec3(1.0) - cmp1.yzx; //= step(vec3(0.0), x[0] - x[0].yzx);
    vec3 ii1 = cmp1*cmp2;    //Largest component is 1.0, others are 0.0.
    vec3 ii2 = min(cmp1 + cmp2, 1.0);    //Smallest component is 0.0, others are 1.0.
    vec3 ii3 = vec3(1.0);

//  vec3 id[1] = id[0] + ii1;
//  x[1] = p - (id[1] + vec3(id[1].x + id[1].y + id[1].z)*unskew);
//       = p - (id[0] + ii1 + vec3(id[0].x + id[0].y + id[0].z + 1.0)*unskew);
//       = p - (id[0] + vec3(id[0].x + id[0].y + id[0].z)*unskew) - ii1 - vec3(1.0)*unskew);
    x[1] = x[0] - ii1 - vec3(unskew);
    x[2] = x[0] - ii2 - vec3(2.0*unskew);
    x[3] = x[0] - ii3 - vec3(3.0*unskew);

    float m[4];
    for(int i=0; i<4; ++i)
    {
        m[i] = max(0.5 - dot(x[i], x[i]), 0.0);
        m[i] = m[i]*m[i];
        m[i] = m[i]*m[i];
    }

    id[1] = id[0]+ii1;
    id[2] = id[0]+ii2;
    id[3] = id[0]+ii3;

    vec2 tp[4];
    randThetaPhi(id, randKey, tp);

    //Gradients;
    vec3 g[4];
    for(int i=0; i<4; ++i)
    {
        float r = sin(tp[i].y);
        g[i] = vec3(r*cos(tp[i].x), r*sin(tp[i].x), cos(tp[i].y));
    }

    float ret = 0.0;
    deriv = vec3(0.0);
    for(int i=0; i<4; ++i)
    {
        ret += m[i] * dot(x[i], g[i]);
        deriv += smplxNoise3DDeriv(x[i], m[i], g[i]);
    }

    return ret;
}

#endif
