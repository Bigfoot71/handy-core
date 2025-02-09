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

#ifndef HC_MATH_H
#define HC_MATH_H

#include <stdint.h>
#include <math.h>

/* Macros and defintions */

#ifndef HCSAPI
#   define HCSAPI static inline
#endif // HCSAPI

#ifndef HC_FISR
#   define hc_rsqrtf(x) (1.0f / sqrtf(x))
#else
// NOTE: More useful on older platforms.
// SEE: http://www.lomont.org/papers/2003/InvSqrt.pdf
HCSAPI float hc_rsqrtf(float x)
{
    float xhalf = 0.5f*x;
    int i = *(int*)&x;              // get bits for floating value
    i = 0x5f375a86 - (i >> 1);      // gives initial guess y0
    x = *(float*)&i;                // convert bits back to float
    x = x*(1.5f - xhalf*x*x);       // Newton step, repeating increases accuracy
    return x;
}
#endif // HC_FISR

#define HC_PI 3.14159265358979323846
#define HC_TAU (2.0 * HC_PI)

#define HC_DEG2RAD (HC_PI / 180.0)
#define HC_RAD2DEG (180.0 / HC_PI)

/* Platform Specific */

#ifndef HC_RESTRICT
#   ifdef _MSC_VER
#       define HC_RESTRICT __restrict
#   else // ANY_PLATFORM
#       define HC_RESTRICT restrict
#   endif // PLATFORM
#endif // HC_RESTRICT

/* Types definitions */

typedef float hc_vec2_t[2];
typedef float* hc_vec2_ptr_t;
typedef float const* hc_vec2_cptr_t;

typedef float hc_vec3_t[3];
typedef float* hc_vec3_ptr_t;
typedef float const* hc_vec3_cptr_t;

typedef float hc_vec4_t[4];
typedef float* hc_vec4_ptr_t;
typedef float const* hc_vec4_cptr_t;

typedef float hc_mat4_t[16];
typedef float* hc_mat4_ptr_t;
typedef float const* hc_mat4_cptr_t;

/* Scalar functions */

HCSAPI int
hc_clampi(int x, int min, int max)
{
    return x < min ? min : (x > max ? max : x);
}

HCSAPI float
hc_clampf(float x, float min, float max)
{
    return x < min ? min : (x > max ? max : x);
}

HCSAPI float
hc_saturate(float x)
{
    return x < 0.0f ? 0.0f : (x > 1.0f ? 1.0f : x);
}

HCSAPI int
hc_wrapi(int value, int min, int max)
{
    int range = max - min;
    return min + (value - min) % range;
}

HCSAPI float
hc_wrapf(float value, float min, float max)
{
    float range = max - min;
    return min + fmod(value - min, range);
}

HCSAPI float
hc_wrap_angle(float radians)
{
    float wrapped = fmod(radians, HC_TAU);
    if (wrapped < -HC_PI) {
        wrapped += HC_TAU;
    } else if (wrapped > HC_PI) {
        wrapped -= HC_TAU;
    }
    return wrapped;
}

HCSAPI float
hc_normalize(float value, float start, float end)
{
    return (value - start) / (end - start);
}

HCSAPI float
hc_remap(float value, float input_start, float input_end, float output_start, float output_end)
{
    return (value - input_start) / (input_end - input_start) * (output_end - output_start) + output_start;
}

HCSAPI float
hc_fract(float x)
{
    return x - floorf(x);
}

HCSAPI float
hc_step(float edge, float x)
{
    return (x < edge) ? 0.0 : 1.0;
}

HCSAPI int
hs_sign(int x)
{
    return (x > 0) - (x < 0);
}

HCSAPI int
hc_approx(float a, float b, float epsilon)
{
    return fabsf(a - b) < epsilon;
}

HCSAPI float
hc_lerp(float a, float b, float t)
{
    return a + t * (b - a);
}

HCSAPI float
hc_lerp_angle(float a, float b, float t)
{
    float diff = hc_wrap_angle(b - a);
    return a + diff * t;
}

HCSAPI float
hc_inverse_lerp(float a, float b, float value)
{
    return (value - a) / (b - a);
}

HCSAPI float
hc_smoothstep(float edge0, float edge1, float x)
{
    float t = (x - edge0) / (edge1 - edge0);
    t = t < 0.0 ? 0.0 : (t > 1.0 ? 1.0 : t);
    return t * t * t * (t * (t * 6 - 15) + 10);
}

HCSAPI float
hc_exp_decay(float initial, float decay_rate, float time)
{
    return initial * expf(-decay_rate * time);
}

HCSAPI float
hc_move_towards(float current, float target, float max_delta)
{
    float delta = target - current;
    float distance = fabsf(delta);
    if (distance <= max_delta) return target;
    else return current + (delta / distance) * max_delta;
}

HCSAPI uint32_t
hc_next_po2(uint32_t x)
{
    if (x == 0) return 1;
    if ((x & (x - 1)) == 0) return x << 1; // (x * 2)
    x--;
    x |= x >> 1;
    x |= x >> 2;
    x |= x >> 4;
    x |= x >> 8;
    x |= x >> 16;
    x++;
    return x;
}

HCSAPI uint32_t
hc_previous_po2(uint32_t x)
{
    if (x == 0) return 0;
    if ((x & (x - 1)) == 0) return x >> 1; // (x / 2)
    x |= x >> 1;
    x |= x >> 2;
    x |= x >> 4;
    x |= x >> 8;
    x |= x >> 16;
    return x - (x >> 1);
}

HCSAPI uint32_t
hc_nearest_po2(uint32_t x)
{
    uint32_t next = hc_next_po2(x);
    uint32_t prev = hc_previous_po2(x);
    return (x - prev < next - x) ? prev : next;
}

HCSAPI uint64_t
hc_next_po2_u64(uint64_t x)
{
    if (x == 0) return 1;
    if ((x & (x - 1)) == 0) return x << 1; // (x * 2)
    x--;
    x |= x >> 1;
    x |= x >> 2;
    x |= x >> 4;
    x |= x >> 8;
    x |= x >> 16;
    x |= x >> 32; //< Only for 64 bits
    x++;
    return x;
}

HCSAPI uint64_t
hc_previous_po2_u64(uint64_t x)
{
    if (x == 0) return 0;
    if ((x & (x - 1)) == 0) return x >> 1; // (x / 2)
    x |= x >> 1;
    x |= x >> 2;
    x |= x >> 4;
    x |= x >> 8;
    x |= x >> 16;
    x |= x >> 32; //< Only for 64 bits
    return x - (x >> 1);
}

HCSAPI uint64_t
hc_nearest_po2_u64(uint64_t x)
{
    uint64_t next = hc_next_po2_u64(x);
    uint64_t prev = hc_previous_po2_u64(x);
    return (x - prev < next - x) ? prev : next;
}

/* 2D Vector function definitions */

HCSAPI void
hc_vec2_zero(hc_vec2_t dst)
{
    for (int_fast8_t i = 0; i < 2; i++) {
        dst[i] = 0;
    }
}

HCSAPI void
hc_vec2_one(hc_vec2_t dst, float v)
{
    dst[0] = dst[1] = v;
}

HCSAPI void
hc_vec2_set(hc_vec2_t dst, float x, float y)
{
    dst[0] = x, dst[1] = y;
}

HCSAPI void
hc_vec2_copy(hc_vec2_ptr_t HC_RESTRICT dst, const hc_vec2_ptr_t HC_RESTRICT src)
{
    for (int_fast8_t i = 0; i < 2; i++) {
        dst[i] = src[i];
    }
}

HCSAPI void
hc_vec2_swap(hc_vec2_ptr_t HC_RESTRICT a, hc_vec2_ptr_t HC_RESTRICT b)
{
    for (int_fast8_t i = 0; i < 2; i++) {
        float tmp = a[i];
        a[i] = b[i];
        b[i] = tmp;
    }
}

HCSAPI void
hc_vec2_neg(hc_vec2_t dst, const hc_vec2_t v)
{
    for (int_fast8_t i = 0; i < 2; i++) {
        dst[i] = -v[i];
    }
}

HCSAPI void
hc_vec2_neg_r(hc_vec2_ptr_t HC_RESTRICT dst, const hc_vec2_t v)
{
    for (int_fast8_t i = 0; i < 2; i++) {
        dst[i] = -v[i];
    }
}

HCSAPI void
hc_vec2_add(hc_vec2_t dst, const hc_vec2_t v1, const hc_vec2_t v2)
{
    for (int_fast8_t i = 0; i < 2; i++) {
        dst[i] = v1[i] + v2[i];
    }
}

HCSAPI void
hc_vec2_add_r(hc_vec2_ptr_t HC_RESTRICT dst, const hc_vec2_t v1, const hc_vec2_t v2)
{
    for (int_fast8_t i = 0; i < 2; i++) {
        dst[i] = v1[i] + v2[i];
    }
}

HCSAPI void
hc_vec2_sub(hc_vec2_t dst, const hc_vec2_t v1, const hc_vec2_t v2)
{
    for (int_fast8_t i = 0; i < 2; i++) {
        dst[i] = v1[i] - v2[i];
    }
}

HCSAPI void
hc_vec2_sub_r(hc_vec2_ptr_t HC_RESTRICT dst, const hc_vec2_t v1, const hc_vec2_t v2)
{
    for (int_fast8_t i = 0; i < 2; i++) {
        dst[i] = v1[i] - v2[i];
    }
}

HCSAPI void
hc_vec2_mul(hc_vec2_t dst, const hc_vec2_t v1, const hc_vec2_t v2)
{
    for (int_fast8_t i = 0; i < 2; i++) {
        dst[i] = v1[i] * v2[i];
    }
}

HCSAPI void
hc_vec2_mul_r(hc_vec2_ptr_t HC_RESTRICT dst, const hc_vec2_t v1, const hc_vec2_t v2)
{
    for (int_fast8_t i = 0; i < 2; i++) {
        dst[i] = v1[i] * v2[i];
    }
}

HCSAPI void
hc_vec2_div(hc_vec2_t dst, const hc_vec2_t v1, const hc_vec2_t v2)
{
    for (int_fast8_t i = 0; i < 2; i++) {
        dst[i] = v1[i]/v2[i];
    }
}

HCSAPI void
hc_vec2_div_r(hc_vec2_ptr_t HC_RESTRICT dst, const hc_vec2_t v1, const hc_vec2_t v2)
{
    for (int_fast8_t i = 0; i < 2; i++) {
        dst[i] = v1[i]/v2[i];
    }
}

HCSAPI void
hc_vec2_offset(hc_vec2_t dst, const hc_vec2_t v, float scalar)
{
    for (int_fast8_t i = 0; i < 2; i++) {
        dst[i] = v[i] + scalar;
    }
}

HCSAPI void
hc_vec2_offset_r(hc_vec2_ptr_t HC_RESTRICT dst, const hc_vec2_t v, float scalar)
{
    for (int_fast8_t i = 0; i < 2; i++) {
        dst[i] = v[i] + scalar;
    }
}

HCSAPI void
hc_vec2_scale(hc_vec2_t dst, const hc_vec2_t v, float scalar)
{
    for (int_fast8_t i = 0; i < 2; i++) {
        dst[i] = v[i]*scalar;
    }
}

HCSAPI void
hc_vec2_scale_r(hc_vec2_ptr_t HC_RESTRICT dst, const hc_vec2_t v, float scalar)
{
    for (int_fast8_t i = 0; i < 2; i++) {
        dst[i] = v[i]*scalar;
    }
}

HCSAPI void
hc_vec2_normalize(hc_vec2_t dst, const hc_vec2_t v)
{
    float length_sq = v[0] * v[0] + v[1] * v[1];
    if (length_sq == 0.0f) return;

    float inv_lenght = hc_rsqrtf(length_sq);
    for (int_fast8_t i = 0; i < 2; i++) {
        dst[i] = v[i] * inv_lenght;
    }
}

HCSAPI void
hc_vec2_normalize_r(hc_vec2_ptr_t HC_RESTRICT dst, const hc_vec2_t v)
{
    float length_sq = v[0] * v[0] + v[1] * v[1];
    if (length_sq == 0.0f) return;

    float inv_lenght = hc_rsqrtf(length_sq);
    for (int_fast8_t i = 0; i < 2; i++) {
        dst[i] = v[i] * inv_lenght;
    }
}

HCSAPI float
hc_vec2_length(const hc_vec2_t v)
{
    return sqrtf(v[0] * v[0] + v[1] * v[1]);
}

HCSAPI float
hc_length_sq_vec2(const hc_vec2_t v)
{
    return v[0] * v[0] + v[1] * v[1];
}

HCSAPI float
hc_vec2_dot(const hc_vec2_t v1, const hc_vec2_t v2)
{
    return v1[0] * v2[0] + v1[1] * v2[1];
}

HCSAPI float
hc_vec2_distance(const hc_vec2_t v1, const hc_vec2_t v2)
{
    hc_vec2_t dt = {
        v1[0] - v2[0],
        v1[1] - v2[1]
    };

    float dist_sq = dt[0]*dt[0] +
                           dt[1]*dt[1];

#ifdef HC_FISR
    return dist_sq*hc_rsqrtf(dist_sq);
#else
    return sqrtf(dist_sq);
#endif
}

HCSAPI float
hc_distance_sq_vec2(const hc_vec2_t v1, const hc_vec2_t v2)
{
    hc_vec2_t dt = { v1[0] - v2[0], v1[1] - v2[1] };
    return dt[0]*dt[0] + dt[1]*dt[1];
}

HCSAPI void
hc_vec2_direction(hc_vec2_t dst, const hc_vec2_t v1, const hc_vec2_t v2)
{
    hc_vec2_t tmp;
    float lengthSq = 0.0f;

#   ifdef _OPENMP
#       pragma omp simd
#   endif
    for (int_fast8_t i = 0; i < 2; i++) {
        tmp[i] = v1[i] - v2[i];
        lengthSq += tmp[i]*tmp[i];
    }

    float inv_lenght = hc_rsqrtf(lengthSq);

#   ifdef _OPENMP
#       pragma omp simd
#   endif
    for (int_fast8_t i = 0; i < 2; i++) {
        dst[i] = tmp[i]*inv_lenght;
    }
}

HCSAPI void
hc_vec2_direction_r(hc_vec2_ptr_t HC_RESTRICT dst, const hc_vec2_t v1, const hc_vec2_t v2)
{
    float lengthSq = 0.0f;

#   ifdef _OPENMP
#       pragma omp simd
#   endif
    for (int_fast8_t i = 0; i < 2; i++) {
        dst[i] = v1[i] - v2[i];
        lengthSq += dst[i]*dst[i];
    }

    float inv_lenght = hc_rsqrtf(lengthSq);

#   ifdef _OPENMP
#       pragma omp simd
#   endif
    for (int_fast8_t i = 0; i < 2; i++) {
        dst[i] = dst[i]*inv_lenght;
    }
}

HCSAPI void
hc_vec2_lerp(hc_vec2_t dst, const hc_vec2_t v1, const hc_vec2_t v2, float t)
{
    for (int_fast8_t i = 0; i < 2; i++) {
        dst[i] = v1[i] + t*(v2[i]-v1[i]);
    }
}

HCSAPI void
hc_vec2_lerp_r(hc_vec2_ptr_t HC_RESTRICT dst, const hc_vec2_t v1, const hc_vec2_t v2, float t)
{
    for (int_fast8_t i = 0; i < 2; i++) {
        dst[i] = v1[i] + t*(v2[i]-v1[i]);
    }
}

HCSAPI void
hc_vec2_barysmooth(hc_vec2_t dst, const hc_vec2_t v1, const hc_vec2_t v2, const hc_vec2_t v3, float w1, float w2, float w3)
{
    for (int_fast8_t i = 0; i < 2; i++) {
        dst[i] = w1*v1[i] + w2*v2[i] + w3*v3[i];
    }
}

HCSAPI void
hc_vec2_barysmooth_r(hc_vec2_ptr_t HC_RESTRICT dst, const hc_vec2_t v1, const hc_vec2_t v2, const hc_vec2_t v3, float w1, float w2, float w3)
{
    for (int_fast8_t i = 0; i < 2; i++) {
        dst[i] = w1*v1[i] + w2*v2[i] + w3*v3[i];
    }
}

HCSAPI void
hc_vec2_barysmooth_v(hc_vec2_t dst, const hc_vec2_t v1, const hc_vec2_t v2, const hc_vec2_t v3, const hc_vec3_t w)
{
    for (int_fast8_t i = 0; i < 2; i++) {
        dst[i] = w[0] * v1[i] + w[1] * v2[i] + w[2] * v3[i];
    }
}

HCSAPI void
hc_vec2_barysmooth_vr(hc_vec2_ptr_t HC_RESTRICT dst, const hc_vec2_t v1, const hc_vec2_t v2, const hc_vec2_t v3, const hc_vec3_t w)
{
    for (int_fast8_t i = 0; i < 2; i++) {
        dst[i] = w[0] * v1[i] + w[1] * v2[i] + w[2] * v3[i];
    }
}

HCSAPI void
hc_vec2_baryflat(hc_vec4_t dst, const hc_vec4_t v1, const hc_vec4_t v2, const hc_vec4_t v3, float w1, float w2, float w3)
{
    // Find the maximum weight
    float max_w = w1;
    if (w2 > max_w) max_w = w2;
    if (w3 > max_w) max_w = w3;

    // Determine which weight is the maximum
    int_fast8_t isW1Max = (max_w == w1);
    int_fast8_t isW2Max = (max_w == w2);

    // Select the corresponding color based on the maximum weight
    for (int_fast8_t i = 0; i < 2; ++i) {
        if (isW1Max) {
            dst[i] = v1[i];
        } else if (isW2Max) {
            dst[i] = v2[i];
        } else {
            dst[i] = v3[i];
        }
    }
}

HCSAPI void
hc_vec2_baryflat_r(hc_vec4_ptr_t HC_RESTRICT dst, const hc_vec4_t v1, const hc_vec4_t v2, const hc_vec4_t v3, float w1, float w2, float w3)
{
    // Find the maximum weight
    float max_w = w1;
    if (w2 > max_w) max_w = w2;
    if (w3 > max_w) max_w = w3;

    // Determine which weight is the maximum
    int_fast8_t isW1Max = (max_w == w1);
    int_fast8_t isW2Max = (max_w == w2);

    // Select the corresponding color based on the maximum weight
    for (int_fast8_t i = 0; i < 2; ++i) {
        if (isW1Max) {
            dst[i] = v1[i];
        } else if (isW2Max) {
            dst[i] = v2[i];
        } else {
            dst[i] = v3[i];
        }
    }
}

HCSAPI void
hc_vec2_baryflat_v(hc_vec4_t dst, const hc_vec4_t v1, const hc_vec4_t v2, const hc_vec4_t v3, const hc_vec3_t w)
{
    // Find the maximum weight
    float max_w = w[0];
    if (w[1] > max_w) max_w = w[1];
    if (w[2] > max_w) max_w = w[2];

    // Determine which weight is the maximum
    int_fast8_t isW1Max = (max_w == w[0]);
    int_fast8_t isW2Max = (max_w == w[1]);

    // Select the corresponding color based on the maximum weight
    for (int_fast8_t i = 0; i < 2; ++i) {
        if (isW1Max) {
            dst[i] = v1[i];
        } else if (isW2Max) {
            dst[i] = v2[i];
        } else {
            dst[i] = v3[i];
        }
    }
}

HCSAPI void
hc_vec2_baryflat_vr(hc_vec4_ptr_t HC_RESTRICT dst, const hc_vec4_t v1, const hc_vec4_t v2, const hc_vec4_t v3, const hc_vec3_t w)
{
    // Find the maximum weight
    float max_w = w[0];
    if (w[1] > max_w) max_w = w[1];
    if (w[2] > max_w) max_w = w[2];

    // Determine which weight is the maximum
    int_fast8_t isW1Max = (max_w == w[0]);
    int_fast8_t isW2Max = (max_w == w[1]);

    // Select the corresponding color based on the maximum weight
    for (int_fast8_t i = 0; i < 2; ++i) {
        if (isW1Max) {
            dst[i] = v1[i];
        } else if (isW2Max) {
            dst[i] = v2[i];
        } else {
            dst[i] = v3[i];
        }
    }
}

HCSAPI void
hc_vec2_transform(hc_vec2_t dst, const hc_vec2_t v, const hc_mat4_t mat)
{
    hc_vec2_t tmp = {
        mat[0] * v[0] + mat[4] * v[1] + mat[12],
        mat[1] * v[0] + mat[5] * v[1] + mat[13]
    };

    for (int_fast8_t i = 0; i < 2; i++) {
        dst[i] = tmp[i];
    }
}

HCSAPI void
hc_vec2_transform_r(hc_vec2_ptr_t HC_RESTRICT dst, const hc_vec2_t v, const hc_mat4_t mat)
{
    dst[0] = mat[0] * v[0] + mat[4] * v[1] + mat[12];
    dst[1] = mat[1] * v[0] + mat[5] * v[1] + mat[13];
}

HCSAPI void
hc_vec2_transform_wt(hc_vec2_t dst, const hc_vec2_t v, float w_translation, const hc_mat4_t mat)
{
    hc_vec2_t tmp = {
        mat[0] * v[0] + mat[4] * v[1] + w_translation * mat[12],
        mat[1] * v[0] + mat[5] * v[1] + w_translation * mat[13]
    };

    for (int_fast8_t i = 0; i < 2; i++) {
        dst[i] = tmp[i];
    }
}

HCSAPI void
hc_vec2_transform_wtr(hc_vec2_ptr_t HC_RESTRICT dst, const hc_vec2_t v, float w_translation, const hc_mat4_t mat)
{
    dst[0] = mat[0] * v[0] + mat[4] * v[1] + w_translation*mat[12];
    dst[1] = mat[1] * v[0] + mat[5] * v[1] + w_translation*mat[13];
}

/* 3D Vector function definitions */

HCSAPI void
hc_vec3_zero(hc_vec3_t dst)
{
    for (int_fast8_t i = 0; i < 3; i++) {
        dst[i] = 0;
    }
}

HCSAPI void
hc_vec3_one(hc_vec3_t dst, float v)
{
    dst[0] = dst[1] = dst[2] = v;
}

HCSAPI void
hc_vec3_set(hc_vec3_t dst, float x, float y, float z)
{
    dst[0] = x, dst[1] = y, dst[2] = z;
}

HCSAPI void
hc_vec3_copy(hc_vec3_ptr_t HC_RESTRICT dst, const hc_vec3_ptr_t HC_RESTRICT src)
{
    for (int_fast8_t i = 0; i < 3; i++) {
        dst[i] = src[i];
    }
}

HCSAPI void
hc_vec3_swap(hc_vec3_ptr_t HC_RESTRICT a, hc_vec3_ptr_t HC_RESTRICT b)
{
#   ifdef _OPENMP
#       pragma omp simd
#   endif
    for (int_fast8_t i = 0; i < 3; i++) {
        float tmp = a[i];
        a[i] = b[i];
        b[i] = tmp;
    }
}

HCSAPI void
hc_vec3_neg(hc_vec3_t dst, const hc_vec3_t v)
{
#   ifdef _OPENMP
#       pragma omp simd
#   endif
    for (int_fast8_t i = 0; i < 3; i++) {
        dst[i] = -v[i];
    }
}

HCSAPI void
hc_vec3_neg_r(hc_vec3_ptr_t HC_RESTRICT dst, const hc_vec3_t v)
{
#   ifdef _OPENMP
#       pragma omp simd
#   endif
    for (int_fast8_t i = 0; i < 3; i++) {
        dst[i] = -v[i];
    }
}

HCSAPI void
hc_vec3_add(hc_vec3_t dst, const hc_vec3_t v1, const hc_vec3_t v2)
{
#   ifdef _OPENMP
#       pragma omp simd
#   endif
    for (int_fast8_t i = 0; i < 3; i++) {
        dst[i] = v1[i] + v2[i];
    }
}

HCSAPI void
hc_vec3_add_r(hc_vec3_ptr_t HC_RESTRICT dst, const hc_vec3_t v1, const hc_vec3_t v2)
{
#   ifdef _OPENMP
#       pragma omp simd
#   endif
    for (int_fast8_t i = 0; i < 3; i++) {
        dst[i] = v1[i] + v2[i];
    }
}

HCSAPI void
hc_vec3_sub(hc_vec3_t dst, const hc_vec3_t v1, const hc_vec3_t v2)
{
#   ifdef _OPENMP
#       pragma omp simd
#   endif
    for (int_fast8_t i = 0; i < 3; i++) {
        dst[i] = v1[i] - v2[i];
    }
}

HCSAPI void
hc_vec3_sub_r(hc_vec3_ptr_t HC_RESTRICT dst, const hc_vec3_t v1, const hc_vec3_t v2)
{
#   ifdef _OPENMP
#       pragma omp simd
#   endif
    for (int_fast8_t i = 0; i < 3; i++) {
        dst[i] = v1[i] - v2[i];
    }
}

HCSAPI void
hc_vec3_mul(hc_vec3_t dst, const hc_vec3_t v1, const hc_vec3_t v2)
{
#   ifdef _OPENMP
#       pragma omp simd
#   endif
    for (int_fast8_t i = 0; i < 3; i++) {
        dst[i] = v1[i] * v2[i];
    }
}

HCSAPI void
hc_vec3_mul_r(hc_vec3_ptr_t HC_RESTRICT dst, const hc_vec3_t v1, const hc_vec3_t v2)
{
#   ifdef _OPENMP
#       pragma omp simd
#   endif
    for (int_fast8_t i = 0; i < 3; i++) {
        dst[i] = v1[i] * v2[i];
    }
}

HCSAPI void
hc_vec3_div(hc_vec3_t dst, const hc_vec3_t v1, const hc_vec3_t v2)
{
#   ifdef _OPENMP
#       pragma omp simd
#   endif
    for (int_fast8_t i = 0; i < 3; i++) {
        dst[i] = v1[i]/v2[i];
    }
}

HCSAPI void
hc_vec3_div_r(hc_vec3_ptr_t HC_RESTRICT dst, const hc_vec3_t v1, const hc_vec3_t v2)
{
#   ifdef _OPENMP
#       pragma omp simd
#   endif
    for (int_fast8_t i = 0; i < 3; i++) {
        dst[i] = v1[i]/v2[i];
    }
}

HCSAPI void
hc_vec3_offset(hc_vec3_t dst, const hc_vec3_t v, float scalar)
{
#   ifdef _OPENMP
#       pragma omp simd
#   endif
    for (int_fast8_t i = 0; i < 3; i++) {
        dst[i] = v[i] + scalar;
    }
}

HCSAPI void
hc_vec3_offset_r(hc_vec3_ptr_t HC_RESTRICT dst, const hc_vec3_t v, float scalar)
{
#   ifdef _OPENMP
#       pragma omp simd
#   endif
    for (int_fast8_t i = 0; i < 3; i++) {
        dst[i] = v[i] + scalar;
    }
}

HCSAPI void
hc_vec3_scale(hc_vec3_t dst, const hc_vec3_t v, float scalar)
{
#   ifdef _OPENMP
#       pragma omp simd
#   endif
    for (int_fast8_t i = 0; i < 3; i++) {
        dst[i] = v[i]*scalar;
    }
}

HCSAPI void
hc_vec3_scale_r(hc_vec3_ptr_t HC_RESTRICT dst, const hc_vec3_t v, float scalar)
{
#   ifdef _OPENMP
#       pragma omp simd
#   endif
    for (int_fast8_t i = 0; i < 3; i++) {
        dst[i] = v[i]*scalar;
    }
}

HCSAPI void
hc_vec3_normalize(hc_vec3_t dst, const hc_vec3_t v)
{
    float length_sq = v[0] * v[0] + v[1] * v[1] + v[2] * v[2];
    if (length_sq == 0.0f) return;

    float inv_lenght = hc_rsqrtf(length_sq);

#   ifdef _OPENMP
#       pragma omp simd
#   endif
    for (int_fast8_t i = 0; i < 3; i++) {
        dst[i] = v[i] * inv_lenght;
    }
}

HCSAPI void
hc_vec3_normalize_r(hc_vec3_ptr_t HC_RESTRICT dst, const hc_vec3_t v)
{
    float length_sq = v[0] * v[0] + v[1] * v[1] + v[2] * v[2];
    if (length_sq == 0.0f) return;

    float inv_lenght = hc_rsqrtf(length_sq);

#   ifdef _OPENMP
#       pragma omp simd
#   endif
    for (int_fast8_t i = 0; i < 3; i++) {
        dst[i] = v[i] * inv_lenght;
    }
}

HCSAPI float
hc_vec3_length(const hc_vec3_t v)
{
    return sqrtf(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
}

HCSAPI float
hc_length_sq_vec3(const hc_vec3_t v)
{
    return v[0] * v[0] + v[1] * v[1] + v[2] * v[2];
}

HCSAPI float
hc_vec3_dot(const hc_vec3_t v1, const hc_vec3_t v2)
{
#ifdef _OPENMP
    float dotProduct = 0.0f;
#   pragma omp simd
    for (int_fast8_t i = 0; i < 3; i++) {
        dotProduct += v1[i] * v2[i];
    }
    return dotProduct;
#else
    return v1[0] * v2[0] + v1[1] * v2[1] + v1[2] * v2[2];
#endif
}

HCSAPI void
hc_vec3_cross(hc_vec3_t dst, const hc_vec3_t v1, const hc_vec3_t v2)
{
    hc_vec3_t tmp = {
        v1[1] * v2[2] - v1[2] * v2[1],
        v1[2] * v2[0] - v1[0] * v2[2],
        v1[0] * v2[1] - v1[1] * v2[0]
    };

    for (int_fast8_t i = 0; i < 3; i++) {
        dst[i] = tmp[i];
    }
}

HCSAPI void
hc_vec3_cross_r(hc_vec3_ptr_t HC_RESTRICT dst, const hc_vec3_t v1, const hc_vec3_t v2)
{
    dst[0] = v1[1] * v2[2] - v1[2] * v2[1];
    dst[1] = v1[2] * v2[0] - v1[0] * v2[2];
    dst[2] = v1[0] * v2[1] - v1[1] * v2[0];
}

HCSAPI float
hc_vec3_distance(const hc_vec3_t v1, const hc_vec3_t v2)
{
    // NOTE: Here, the loop version adds a conditional branch, in doubt we keep it simple.

    hc_vec3_t dt = {
        v1[0] - v2[0],
        v1[1] - v2[1],
        v1[2] - v2[2]
    };

    float dist_sq = dt[0]*dt[0] +
                    dt[1]*dt[1] +
                    dt[2]*dt[2];

#ifdef HC_FISR
    return dist_sq*hc_rsqrtf(dist_sq);
#else
    return sqrtf(dist_sq);
#endif
}

HCSAPI float
hc_distance_sq_vec3(const hc_vec3_t v1, const hc_vec3_t v2)
{
    // NOTE 1: The code generated by GCC 11/13 in O3 utilizes SIMD operations more efficiently than the non-loop version
    // NOTE 2: Still with GCC 13 in O3, the code generated with 'omp simd' is the same as without, but on GCC versions lower than 11.1 the code generated with 'omp simd' retains the loop...

    float dist_sq = 0.0f;
    for (int_fast8_t i = 0; i < 3; i++) {
        float dt = v1[i] - v2[i];
        dist_sq += dt*dt;
    }
    return dist_sq;
}

HCSAPI void
hc_vec3_direction(hc_vec3_t dst, const hc_vec3_t v1, const hc_vec3_t v2)
{
    hc_vec3_t tmp;
    float lengthSq = 0.0f;

#   ifdef _OPENMP
#       pragma omp simd
#   endif
    for (int_fast8_t i = 0; i < 3; i++) {
        tmp[i] = v1[i] - v2[i];
        lengthSq += tmp[i]*tmp[i];
    }

    float inv_lenght = hc_rsqrtf(lengthSq);

#   ifdef _OPENMP
#       pragma omp simd
#   endif
    for (int_fast8_t i = 0; i < 3; i++) {
        dst[i] = tmp[i]*inv_lenght;
    }
}

HCSAPI void
hc_vec3_direction_r(hc_vec3_ptr_t HC_RESTRICT dst, const hc_vec3_t v1, const hc_vec3_t v2)
{
    float lengthSq = 0.0f;

#   ifdef _OPENMP
#       pragma omp simd
#   endif
    for (int_fast8_t i = 0; i < 3; i++) {
        dst[i] = v1[i] - v2[i];
        lengthSq += dst[i]*dst[i];
    }

    float inv_lenght = hc_rsqrtf(lengthSq);

#   ifdef _OPENMP
#       pragma omp simd
#   endif
    for (int_fast8_t i = 0; i < 3; i++) {
        dst[i] = dst[i]*inv_lenght;
    }
}

HCSAPI void
hc_vec3_lerp(hc_vec3_t dst, const hc_vec3_t v1, const hc_vec3_t v2, float t)
{
#   ifdef _OPENMP
#       pragma omp simd
#   endif
    for (int_fast8_t i = 0; i < 3; i++) {
        dst[i] = v1[i] + t*(v2[i]-v1[i]);
    }
}

HCSAPI void
hc_vec3_lerp_r(hc_vec3_ptr_t HC_RESTRICT dst, const hc_vec3_t v1, const hc_vec3_t v2, float t)
{
#   ifdef _OPENMP
#       pragma omp simd
#   endif
    for (int_fast8_t i = 0; i < 3; i++) {
        dst[i] = v1[i] + t*(v2[i]-v1[i]);
    }
}

HCSAPI void
hc_vec3_barysmooth(hc_vec3_t dst, const hc_vec3_t v1, const hc_vec3_t v2, const hc_vec3_t v3, float w1, float w2, float w3)
{
#   ifdef _OPENMP
#       pragma omp simd
#   endif
    for (int_fast8_t i = 0; i < 3; i++) {
        dst[i] = w1*v1[i] + w2*v2[i] + w3*v3[i];
    }
}

HCSAPI void
hc_vec3_barysmooth_r(hc_vec3_ptr_t HC_RESTRICT dst, const hc_vec3_t v1, const hc_vec3_t v2, const hc_vec3_t v3, float w1, float w2, float w3)
{
#   ifdef _OPENMP
#       pragma omp simd
#   endif
    for (int_fast8_t i = 0; i < 3; i++) {
        dst[i] = w1*v1[i] + w2*v2[i] + w3*v3[i];
    }
}

HCSAPI void
hc_vec3_barysmooth_v(hc_vec3_t dst, const hc_vec3_t v1, const hc_vec3_t v2, const hc_vec3_t v3, const hc_vec3_t w)
{
#   ifdef _OPENMP
#       pragma omp simd
#   endif
    for (int_fast8_t i = 0; i < 3; i++) {
        dst[i] = w[0] * v1[i] + w[1] * v2[i] + w[2] * v3[i];
    }
}

HCSAPI void
hc_vec3_barysmooth_vr(hc_vec3_ptr_t HC_RESTRICT dst, const hc_vec3_t v1, const hc_vec3_t v2, const hc_vec3_t v3, const hc_vec3_t w)
{
#   ifdef _OPENMP
#       pragma omp simd
#   endif
    for (int_fast8_t i = 0; i < 3; i++) {
        dst[i] = w[0] * v1[i] + w[1] * v2[i] + w[2] * v3[i];
    }
}

HCSAPI void
hc_vec3_baryflat(hc_vec4_t dst, const hc_vec4_t v1, const hc_vec4_t v2, const hc_vec4_t v3, float w1, float w2, float w3)
{
    // Find the maximum weight
    float max_w = w1;
    if (w2 > max_w) max_w = w2;
    if (w3 > max_w) max_w = w3;

    // Determine which weight is the maximum
    int_fast8_t isW1Max = (max_w == w1);
    int_fast8_t isW2Max = (max_w == w2);

    // Select the corresponding color based on the maximum weight
    for (int_fast8_t i = 0; i < 3; ++i) {
        if (isW1Max) {
            dst[i] = v1[i];
        } else if (isW2Max) {
            dst[i] = v2[i];
        } else {
            dst[i] = v3[i];
        }
    }
}

HCSAPI void
hc_vec3_baryflat_r(hc_vec4_ptr_t HC_RESTRICT dst, const hc_vec4_t v1, const hc_vec4_t v2, const hc_vec4_t v3, float w1, float w2, float w3)
{
    // Find the maximum weight
    float max_w = w1;
    if (w2 > max_w) max_w = w2;
    if (w3 > max_w) max_w = w3;

    // Determine which weight is the maximum
    int_fast8_t isW1Max = (max_w == w1);
    int_fast8_t isW2Max = (max_w == w2);

    // Select the corresponding color based on the maximum weight
    for (int_fast8_t i = 0; i < 3; ++i) {
        if (isW1Max) {
            dst[i] = v1[i];
        } else if (isW2Max) {
            dst[i] = v2[i];
        } else {
            dst[i] = v3[i];
        }
    }
}

HCSAPI void
hc_vec3_baryflat_v(hc_vec4_t dst, const hc_vec4_t v1, const hc_vec4_t v2, const hc_vec4_t v3, const hc_vec3_t w)
{
    // Find the maximum weight
    float max_w = w[0];
    if (w[1] > max_w) max_w = w[1];
    if (w[2] > max_w) max_w = w[2];

    // Determine which weight is the maximum
    int_fast8_t isW1Max = (max_w == w[0]);
    int_fast8_t isW2Max = (max_w == w[1]);

    // Select the corresponding color based on the maximum weight
    for (int_fast8_t i = 0; i < 3; ++i) {
        if (isW1Max) {
            dst[i] = v1[i];
        } else if (isW2Max) {
            dst[i] = v2[i];
        } else {
            dst[i] = v3[i];
        }
    }
}

HCSAPI void
hc_vec3_baryflat_vr(hc_vec4_ptr_t HC_RESTRICT dst, const hc_vec4_t v1, const hc_vec4_t v2, const hc_vec4_t v3, const hc_vec3_t w)
{
    // Find the maximum weight
    float max_w = w[0];
    if (w[1] > max_w) max_w = w[1];
    if (w[2] > max_w) max_w = w[2];

    // Determine which weight is the maximum
    int_fast8_t isW1Max = (max_w == w[0]);
    int_fast8_t isW2Max = (max_w == w[1]);

    // Select the corresponding color based on the maximum weight
    for (int_fast8_t i = 0; i < 3; ++i) {
        if (isW1Max) {
            dst[i] = v1[i];
        } else if (isW2Max) {
            dst[i] = v2[i];
        } else {
            dst[i] = v3[i];
        }
    }
}

HCSAPI void
hc_vec3_transform(hc_vec3_t dst, const hc_vec3_t v, const hc_mat4_t mat)
{
    hc_vec3_t tmp = {
        mat[0] * v[0] + mat[4] * v[1] + mat[8] * v[2] + mat[12],
        mat[1] * v[0] + mat[5] * v[1] + mat[9] * v[2] + mat[13],
        mat[2] * v[0] + mat[6] * v[1] + mat[10] * v[2] + mat[14]
    };

    for (int_fast8_t i = 0; i < 3; i++) {
        dst[i] = tmp[i];
    }
}

HCSAPI void
hc_vec3_transform_r(hc_vec3_ptr_t HC_RESTRICT dst, const hc_vec3_t v, const hc_mat4_t mat)
{
    dst[0] = mat[0] * v[0] + mat[4] * v[1] + mat[8] * v[2] + mat[12];
    dst[1] = mat[1] * v[0] + mat[5] * v[1] + mat[9] * v[2] + mat[13];
    dst[2] = mat[2] * v[0] + mat[6] * v[1] + mat[10] * v[2] + mat[14];
}

HCSAPI void
hc_vec3_transform_wt(hc_vec3_t dst, const hc_vec3_t v, float w_translation, const hc_mat4_t mat)
{
    hc_vec3_t tmp = {
        mat[0] * v[0] + mat[4] * v[1] + mat[8] * v[2] + w_translation*mat[12],
        mat[1] * v[0] + mat[5] * v[1] + mat[9] * v[2] + w_translation*mat[13],
        mat[2] * v[0] + mat[6] * v[1] + mat[10] * v[2] + w_translation*mat[14]
    };

    for (int_fast8_t i = 0; i < 3; i++) {
        dst[i] = tmp[i];
    }
}

HCSAPI void
hc_vec3_transform_wtr(hc_vec3_ptr_t HC_RESTRICT dst, const hc_vec3_t v, float w_translation, const hc_mat4_t mat)
{
    dst[0] = mat[0] * v[0] + mat[4] * v[1] + mat[8] * v[2] + w_translation*mat[12];
    dst[1] = mat[1] * v[0] + mat[5] * v[1] + mat[9] * v[2] + w_translation*mat[13];
    dst[2] = mat[2] * v[0] + mat[6] * v[1] + mat[10] * v[2] + w_translation*mat[14];
}

HCSAPI void
hc_vec3_reflect(hc_vec3_t dst, const hc_vec3_t incident, const hc_vec3_t normal)
{
    float dotProduct = 0.0f;

#   ifdef _OPENMP
#       pragma omp simd
#   endif
    for (int_fast8_t i = 0; i < 3; i++) {
        dotProduct += incident[i]*normal[i];
    }

    dotProduct *= 2.0f;

#   ifdef _OPENMP
#       pragma omp simd
#   endif
    for (int_fast8_t i = 0; i < 3; i++) {
        dst[i] = incident[i] - dotProduct*normal[i];
    }
}

HCSAPI void
hc_vec3_reflect_r(hc_vec3_ptr_t HC_RESTRICT dst, const hc_vec3_t incident, const hc_vec3_t normal)
{
    float dotProduct = 0.0f;

#   ifdef _OPENMP
#       pragma omp simd
#   endif
    for (int_fast8_t i = 0; i < 3; i++) {
        dotProduct += incident[i]*normal[i];
    }

    dotProduct *= 2.0f;

#   ifdef _OPENMP
#       pragma omp simd
#   endif
    for (int_fast8_t i = 0; i < 3; i++) {
        dst[i] = incident[i] - dotProduct*normal[i];
    }
}

/* 4D Vector function definitions */

HCSAPI void
hc_vec4_zero(hc_vec4_t dst)
{
    for (int_fast8_t i = 0; i < 4; i++) {
        dst[i] = 0;
    }
}

HCSAPI void
hc_vec4_one(hc_vec4_t dst, float v)
{
    dst[0] = dst[1] = dst[2] = dst[3] = v;
}

HCSAPI void
hc_vec4_set(hc_vec4_t dst, float x, float y, float z, float w)
{
    dst[0] = x, dst[1] = y, dst[2] = z, dst[3] = w;
}

HCSAPI void
hc_vec4_copy(hc_vec4_ptr_t HC_RESTRICT dst, const hc_vec4_ptr_t HC_RESTRICT src)
{
    for (int_fast8_t i = 0; i < 4; i++) {
        dst[i] = src[i];
    }
}

HCSAPI void
hc_vec4_swap(hc_vec4_ptr_t HC_RESTRICT a, hc_vec4_ptr_t HC_RESTRICT b)
{
#   ifdef _OPENMP
#       pragma omp simd
#   endif
    for (int_fast8_t i = 0; i < 4; i++) {
        float tmp = a[i];
        a[i] = b[i];
        b[i] = tmp;
    }
}

HCSAPI void
hc_vec4_neg(hc_vec4_t dst, const hc_vec4_t v)
{
#   ifdef _OPENMP
#       pragma omp simd
#   endif
    for (int_fast8_t i = 0; i < 4; i++) {
        dst[i] = -v[i];
    }
}

HCSAPI void
hc_vec4_neg_r(hc_vec4_ptr_t HC_RESTRICT dst, const hc_vec4_t v)
{
#   ifdef _OPENMP
#       pragma omp simd
#   endif
    for (int_fast8_t i = 0; i < 4; i++) {
        dst[i] = -v[i];
    }
}

HCSAPI void
hc_vec4_add(hc_vec4_t dst, const hc_vec4_t v1, const hc_vec4_t v2)
{
#   ifdef _OPENMP
#       pragma omp simd
#   endif
    for (int_fast8_t i = 0; i < 4; i++) {
        dst[i] = v1[i] + v2[i];
    }
}

HCSAPI void
hc_vec4_add_r(hc_vec4_ptr_t HC_RESTRICT dst, const hc_vec4_t v1, const hc_vec4_t v2)
{
#   ifdef _OPENMP
#       pragma omp simd
#   endif
    for (int_fast8_t i = 0; i < 4; i++) {
        dst[i] = v1[i] + v2[i];
    }
}

HCSAPI void
hc_vec4_sub(hc_vec4_t dst, const hc_vec4_t v1, const hc_vec4_t v2)
{
#   ifdef _OPENMP
#       pragma omp simd
#   endif
    for (int_fast8_t i = 0; i < 4; i++) {
        dst[i] = v1[i] - v2[i];
    }
}

HCSAPI void
hc_vec4_sub_r(hc_vec4_ptr_t HC_RESTRICT dst, const hc_vec4_t v1, const hc_vec4_t v2)
{
#   ifdef _OPENMP
#       pragma omp simd
#   endif
    for (int_fast8_t i = 0; i < 4; i++) {
        dst[i] = v1[i] - v2[i];
    }
}

HCSAPI void
hc_vec4_mul(hc_vec4_t dst, const hc_vec4_t v1, const hc_vec4_t v2)
{
#   ifdef _OPENMP
#       pragma omp simd
#   endif
    for (int_fast8_t i = 0; i < 4; i++) {
        dst[i] = v1[i] * v2[i];
    }
}

HCSAPI void
hc_vec4_mul_r(hc_vec4_ptr_t HC_RESTRICT dst, const hc_vec4_t v1, const hc_vec4_t v2)
{
#   ifdef _OPENMP
#       pragma omp simd
#   endif
    for (int_fast8_t i = 0; i < 4; i++) {
        dst[i] = v1[i] * v2[i];
    }
}

HCSAPI void
hc_vec4_div(hc_vec4_t dst, const hc_vec4_t v1, const hc_vec4_t v2)
{
#   ifdef _OPENMP
#       pragma omp simd
#   endif
    for (int_fast8_t i = 0; i < 4; i++) {
        dst[i] = v1[i]/v2[i];
    }
}

HCSAPI void
hc_vec4_div_r(hc_vec4_ptr_t HC_RESTRICT dst, const hc_vec4_t v1, const hc_vec4_t v2)
{
#   ifdef _OPENMP
#       pragma omp simd
#   endif
    for (int_fast8_t i = 0; i < 4; i++) {
        dst[i] = v1[i]/v2[i];
    }
}

HCSAPI void
hc_vec4_offset(hc_vec4_t dst, const hc_vec4_t v, float scalar)
{
#   ifdef _OPENMP
#       pragma omp simd
#   endif
    for (int_fast8_t i = 0; i < 4; i++) {
        dst[i] = v[i] + scalar;
    }
}

HCSAPI void
hc_vec4_offset_r(hc_vec4_ptr_t HC_RESTRICT dst, const hc_vec4_t v, float scalar)
{
#   ifdef _OPENMP
#       pragma omp simd
#   endif
    for (int_fast8_t i = 0; i < 4; i++) {
        dst[i] = v[i] + scalar;
    }
}

HCSAPI void
hc_vec4_scale(hc_vec4_t dst, const hc_vec4_t v, float scalar)
{
#   ifdef _OPENMP
#       pragma omp simd
#   endif
    for (int_fast8_t i = 0; i < 4; i++) {
        dst[i] = v[i]*scalar;
    }
}

HCSAPI void
hc_vec4_scale_r(hc_vec4_ptr_t HC_RESTRICT dst, const hc_vec4_t v, float scalar)
{
#   ifdef _OPENMP
#       pragma omp simd
#   endif
    for (int_fast8_t i = 0; i < 4; i++) {
        dst[i] = v[i]*scalar;
    }
}

HCSAPI void
hc_vec4_normalize(hc_vec4_t dst, const hc_vec4_t v)
{
    float length_sq = v[0] * v[0] + v[1] * v[1] + v[2] * v[2] + v[3] * v[3];
    if (length_sq == 0.0f) return;

    float inv_lenght = hc_rsqrtf(length_sq);

#   ifdef _OPENMP
#       pragma omp simd
#   endif
    for (int_fast8_t i = 0; i < 4; i++) {
        dst[i] = v[i] * inv_lenght;
    }
}

HCSAPI void
hc_vec4_normalize_r(hc_vec4_ptr_t HC_RESTRICT dst, const hc_vec4_t v)
{
    float length_sq = v[0] * v[0] + v[1] * v[1] + v[2] * v[2] + v[3] * v[3];
    if (length_sq == 0.0f) return;

    float inv_lenght = hc_rsqrtf(length_sq);

#   ifdef _OPENMP
#       pragma omp simd
#   endif
    for (int_fast8_t i = 0; i < 4; i++) {
        dst[i] = v[i] * inv_lenght;
    }
}

HCSAPI float
hc_vec4_length(const hc_vec4_t v)
{
    return sqrtf(v[0] * v[0] + v[1] * v[1] + v[2] * v[2] + v[3] * v[3]);
}

HCSAPI float
hc_length_sq_vec4(const hc_vec4_t v)
{
    return v[0] * v[0] + v[1] * v[1] + v[2] * v[2] + v[3] * v[3];
}

HCSAPI float
hc_vec4_dot(const hc_vec4_t v1, const hc_vec4_t v2)
{
    return v1[0] * v2[0] + v1[1] * v2[1] + v1[2] * v2[2] + v1[3] * v2[3];
}

HCSAPI void
hc_vec4_lerp(hc_vec4_t dst, const hc_vec4_t v1, const hc_vec4_t v2, float t)
{
#   ifdef _OPENMP
#       pragma omp simd
#   endif
    for (int_fast8_t i = 0; i < 4; i++) {
        dst[i] = v1[i] + t*(v2[i]-v1[i]);
    }
}

HCSAPI void
hc_vec4_lerp_r(hc_vec4_ptr_t HC_RESTRICT dst, const hc_vec4_t v1, const hc_vec4_t v2, float t)
{
#   ifdef _OPENMP
#       pragma omp simd
#   endif
    for (int_fast8_t i = 0; i < 4; i++) {
        dst[i] = v1[i] + t*(v2[i]-v1[i]);
    }
}

HCSAPI void
hc_vec4_barysmooth(hc_vec4_t dst, const hc_vec4_t v1, const hc_vec4_t v2, const hc_vec4_t v3, float w1, float w2, float w3)
{
#   ifdef _OPENMP
#       pragma omp simd
#   endif
    for (int_fast8_t i = 0; i < 4; i++) {
        dst[i] = w1*v1[i] + w2*v2[i] + w3*v3[i];
    }
}

HCSAPI void
hc_vec4_barysmooth_r(hc_vec4_ptr_t HC_RESTRICT dst, const hc_vec4_t v1, const hc_vec4_t v2, const hc_vec4_t v3, float w1, float w2, float w3)
{
#   ifdef _OPENMP
#       pragma omp simd
#   endif
    for (int_fast8_t i = 0; i < 4; i++) {
        dst[i] = w1*v1[i] + w2*v2[i] + w3*v3[i];
    }
}

HCSAPI void
hc_vec4_barysmooth_v(hc_vec4_t dst, const hc_vec4_t v1, const hc_vec4_t v2, const hc_vec4_t v3, const hc_vec3_t w)
{
#   ifdef _OPENMP
#       pragma omp simd
#   endif
    for (int_fast8_t i = 0; i < 4; i++) {
        dst[i] = w[0] * v1[i] + w[1] * v2[i] + w[2] * v3[i];
    }
}

HCSAPI void
hc_vec4_barysmooth_vr(hc_vec4_ptr_t HC_RESTRICT dst, const hc_vec4_t v1, const hc_vec4_t v2, const hc_vec4_t v3, const hc_vec3_t w)
{
#   ifdef _OPENMP
#       pragma omp simd
#   endif
    for (int_fast8_t i = 0; i < 4; i++) {
        dst[i] = w[0] * v1[i] + w[1] * v2[i] + w[2] * v3[i];
    }
}

HCSAPI void
hc_vec4_baryflat(hc_vec4_t dst, const hc_vec4_t v1, const hc_vec4_t v2, const hc_vec4_t v3, float w1, float w2, float w3)
{
    // Find the maximum weight
    float max_w = w1;
    if (w2 > max_w) max_w = w2;
    if (w3 > max_w) max_w = w3;

    // Determine which weight is the maximum
    int_fast8_t isW1Max = (max_w == w1);
    int_fast8_t isW2Max = (max_w == w2);

    // Select the corresponding color based on the maximum weight
    for (int_fast8_t i = 0; i < 4; ++i) {
        if (isW1Max) {
            dst[i] = v1[i];
        } else if (isW2Max) {
            dst[i] = v2[i];
        } else {
            dst[i] = v3[i];
        }
    }
}

HCSAPI void
hc_vec4_baryflat_r(hc_vec4_ptr_t HC_RESTRICT dst, const hc_vec4_t v1, const hc_vec4_t v2, const hc_vec4_t v3, float w1, float w2, float w3)
{
    // Find the maximum weight
    float max_w = w1;
    if (w2 > max_w) max_w = w2;
    if (w3 > max_w) max_w = w3;

    // Determine which weight is the maximum
    int_fast8_t isW1Max = (max_w == w1);
    int_fast8_t isW2Max = (max_w == w2);

    // Select the corresponding color based on the maximum weight
    for (int_fast8_t i = 0; i < 4; ++i) {
        if (isW1Max) {
            dst[i] = v1[i];
        } else if (isW2Max) {
            dst[i] = v2[i];
        } else {
            dst[i] = v3[i];
        }
    }
}

HCSAPI void
hc_vec4_baryflat_v(hc_vec4_t dst, const hc_vec4_t v1, const hc_vec4_t v2, const hc_vec4_t v3, const hc_vec3_t w)
{
    // Find the maximum weight
    float max_w = w[0];
    if (w[1] > max_w) max_w = w[1];
    if (w[2] > max_w) max_w = w[2];

    // Determine which weight is the maximum
    int_fast8_t isW1Max = (max_w == w[0]);
    int_fast8_t isW2Max = (max_w == w[1]);

    // Select the corresponding color based on the maximum weight
    for (int_fast8_t i = 0; i < 4; ++i) {
        if (isW1Max) {
            dst[i] = v1[i];
        } else if (isW2Max) {
            dst[i] = v2[i];
        } else {
            dst[i] = v3[i];
        }
    }
}

HCSAPI void
hc_vec4_baryflat_vr(hc_vec4_ptr_t HC_RESTRICT dst, const hc_vec4_t v1, const hc_vec4_t v2, const hc_vec4_t v3, const hc_vec3_t w)
{
    // Find the maximum weight
    float max_w = w[0];
    if (w[1] > max_w) max_w = w[1];
    if (w[2] > max_w) max_w = w[2];

    // Determine which weight is the maximum
    int_fast8_t isW1Max = (max_w == w[0]);
    int_fast8_t isW2Max = (max_w == w[1]);

    // Select the corresponding color based on the maximum weight
    for (int_fast8_t i = 0; i < 4; ++i) {
        if (isW1Max) {
            dst[i] = v1[i];
        } else if (isW2Max) {
            dst[i] = v2[i];
        } else {
            dst[i] = v3[i];
        }
    }
}

HCSAPI void
hc_vec4_transform(hc_vec4_t dst, const hc_vec4_t v, const hc_mat4_t mat)
{
    hc_vec4_t tmp = {
        mat[0] * v[0] + mat[4] * v[1] + mat[8] * v[2] + mat[12] * v[3],
        mat[1] * v[0] + mat[5] * v[1] + mat[9] * v[2] + mat[13] * v[3],
        mat[2] * v[0] + mat[6] * v[1] + mat[10] * v[2] + mat[14] * v[3],
        mat[3] * v[0] + mat[7] * v[1] + mat[11] * v[2] + mat[15] * v[3]
    };

    for (int_fast8_t i = 0; i < 4; i++) {
        dst[i] = tmp[i];
    }
}

HCSAPI void
hc_vec4_transform_r(hc_vec4_ptr_t HC_RESTRICT dst, const hc_vec4_t v, const hc_mat4_t mat)
{
    dst[0] = mat[0] * v[0] + mat[4] * v[1] + mat[8] * v[2] + mat[12] * v[3];
    dst[1] = mat[1] * v[0] + mat[5] * v[1] + mat[9] * v[2] + mat[13] * v[3];
    dst[2] = mat[2] * v[0] + mat[6] * v[1] + mat[10] * v[2] + mat[14] * v[3];
    dst[3] = mat[3] * v[0] + mat[7] * v[1] + mat[11] * v[2] + mat[15] * v[3];
}

/* Matrix 4x4 function definitions */

HCSAPI void
hc_mat4_identity(hc_mat4_t dst)
{
    for (int_fast8_t i = 0; i < 16; i++) {
        dst[i] = (float)(!(i % 5));
    }
}

HCSAPI void
hc_mat4_copy(hc_mat4_ptr_t HC_RESTRICT dst, const hc_mat4_ptr_t HC_RESTRICT src)
{
    for (int_fast8_t i = 0; i < 16; i++) {
        dst[i] = src[i];
    }
}

HCSAPI void
hc_mat4_translate(hc_mat4_t dst, float x, float y, float z)
{
    for (int_fast8_t i = 0; i < 16; i++) {
        dst[i] = (float)(!(i % 5));
    }
    dst[12] = x;
    dst[13] = y;
    dst[14] = z;
}

HCSAPI void
hc_mat4_rotate(hc_mat4_t dst, const hc_vec3_t axis, float radians)
{
    // hc_mat4_identity()
    for (int_fast8_t i = 0; i < 16; i++) {
        dst[i] = (float)(!(i % 5));
    }

    float x = axis[0], y = axis[1], z = axis[2];
    float lengthSq = x*x + y*y + z*z;

    if (lengthSq != 1.0f && lengthSq != 0.0f) {
        float invLenght = hc_rsqrtf(lengthSq);
        x *= invLenght;
        y *= invLenght;
        z *= invLenght;
    }

    float sinres = sinf(radians);
    float cosres = cosf(radians);
    float t = 1.0f - cosres;

    dst[0]  = x*x*t + cosres;
    dst[1]  = y*x*t + z*sinres;
    dst[2]  = z*x*t - y*sinres;

    dst[4]  = x*y*t - z*sinres;
    dst[5]  = y*y*t + cosres;
    dst[6]  = z*y*t + x*sinres;

    dst[8]  = x*z*t + y*sinres;
    dst[9]  = y*z*t - x*sinres;
    dst[10] = z*z*t + cosres;
}

HCSAPI void
hc_rotate_x_mat4(hc_mat4_t dst, float radians)
{
    // hc_mat4_identity()
    for (int_fast8_t i = 0; i < 16; i++) {
        dst[i] = (float)(!(i % 5));
    }

    float cosres = cosf(radians);
    float sinres = sinf(radians);

    dst[5]  = cosres;
    dst[6]  = sinres;
    dst[9]  = -sinres;
    dst[10] = cosres;
}


HCSAPI void
hc_rotate_y_mat4(hc_mat4_t dst, float radians)
{
    // hc_mat4_identity()
    for (int_fast8_t i = 0; i < 16; i++) {
        dst[i] = (float)(!(i % 5));
    }

    float cosres = cosf(radians);
    float sinres = sinf(radians);

    dst[0]  = cosres;
    dst[2]  = -sinres;
    dst[8]  = sinres;
    dst[10] = cosres;
}

HCSAPI void
hc_rotate_z_mat4(hc_mat4_t dst, float radians)
{
    // hc_mat4_identity()
    for (int_fast8_t i = 0; i < 16; i++) {
        dst[i] = (float)(!(i % 5));
    }

    float cosres = cosf(radians);
    float sinres = sinf(radians);

    dst[0] = cosres;
    dst[1] = sinres;
    dst[4] = -sinres;
    dst[5] = cosres;
}


HCSAPI void
hc_rotate_xyz_mat4(hc_mat4_t dst, const hc_vec3_t radians)
{
    // hc_mat4_identity()
    for (int_fast8_t i = 0; i < 16; i++) {
        dst[i] = (float)(!(i % 5));
    }

    float cosz = cosf(-radians[2]);
    float sinz = sinf(-radians[2]);
    float cosy = cosf(-radians[1]);
    float siny = sinf(-radians[1]);
    float cosx = cosf(-radians[0]);
    float sinx = sinf(-radians[0]);

    dst[0]  = cosz*cosy;
    dst[1]  = (cosz*siny*sinx) - (sinz*cosx);
    dst[2]  = (cosz*siny*cosx) + (sinz*sinx);

    dst[4]  = sinz*cosy;
    dst[5]  = (sinz*siny*sinx) + (cosz*cosx);
    dst[6]  = (sinz*siny*cosx) - (cosz*sinx);

    dst[8]  = -siny;
    dst[9]  = cosy*sinx;
    dst[10] = cosy*cosx;
}


HCSAPI void
h_rotate_zyx_mat4(hc_mat4_t dst, const hc_vec3_t radians)
{
    // hc_mat4_identity()
    for (int_fast8_t i = 0; i < 16; i++) {
        dst[i] = (float)(!(i % 5));
    }

    float cz = cosf(radians[2]);
    float sz = sinf(radians[2]);
    float cy = cosf(radians[1]);
    float sy = sinf(radians[1]);
    float cx = cosf(radians[0]);
    float sx = sinf(radians[0]);

    dst[0] = cz*cy;
    dst[4] = cz*sy*sx - cx*sz;
    dst[8] = sz*sx + cz*cx*sy;

    dst[1] = cy*sz;
    dst[5] = cz*cx + sz*sy*sx;
    dst[9] = cx*sz*sy - cz*sx;

    dst[2] = -sy;
    dst[6] = cy*sx;
    dst[10] = cy*cx;
}

HCSAPI void
hc_mat4_scale(hc_mat4_t dst, float x, float y, float z)
{
    for (int_fast8_t i = 0; i < 16; i++) dst[i] = 0;
    dst[0] = x, dst[5] = y, dst[10] = z, dst[15] = 1.0;
}

HCSAPI void
hc_mat4_frustum(hc_mat4_t dst, float left, float right, float bottom, float top, float znear, float zfar)
{
    for (int_fast8_t i = 0; i < 16; i++) {
        dst[i] = 0;
    }

    float rl = right - left;
    float tb = top - bottom;
    float fn = zfar - znear;

    dst[0] = (znear * 2.0f) / rl;
    dst[5] = (znear * 2.0f) / tb;

    dst[8] = (right + left) / rl;
    dst[9] = (top + bottom) / tb;
    dst[10] = -(zfar + znear) / fn;
    dst[11] = -1.0f;

    dst[14] = -(zfar * znear * 2.0f) / fn;
}

HCSAPI void
hc_mat4_perspective(hc_mat4_t dst, float fovy_rad, float aspect, float znear, float zfar)
{
    for (int_fast8_t i = 0; i < 16; i++) {
        dst[i] = 0;
    }

    float top = znear * tanf(fovy_rad * 0.5f);
    float bottom = -top;
    float right = top * aspect;
    float left = -right;

    // hc_mat4_frustum(-right, right, -top, top, near, far);
    float rl = right - left;
    float tb = top - bottom;
    float fn = zfar - znear;

    dst[0] = (znear * 2.0f) / rl;
    dst[5] = (znear * 2.0f) / tb;

    dst[8] = (right + left) / rl;
    dst[9] = (top + bottom) / tb;
    dst[10] = -(zfar + znear) / fn;
    dst[11] = -1.0f;

    dst[14] = -(zfar * znear * 2.0f) / fn;
}

HCSAPI void
hc_mat4_ortho(hc_mat4_t dst, float left, float right, float bottom, float top, float znear, float zfar)
{
    for (int_fast8_t i = 0; i < 16; i++) {
        dst[i] = 0;
    }

    float rl = (right - left);
    float tb = (top - bottom);
    float fn = (zfar - znear);

    dst[0] = 2.0f / rl;
    dst[5] = 2.0f / tb;

    dst[10] = -2.0f / fn;
    dst[11] = 0.0f;
    dst[12] = -(left + right) / rl;
    dst[13] = -(top + bottom) / tb;

    dst[14] = -(zfar + znear) / fn;
    dst[15] = 1.0f;
}

HCSAPI void
hc_look_at_mat4(hc_mat4_t dst, const hc_vec3_t eye, const hc_vec3_t target, const hc_vec3_t up)
{
    for (int_fast8_t i = 0; i < 16; i++) {
        dst[i] = 0;
    }

    float length = 0.0f;
    float invLenght = 0.0f;

    // hc_sub_ve3(eye, target)
    hc_vec3_t vz = {
        eye[0] - target[0],
        eye[1] - target[1],
        eye[2] - target[2]
    };

    // hc_vec3_normalize(vz)
    hc_vec3_t v = { vz[0], vz[1], vz[2] };
    length = sqrtf(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
    if (length == 0.0f) length = 1.0f;
    invLenght = 1.0f/length;
    vz[0] *= invLenght;
    vz[1] *= invLenght;
    vz[2] *= invLenght;

    // hc_vec3_cross(up, vz)
    hc_vec3_t vx = {
        up[1] * vz[2] - up[2] * vz[1],
        up[2] * vz[0] - up[0] * vz[2],
        up[0] * vz[1] - up[1] * vz[0]
    };

    // hc_vec3_normalize(x)
    for (int_fast8_t i = 0; i < 3; i++) v[i] = vx[i];
    length = sqrtf(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
    if (length == 0.0f) length = 1.0f;
    invLenght = 1.0f/length;
    vx[0] *= invLenght;
    vx[1] *= invLenght;
    vx[2] *= invLenght;

    // hc_vec3_cross(vz, vx)
    hc_vec3_t vy = {
        vz[1] * vx[2] - vz[2] * vx[1],
        vz[2] * vx[0] - vz[0] * vx[2],
        vz[0] * vx[1] - vz[1] * vx[0]
    };

    dst[0] = vx[0];
    dst[1] = vy[0];
    dst[2] = vz[0];

    dst[4] = vx[1];
    dst[5] = vy[1];
    dst[6] = vz[1];

    dst[8] = vx[2];
    dst[9] = vy[2];
    dst[10] = vz[2];

    dst[12] = -(vx[0]*eye[0] + vx[1]*eye[1] + vx[2]*eye[2]);   // hc_vec3_dot(vx, eye)
    dst[13] = -(vy[0]*eye[0] + vy[1]*eye[1] + vy[2]*eye[2]);   // hc_vec3_dot(vy, eye)
    dst[14] = -(vz[0]*eye[0] + vz[1]*eye[1] + vz[2]*eye[2]);   // hc_vec3_dot(vz, eye)
    dst[15] = 1.0f;
}

HCSAPI float
hc_mat4_determinant(const hc_mat4_t mat)
{
    float result = 0.0f;

    // Cache the matrix values (speed optimization)
    float a00 = mat[0],  a01 = mat[1],  a02 = mat[2],  a03 = mat[3];
    float a10 = mat[4],  a11 = mat[5],  a12 = mat[6],  a13 = mat[7];
    float a20 = mat[8],  a21 = mat[9],  a22 = mat[10], a23 = mat[11];
    float a30 = mat[12], a31 = mat[13], a32 = mat[14], a33 = mat[15];

    result = a30*a21*a12*a03 - a20*a31*a12*a03 - a30*a11*a22*a03 + a10*a31*a22*a03 +
             a20*a11*a32*a03 - a10*a21*a32*a03 - a30*a21*a02*a13 + a20*a31*a02*a13 +
             a30*a01*a22*a13 - a00*a31*a22*a13 - a20*a01*a32*a13 + a00*a21*a32*a13 +
             a30*a11*a02*a23 - a10*a31*a02*a23 - a30*a01*a12*a23 + a00*a31*a12*a23 +
             a10*a01*a32*a23 - a00*a11*a32*a23 - a20*a11*a02*a33 + a10*a21*a02*a33 +
             a20*a01*a12*a33 - a00*a21*a12*a33 - a10*a01*a22*a33 + a00*a11*a22*a33;

    return result;
}

HCSAPI void
hc_mat4_transpose(hc_mat4_t dst, const hc_mat4_t src)
{
    hc_mat4_t result;
    for (int_fast8_t i = 0; i < 4; i++) {
        for (int_fast8_t j = 0; j < 4; j++) {
            result[i * 4 + j] = src[j * 4 + i];
        }
    }
    for (int_fast8_t i = 0; i < 16; i++) {
        dst[i] = result[i];
    }
}

HCSAPI void
hc_mat4_transpose_r(hc_mat4_ptr_t HC_RESTRICT dst, const hc_mat4_t src)
{
    for (int_fast8_t i = 0; i < 4; i++) {
        for (int_fast8_t j = 0; j < 4; j++) {
            dst[i * 4 + j] = src[j * 4 + i];
        }
    }
}

HCSAPI void
hc_mat4_invert(hc_mat4_t dst, const hc_mat4_t src)
{
    // Cache the matrix values (speed optimization)
    float a00 = src[0],  a01 = src[1],  a02 = src[2],  a03 = src[3];
    float a10 = src[4],  a11 = src[5],  a12 = src[6],  a13 = src[7];
    float a20 = src[8],  a21 = src[9],  a22 = src[10], a23 = src[11];
    float a30 = src[12], a31 = src[13], a32 = src[14], a33 = src[15];

    float b00 = a00*a11 - a01*a10;
    float b01 = a00*a12 - a02*a10;
    float b02 = a00*a13 - a03*a10;
    float b03 = a01*a12 - a02*a11;
    float b04 = a01*a13 - a03*a11;
    float b05 = a02*a13 - a03*a12;
    float b06 = a20*a31 - a21*a30;
    float b07 = a20*a32 - a22*a30;
    float b08 = a20*a33 - a23*a30;
    float b09 = a21*a32 - a22*a31;
    float b10 = a21*a33 - a23*a31;
    float b11 = a22*a33 - a23*a32;

    // Calculate the invert determinant (inlined to avoid double-caching)
    float invDet = 1.0f/(b00*b11 - b01*b10 + b02*b09 + b03*b08 - b04*b07 + b05*b06);

    dst[0] = (a11*b11 - a12*b10 + a13*b09)*invDet;
    dst[1] = (-a01*b11 + a02*b10 - a03*b09)*invDet;
    dst[2] = (a31*b05 - a32*b04 + a33*b03)*invDet;
    dst[3] = (-a21*b05 + a22*b04 - a23*b03)*invDet;
    dst[4] = (-a10*b11 + a12*b08 - a13*b07)*invDet;
    dst[5] = (a00*b11 - a02*b08 + a03*b07)*invDet;
    dst[6] = (-a30*b05 + a32*b02 - a33*b01)*invDet;
    dst[7] = (a20*b05 - a22*b02 + a23*b01)*invDet;
    dst[8] = (a10*b10 - a11*b08 + a13*b06)*invDet;
    dst[9] = (-a00*b10 + a01*b08 - a03*b06)*invDet;
    dst[10] = (a30*b04 - a31*b02 + a33*b00)*invDet;
    dst[11] = (-a20*b04 + a21*b02 - a23*b00)*invDet;
    dst[12] = (-a10*b09 + a11*b07 - a12*b06)*invDet;
    dst[13] = (a00*b09 - a01*b07 + a02*b06)*invDet;
    dst[14] = (-a30*b03 + a31*b01 - a32*b00)*invDet;
    dst[15] = (a20*b03 - a21*b01 + a22*b00)*invDet;
}

HCSAPI float
hc_mat4_trace(const hc_mat4_t mat)
{
    return mat[0] + mat[5] + mat[10] + mat[15];
}

HCSAPI void
hc_mat4_add(hc_mat4_t dst, const hc_mat4_t left, const hc_mat4_t right)
{
#   ifdef _OPENMP
#       pragma omp simd
#   endif
    for (int_fast8_t i = 0; i < 16; i++) {
        dst[i] = left[i] + right[i];
    }
}

HCSAPI void
hc_mat4_add_r(hc_mat4_ptr_t HC_RESTRICT dst, const hc_mat4_t left, const hc_mat4_t right)
{
#   ifdef _OPENMP
#       pragma omp simd
#   endif
    for (int_fast8_t i = 0; i < 16; i++) {
        dst[i] = left[i] + right[i];
    }
}

HCSAPI void
hc_mat4_sub(hc_mat4_t dst, const hc_mat4_t left, const hc_mat4_t right)
{
#   ifdef _OPENMP
#       pragma omp simd
#   endif
    for (int_fast8_t i = 0; i < 16; i++) {
        dst[i] = left[i] - right[i];
    }
}

HCSAPI void
hc_mat4_sub_r(hc_mat4_ptr_t HC_RESTRICT dst, const hc_mat4_t left, const hc_mat4_t right)
{
#   ifdef _OPENMP
#       pragma omp simd
#   endif
    for (int_fast8_t i = 0; i < 16; i++) {
        dst[i] = left[i] - right[i];
    }
}

HCSAPI void
hc_mat4_mul(hc_mat4_t dst, const hc_mat4_t left, const hc_mat4_t right)
{
    hc_mat4_t result;
#   ifdef _OPENMP
#       pragma omp simd
#   endif
    for (int_fast8_t i = 0; i < 4; i++) {
        for (int_fast8_t j = 0; j < 4; j++) {
            float sum = 0.0;
            for (int_fast8_t k = 0; k < 4; k++) {
                sum += left[i * 4 + k] * right[k * 4 + j];
            }
            result[i * 4 + j] = sum;
        }
    }
    for (int_fast8_t i = 0; i < 16; i++) {
        dst[i] = result[i];
    }
}

HCSAPI void
hc_mat4_mul_r(hc_mat4_ptr_t HC_RESTRICT dst, const hc_mat4_t left, const hc_mat4_t right)
{
#   ifdef _OPENMP
#       pragma omp simd
#   endif
    for (int_fast8_t i = 0; i < 4; i++) {
        for (int_fast8_t j = 0; j < 4; j++) {
            float sum = 0.0;
            for (int_fast8_t k = 0; k < 4; k++) {
                sum += left[i * 4 + k] * right[k * 4 + j];
            }
            dst[i * 4 + j] = sum;
        }
    }
}

#endif //HC_MATH_H
