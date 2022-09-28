#include "memory.h"

const char* EFI_MEMORY_TYPE_STRINGS[] {
  "EfiReservedMemoryType",
  "EfiLoaderCode",
  "EfiLoaderData",
  "EfiBootServicesCode",
  "EfiBootServicesData",
  "EfiRuntimeServicesCode",
  "EfiRuntimeServicesData",
  "EfiConventionalMemory",
  "EfiUnusableMemory",
  "EfiACPIReclaimMemory",
  "EfiACPIMemoryNVS",
  "EfiMemoryMappedIO",
  "EfiMemoryMappedIOPortSpace",
  "EfiPalCode"
};

uint64_t getMemorySize(EFI_MEMORY_DESCRIPTOR* map, uint64_t entries, uint64_t descSize) {
  static uint64_t memSizeBytes = 0;
  if (memSizeBytes > 0) return memSizeBytes;

  for (int i = 0; i < entries; i++) {
    EFI_MEMORY_DESCRIPTOR* desc = (EFI_MEMORY_DESCRIPTOR*)((uint64_t)map + (i * descSize));
    memSizeBytes += desc->pages * 4096;
  }

  return memSizeBytes;
}

void set(void* A, uint8_t V, uint64_t C) {
  for (uint64_t i = 0; i < C; i++) {
    *(uint8_t*)((uint64_t)A + i) = V;
  }
}

uint64_t freeMem;
uint64_t reservedMem;
uint64_t usedMem;
bool initialized = false;
pageAllocator allocator;

void* pageAlign(void* addr) {
  return (void*)(((uint64_t)addr / 0x1000) * 0x1000);
}

void pageAllocator::readMap(EFI_MEMORY_DESCRIPTOR* map, size_t size, size_t descSize) {
  if (initialized) return;
  initialized = true;

  uint64_t entries = size / descSize;

  void* largestSeg = NULL;
  size_t largestSegSize = 0;

  for (uint64_t i = 0; i < entries; i++) {
    EFI_MEMORY_DESCRIPTOR* desc = (EFI_MEMORY_DESCRIPTOR*)((uint64_t)map + (i * descSize));
    if (desc->type == 7) {
      if (desc->pages * 4096 > largestSegSize) {
        largestSeg = desc->physAddr;
        largestSegSize = desc->pages * 4096;
      }
    } 
  }

  uint64_t memSize = getMemorySize(map, entries, descSize);
  freeMem = memSize;

  uint64_t bitmapSize = memSize / 4096 / 8 + 1;

  bitmapInit(bitmapSize, largestSeg);

  locks(pageBitmap.buffer, pageBitmap.size / 4096 + 1);

  reserves(0, memSize / 4096 + 1);
  for (uint64_t i = 0; i < entries; i++) {
    EFI_MEMORY_DESCRIPTOR* desc = (EFI_MEMORY_DESCRIPTOR*)((uint64_t)map + (i * descSize));
    if (desc->type == 7) {
      releases(desc->physAddr, desc->pages);
    } 
  }
}

void pageAllocator::bitmapInit(size_t size, void* bufferAddr) {
  pageBitmap.size = size;
  pageBitmap.buffer = (uint8_t*)bufferAddr;

  for (uint32_t i = 0; i < size; i++) {
    *(uint8_t*)(pageBitmap.buffer + i) = 0;
  }
}


/*
** Finds and locks a free page in memory and returns it's address.
*/
uint64_t pageI;
void* pageAllocator::getPage() {
  for (; pageI < pageBitmap.size * 8 - 1; pageI++) {
    if (pageBitmap[pageI] == true) continue;
    lock((void*)(pageI * 4096));
    return (void*)(pageI * 4096);
  }
  
  pageI = 0;
  for (; pageI < pageBitmap.size * 8 - 1; pageI++) {
    if (pageBitmap[pageI] == true) continue;
    lock((void*)(pageI * 4096));
    return (void*)(pageI * 4096);
  }
  basicRender.cls();
  cur.reset();
  basicRender.printString( "[ ", 0xffffffff);
  basicRender.printString( "PANIC", 0xffff0000);
  basicRender.printString( " ] ", 0xffffffff);
  basicRender.printString("Out of RAM", 0xffffffff);
  for (;;) asm volatile("hlt");
  return NULL; // TODO swap ram
}

/*
** Frees the page at `addr`.
*/
void pageAllocator::free(void* addr) {
  uint64_t index = (uint64_t)addr / 4096;
  if (pageBitmap[index] == false) return;
  if (pageBitmap.set(index, false)) {
    if (pageI > index) pageI = index;
    freeMem += 4096;
    usedMem -= 4096;
  }
}

/*
** Frees `count` amount of pages starting at address `addr`.
*/
void pageAllocator::frees(void* addr, uint64_t count) {
  for (int i = 0; i < count; i++) {
    free((void*)((uint64_t)addr + (i * 4096)));
  }
}

/*
** Locks the page at `addr`.
*/
void pageAllocator::lock(void* addr) {
  uint64_t index = (uint64_t)addr / 4096;
  if (pageBitmap[index] == true) return;
  if (pageBitmap.set(index, true)) {
    freeMem -= 4096;
    usedMem += 4096;
  }
}

/*
** Locks `count` amount of pages starting at address `addr`.
*/
void pageAllocator::locks(void* addr, uint64_t count) {
  for (uint64_t i = 0; i < count; i++) {
    lock((void*)((uint64_t)addr + (i * 4096)));
  }
}

uint64_t pageAllocator::getFreeMem() {
  return freeMem;
}

uint64_t pageAllocator::getUsedMem() {
  return usedMem;
}

uint64_t pageAllocator::getResdMem() {
  return reservedMem;
}

/*
** Frees the reserved page at `addr`.
*/
void pageAllocator::release(void* addr) {
  uint64_t index = (uint64_t)addr / 4096;
  if (pageBitmap[index] == false) return;
  if (pageBitmap.set(index, false)) {
    if (pageI > index) pageI = index;
    freeMem += 4096;
    reservedMem -= 4096;
  }
}

/*
** Frees `count` amount of reserved pages starting at address `addr`.
*/
void pageAllocator::releases(void* addr, uint64_t count) {
  for (int i = 0; i < count; i++) {
    release((void*)((uint64_t)addr + (i * 4096)));
  }
}

/*
** Reserves the page at `addr`.
*/
void pageAllocator::reserve(void* addr) {
  uint64_t index = (uint64_t)addr / 4096;
  if (pageBitmap[index] == true) return;
  if (pageBitmap.set(index, true)) {
    freeMem -= 4096;
    reservedMem += 4096;
  }
}

/*
** Reserves `count` amount of pages starting at address `addr`.
*/
void pageAllocator::reserves(void* addr, uint64_t count) {
  for (int i = 0; i < count; i++) {
    reserve((void*)((uint64_t)addr + (i * 4096)));
  }
}