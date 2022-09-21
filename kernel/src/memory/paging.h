#pragma once
#include <stdint.h>
#include "memory.h"
#include "../basicRenderer.h"
#include "../libs/string.h"

extern uint8_t __stack_chk_fail;

enum PTF {
  P = 0,
  RW = 1,
  US = 2,
  WT = 3,
  CD = 4,
  A = 5,
  LP = 7,
  AV0 = 9,
  AV1 = 10,
  AV2 = 11,
  nx = 63
};


struct dirEntry {
  uint64_t V;
  void set(PTF flag, bool B);
  bool get(PTF flag);
  void setAddr(uint64_t address);
  uint64_t getAddr();
};

struct table {
  dirEntry entries[512];
}__attribute__((aligned(0x1000)));

class pMapIndexer {
  public:
  pMapIndexer(uint64_t virtAddr);
  uint64_t PDP_i;
  uint64_t PD_i;
  uint64_t PT_i;
  uint64_t P_i;
};

class pTableMan {
  public:
  pTableMan(table* PML4 /*level four address*/);
  table* PML4;
  void map(void* V, void* P);
  void* getPool(uint64_t size);
};

extern void* VAI;