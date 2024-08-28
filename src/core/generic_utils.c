#include "generic_utils.h"

f32 float_diff(f32 a, f32 b) {
    if (a < b) {
        return fabs(a - b);
    }
    else {
        return fabs(b - a);
    }
}

boolean almost_equals(float a, float b, float epsilon) {
 return fabs(a - b) <= epsilon;
}

boolean animate_f32_to_target(float* value, float target, float delta_t, float rate) {
	*value += (target - *value) * (1.0 - pow(2.0f, -rate * delta_t));
	if (almost_equals(*value, target, 0.001f))
	{
		*value = target;
		return true; // reached
	}
	return false;
}
