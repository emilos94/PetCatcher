#ifndef GENERIC_UTILS_H
#define GENERIC_UTILS_H

#include "core.h"
#include "math.h"

f32 float_diff(f32 a, f32 b);
boolean almost_equals(float a, float b, float epsilon);
boolean animate_f32_to_target(float* value, float target, float delta_t, float rate);

#endif // GENERIC_UTILS_H