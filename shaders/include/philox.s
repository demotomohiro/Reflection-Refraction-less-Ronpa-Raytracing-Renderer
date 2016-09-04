#pragma once

/*
Counter based  pseudorandom number generator Philox

It is introduced in this paper:
Parallel Random Numbers: As Easy as 1, 2, 3
by John K. Salmon, Mark A. Moraes, Ron O. Dror, and David E. Shaw

Philox is a modification of Threefish.
It is explained in 2.2 and 3.3 in this paper:
The Skein Hash Function Family
by Niels Ferguson, Stefan Lucks, Bruce Schneier, Doug Whiting, Mihir Bellare, Tadayoshi Kohno, Jon Callas, Jesse Walker

uvec4 counter;
uvec2 key;
uintToFloat(Philox4x32(counter, key))
returns pseudorandom vec4 value where each components are [0, 1).
*/

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

