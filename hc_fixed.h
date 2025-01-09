/**
 * MIT License
 * 
 * Copyright (c) 2024-2025 Le Juez Victor
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#ifndef HC_FIXED_H
#define HC_FIXED_H

#include <stdint.h>

/* Customizable macros and definitions */

#ifndef HCSAPI
#   define HCSAPI static inline
#endif //HCSAPI

#ifndef HC_FX32_FRACTIONAL_BITS
#   define HC_FX32_FRACTIONAL_BITS 16
#endif //HC_FX32_FRACTIONAL_BITS

#ifndef HC_FX16_FRACTIONAL_BITS
#   define HC_FX16_FRACTIONAL_BITS 8
#endif //HC_FX16_FRACTIONAL_BITS

/* Constant macros and definitions */

#define HC_FX32_ONE (1 << HC_FX32_FRACTIONAL_BITS)
#define HC_FX16_ONE (1 << HC_FX16_FRACTIONAL_BITS)
#define HC_FR16_ONE 0xFFFF

/* Types definitions */

typedef int32_t hc_fx32_t;
typedef int16_t hc_fx16_t;
typedef uint16_t hc_fr16_t;
typedef uint16_t hc_float16_t;

/* Fixed point 32 bits */

HCSAPI hc_fx32_t
hc_cvt_f32_fx32(float x)
{
    return (hc_fx32_t)(x * (1 << HC_FX32_FRACTIONAL_BITS));
}

HCSAPI float
hc_cvt_fx32_f32(hc_fx32_t x)
{
    return (float)x / (1 << HC_FX32_FRACTIONAL_BITS);
}

HCSAPI hc_fx32_t
hc_cvt_i32_fx32(int32_t x)
{
    return (hc_fx32_t)(x << HC_FX32_FRACTIONAL_BITS);
}

HCSAPI int32_t
hc_cvt_fx32_i32(hc_fx32_t x)
{
    return (int)(x >> HC_FX32_FRACTIONAL_BITS);
}

HCSAPI hc_fx32_t
hc_abs_fx32(hc_fx32_t x)
{
    return x < 0 ? -x : x;
}

HCSAPI hc_fx32_t
hc_round_fx32(hc_fx32_t x)
{
    hc_fx32_t fractionalPart = x & ((1 << HC_FX32_FRACTIONAL_BITS) - 1);
    if (fractionalPart >= (1 << (HC_FX32_FRACTIONAL_BITS - 1))) {
        return (x >> HC_FX32_FRACTIONAL_BITS) + 1;
    } else {
        return x >> HC_FX32_FRACTIONAL_BITS;
    }
}

HCSAPI hc_fx32_t
hc_floor_fx32(hc_fx32_t x)
{
    return x & ~((1 << HC_FX32_FRACTIONAL_BITS) - 1);
}

HCSAPI hc_fx32_t
hc_fract_fx32(hc_fx32_t x)
{
    return x & ((1 << HC_FX32_FRACTIONAL_BITS) - 1);
}

HCSAPI hc_fx32_t
hc_add_fx32(hc_fx32_t x, hc_fx32_t y)
{
    return x + y;
}

HCSAPI hc_fx32_t
hc_sub_fx32(hc_fx32_t x, hc_fx32_t y)
{
    return x - y;
}

HCSAPI hc_fx32_t
hc_mul_fx32(hc_fx32_t x, hc_fx32_t y)
{
    return (hc_fx32_t)(((int64_t)x * y) >> HC_FX32_FRACTIONAL_BITS);
}

HCSAPI hc_fx32_t
hc_div_fx32(hc_fx32_t x, hc_fx32_t y)
{
    return (hc_fx32_t)(((int64_t)x << HC_FX32_FRACTIONAL_BITS) / y);
}

HCSAPI hc_fx32_t
hc_sqrt_fx32(hc_fx32_t x)
{
    if (x <= 0) return 0;

    // Initial estimate
    hc_fx32_t r = (x >> 1) + (1 << (HC_FX32_FRACTIONAL_BITS - 1));

    r = (r + hc_div_fx32(x, r)) >> 1; // An iteration of Newton-Raphson
    r = (r + hc_div_fx32(x, r)) >> 1; // Second iteration for more precision

    return r;
}

HCSAPI hc_fx32_t
hc_rsqrt_fx32(hc_fx32_t x)
{
    if (x <= 0) return 0;

    // Initial estimate based on the "Fast Inverse Square Root" algorithm
    int32_t i = 0x5f3759df - (x >> 1);
    hc_fx32_t r = *(hc_fx32_t*)&i;

    // An iteration of Newton-Raphson
    hc_fx32_t halfx = x >> 1;
    r = hc_mul_fx32(r, 0x30000000 - hc_mul_fx32(halfx, hc_mul_fx32(r, r)));

    // Second iteration for more precision
    r = hc_mul_fx32(r, 0x30000000 - hc_mul_fx32(halfx, hc_mul_fx32(r, r)));

    return r;
}

/* Fixed point - 16 bits */

HCSAPI hc_fx16_t
hc_cvt_f32_fx16(float x)
{
    return (hc_fx16_t)(x * (1 << HC_FX16_FRACTIONAL_BITS));
}

HCSAPI float
hc_cvt_fx16_f32(hc_fx16_t x)
{
    return (float)x / (1 << HC_FX16_FRACTIONAL_BITS);
}

HCSAPI hc_fx16_t
hc_cvt_i32_fx16(int32_t x)
{
    return (hc_fx32_t)(x << HC_FX16_FRACTIONAL_BITS);
}

HCSAPI int32_t
hc_cvt_fx16_i32(hc_fx16_t x)
{
    return (int)(x >> HC_FX16_FRACTIONAL_BITS);
}

HCSAPI hc_fx16_t
hc_abs_fx16(hc_fx16_t x)
{
    return x < 0 ? -x : x;
}

HCSAPI hc_fx16_t
hc_round_fx16(hc_fx16_t x)
{
    hc_fx16_t fractionalPart = x & ((1 << HC_FX16_FRACTIONAL_BITS) - 1);
    if (fractionalPart >= (1 << (HC_FX16_FRACTIONAL_BITS - 1))) {
        return (x >> HC_FX16_FRACTIONAL_BITS) + 1;
    } else {
        return x >> HC_FX16_FRACTIONAL_BITS;
    }
}

HCSAPI hc_fx16_t
hc_floor_fx16(hc_fx16_t x)
{
    return x & ~((1 << HC_FX16_FRACTIONAL_BITS) - 1);
}

HCSAPI hc_fx16_t
hc_fract_fx16(hc_fx16_t x)
{
    return x & ((1 << HC_FX16_FRACTIONAL_BITS) - 1);
}

HCSAPI hc_fx16_t
hc_add_fx16(hc_fx16_t x, hc_fx16_t y)
{
    return x + y;
}

HCSAPI hc_fx16_t
hc_sub_fx16(hc_fx16_t x, hc_fx16_t y)
{
    return x - y;
}

HCSAPI hc_fx16_t
hc_mul_fx16(hc_fx16_t x, hc_fx16_t y)
{
    return (hc_fx16_t)(((int32_t)x * y) >> HC_FX16_FRACTIONAL_BITS);
}

HCSAPI hc_fx16_t
hc_div_fx16(hc_fx16_t x, hc_fx16_t y)
{
    return (hc_fx16_t)(((int32_t)x << HC_FX16_FRACTIONAL_BITS) / y);
}

/* Fract type 16 bits */

HCSAPI hc_fr16_t
hc_cvt_f32_fr16(float x)
{
    if (x <= 0.0f) return 0;
    if (x >= 1.0f) return HC_FR16_ONE;
    return (hc_fr16_t)(x * HC_FR16_ONE + 0.5f);
}

HCSAPI float
hc_cvt_fr16_f32(hc_fr16_t x)
{
    return (float)x / HC_FR16_ONE;
}

HCSAPI hc_fr16_t
hc_cvt_fx16_fr16(hc_fx16_t x)
{
    if (x <= 0) return 0;
    if (x >= (1 << HC_FX16_FRACTIONAL_BITS)) return HC_FR16_ONE;
    return (hc_fr16_t)((uint32_t)x * HC_FR16_ONE >> HC_FX16_FRACTIONAL_BITS);
}

HCSAPI hc_fx16_t
hc_cvt_fr16_fx16(hc_fr16_t x)
{
    return (hc_fx16_t)((uint32_t)x * (1 << HC_FX16_FRACTIONAL_BITS) / HC_FR16_ONE);
}

HCSAPI hc_fr16_t
hc_add_fr16(hc_fr16_t x, hc_fr16_t y)
{
    uint32_t result = (uint32_t)x + y;
    return (result > HC_FR16_ONE) ? HC_FR16_ONE : (hc_fr16_t)result;
}

HCSAPI hc_fr16_t
hc_sub_fr16(hc_fr16_t x, hc_fr16_t y)
{
    return (x > y) ? (x - y) : 0;
}

HCSAPI hc_fr16_t
hc_mul_fr16(hc_fr16_t x, hc_fr16_t y)
{
    return (hc_fr16_t)(((uint32_t)x * y + HC_FR16_ONE / 2) >> 16);
}

HCSAPI hc_fr16_t
hc_div_fr16(hc_fr16_t x, hc_fr16_t y)
{
    return (hc_fr16_t)(((uint32_t)x << 16) / y);
}

#endif // HC_FIXED_H
