#include "screen.h"

void Screen::pixel(int x, int y, uint8_t a, uint8_t r, uint8_t g, uint8_t b) {
    if (!((x * 4 + (y * doubleBuffer->ppsl * 4) + (char*)doubleBuffer->BaseAddr) < (char*)doubleBuffer->BaseAddr + doubleBuffer->Size)) {return;}
    uint32_t color = 0;
    color |= b;
    color |= g << 8;
    color |= r << 16;
    color |= a << 24;
    ((uint32_t*)doubleBuffer->BaseAddr)[x + y * doubleBuffer->Width] = color;
}

void Screen::pixel(int x, int y, uint32_t color) {
    if (!((x * 4 + (y * doubleBuffer->ppsl * 4) + (char*)doubleBuffer->BaseAddr) < (char*)doubleBuffer->BaseAddr + doubleBuffer->Size)) {return;}
    ((uint32_t*)doubleBuffer->BaseAddr)[x + y * doubleBuffer->Width] = color;
}

Screen screen;