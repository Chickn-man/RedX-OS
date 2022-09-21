#pragma once

#include "libs/math.h"

typedef struct {
  void* BaseAddr;
  size_t Size;
  unsigned int Width;
  unsigned int Height;
  unsigned int ppsl; //Pixels per scan line
} Framebuffer;

class Screen {
  public:
  Framebuffer *buffer;
  Framebuffer *doubleBuffer;
  void pixel(int x, int y, uint8_t a, uint8_t r, uint8_t g, uint8_t b);
  void pixel(int x, int y, uint32_t color);
};

extern Screen screen;