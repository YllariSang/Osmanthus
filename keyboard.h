#ifndef KEYBOARD_H
#define KEYBOARD_H

#include "idt.h"

#ifdef __cplusplus
extern "C" {
#endif

void init_keyboard(void);
void keyboard_handler(registers_t* regs);

#ifdef __cplusplus
}
#endif

#endif
