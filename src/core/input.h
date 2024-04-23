#ifndef INPUT_H
#define INPUT_H

#include "core/core.h"

void input_init(void);
void input_endframe(void);
boolean input_keydown(int key);
boolean input_keyjustdown(int key);


#endif