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

uvec2 sbox32(uvec2 LR, uint key) {
    const uint M = 0xCD9E8D57;

    uvec2 ret;
    umulExtended(LR.y, M, ret.y, ret.x);
    ret.y = ret.y ^ key ^ LR.x;
    return ret;
}

uvec4 Philox4x32(uvec4 plain, uvec2 key) {
    uvec4 state = plain;
    uvec2 round_key = key;

    for(int i=0; i<7; ++i) {
        state.xy = sbox32(state.xy, round_key.x);
        state.zw = sbox32(state.zw, round_key.y);

        uint ty = state.y;
        state.y = state.w;
        state.w = ty;
        uint carry;
        round_key.x = uaddCarry(round_key.x, 0x84CAA73B, carry);
        round_key.y += 0xBB67AE85 + carry;
    }

    return state;
}

float uintToFloat(uint src) {
    return uintBitsToFloat(0x3f800000u | (src & 0x7fffffu))-1.0;
}

vec4 uintToFloat(uvec4 src) {
    return vec4(uintToFloat(src.x), uintToFloat(src.y), uintToFloat(src.z), uintToFloat(src.w));
}

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

