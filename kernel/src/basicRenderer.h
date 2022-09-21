#pragma once

#include "screen.h"
#include "terminal/terminal.h"
#include "psf.h"

struct cursor {
  public:
  unsigned int x;
  unsigned int y;
  unsigned int xm;
  unsigned int ym;
  void reset();
  void newLine();
  void back();
  void up();
  void down();
  void left();
  void right();
  void move(uint32_t x, uint32_t y);
};

extern const uint8_t mCurBitmap[32];

class basicRenderer {
  public:
  Framebuffer *buffer;
  PSF_FONT *font;
  void plotPixel(unsigned int x, unsigned int y, unsigned int color);
  void drawLine(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2, unsigned int color);
  void rect(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2, unsigned int color);
  void cls();
  void fill(uint32_t color);
  void printChar(char chr, unsigned int x, unsigned int y, unsigned int color);
  void drawMouse(unsigned int x, unsigned int y, uint32_t color);
  void printChar(char chr, unsigned int color);
  void putChar(char chr, unsigned int color);
  void delChar(unsigned int x, unsigned int y, unsigned int color);
  void clChar(unsigned int x, unsigned int y, unsigned int color);
  void printString(const char* str, unsigned int color);
  void print(const char* str, unsigned int color);
};

extern basicRenderer basicRender;
extern cursor cur;