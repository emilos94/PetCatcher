#include "stdio.h"
#include "graphics/shaderprogram.h"
#include "graphics/vertexarray.h"
#include "graphics/render_pipe.h"
#include "graphics/texture.h"
#include "cglm/cglm.h"
#include "cglm/struct.h"
#include "core/input.h"
#include "core/file_loader.h"
#include "game.h"

int main(void) {
    if (!window_init(1280, 720, "Pet catcher")) {
        printf("[ERROR] failed to initialise window");
        goto cleanup;
    }
    
    input_init();

    GL_CALL(glEnable(GL_CULL_FACE));
    GL_CALL(glCullFace(GL_BACK));
    GL_CALL(glEnable(GL_DEPTH_TEST));
    GL_CALL(glEnable(GL_BLEND));
    GL_CALL(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
    GL_CALL(glClearColor(0.0, 0.2, 0.8, 0.0));
    
    RenderState render_state;
    if (!renderstate_init(&render_state)) {
        printf("[ERROR] failed to initialise render state");
        goto cleanup;
    }

    GameState game_state;
    if (!gamestate_init(&game_state)) {
        printf("[ERROR] failed to initialise game state");
        goto cleanup;
    }

    if (!ui_init()) {
        printf("[ERROR] failed to initialise ui module");
        goto cleanup;
    }

    boolean mouse_enabled = false;
    f32 accumulator_time = 0.0, last_time = glfwGetTime(), seconds_per_frame = 1.0 / 60.0;
    while (!game_state.quiting && !window_should_exit()) {
        f32 current_time = glfwGetTime();
        f32 elapsed_time = current_time - last_time;
        accumulator_time += elapsed_time;
        last_time = current_time;
        game_state.second_timer += elapsed_time;

        // input
        if (input_keydown(GLFW_KEY_ESCAPE)) {
            game_state.quiting = true;
        }
        if (input_keydown(GLFW_KEY_LEFT_CONTROL) && input_keyjustdown(GLFW_KEY_M)) {
            mouse_enabled = !mouse_enabled;
            window_enable_cursor(mouse_enabled);
        }
        game_input(&game_state);
        camera_input(&game_state, &render_state, elapsed_time);

        // update
        if (accumulator_time > seconds_per_frame) {
            accumulator_time -= seconds_per_frame;
            game_state.fps++;
            game_update(&game_state, seconds_per_frame);        
            input_endframe();
        }

        if (game_state.second_timer >= 1.0) {
            game_state.fps = 0;
            game_state.second_timer -= 1.0;
        }

        // render
        game_render(&game_state, &render_state, elapsed_time);
        
        window_swapandpoll();
        GL_CALL(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
    }

cleanup:
    shader_destroy(render_state.shader);
    renderpipe_destroy(&render_state.render_pipe);
    ui_destroy();
    texture_destroy(&game_state.test_texture);
    window_destroy();

    return 0;
}
