#pragma once

#include <stdint.h>

#include "gdt/gdt.h"
#include "memory/memory.h"
#include "interrupts/interrupts.h"

struct KernelParameters {
  Framebuffer* buffer;
  PSF_FONT* font;
  EFI_MEMORY_DESCRIPTOR* map;
  uint64_t mapSize;
  uint64_t descSize;
};

void gdtInit();

extern pTableMan pageTableManager;
void memoryInit(KernelParameters *params, EFI_MEMORY_DESCRIPTOR *stack);

void interuptInit();

void welcome();

void panic(const char *message);

void panic(char *message);