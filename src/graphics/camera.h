#ifndef CAMERA_H
#define CAMERA_H

#include "core/core.h"
#include "cglm/cglm.h"
#include "core/input.h"

struct Camera {
    vec3 position, front, up, right;
    f32 panning_speed, pan_sensitivity, yaw, pitch;
};
typedef struct Camera Camera;

void camera_init(Camera* camera, vec3 position, vec3 front, vec3 up, f32 pan_speed, f32 pan_sensitivity);
void camera_lookaround_update(Camera* camera, f32 delta);

#endif // CAMERA_H