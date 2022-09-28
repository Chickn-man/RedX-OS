#pragma once
#include <stdint.h>
#include "../terminal/terminal.h"
#include "../basicRenderer.h"

#define LeftShift 0x2A
#define RightShift 0x36
#define Enter 0x1C
#define BackSpace 0x0E
#define Spacebar 0x39

#define Up 0x48
#define Down 0x50
#define Left 0x4B
#define Right 0x4D

extern const char ascii[];
char lookup(uint8_t scancode, bool uppercase);

void handleKeyboard(uint8_t scancode);