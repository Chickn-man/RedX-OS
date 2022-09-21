#pragma once

#include <stdint.h>
#include "../libs/math.h"
#include "../basicRenderer.h"

struct EFI_MEMORY_DESCRIPTOR {
  uint32_t type;
  void* physAddr;
  void* virtAddr;
  uint64_t pages;
  uint64_t attribs;
};

extern const char* EFI_MEMORY_TYPE_STRINGS[];

uint64_t getMemorySize(EFI_MEMORY_DESCRIPTOR* map, uint64_t mapEntries, uint64_t descSize);
void set(void* A, uint8_t V, uint64_t C);
void* pageAlign(void* addr);

class pageAllocator {
  public:
  void readMap(EFI_MEMORY_DESCRIPTOR* map, size_t size, size_t descSize);
  bitmap pageBitmap;
  void free(void* addr);
  void frees(void* addr, uint64_t count);
  void lock(void* addr);
  void locks(void* addr, uint64_t count);
  void* getPage();
  uint64_t getFreeMem();
  uint64_t getUsedMem();
  uint64_t getResdMem();

  private:
  void bitmapInit(size_t size, void* bufferAddr);
  void release(void* addr);
  void releases(void* addr, uint64_t count);
  void reserve(void* addr);
  void reserves(void* addr, uint64_t count);
};

extern pageAllocator allocator;