#include "ui.h"

static UIWidget root;
static ArrayList widgets;
static u64 frame_count[1];

UIWidget* current_parent;
ArrayList color_list;

#define UI_RENDER_STATE_CAPACITY 100

// forward declarations
UIWidget* _ui_findbyid(char* id);

struct UIRenderState {
    ShaderProgram shader;
    
    VertexArray vertex_array;

    int* indices;
    u32 indices_count;

    f32* positions;
    u32 positions_count;

    f32* colors;
    u32 colors_count;
};
typedef struct UIRenderState UIRenderState;

static UIRenderState ui_render_state;

void ui_push_parent(UIWidget* widget) {
    current_parent = widget;
}

void ui_push_color(vec3 color) {
    f32* _color = arraylist_push(&color_list);
    _color[0] = color[0];
    _color[1] = color[1];
    _color[2] = color[2];
}


void ui_pop_color() {
    arraylist_popkback(&color_list);
}

UIInfo ui_box(char* id, vec2 position, vec2 size) {
    UIWidget* widget = _ui_findbyid(id);
    if (!widget) {
        widget = arraylist_push(&widgets);
        int len = strlen(id);
        string_copy_n(&widget->id, id, len);
    }

    widget->last_frame = frame_count[0];

    glm_vec2_copy(position, widget->position);
    glm_vec2_copy(size, widget->size);

    f32 mouse_x = input_mouse_x_rel();
    f32 mouse_y = input_mouse_y_rel();

    boolean hot = mouse_x >= widget->position[0] && mouse_x <= widget->position[0] + widget->size[0] &&
                  mouse_y >= widget->position[1] && mouse_y <= widget->position[1] + widget->size[1];

    // todo: add scrolling to this if necessary
    boolean active = hot && (input_mouse_keyrelease(GLFW_MOUSE_BUTTON_LEFT) || 
                             input_mouse_keyrelease(GLFW_MOUSE_BUTTON_RIGHT));
    
    UIInfo info = {
        .widget = widget,
        .active = active,
        .hot = hot
    };
    
    return info;
}

UIWidget* _ui_findbyid(char* id) {
    ARRAYLIST_FOREACH(widgets, UIWidget, widget) {
        if (string_equals_lit(widget->id, id)) {
            return widget;
        }
    }

    return NULL;
}

boolean ui_init(void) {
    frame_count[0] = 0;
    glm_vec2_fill(root.size, 1.0);
    glm_vec2_fill(root.position, 0.0);
    arraylist_initialise(&widgets, 100, sizeof(UIWidget));

    if (!shader_initialise(&ui_render_state.shader, "shaders/ui_vert.txt", "shaders/ui_frag.txt")) {
        printf("Failed to load ui shader program!\n");
        return false;
    }

    ui_render_state.positions_count = UI_RENDER_STATE_CAPACITY * 2 * 4;
    ui_render_state.positions = malloc(sizeof(f32) * ui_render_state.positions_count);

    ui_render_state.colors_count = UI_RENDER_STATE_CAPACITY * 3 * 4;
    ui_render_state.colors = malloc(sizeof(f32) * ui_render_state.colors_count);

    vertexarray_initialise(&ui_render_state.vertex_array);

    ui_render_state.indices_count = UI_RENDER_STATE_CAPACITY * 6;
    ui_render_state.indices = malloc(sizeof(int) * ui_render_state.indices_count);
    u32 value = 0;
    for (int i = 0; i < UI_RENDER_STATE_CAPACITY * 6; i+=6) {
        ui_render_state.indices[i] = value + 0;
        ui_render_state.indices[i+1] = value + 1;
        ui_render_state.indices[i+2] = value + 2;
        ui_render_state.indices[i+3] = value + 2;
        ui_render_state.indices[i+4] = value + 3;
        ui_render_state.indices[i+5] = value + 0;
        value += 4;
    }

    vertexarray_add_element_indices(
        &ui_render_state.vertex_array,
        ui_render_state.indices,
        ui_render_state.indices_count,
        GL_STATIC_DRAW
    );

    vertexarray_addbufferf(
        &ui_render_state.vertex_array,
        ui_render_state.positions,
        ui_render_state.positions_count,
        GL_DYNAMIC_DRAW,
        2,
        0
    );

    vertexarray_addbufferf(
        &ui_render_state.vertex_array,
        ui_render_state.colors,
        ui_render_state.colors_count,
        GL_DYNAMIC_DRAW,
        3,
        0
    );

    arraylist_initialise(&color_list, 10, sizeof(vec3));
    f32* default_color = arraylist_push(&color_list);
    default_color[0] = 0.0;
    default_color[1] = 1.0;
    default_color[2] = 1.0;

    return true;
}

void _ui_render_flush(u32 indices_count) {
    if (indices_count == 0) return;

    shader_bind(ui_render_state.shader);
    vertexarray_bind(&ui_render_state.vertex_array);

    u32 element_count = indices_count / 6;

    vertexarray_buffersubdata_f(
        &ui_render_state.vertex_array,
        0,
        ui_render_state.positions,
        element_count * 8
    );
        
    vertexarray_buffersubdata_f(
        &ui_render_state.vertex_array,
        1,
        ui_render_state.colors,
        element_count * 12
    );

    GL_CALL(glDrawElements(GL_TRIANGLES, indices_count, GL_UNSIGNED_INT, NULL));

    shader_unbind();
    vertexarray_unbind();
}

void ui_render(void) {
    { // prune if not touched this frame
        ARRAYLIST_FOREACH(widgets, UIWidget, widget) {
            if (widget->last_frame < frame_count[0]) {
                arraylist_remove(&widgets, index);
                index--;
            }
        }
    }

    u32 positions_offset = 0, indices_count = 0, colors_offset = 0;
    ARRAYLIST_FOREACH(widgets, UIWidget, widget) {
        // top left
        ui_render_state.positions[positions_offset + 0] = widget->position[0];
        ui_render_state.positions[positions_offset + 1] = widget->position[1];

        // top right
        ui_render_state.positions[positions_offset + 2] = (widget->position[0] + widget->size[0]);
        ui_render_state.positions[positions_offset + 3] = widget->position[1];

        // bottom right
        ui_render_state.positions[positions_offset + 4] = (widget->position[0] + widget->size[0]);
        ui_render_state.positions[positions_offset + 5] = (widget->position[1] + widget->size[1]);

        // bottom left
        ui_render_state.positions[positions_offset + 6] = widget->position[0];
        ui_render_state.positions[positions_offset + 7] = (widget->position[1] + widget->size[1]);

        // background color
        ui_render_state.colors[colors_offset + 0] = widget->background_color[0];
        ui_render_state.colors[colors_offset + 1] = widget->background_color[1];
        ui_render_state.colors[colors_offset + 2] = widget->background_color[2];

        ui_render_state.colors[colors_offset + 3] = widget->background_color[0];
        ui_render_state.colors[colors_offset + 4] = widget->background_color[1];
        ui_render_state.colors[colors_offset + 5] = widget->background_color[2];

        ui_render_state.colors[colors_offset + 6] = widget->background_color[0];
        ui_render_state.colors[colors_offset + 7] = widget->background_color[1];
        ui_render_state.colors[colors_offset + 8] = widget->background_color[2];

        ui_render_state.colors[colors_offset + 9] = widget->background_color[0];
        ui_render_state.colors[colors_offset + 10] = widget->background_color[1];
        ui_render_state.colors[colors_offset + 11] = widget->background_color[2];

        colors_offset += 12;
        positions_offset += 8;
        indices_count += 6;

        if (positions_offset >= ui_render_state.positions_count) {
            _ui_render_flush(indices_count);
            positions_offset = 0;
            colors_offset = 0;
            indices_count = 0;
        }
    }

    _ui_render_flush(indices_count);
    frame_count[0]++;
}

void ui_set_framecount(u64 _frame_count) {
    frame_count[0] = _frame_count;
}

void ui_destroy(void) {
    arraylist_free(&widgets);
    shader_destroy(ui_render_state.shader);
    vertexarray_destroy(&ui_render_state.vertex_array);
    free(ui_render_state.positions);
    free(ui_render_state.colors);
    free(ui_render_state.indices);
}
