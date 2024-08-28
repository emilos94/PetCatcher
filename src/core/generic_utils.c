#include "generic_utils.h"

f32 float_diff(f32 a, f32 b) {
    if (a < b) {
        return fabs(a - b);
    }
    else {
        return fabs(b - a);
    }
}
