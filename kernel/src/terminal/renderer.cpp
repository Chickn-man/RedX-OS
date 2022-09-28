#include "renderer.h"

void TerminalRenderer::drawChar(char chr, unsigned int x, unsigned int y, unsigned int color) {
    if (chr == 0x00) return;
    char* fontPtr = (char*)font->buffer + (chr * font->header->charsize);
    for (unsigned long u = y; u < y + 16; u++) {
        for (unsigned long c = x; c < x + 8; c++) {
            if ((*fontPtr & (0b10000000 >> (c - x))) > 0) {
                screen.pixel(c, u, color);
            }
        }
        fontPtr++;
    }
}

void TerminalRenderer::clearChar(unsigned int x, unsigned int y, unsigned int color) {
  for (unsigned long u = y; u < y + 16; u++) {
    for (unsigned long c = x; c < x + 8; c++) {
      screen.pixel(c, u, color);
    }
  }
}

void TerminalRenderer::render() {
    for (int y = 0; y < terminal.height; y++) {
        for (int x = 0; x < terminal.width; x++) {
            if (terminal.textBuffer[x + y * terminal.width] == '\n') continue;
            drawChar(terminal.textBuffer[x + y * terminal.width], x * 8, y * 16, 0xFFFFFFFF);
        }
    }
}

TerminalRenderer terminalRenderer;