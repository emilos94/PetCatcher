#include "stdio.h"
#include "graphics/window.h"
#include "graphics/shaderprogram.h"
#include "graphics/vertexarray.h"
#include "core/input.h"
#include "cglm/cglm.h"
#include "core/file_loader.h"

int main(void) {

    Window window;
    window_init(&window, 1280, 720);
    boolean running = true;

    ShaderProgram shader;
    if (!shader_initialise(&shader, "shaders/vert.txt", "shaders/frag.txt")) {
        printf("[ERROR] failed to initialise shader");
        return 0;
    }

    mat4 m;

    VertexArray vertex_array;
    vertexarray_initialise(&vertex_array);

    float positions[18] = {
        -0.5, 0.5, 0.0,
        0.5, 0.5, 0.0,
        0.5, -0.5, 0.0,
        0.5, -0.5, 0.0,
        -0.5, -0.5, 0.0,
        -0.5, 0.5, 0.0
    };

    XmlNode test = {};
    if (file_loadxml(&test, "../res/test.xml")) {
        printf("Loaded xml file!\nTag: %s\n", test.tag.chars);
        printf("Attributes:\n");
        {
            ArrayList* list = &test.attributes;
            ARRAYLIST_FOREACH(list, XmlAttribute, attribute) {
                printf("Attribute: %s -> %s\n", attribute->key.chars, attribute->value.chars);
            }
        }

        {
            ArrayList* children = &test.children;
            ARRAYLIST_FOREACH(children, XmlNode, node) {
                printf("%s, %s\n", node->tag.chars, node->inner_text.chars);
            }
        }

        xmlnode_free(&test);
    }

    vertexarray_addbufferf(&vertex_array, &positions[0], 18, GL_STATIC_DRAW, 3);

    while (running && !window_should_exit(&window)) {
        if (input_keydown(GLFW_KEY_ESCAPE)) {
            running = false;
        }

        shader_bind(shader);
        vertexarray_bind(&vertex_array);

        GL_CALL(glDrawArrays(GL_TRIANGLES, 0, 6));
        
        window_swapandpoll(&window);
    }

    shader_destroy(shader);
    vertexarray_destroy(&vertex_array);
    window_destroy(&window);
    
    return 0;
}
