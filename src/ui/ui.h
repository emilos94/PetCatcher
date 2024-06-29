#ifndef UI_H
#define UI_H

#include "core/core.h"
#include "core/mystring.h"
#include "graphics/window.h"
#include "core/input.h"
#include "datastructures/arraylist.h"
#include "datastructures/list.h"
#include "cglm/cglm.h"
#include "graphics/shaderprogram.h"
#include "graphics/vertexarray.h"
#include "core/file_loader.h"

enum UISizeKind {
    UISizeKind_Relative,
    UISizeKind_Pixel,
    UISizeKind_Fill
};
typedef u32 UISizeKind;

enum UIFlag {
    UIFlag_RenderBackground = 1 << 0,
    UIFlag_Text = 1 << 1,
    UIFlag_Clickable = 1 << 2,
    UIFlag_Hoverable = 1 << 3
};

struct UIWidget {
    struct UIWidget* parent;
    struct UIWidget* child_first;
    struct UIWidget* next;

    String id;
    vec2 position, size;
    UISizeKind size_kind[2];
    u32 flags;

    vec3 background_color;

    // interaction
    boolean hot, active;

    // text
    String text;
    vec3 text_color;
    boolean text_center_x, text_center_y;
    f32 font_size;

    vec2 rel_position, rel_size;

    u64 last_frame;
};
typedef struct UIWidget UIWidget;

void ui_push_parent(UIWidget* widget);
void ui_push_color(vec3 color);
void ui_pop_color();

UIWidget* ui_box(char* id, vec2 position, vec2 size);
UIWidget* ui_label(char* id, char* text, vec2 position, vec2 size, f32 font_size);
UIWidget* ui_button(char* id, char* text, vec2 position, vec2 size, f32 font_size);

boolean ui_init(void);
void ui_render(void);
void ui_destroy(void);

void ui_set_framecount(u64 frame_count);

#endif // UI_H