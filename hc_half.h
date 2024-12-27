/**
 *  Copyright (c) 2024 Le Juez Victor
 *
 *  This software is provided "as-is", without any express or implied warranty. In no event 
 *  will the authors be held liable for any damages arising from the use of this software.
 *
 *  Permission is granted to anyone to use this software for any purpose, including commercial 
 *  applications, and to alter it and redistribute it freely, subject to the following restrictions:
 *
 *  1. The origin of this software must not be misrepresented; you must not claim that you 
 *  wrote the original software. If you use this software in a product, an acknowledgment 
 *  in the product documentation would be appreciated but is not required.
 *
 *  2. Altered source versions must be plainly marked as such, and must not be misrepresented
 *  as being the original software.
 *
 *   3. This notice may not be removed or altered from any source distribution.
 */

#ifndef HC_HALF_H
#define HC_HALF_H

#include <stdint.h>

/* Customizable macros and definitions */

#ifndef HCSAPI
#   define HCSAPI static inline
#endif //HCSAPI

/* Types definitions */

typedef uint16_t hc_half_t;

/* Float type 16 bits */

HCSAPI uint16_t
hc_cvt_fhi(uint32_t ui)
{
    int32_t s = (ui >> 16) & 0x8000;
    int32_t em = ui & 0x7fffffff;

    // bias exponent and round to nearest; 112 is relative exponent bias (127-15)
    int32_t h = (em - (112 << 23) + (1 << 12)) >> 13;

    // underflow: flush to zero; 113 encodes exponent -14
    h = (em < (113 << 23)) ? 0 : h;

    // overflow: infinity; 143 encodes exponent 16
    h = (em >= (143 << 23)) ? 0x7c00 : h;

    // NaN; note that we convert all types of NaN to qNaN
    h = (em > (255 << 23)) ? 0x7e00 : h;

    return (uint16_t)(s | h);
}

HCSAPI uint32_t
hc_cvt_hfi(uint16_t h)
{
    uint32_t s = (unsigned)(h & 0x8000) << 16;
    int32_t em = h & 0x7fff;

    // bias exponent and pad mantissa with 0; 112 is relative exponent bias (127-15)
    int32_t r = (em + (112 << 10)) << 13;

    // denormal: flush to zero
    r = (em < (1 << 10)) ? 0 : r;

    // infinity/NaN; note that we preserve NaN payload as a byproduct of unifying inf/nan cases
    // 112 is an exponent bias fixup; since we already applied it once, applying it twice converts 31 to 255
    r += (em >= (31 << 10)) ? (112 << 23) : 0;

    return s | r;
}

HCSAPI hc_half_t
hc_cvt_fh(float i)
{
    union { float f; uint32_t i; } v;
    v.f = i;
    return hc_cvt_fhi(v.i);
}

HCSAPI float
hc_cvt_hf(hc_half_t y)
{
    union { float f; uint32_t i; } v;
    v.i = hc_cvt_hfi(y);
    return v.f;
}

#endif // HC_HALF_H
