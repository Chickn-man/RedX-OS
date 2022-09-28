#pragma once

#include <stdint.h>

#include "gdt/gdt.h"
#include "memory/memory.h"
#include "interrupts/interrupts.h"
#include "terminal/renderer.h"
#include "memory/paging.h"
#include "psf.h"

struct KernelParameters {
  Framebuffer* buffer;
  PSF_FONT* font;
  EFI_MEMORY_DESCRIPTOR* map;
  uint64_t mapSize;
  uint64_t descSize;
  void *stack;
  size_t stackSize;
  void *pageDirectoryAddress;
  size_t pageDirectorySize;
};

void gdtInit();

void memoryInit(KernelParameters *params);

void interuptInit();

void welcome();

void panic(const char *message);

void panic(char *message);