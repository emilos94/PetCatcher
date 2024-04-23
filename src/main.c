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

    boolean running = true;

    RenderState render_state;
    if (!renderstate_init(&render_state)) {
        return 0;
    }

    ColladaData test = {};
    if (!file_loadcollada(&test, "../res/cube.dae")) {

        printf("Failed to load collada file!\n");
        return 0;
    }

    // :lighting
    vec3 light_color = {0.9, 0.9, 0.9};
    f32 ambient_strength = 0.4;

    Texture texture;
    texture_init(&texture, "../res/textures/apollo-8x.png");

    u32 texture_index = 0;

    while (running && !window_should_exit()) {
        // input
        if (input_keydown(GLFW_KEY_ESCAPE)) {
            running = false;
        }

        // update


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
