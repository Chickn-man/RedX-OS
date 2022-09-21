#pragma once
#include <stdint.h>
#include "io.h"
#include "../basicRenderer.h"
#include "../libs/string.h"

#define YOFlow 0b10000000
#define XOFlow 0b01000000

#define YSign 0b00100000
#define XSign 0b00010000

#define MidBtn 0b00000100
#define RigBtn 0b00000010
#define LefBtn 0b00000001


void mouseWait();

extern cursor mouse;
void handleMouse(uint8_t data);

void processMPacket();

void ps2MouseInit();