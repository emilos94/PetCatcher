#include "stdio.h"
#include "graphics/window.h"
#include "graphics/shaderprogram.h"
#include "graphics/vertexarray.h"
#include "graphics/render_pipe.h"
#include "graphics/texture.h"
#include "core/input.h"
#include "cglm/cglm.h"
#include "cglm/struct.h"
#include "core/file_loader.h"
#include "game.h"

int main(void) {
    input_init();

    if (!window_init(1280, 720, "Pet catcher")) {
        printf("[ERROR] failed to initialise shader");
        return 0;
    }

    GL_CALL(glEnable(GL_CULL_FACE));
    GL_CALL(glCullFace(GL_BACK));

    boolean running = true;

    RenderState render_state;
    if (!renderstate_init(&render_state)) {
        return 0;
    }

    GameState game_state;
    if (!gamestate_init(&game_state)) {
        return 0;
    }

    ColladaData test = {};
    if (!file_loadcollada(&test, "../res/cube1.dae")) {

        printf("Failed to load collada file!\n");
        return 0;
    }

    Texture texture;
    texture_init(&texture, "../res/textures/apollo-8x.png");

    u32 texture_index = 0;

    f32 accumulator_time = 0.0, last_time = glfwGetTime(), seconds_per_frame = 1.0 / 60.0;
    while (running && !window_should_exit()) {
        f32 current_time = glfwGetTime();
        f32 elapsed_time = current_time - last_time;
        accumulator_time += elapsed_time;
        last_time = current_time;

        // input
        if (input_keydown(GLFW_KEY_ESCAPE)) {
            running = false;
        }
        camera_input(&game_state, &render_state, elapsed_time);

        // update
        if (accumulator_time > seconds_per_frame) {
            accumulator_time -= seconds_per_frame;

            game_state.print_timer += seconds_per_frame;

        }

        // render
        shader_bind(render_state.shader);
        texture_bind(&texture);

        for (int i = 0; i < test.mesh_count; i++) {
            renderpipe_render_mesh(&render_state.render_pipe, render_state.shader, test.meshes + i);
        }
        renderpipe_flush(&render_state.render_pipe, render_state.shader);
        
        input_endframe();
        window_swapandpoll();
        GL_CALL(glClear(GL_COLOR_BUFFER_BIT));
    }

    // :cleanup
    shader_destroy(render_state.shader);
    renderpipe_destroy(&render_state.render_pipe);
    colladadata_free(&test);
    texture_destroy(&texture);
    window_destroy();
    
    return 0;
}
