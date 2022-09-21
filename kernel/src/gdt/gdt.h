#pragma once
#include <stdint.h>

struct GDTDesc {
  uint16_t size;
  uint64_t offset;
}__attribute__((packed));

struct GDTEntry {
  uint16_t limit0;
  uint16_t base0;
  uint8_t base1;
  uint8_t accessByte;
  uint8_t limit1;
  uint8_t base2;
}__attribute__((packed));

struct GDT {
  GDTEntry null;
  GDTEntry kCode;
  GDTEntry kData;
  GDTEntry uNull;
  GDTEntry uCode;
  GDTEntry uData;
}__attribute__((packed)) __attribute__((aligned(0x1000)));

extern GDT globalDescrptorTable;

extern "C" void loadGDT(GDTDesc* desc);