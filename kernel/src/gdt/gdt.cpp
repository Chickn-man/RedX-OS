#include "gdt.h"

__attribute__((aligned(0x1000)))
GDT globalDescrptorTable = {
  {0, 0, 0, 0x00, 0x00, 0}, // null
  {0, 0, 0, 0x9a, 0xa0, 0}, // kCode
  {0, 0, 0, 0x92, 0xa0, 0}, // kData
  {0, 0, 0, 0x00, 0x00, 0}, // uNull
  {0, 0, 0, 0x9a, 0xa0, 0}, // uCode
  {0, 0, 0, 0x92, 0xa0, 0}  // uData
};