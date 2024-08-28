#include "camera.h"

void camera_init(Camera* camera, vec3 position, vec3 front, vec3 up, f32 pan_speed, f32 pan_sensitivity) {
    glm_vec3_copy(position, camera->position);
    glm_vec3_copy(front, camera->front);
    glm_vec3_copy(up, camera->up);
    glm_vec3_zero(camera->right);

    camera->panning_speed = 5.0;
    camera->pan_sensitivity = 0.02;
    camera->yaw = -90.0;
    camera->pitch = 0.0;
}

void camera_lookaround_update(Camera* camera, f32 delta) {
    { // panning (look around)
        camera->yaw += input_mouse_x_delta() * camera->pan_sensitivity;
        camera->pitch += input_mouse_y_delta() * camera->pan_sensitivity;
        if (camera->pitch > 89.0) {
            camera->pitch = 89.0;
        }
        if (camera->pitch < -89.0) {
            camera->pitch = -89.0;
        }

        vec3 look_direction = GLM_VEC3_ZERO_INIT;
        look_direction[0] = cos(glm_rad(camera->yaw)) * cos(glm_rad(camera->pitch));
        look_direction[1] = sin(glm_rad(camera->pitch));
        look_direction[2] = sin(glm_rad(camera->yaw)) * cos(glm_rad(camera->pitch));
        glm_normalize(look_direction);
        glm_vec3_copy(look_direction, camera->front);
        
        glm_vec3_zero(camera->right);
        glm_vec3_cross(GLM_YUP, look_direction, camera->right);
        glm_normalize(camera->right);

        glm_vec3_cross(look_direction, camera->right, camera->up);
        glm_normalize(camera->up);
    }
}
