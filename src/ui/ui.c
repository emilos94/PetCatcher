#include "ui.h"

static UIWidget root;
static ArrayList widgets;
static u64 frame_count[1];

UIWidget* current_parent;
ArrayList color_list;

#define UI_RENDER_STATE_CAPACITY 100
#define UI_RENDER_STATE_TEXT_VERTEX_CAPACITY 600

// forward declarations
UIWidget* _ui_findbyid(char* id);
UIWidget* _ui_widget_create(char* id, u32 flags);
void _ui_widget_check_interaction(UIWidget* widget);
CharacterInfo* _ui_charinfo_by_char(FontFileResult* font, char c);
f32 _ui_seek_word_width(FontFileResult* font, String str, u32 offset, f32 font_size);
f32 _ui_text_width(FontFileResult* font, UIWidget* widget);
f32 _ui_text_height(FontFileResult* font, UIWidget* widget);
void _ui_fill_quad_into(f32* destination, u32 offset, vec2 top_left, vec2 bottom_right);
void _ui_text_flush(u32 indices_count);

struct UIRenderState {
    ShaderProgram shader;
    
    VertexArray vertex_array;

    int* indices;
    u32 indices_count;

    f32* positions;
    u32 positions_count;

    f32* colors;
    u32 colors_count;

    // text
    ShaderProgram shader_text;

    f32* text_positions;
    u32 text_positions_count;

    f32* text_uvs;
    u32 text_uvs_count;

    int* text_indices;
    u32 text_indices_count;

    FontFileResult font;

    VertexArray text_vertex_array;
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

UIWidget* ui_box(char* id, vec2 position, vec2 size) {
    UIWidget* widget = _ui_findbyid(id);
    if (!widget) {
        widget = _ui_widget_create(id, UIFlag_Clickable | UIFlag_Hoverable | UIFlag_RenderBackground);
    }

    widget->last_frame = frame_count[0];

    glm_vec2_copy(position, widget->position);
    glm_vec2_copy(size, widget->size);
    
    return widget;
}

UIWidget* ui_label(char* id, char* text, vec2 position, vec2 size, f32 font_size) {
    UIWidget* widget = _ui_findbyid(id);
    if (!widget) {
        widget = _ui_widget_create(id, UIFlag_Text);
    }

    widget->last_frame = frame_count[0];

    glm_vec2_copy(position, widget->position);
    glm_vec2_copy(size, widget->size);

    if (!string_equals_lit(widget->text, text)) {
        free(widget->text.chars);
        string_copy_lit(&widget->text, text);
    }
    widget->font_size = font_size;
    
    _ui_widget_check_interaction(widget);
    return widget;;
}


UIWidget* ui_button(char* id, char* text, vec2 position, vec2 size, f32 font_size) {
    UIWidget* widget = _ui_findbyid(id);
    if (!widget) {
        widget = _ui_widget_create(id, UIFlag_Clickable | UIFlag_Hoverable | UIFlag_Text | UIFlag_RenderBackground);
    }

    widget->last_frame = frame_count[0];

    glm_vec2_copy(position, widget->position);
    glm_vec2_copy(size, widget->size);

    if (!string_equals_lit(widget->text, text)) {
        free(widget->text.chars);
        string_copy_lit(&widget->text, text);
    }
    widget->font_size = font_size;
    
    _ui_widget_check_interaction(widget);
    return widget;
}

UIWidget* _ui_widget_create(char* id, u32 flags) {
    UIWidget* widget = arraylist_push(&widgets);
    memset(widget, 0, sizeof(UIWidget));
    string_copy_lit(&widget->id, id);
    widget->flags = flags;

    // defaults
    glm_vec3_copy((vec3){1.0, 1.0, 1.0}, widget->text_color);
    widget->text_center_x = true;
    widget->text_center_y = true;

    return widget;
}

void _ui_widget_check_interaction(UIWidget* widget) {
    if (widget->flags & UIFlag_Hoverable) {
        f32 mouse_x = input_mouse_x_rel();
        f32 mouse_y = input_mouse_y_rel();

        widget->hot = mouse_x >= widget->position[0] && mouse_x <= widget->position[0] + widget->size[0] &&
                mouse_y >= widget->position[1] && mouse_y <= widget->position[1] + widget->size[1];
    }

    if (widget->flags & UIFlag_Clickable && widget->hot) {
        // todo: add scrolling to this if necessary
        widget->active = input_mouse_keyrelease(GLFW_MOUSE_BUTTON_LEFT) || input_mouse_keyrelease(GLFW_MOUSE_BUTTON_RIGHT);
    }
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

    if (!file_loadfont(&ui_render_state.font, "../res/fonts/candara.fnt", "../res/fonts/candara.png")) {
        printf("Failed to load font!\n");
        return false;
    }

    if (!shader_initialise(&ui_render_state.shader_text, "shaders/text_vert.txt", "shaders/text_frag.txt")) {
        printf("Failed to load text shader program!\n");
        return false;
    }

    ui_render_state.positions_count = UI_RENDER_STATE_CAPACITY * 2 * 4;
    ui_render_state.positions = malloc(sizeof(f32) * ui_render_state.positions_count);

    ui_render_state.colors_count = UI_RENDER_STATE_CAPACITY * 3 * 4;
    ui_render_state.colors = malloc(sizeof(f32) * ui_render_state.colors_count);

    ui_render_state.text_indices_count = UI_RENDER_STATE_TEXT_VERTEX_CAPACITY;
    ui_render_state.text_indices = malloc(sizeof(u32) * ui_render_state.text_indices_count);
    u32 indices_value = 0;
    for (int i = 0; i < ui_render_state.text_indices_count; i+=6) {
        ui_render_state.text_indices[i] = indices_value + 0;
        ui_render_state.text_indices[i+1] = indices_value + 1;
        ui_render_state.text_indices[i+2] = indices_value + 2;
        ui_render_state.text_indices[i+3] = indices_value + 2;
        ui_render_state.text_indices[i+4] = indices_value + 3;
        ui_render_state.text_indices[i+5] = indices_value + 0;
        indices_value += 4;
    }

    ui_render_state.text_positions_count = UI_RENDER_STATE_TEXT_VERTEX_CAPACITY * 2;
    ui_render_state.text_positions = malloc(sizeof(u32) * ui_render_state.text_positions_count);

    ui_render_state.text_uvs_count = UI_RENDER_STATE_TEXT_VERTEX_CAPACITY * 2;
    ui_render_state.text_uvs = malloc(sizeof(u32) * ui_render_state.text_uvs_count);

    vertexarray_initialise(&ui_render_state.text_vertex_array);
    vertexarray_addbufferf(&ui_render_state.text_vertex_array, ui_render_state.text_positions, ui_render_state.text_positions_count, GL_DYNAMIC_DRAW, 2, 0);
    vertexarray_addbufferf(&ui_render_state.text_vertex_array, ui_render_state.text_uvs, ui_render_state.text_uvs_count, GL_DYNAMIC_DRAW, 2, 0);
    vertexarray_add_element_indices(&ui_render_state.text_vertex_array, ui_render_state.text_indices, ui_render_state.text_indices_count, GL_DYNAMIC_DRAW);
    vertexarray_unbind();

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
    boolean any_hovered = false;
    { // prune if not touched this frame
        ARRAYLIST_FOREACH(widgets, UIWidget, widget) {
            if (widget->last_frame < frame_count[0]) {
                arraylist_remove(&widgets, index);
                index--;
            }
            if (widget->hot && widget->flags & UIFlag_Clickable) {
                any_hovered = true;
            }
        }
    }

    if (any_hovered) {
        window_set_cursor_hand();
    }
    else {
        window_set_cursor_arrow();
    }
    
    GL_CALL(glDisable(GL_DEPTH_TEST));

    u32 positions_offset = 0, indices_count = 0, colors_offset = 0;
    
    u32 text_pos_offset = 0, text_uvs_offset = 0, text_indices_count = 0;
    ARRAYLIST_FOREACH(widgets, UIWidget, widget) {
        if (widget->flags & UIFlag_RenderBackground) {
            _ui_fill_quad_into(
                ui_render_state.positions,
                positions_offset,
                (vec2){widget->position[0], widget->position[1]},
                (vec2){widget->position[0] + widget->size[0], widget->position[1] + widget->size[1]}
            );

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

        if (widget->flags & UIFlag_Text) {
            f32 x_offset = widget->position[0], y_offset = widget->position[1];

            // todo: needs to be based on line by line width to center each line
            if (widget->text_center_x) {
                f32 text_width = _ui_text_width(&ui_render_state.font, widget);
                if (text_width < widget->size[0]) {
                    x_offset += (widget->size[0] - text_width) / 2.0;
                }
            }
            if (widget->text_center_y) {
                f32 text_height = _ui_text_height(&ui_render_state.font, widget);
                if (text_height < widget->size[1]) {
                    y_offset -= (widget->size[1] - text_height) / 2.0;
                }
            }

            f32 x_cursor = x_offset, y_cursor = y_offset;
            for (int i = 0; i < widget->text.length; i++) {
                /*f32 word_width = _ui_seek_word_width(&ui_render_state.font, widget->text, i, widget->font_size);
                if (x_cursor + word_width > widget->size[0]) {
                    x_cursor = x_offset;
                    y_cursor += ui_render_state.font.line_height * widget->font_size;
                }*/
                char c = widget->text.chars[i];
                CharacterInfo* char_info = _ui_charinfo_by_char(&ui_render_state.font, c);
                if (!char_info) {
                    continue;
                }

                f32 y_cursor_top = y_cursor + ui_render_state.font.line_height * widget->font_size;
                f32 top_y = y_cursor_top - char_info->y_offset * widget->font_size;

                _ui_fill_quad_into(
                    ui_render_state.text_positions, 
                    text_pos_offset, 
                    (vec2){x_cursor, top_y - (f32)char_info->height * widget->font_size}, 
                    (vec2){x_cursor + char_info->width * widget->font_size, top_y}
                );
                text_pos_offset += 8;

                _ui_fill_quad_into(
                    ui_render_state.text_uvs, 
                    text_uvs_offset, 
                    (vec2){
                        (f32)char_info->x / (f32)ui_render_state.font.texture.width, 
                        (f32)(char_info->y + char_info->height) / (f32)ui_render_state.font.texture.height
                    }, 
                    (vec2){
                        (f32)(char_info->x + char_info->width) / (f32)ui_render_state.font.texture.width, 
                        (f32)char_info->y / (f32)ui_render_state.font.texture.height
                    }
                );
                text_uvs_offset += 8;

                x_cursor += char_info->x_advance * widget->font_size;
                text_indices_count += 6;

                if (text_pos_offset >= ui_render_state.positions_count) {
                    _ui_text_flush(text_indices_count);
                    text_indices_count = 0;
                    text_pos_offset = 0;
                    text_uvs_offset = 0;
                }
            }

            // check if text char length overflows current buffer

            // add quad for each letter in text

            // add counter in state
        }
    }

    _ui_render_flush(indices_count);
    _ui_text_flush(text_indices_count);
    frame_count[0]++;
    
    GL_CALL(glEnable(GL_DEPTH_TEST));
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

    free(ui_render_state.text_positions);
    free(ui_render_state.text_uvs);
    free(ui_render_state.text_indices);
    vertexarray_destroy(&ui_render_state.text_vertex_array);
    shader_destroy(ui_render_state.shader_text);
}

CharacterInfo* _ui_charinfo_by_char(FontFileResult* font, char c) {
    for (int i = 0; i < font->character_info_count; i++) {
        if (font->character_infos[i].id == c) {
            return font->character_infos + i;
        }
    }

    return NULL;
}

f32 _ui_seek_word_width(FontFileResult* font, String str, u32 offset, f32 font_size) {
    f32 word_width = 0.0;
    for (int i = offset; i < str.length; i++) {
        char c = str.chars[i];
        if (c == ' ' || c == '\n') {
            return word_width;
        }

        CharacterInfo* info = _ui_charinfo_by_char(&ui_render_state.font, c);
        word_width += (info->width + info->x_advance) * font_size;
    }

    return word_width;
}

f32 _ui_text_width(FontFileResult* font, UIWidget* widget) {
    f32 max_width = 0.0, current_width = 0.0;
    f32 line_max_width = widget->size[0];
    for (int i = 0; i < widget->text.length; i++) {
        char c = widget->text.chars[i];
        if (c == ' ') {
            if (current_width > max_width) {
                max_width = current_width;
            }
        }
        else {
            CharacterInfo* char_info = _ui_charinfo_by_char(font, c);
            if (char_info) {
                current_width += char_info->x_advance * widget->font_size;
            }
        }
    }

    if (current_width > max_width) {
        max_width = current_width;
    }

    return max_width;
}

f32 _ui_text_height(FontFileResult* font, UIWidget* widget) {
    f32 result = 0.0;
    for (int i = 0; i < widget->text.length; i++) {
        char c = widget->text.chars[i];
        CharacterInfo* char_info = _ui_charinfo_by_char(font, c);

        if (char_info) {
            f32 height = char_info->height * widget->font_size;
            if (height > result) {
                result = height;
            }
        }
    }

    return result;
}


void _ui_fill_quad_into(f32* destination, u32 offset, vec2 top_left, vec2 bottom_right) {
    // top left
    destination[offset + 0] = top_left[0];
    destination[offset + 1] = top_left[1];

    // top right
    destination[offset + 2] = bottom_right[0];
    destination[offset + 3] = top_left[1];

    // bottom right
    destination[offset + 4] = bottom_right[0];
    destination[offset + 5] = bottom_right[1];

    // bottom left
    destination[offset + 6] = top_left[0];
    destination[offset + 7] = bottom_right[1];
}

void _ui_text_flush(u32 indices_count) {
    if (indices_count == 0) return;

    shader_bind(ui_render_state.shader_text);
    vertexarray_bind(&ui_render_state.text_vertex_array);
    texture_bind(&ui_render_state.font.texture);

    u32 element_count = indices_count / 6;

    vertexarray_buffersubdata_f(
        &ui_render_state.text_vertex_array,
        0,
        ui_render_state.text_positions,
        element_count * 8
    );
        
    vertexarray_buffersubdata_f(
        &ui_render_state.text_vertex_array,
        1,
        ui_render_state.text_uvs,
        element_count * 8
    );

    GL_CALL(glDrawElements(GL_TRIANGLES, indices_count, GL_UNSIGNED_INT, NULL));

    shader_unbind();
    vertexarray_unbind();
}