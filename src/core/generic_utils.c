#include "generic_utils.h"

f32 float_diff(f32 a, f32 b) {
    if (a < b) {
        return fabs(a - b);
    }
    else {
        return fabs(b - a);
    }
}

boolean f32_almost_equals(f32 a, f32 b, f32 epsilon) {
 return fabs(a - b) <= epsilon;
}

boolean animate_f32_lerp(f32* value, f32 start, f32 target, f32 progress) {
    f32 delta = start + (target - start) * f32_clamp(progress, 0.0, 1.0);
    *value = delta;

    if (f32_almost_equals(*value, target, 0.001)) {
        *value = target;
        return true;
    }

    return false;
}

boolean animate_f32_to_target(f32* value, f32 target, f32 delta_t, f32 rate) {
	*value += (target - *value) * (1.0 - pow(2.0f, -rate * delta_t));
	if (f32_almost_equals(*value, target, 0.001)) {
		*value = target;
		return true; // reached
	}
	return false;
}

f32 f32_ease_out_back(f32 progress) {
    f32 c1 = 1.70158;
    f32 c3 = c1 + 1.0;

    f32 result = 1.0 + c3 * pow(progress - 1.0, 3.0) + c1 * pow(progress - 1.0, 2.0);
    return result;
}

f32 f32_clamp(f32 value, f32 min, f32 max) {
    f32 result = value;
    if (result < min) {
        result = min;
    }
    if (result > max) {
        result = max;
    }

    return result;
}

u32 rand_u32_between(u32 min, u32 max) {
    u32 result = rand() % (max - min + 1) + min;
    return result;
}


int min_i(int a, int b) {
    if (a < b) {
        return a;
    }

    return b;
}
