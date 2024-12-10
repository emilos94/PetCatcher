#ifndef GENERIC_UTILS_H
#define GENERIC_UTILS_H

#include "core.h"
#include "math.h"

f32 float_diff(f32 a, f32 b);
boolean f32_almost_equals(f32 a, f32 b, f32 epsilon);
boolean animate_f32_lerp(f32* value, f32 start, f32 target, f32 progress);
boolean animate_f32_to_target(f32* value, f32 target, f32 delta_t, f32 rate);
f32 f32_ease_out_back(f32 progress);
f32 f32_clamp(f32 value, f32 min, f32 max);
u32 rand_u32_between(u32 min, u32 max);
int min_i(int a, int b);

#endif // GENERIC_UTILS_H