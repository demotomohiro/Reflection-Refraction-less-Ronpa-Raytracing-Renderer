#ifndef SIMPLEXNOISE2D_S
#define SIMPLEXNOISE2D_S

#include "constants.s"
#include "philox.s"
#include "simplexnoise_common.s"

vec3 randTheta(vec2 i0, vec2 i1, vec2 i2, uvec2 randKey)
{
    //According to 5.4.1 Conversion and Scalar Constructors in The OpenGL Shading Language Version 4.5, 
    //it is undefined to convert a negative floating-point value to an uint.
    //The constructor uint(int) preserves the bit pattern in the argument, which will change its value if it is negative.
    vec3 r = 2.0*PI*vec3(
        uintToFloat(Philox4x32(uvec4(ivec2(i0), 0, 0), randKey).x),
        uintToFloat(Philox4x32(uvec4(ivec2(i1), 0, 0), randKey).x),
        uintToFloat(Philox4x32(uvec4(ivec2(i2), 0, 0), randKey).x));
    return r;
}

vec2 smplxNoise2DDeriv(vec2 x, float m, vec2 g)
{
    vec2 dmdxy = max(vec2(0.5) - dot(x, x), 0.0);
    dmdxy = -8.0*x*dmdxy*dmdxy*dmdxy;
    return dmdxy*dot(x, g) + m*g;
}

vec2 smplxNoise2DDeriv2(vec2 x, vec2 g)
{
/*
    f(X) = (0.5 - dot(X, X))^4*dot(X, G)

    ∂f(X)/∂x = -4*2*Xx*(0.5 - dot(X, X))^3 * dot(X, G) + (0.5 - dot(X, X))^4*Gx
          = -8*Xx*(0.5 - dot(X, X))^3 * dot(X, G) + (0.5 - dot(X, X))^4*Gx

    ∂/∂x・∂f(X)/∂x = (-8*(0.5 - dot(X, X))^3 - 8*Xx*3*(-2*Xx)*(0.5 - dot(X, X))^2) * dot(X, G) - 8*Xx*(0.5 - dot(X, X))^3*Gx + 4*(-2*Xx)*(0.5 - dot(X, X))^3*Gx
                   = (0.5 - dot(X, X))^3*(-8*dot(X, G) - 8*Xx*Gx -8*Xx*Gx) + 48*Xx*Xx*dot(X, G)*(0.5 - dot(X, X))^2
                   = (0.5 - dot(X, X))^3*(-8*dot(X, G) - 16*Xx*Gx) + 48*Xx*Xx*dot(X, G)*(0.5 - dot(X, X))^2
*/

    vec2 a = max(vec2(0.5) - dot(x, x), 0.0);
    float dotxg = dot(x, g);
    return a*a*a*(vec2(-8.0*dotxg) - 16.0*x*g) + 48.0*x*x*dotxg*a*a;
}

float smplxNoise2D(vec2 p, out vec2 deriv, out vec2 deriv2, uvec2 randKey)
{
    //i is a skewed coordinate of a bottom vertex of a simplex where p is in.
    vec2 i0 = floor(p + vec2( (p.x + p.y)*skewF(2.0) ));
    //x0, x1, x2 are unskewed displacement vectors.
    float unskew = unskewG(2.0);
    vec2 x0 = p - (i0 + vec2((i0.x + i0.y)*unskew));

    vec2 ii1 = x0.x > x0.y ? vec2(1.0, 0.0) : vec2(0.0, 1.0);
    vec2 ii2 = vec2(1.0);

//  vec2 i1 = i0 + ii1;
//  vec2 x1 = p - (i1 + vec2((i1.x + i1.y)*unskew));
//          = p - (i0 + ii1 + vec2((i0.x + i0.y + 1.0)*unskew));
//          = p - (i0 + vec2((i0.x + i0.y)*unskew)) - ii1 - vec2(1.0)*unskew;
    vec2 x1 = x0 - ii1 - vec2(unskew);
//  vec2 i2 = i0 + ii2;
//  vec2 x2 = p - (i2 + vec2((i2.x + i2.y)*unskew));
//          = p - (i0 + ii2 + vec2((i0.x + i0.y + 2.0)*unskew));
//          = p - (i0 + vec2((i0.x + i0.y)*unskew)) - ii2 - vec2(2.0)*unskew;
    vec2 x2 = x0 - ii2 - vec2(2.0*unskew);

    vec3 m = max(vec3(0.5) - vec3(dot(x0, x0), dot(x1, x1), dot(x2, x2)), 0.0);
    m = m*m;
    m = m*m;

    vec3 r = randTheta(i0, i0 + ii1, i0 + ii2, randKey);

    //Gradients;
    vec2 g0 = vec2(cos(r.x), sin(r.x));
    vec2 g1 = vec2(cos(r.y), sin(r.y));
    vec2 g2 = vec2(cos(r.z), sin(r.z));

    deriv = smplxNoise2DDeriv(x0, m.x, g0) + smplxNoise2DDeriv(x1, m.y, g1) + smplxNoise2DDeriv(x2, m.z, g2);
    deriv2 = smplxNoise2DDeriv2(x0, g0) + smplxNoise2DDeriv2(x1, g1) + smplxNoise2DDeriv2(x2, g2);
    return dot(m*vec3(dot(x0, g0), dot(x1, g1), dot(x2, g2)), vec3(1.0));
//    return dot(m*vec3(length(x0), length(x1), length(x2)), vec3(1.0));
}

#endif
