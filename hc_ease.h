#ifndef HC_EASE_H
#define HC_EASE_H

#include <math.h>

#ifndef HCSAPI
#   define HCSAPI static inline
#endif // HCSAPI

#ifndef HC_PI
#   define HC_PI 3.14159265358979323846
#endif // HC_PI

HCSAPI float
hc_ease_sine_in(float t)
{
    return sinf(HC_PI / 2 * t);
}

HCSAPI float
hc_ease_sine_out(float t)
{
    return 1 + sinf(HC_PI / 2 * (--t));
}

HCSAPI float
hc_ease_sine_in_out(float t)
{
    return 0.5 * (1 + sinf(HC_PI * (t - 0.5)));
}

HCSAPI float
hc_ease_quad_in(float t)
{
    return t * t;
}

HCSAPI float
hc_ease_quad_out(float t)
{
    return t * (2 - t);
}

HCSAPI float
hc_ease_quad_in_out(float t)
{
    return t < 0.5 ? 2 * t * t : t * (4 - 2 * t) - 1;
}

HCSAPI float
hc_ease_cubic_in(float t)
{
    return t * t * t;
}

HCSAPI float
hc_ease_cubic_out(float t)
{
    --t; return 1 + t * t * t;
}

HCSAPI float
hc_ease_cubic_in_out(float t)
{
    return t < 0.5 ? 4 * t * t * t : (t - 1) * (2 * t - 2) * (2 * t - 2) + 1;
}

HCSAPI float
hc_ease_quart_in(float t)
{
    t *= t;
    return t * t;
}

HCSAPI float
hc_ease_quart_out(float t)
{
    --t; t = t * t;
    return 1 - t * t;
}

HCSAPI float
hc_ease_quart_in_out(float t)
{
    if (t < 0.5) {
        t *= t;
        return 8 * t * t;
    } else {
        --t; t = t * t;
        return 1 - 8 * t * t;
    }
}

HCSAPI float
hc_ease_quint_in(float t)
{
    float t2 = t * t;
    return t * t2 * t2;
}

HCSAPI float
hc_ease_quint_out(float t)
{
    --t; float t2 = t * t;
    return 1 + t * t2 * t2;
}

HCSAPI float
hc_ease_quint_in_out(float t)
{
    float t2;
    if (t < 0.5) {
        t2 = t * t;
        return 16 * t * t2 * t2;
    }
    --t; t2 = t * t;
    return 1 + 16 * t * t2 * t2;
}

HCSAPI float
hc_ease_expo_in(float t)
{
    return (powf(2, 8 * t) - 1) / 255;
}

HCSAPI float
hc_ease_expo_out(float t)
{
    return 1 - powf(2, -8 * t);
}

HCSAPI float
hc_ease_expo_in_out(float t)
{
    if (t < 0.5) {
        return (powf(2, 16 * t) - 1) / 510;
    }
    return 1 - 0.5 * powf(2, -16 * (t - 0.5));
}

HCSAPI float
hc_ease_circ_in(float t)
{
    return 1 - sqrtf(1 - t);
}

HCSAPI float
hc_ease_circ_out(float t)
{
    return sqrtf(t);
}

HCSAPI float
hc_ease_circ_in_out(float t)
{
    if (t < 0.5) {
        return (1 - sqrtf(1 - 2 * t)) * 0.5;
    }
    return (1 + sqrtf(2 * t - 1)) * 0.5;
}

HCSAPI float
hc_ease_back_in(float t)
{
    return t * t * (2.70158 * t - 1.70158);
}

HCSAPI float
hc_ease_back_out(float t)
{
    --t; return 1 + t * t * (2.70158 * t + 1.70158);
}

HCSAPI float
hc_ease_back_in_out(float t)
{
    if (t < 0.5) {
        return t * t * (7 * t - 2.5) * 2;
    }
    --t; return 1 + t * t * 2 * (7 * t + 2.5);
}

HCSAPI float
hc_ease_elastic_in(float t)
{
    float t2 = t * t;
    return t2 * t2 * sinf(t * HC_PI * 4.5);
}

HCSAPI float
hc_ease_elastic_out(float t)
{
    float t2 = (t - 1) * (t - 1);
    return 1 - t2 * t2 * cosf(t * HC_PI * 4.5);
}

HCSAPI float
hc_ease_elastic_in_out(float t)
{
    float t2;
    if (t < 0.45) {
        t2 = t * t;
        return 8 * t2 * t2 * sinf(t * HC_PI * 9);
    } else if (t < 0.55) {
        return 0.5 + 0.75 * sinf(t * HC_PI * 4);
    }
    t2 = (t - 1) * (t - 1);
    return 1 - 8 * t2 * t2 * sinf(t * HC_PI * 9);
}

HCSAPI float
hc_ease_bounce_in(float t)
{
    return powf(2, 6 * (t - 1)) * fabsf(sinf(t * HC_PI * 3.5));
}

HCSAPI float
hc_ease_bounce_out(float t)
{
    return 1 - powf(2, -6 * t) * fabsf(cosf(t * HC_PI * 3.5));
}

HCSAPI float
hc_ease_bounce_in_out(float t)
{
    if (t < 0.5) {
        return 8 * powf(2, 8 * (t - 1)) * fabsf(sinf(t * HC_PI * 7));
    }
    return 1 - 8 * powf(2, -8 * t) * fabsf(sinf(t * HC_PI * 7));
}

#endif // HC_EASE_H
