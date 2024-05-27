#ifndef UI_H
#define UI_H

#include "core/core.h"
#include "core/string.h"
#include "graphics/window.h"
#include "datastructures/arraylist.h"
#include "datastructures/list.h"
#include "cglm/cglm.h"
#include "graphics/shaderprogram.h"
#include "graphics/vertexarray.h"

enum UISizeKind {
    UISizeKind_Relative,
    UISizeKind_Pixel,
    UISizeKind_Fill
};
typedef u32 UISizeKind;

struct UIWidget {
    struct UIWidget* parent;
    struct UIWidget* child_first;
    struct UIWidget* next;

    vec2 position, size;
    UISizeKind size_kind[2];

    vec3 background_color;

    String text, id;
    vec3 text_color;

    vec2 rel_position, rel_size;
};
typedef struct UIWidget UIWidget;

struct UIInfo {
    UIWidget* widget;
    boolean hot, active;
};
typedef struct UIInfo UIInfo;

void ui_push_parent(UIWidget* widget);
void ui_push_color(vec3 color);
void ui_pop_color();

UIInfo ui_box(char* id, vec2 position, vec2 size);

boolean ui_init(void);
void ui_render(void);
void ui_destroy(void);

#endif // UI_H