// Copyright(c) 2017 POLYGONTEK
// 
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// 
// http ://www.apache.org/licenses/LICENSE-2.0
// 
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#pragma once

#ifdef __ARM_NEON__
#include "arm_neon.h"
#endif

#ifndef _MM_SHUFFLE
#define _MM_SHUFFLE(fp3, fp2, fp1, fp0)     (((fp3) << 6) | ((fp2) << 4) | ((fp1) << 2) | ((fp0)))
#endif

// Takes the upper 64 bits of a and places it in the low end of the result
// Takes the lower 64 bits of b and places it into the high end of the result.
BE_FORCE_INLINE float32x4_t vshuffleq_f32_1032(float32x4_t a, float32x4_t b) {
    float32x2_t a32 = vget_high_f32(a);
    float32x2_t b10 = vget_low_f32(b);
    return vcombine_f32(a32, b10);
}

// takes the lower two 32-bit values from a and swaps them and places in high
// end of result takes the higher two 32 bit values from b and swaps them and
// places in low end of result.
BE_FORCE_INLINE float32x4_t vshuffleq_f32_2301(float32x4_t a, float32x4_t b) {
    float32x2_t a01 = vrev64_f32(vget_low_f32(a));
    float32x2_t b23 = vrev64_f32(vget_high_f32(b));
    return vcombine_f32(a01, b23);
}

BE_FORCE_INLINE float32x4_t vshuffleq_f32_0321(float32x4_t a, float32x4_t b) {
    float32x2_t a21 = vget_high_f32(vextq_f32(a, a, 3));
    float32x2_t b03 = vget_low_f32(vextq_f32(b, b, 3));
    return vcombine_f32(a21, b03);
}

BE_FORCE_INLINE float32x4_t vshuffleq_f32_2103(float32x4_t a, float32x4_t b) {
    float32x2_t a03 = vget_low_f32(vextq_f32(a, a, 3));
    float32x2_t b21 = vget_high_f32(vextq_f32(b, b, 3));
    return vcombine_f32(a03, b21);
}

BE_FORCE_INLINE float32x4_t vshuffleq_f32_1010(float32x4_t a, float32x4_t b) {
    float32x2_t a10 = vget_low_f32(a);
    float32x2_t b10 = vget_low_f32(b);
    return vcombine_f32(a10, b10);
}

BE_FORCE_INLINE float32x4_t vshuffleq_f32_1001(float32x4_t a, float32x4_t b) {
    float32x2_t a01 = vrev64_f32(vget_low_f32(a));
    float32x2_t b10 = vget_low_f32(b);
    return vcombine_f32(a01, b10);
}

BE_FORCE_INLINE float32x4_t vshuffleq_f32_0101(float32x4_t a, float32x4_t b) {
    float32x2_t a01 = vrev64_f32(vget_low_f32(a));
    float32x2_t b01 = vrev64_f32(vget_low_f32(b));
    return vcombine_f32(a01, b01);
}

// keeps the low 64 bits of b in the low and puts the high 64 bits of a in the high
BE_FORCE_INLINE float32x4_t vshuffleq_f32_3210(float32x4_t a, float32x4_t b) {
    float32x2_t a10 = vget_low_f32(a);
    float32x2_t b32 = vget_high_f32(b);
    return vcombine_f32(a10, b32);
}

BE_FORCE_INLINE float32x4_t vshuffleq_f32_0011(float32x4_t a, float32x4_t b) {
    float32x2_t a11 = vdup_lane_f32(vget_low_f32(a), 1);
    float32x2_t b00 = vdup_lane_f32(vget_low_f32(b), 0);
    return vcombine_f32(a11, b00);
}

BE_FORCE_INLINE float32x4_t vshuffleq_f32_0022(float32x4_t a, float32x4_t b) {
    float32x2_t a22 = vdup_lane_f32(vget_high_f32(a), 0);
    float32x2_t b00 = vdup_lane_f32(vget_low_f32(b), 0);
    return vcombine_f32(a22, b00);
}

BE_FORCE_INLINE float32x4_t vshuffleq_f32_2200(float32x4_t a, float32x4_t b) {
    float32x2_t a00 = vdup_lane_f32(vget_low_f32(a), 0);
    float32x2_t b22 = vdup_lane_f32(vget_high_f32(b), 0);
    return vcombine_f32(a00, b22);
}

BE_FORCE_INLINE float32x4_t vshuffleq_f32_3202(float32x4_t a, float32x4_t b) {
    float32_t a0 = vgetq_lane_f32(a, 0);
    float32x2_t a22 = vdup_lane_f32(vget_high_f32(a), 0);
    float32x2_t a02 = vset_lane_f32(a0, a22, 1); /* TODO: use vzip ?*/
    float32x2_t b32 = vget_high_f32(b);
    return vcombine_f32(a02, b32);
}

BE_FORCE_INLINE float32x4_t vshuffleq_f32_3232(float32x4_t a, float32x4_t b) {
    float32x2_t a32 = vget_high_f32(a);
    float32x2_t b32 = vget_high_f32(b);
    return vcombine_f32(b32, a32);
}

BE_FORCE_INLINE float32x4_t vshuffleq_f32_1133(float32x4_t a, float32x4_t b) {
    float32x2_t a33 = vdup_lane_f32(vget_high_f32(a), 1);
    float32x2_t b11 = vdup_lane_f32(vget_low_f32(b), 1);
    return vcombine_f32(a33, b11);
}

BE_FORCE_INLINE float32x4_t vshuffleq_f32_2010(float32x4_t a, float32x4_t b) {
    float32x2_t a10 = vget_low_f32(a);
    float32_t b2 = vgetq_lane_f32(b, 2);
    float32x2_t b00 = vdup_lane_f32(vget_low_f32((b)), 0);
    float32x2_t b20 = vset_lane_f32(b2, b00, 1);
    return vcombine_f32(a10, b20);
}

BE_FORCE_INLINE float32x4_t vshuffleq_f32_2001(float32x4_t a, float32x4_t b) {
    float32x2_t a01 = vrev64_f32(vget_low_f32(a));
    float32_t b2 = vgetq_lane_f32(b, 2);
    float32x2_t b00 = vdup_lane_f32(vget_low_f32(b), 0);
    float32x2_t b20 = vset_lane_f32(b2, b00, 1);
    return vcombine_f32(a01, b20);
}

BE_FORCE_INLINE float32x4_t vshuffleq_f32_2032(float32x4_t a, float32x4_t b) {
    float32x2_t a32 = vget_high_f32(a);
    float32_t b2 = vgetq_lane_f32(b, 2);
    float32x2_t b00 = vdup_lane_f32(vget_low_f32(b), 0);
    float32x2_t b20 = vset_lane_f32(b2, b00, 1);
    return vcombine_f32(a32, b20);
}

#if 0 /* C version */
FORCE_INLINE float32x4_t vshuffleq_f32_default(float32x4_t a, float32x4_t b, __constrange(0, 255) int imm) {
    float32x4_t ret;
    ret[0] = a[imm & 0x3];
    ret[1] = a[(imm >> 2) & 0x3];
    ret[2] = b[(imm >> 4) & 0x03];
    ret[3] = b[(imm >> 6) & 0x03];
    return ret;
}
#endif
// NEON does not support a general purpose permute intrinsic.
// Selects four specific single-precision, floating-point values from a and b, based on the mask i.
// https://msdn.microsoft.com/en-us/library/vstudio/5f0858x0(v=vs.100).aspx
#define vshuffleq_f32_default(a, b, imm)                                       \
    ({                                                                          \
        float32x4_t ret;                                                        \
        ret = vmovq_n_f32(vgetq_lane_f32((a), (imm) & 0x3));                    \
        ret = vsetq_lane_f32(vgetq_lane_f32((a), ((imm) >> 2) & 0x3), ret, 1);  \
        ret = vsetq_lane_f32(vgetq_lane_f32((b), ((imm) >> 4) & 0x3), ret, 2);  \
        ret = vsetq_lane_f32(vgetq_lane_f32((b), ((imm) >> 6) & 0x3), ret, 3);  \
        (ret);                                                                  \
    })

// BE_FORCE_INLINE float32x4_t vshuffleq_f32(float32x4_t a, float32x4_t b, __constrange(0, 255) int imm)
#define vshuffleq_f32(a, b, imm)                            \
    ({                                                      \
        float32x4_t ret;                                    \
        switch (imm) {                                      \
        case _MM_SHUFFLE(0, 0, 0, 0):                       \
            ret = vdupq_lane_f32(vget_low_f32((a)), 0);     \
            break;                                          \
        case _MM_SHUFFLE(1, 1, 1, 1):                       \
            ret = vdupq_lane_f32(vget_low_f32((a)), 1);     \
            break;                                          \
        case _MM_SHUFFLE(2, 2, 2, 2):                       \
            ret = vdupq_lane_f32(vget_high_f32((a)), 0);    \
            break;                                          \
        case _MM_SHUFFLE(3, 3, 3, 3):                       \
            ret = vdupq_lane_f32(vget_high_f32((a)), 1);    \
            break;                                          \
        case _MM_SHUFFLE(1, 0, 3, 2):                       \
            ret = vshuffleq_f32_1032((a), (b));             \
            break;                                          \
        case _MM_SHUFFLE(2, 3, 0, 1):                       \
            ret = vshuffleq_f32_2301((a), (b));             \
            break;                                          \
        case _MM_SHUFFLE(0, 3, 2, 1):                       \
            ret = vshuffleq_f32_0321((a), (b));             \
            break;                                          \
        case _MM_SHUFFLE(2, 1, 0, 3):                       \
            ret = vshuffleq_f32_2103((a), (b));             \
            break;                                          \
        case _MM_SHUFFLE(1, 0, 1, 0):                       \
            ret = vshuffleq_f32_1010((a), (b));             \
            break;                                          \
        case _MM_SHUFFLE(1, 0, 0, 1):                       \
            ret = vshuffleq_f32_1001((a), (b));             \
            break;                                          \
        case _MM_SHUFFLE(0, 1, 0, 1):                       \
            ret = vshuffleq_f32_0101((a), (b));             \
            break;                                          \
        case _MM_SHUFFLE(3, 2, 1, 0):                       \
            ret = vshuffleq_f32_3210((a), (b));             \
            break;                                          \
        case _MM_SHUFFLE(0, 0, 1, 1):                       \
            ret = vshuffleq_f32_0011((a), (b));             \
            break;                                          \
        case _MM_SHUFFLE(0, 0, 2, 2):                       \
            ret = vshuffleq_f32_0022((a), (b));             \
            break;                                          \
        case _MM_SHUFFLE(2, 2, 0, 0):                       \
            ret = vshuffleq_f32_2200((a), (b));             \
            break;                                          \
        case _MM_SHUFFLE(3, 2, 0, 2):                       \
            ret = vshuffleq_f32_3202((a), (b));             \
            break;                                          \
        case _MM_SHUFFLE(3, 2, 3, 2):                       \
            ret = vshuffleq_f32_3232((b), (a));             \
            break;                                          \
        case _MM_SHUFFLE(1, 1, 3, 3):                       \
            ret = vshuffleq_f32_1133((a), (b));             \
            break;                                          \
        case _MM_SHUFFLE(2, 0, 1, 0):                       \
            ret = vshuffleq_f32_2010((a), (b));             \
            break;                                          \
        case _MM_SHUFFLE(2, 0, 0, 1):                       \
            ret = vshuffleq_f32_2001((a), (b));             \
            break;                                          \
        case _MM_SHUFFLE(2, 0, 3, 2):                       \
            ret = vshuffleq_f32_2032((a), (b));             \
            break;                                          \
        default:                                            \
            ret = vshuffleq_f32_default((a), (b), (imm));   \
            break;                                          \
        }                                                   \
        ret;                                                \
    })

// Takes the upper 64 bits of a and places it in the low end of the result
// Takes the lower 64 bits of a and places it into the high end of the result.
FORCE_INLINE int32x4_t vshuffleq_s32_1032(int32x4_t a) {
    int32x2_t a32 = vget_high_s32(a);
    int32x2_t a10 = vget_low_s32(a);
    return vcombine_s32(a32, a10);
}

// takes the lower two 32-bit values from a and swaps them and places in low end
// of result takes the higher two 32 bit values from a and swaps them and places
// in high end of result.
FORCE_INLINE int32x4_t vshuffleq_s32_2301(int32x4_t a) {
    return vrev64q_s32(a);
}

// rotates the least significant 32 bits into the most signficant 32 bits, and
// shifts the rest down
FORCE_INLINE int32x4_t vshuffleq_s32_0321(int32x4_t a) {
    return vextq_s32(a, a, 1);
}

// rotates the most significant 32 bits into the least signficant 32 bits, and
// shifts the rest up
FORCE_INLINE int32x4_t vshuffleq_s32_2103(int32x4_t a) {
    return vextq_s32(a, a, 3);
}

// gets the lower 64 bits of a, and places it in the upper 64 bits
// gets the lower 64 bits of a and places it in the lower 64 bits
FORCE_INLINE int32x4_t vshuffleq_s32_1010(int32x4_t a) {
    int32x2_t a10 = vget_low_s32(a);
    return vcombine_s32(a10, a10);
}

// gets the lower 64 bits of a, swaps the 0 and 1 elements, and places it in the
// lower 64 bits gets the lower 64 bits of a, and places it in the upper 64 bits
FORCE_INLINE int32x4_t vshuffleq_s32_1001(int32x4_t a) {
    int32x2_t a01 = vrev64_s32(vget_low_s32(a));
    int32x2_t a10 = vget_low_s32(a);
    return vcombine_s32(a01, a10);
}

// gets the lower 64 bits of a, swaps the 0 and 1 elements and places it in the
// upper 64 bits gets the lower 64 bits of a, swaps the 0 and 1 elements, and
// places it in the lower 64 bits
FORCE_INLINE int32x4_t vshuffleq_s32_0101(int32x4_t a) {
    int32x2_t a01 = vrev64_s32(vget_low_s32(a));
    return vcombine_s32(a01, a01);
}

FORCE_INLINE int32x4_t vshuffleq_s32_2211(int32x4_t a) {
    int32x2_t a11 = vdup_lane_s32(vget_low_s32(a), 1);
    int32x2_t a22 = vdup_lane_s32(vget_high_s32(a), 0);
    return vcombine_s32(a11, a22);
}

FORCE_INLINE int32x4_t vshuffleq_s32_0122(int32x4_t a) {
    int32x2_t a22 = vdup_lane_s32(vget_high_s32(a), 0);
    int32x2_t a01 = vrev64_s32(vget_low_s32(a));
    return vcombine_s32(a22, a01);
}

FORCE_INLINE int32x4_t vshuffleq_s32_3332(int32x4_t a) {
    int32x2_t a32 = vget_high_s32(a);
    int32x2_t a33 = vdup_lane_s32(vget_high_s32(a), 1);
    return vcombine_s32(a32, a33);
}

// Shuffle packed 8-bit integers in a according to shuffle control mask in the
// corresponding 8-bit element of b, and store the results in dst.
// https://software.intel.com/sites/landingpage/IntrinsicsGuide/#text=_mm_shuffle_epi8&expand=5146
FORCE_INLINE int32x4_t vshuffleq_s8(int32x4_t a, int32x4_t b) {
#if __aarch64__
    int8x16_t tbl = vreinterpretq_s8_s32(a);   // input a
    uint8x16_t idx = vreinterpretq_u8_s32(b);  // input b
    uint8_t __attribute__((aligned(16))) mask[16] = { 0x8F, 0x8F, 0x8F, 0x8F, 0x8F, 0x8F, 0x8F, 0x8F, 0x8F, 0x8F, 0x8F, 0x8F, 0x8F, 0x8F, 0x8F, 0x8F };
    uint8x16_t idx_masked = vandq_u8(idx, vld1q_u8(mask));  // avoid using meaningless bits

    return vreinterpretq_s32_s8(vqtbl1q_s8(tbl, idx_masked));
#else
    uint8_t *tbl = (uint8_t *)&a;  // input a
    uint8_t *idx = (uint8_t *)&b;  // input b
    int32_t r[4];

    r[0] = ((idx[3] & 0x80) ? 0 : tbl[idx[3] % 16]) << 24;
    r[0] |= ((idx[2] & 0x80) ? 0 : tbl[idx[2] % 16]) << 16;
    r[0] |= ((idx[1] & 0x80) ? 0 : tbl[idx[1] % 16]) << 8;
    r[0] |= ((idx[0] & 0x80) ? 0 : tbl[idx[0] % 16]);

    r[1] = ((idx[7] & 0x80) ? 0 : tbl[idx[7] % 16]) << 24;
    r[1] |= ((idx[6] & 0x80) ? 0 : tbl[idx[6] % 16]) << 16;
    r[1] |= ((idx[5] & 0x80) ? 0 : tbl[idx[5] % 16]) << 8;
    r[1] |= ((idx[4] & 0x80) ? 0 : tbl[idx[4] % 16]);

    r[2] = ((idx[11] & 0x80) ? 0 : tbl[idx[11] % 16]) << 24;
    r[2] |= ((idx[10] & 0x80) ? 0 : tbl[idx[10] % 16]) << 16;
    r[2] |= ((idx[9] & 0x80) ? 0 : tbl[idx[9] % 16]) << 8;
    r[2] |= ((idx[8] & 0x80) ? 0 : tbl[idx[8] % 16]);

    r[3] = ((idx[15] & 0x80) ? 0 : tbl[idx[15] % 16]) << 24;
    r[3] |= ((idx[14] & 0x80) ? 0 : tbl[idx[14] % 16]) << 16;
    r[3] |= ((idx[13] & 0x80) ? 0 : tbl[idx[13] % 16]) << 8;
    r[3] |= ((idx[12] & 0x80) ? 0 : tbl[idx[12] % 16]);

    return vld1q_s32(r);
#endif
}

#if 0 /* C version */
FORCE_INLINE int32x4_t vshuffleq_s32_default(int32x4_t a, __constrange(0, 255) int imm) {
    int32x4_t ret;
    ret[0] = a[imm & 0x3];
    ret[1] = a[(imm >> 2) & 0x3];
    ret[2] = a[(imm >> 4) & 0x03];
    ret[3] = a[(imm >> 6) & 0x03];
    return ret;
}
#endif
#define vshuffleq_s32_default(a, imm)                                           \
    ({                                                                          \
        int32x4_t ret;                                                          \
        ret = vmovq_n_s32(vgetq_lane_s32((a), (imm) & 0x3));                    \
        ret = vsetq_lane_s32(vgetq_lane_s32((a), ((imm) >> 2) & 0x3), ret, 1);  \
        ret = vsetq_lane_s32(vgetq_lane_s32((a), ((imm) >> 4) & 0x3), ret, 2);  \
        ret = vsetq_lane_s32(vgetq_lane_s32((a), ((imm) >> 6) & 0x3), ret, 3);  \
        (ret);                                                                  \
    })

// FORCE_INLINE int32x4_t vshuffleq_s32_splat(int32x4_t a, __constrange(0, 255) int imm)
#if defined(__aarch64__)
#define vshuffleq_s32_splat(a, imm)     vdupq_laneq_s32((a), (imm))
#else
#define vshuffleq_s32_splat(a, imm)     vdupq_n_s32(vgetq_lane_s32((a), (imm)))
#endif

// Shuffles the 4 signed or unsigned 32-bit integers in a as specified by imm.
// https://msdn.microsoft.com/en-us/library/56f67xbk%28v=vs.90%29.aspx
// FORCE_INLINE int32x4_t vshuffleq_s32(int32x4_t a, __constrange(0,255) int imm)
#define vshuffleq_s32(a, imm)                           \
    ({                                                  \
        int32x4_t ret;                                  \
        switch (imm) {                                  \
        case _MM_SHUFFLE(1, 0, 3, 2):                   \
            ret = vshuffleq_s32_1032((a));              \
            break;                                      \
        case _MM_SHUFFLE(2, 3, 0, 1):                   \
            ret = vshuffleq_s32_2301((a));              \
            break;                                      \
        case _MM_SHUFFLE(0, 3, 2, 1):                   \
            ret = vshuffleq_s32_0321((a));              \
            break;                                      \
        case _MM_SHUFFLE(2, 1, 0, 3):                   \
            ret = vshuffleq_s32_2103((a));              \
            break;                                      \
        case _MM_SHUFFLE(1, 0, 1, 0):                   \
            ret = vshuffleq_s32_1010((a));              \
            break;                                      \
        case _MM_SHUFFLE(1, 0, 0, 1):                   \
            ret = vshuffleq_s32_1001((a));              \
            break;                                      \
        case _MM_SHUFFLE(0, 1, 0, 1):                   \
            ret = vshuffleq_s32_0101((a));              \
            break;                                      \
        case _MM_SHUFFLE(2, 2, 1, 1):                   \
            ret = vshuffleq_s32_2211((a));              \
            break;                                      \
        case _MM_SHUFFLE(0, 1, 2, 2):                   \
            ret = vshuffleq_s32_0122((a));              \
            break;                                      \
        case _MM_SHUFFLE(3, 3, 3, 2):                   \
            ret = vshuffleq_s32_3332((a));              \
            break;                                      \
        case _MM_SHUFFLE(0, 0, 0, 0):                   \
            ret = vshuffleq_s32_splat((a), 0);          \
            break;                                      \
        case _MM_SHUFFLE(1, 1, 1, 1):                   \
            ret = vshuffleq_s32_splat((a), 1);          \
            break;                                      \
        case _MM_SHUFFLE(2, 2, 2, 2):                   \
            ret = vshuffleq_s32_splat((a), 2);          \
            break;                                      \
        case _MM_SHUFFLE(3, 3, 3, 3):                   \
            ret = vshuffleq_s32_splat((a), 3);          \
            break;                                      \
        default:                                        \
            ret = vshuffleq_s32_default((a), (imm));    \
            break;                                      \
        }                                               \
        ret;                                            \
    })

// Creates a 4-bit mask from the most significant bits of the four
// single-precision, floating-point values.
// https://msdn.microsoft.com/en-us/library/vstudio/4490ys29(v=vs.100).aspx
FORCE_INLINE int vmovemaskq_f32(float32x4_t a) {
#if 0 /* C version */
    uint32x4_t &ia = *(uint32x4_t *)&a;
    return (ia[0] >> 31) | ((ia[1] >> 30) & 2) | ((ia[2] >> 29) & 4) |
        ((ia[3] >> 28) & 8);
#endif
    static const uint32x4_t movemask = { 1, 2, 4, 8 };
    static const uint32x4_t highbit = { 0x80000000, 0x80000000, 0x80000000, 0x80000000 };
    uint32x4_t t0 = vreinterpretq_u32_f32(a);
    uint32x4_t t1 = vtstq_u32(t0, highbit);
    uint32x4_t t2 = vandq_u32(t1, movemask);
    uint32x2_t t3 = vorr_u32(vget_low_u32(t2), vget_high_u32(t2));
    return vget_lane_u32(t3, 0) | vget_lane_u32(t3, 1);
}

// 4 wide NEON float type.
struct neonf {
    union {
        float32x4_t f32x4;
        int32x4_t   i32x4;
        float       f32[4];
        int32_t     i32[4];
    };

    BE_FORCE_INLINE neonf() = default;
    BE_FORCE_INLINE neonf(const float32x4_t a) { f32x4 = a; }
    BE_FORCE_INLINE neonf(const int32x4_t a) { i32x4 = a; }
    BE_FORCE_INLINE neonf(float a, float b, float c, float d) { ALIGN_AS16 float data[4] = { a, b, c, d }; f32x4 = vld1q_f32(data); }

    BE_FORCE_INLINE neonf(const neonf &other) { f32x4 = other.f32x4; }
    BE_FORCE_INLINE neonf &operator=(const neonf &rhs) { f32x4 = rhs.f32x4; return *this; }

    BE_FORCE_INLINE operator const float32x4_t &() const { return f32x4; };
    BE_FORCE_INLINE operator float32x4_t &() { return f32x4; };

    BE_FORCE_INLINE operator const int32x4_t &() const { return i32x4; };
    BE_FORCE_INLINE operator int32x4_t &() { return i32x4; };

    BE_FORCE_INLINE const float &operator[](size_t index) const { assert(index < 4); return f32[index]; }
    BE_FORCE_INLINE float &operator[](size_t index) { assert(index < 4); return f32[index]; }
};

// 4 wide NEON integer type.
struct neoni {
    union {
        float32x4_t f32x4;
        int32x4_t   i32x4;
        uint32x4_t  u32x4;
        float       f32[4];
        int32_t     i32[4];
        int16_t     i16[8];
        int8_t      i8[16];
    };

    BE_FORCE_INLINE neoni() = default;
    BE_FORCE_INLINE neoni(const float32x4_t a) { f32x4 = a; }
    BE_FORCE_INLINE neoni(const int32x4_t a) { i32x4 = a; }
    BE_FORCE_INLINE neoni(int32_t a, int32_t b, int32_t c, int32_t d) { ALIGN_AS16 int32_t data[4] = { a, b, c, d }; i32x4 = vld1q_s32(data); }

    BE_FORCE_INLINE neoni(const neoni &other) { i32x4 = other.i32x4; }
    BE_FORCE_INLINE neoni &operator=(const neoni &rhs) { i32x4 = rhs.i32x4; return *this; }

    BE_FORCE_INLINE operator const float32x4_t &() const { return f32x4; };
    BE_FORCE_INLINE operator float32x4_t &() { return f32x4; };

    BE_FORCE_INLINE operator const int32x4_t &() const { return i32x4; };
    BE_FORCE_INLINE operator int32x4_t &() { return i32x4; };

    BE_FORCE_INLINE const int32_t &operator[](size_t index) const { assert(index < 4); return i32[index]; }
    BE_FORCE_INLINE int32_t &operator[](size_t index) { assert(index < 4); return i32[index]; }
};

// 4 wide NEON boolean type.
struct neonb {
    union {
        float32x4_t f32x4;
        int32x4_t   i32x4;
        uint32x4_t  u32x4;
        float       f32[4];
        int32_t     i32[4];
        uint32_t    u32[4];
    };

    BE_FORCE_INLINE neonb() = default;
    BE_FORCE_INLINE neonb(const float32x4_t a) { f32x4 = a; }
    BE_FORCE_INLINE neonb(const int32x4_t a) { i32x4 = a; }
    BE_FORCE_INLINE neonb(const uint32x4_t a) { u32x4 = a; }

    BE_FORCE_INLINE neonb(const neonb &other) { u32x4 = other.u32x4; }
    BE_FORCE_INLINE neonb &operator=(const neonb &rhs) { u32x4 = rhs.u32x4; return *this; }

    BE_FORCE_INLINE operator const float32x4_t &() const { return f32x4; };
    BE_FORCE_INLINE operator float32x4_t &() { return f32x4; };

    BE_FORCE_INLINE operator const int32x4_t &() const { return i32x4; };
    BE_FORCE_INLINE operator int32x4_t &() { return i32x4; };

    BE_FORCE_INLINE operator const uint32x4_t &() const { return u32x4; };
    BE_FORCE_INLINE operator uint32x4_t &() { return u32x4; };

    BE_FORCE_INLINE bool operator[](size_t index) const { assert(index < 4); return !!i32[index]; }
    BE_FORCE_INLINE int32_t &operator[](size_t index) { assert(index < 4); return i32[index]; }
};

using simd4f = neonf;
using simd4i = neoni;
using simd4b = neonb;
