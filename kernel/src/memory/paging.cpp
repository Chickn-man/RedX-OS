#include "paging.h"

uint8_t __stack_chk_fail = 0;

void dirEntry::set(PTF flag, bool B) {
  uint64_t bit = (uint64_t)1 << flag;
  V &= ~bit;
  if (B) {
    V |= bit;
  }
}

bool dirEntry::get(PTF flag) {
  uint64_t bit = (uint64_t)1 << flag;
  return V & bit > 0 ? true : false;
}

void dirEntry::setAddr(uint64_t address) {
  address &= 0x000000ffffffffff;
  V &= 0xfff0000000000fff;
  V |= address << 12;
}

uint64_t dirEntry::getAddr() {
  return (V & 0x000ffffffffff000) >> 12;
}

pMapIndexer::pMapIndexer(uint64_t virtAddr) {
  virtAddr = (uint64_t)pageAlign((void*)virtAddr);
  virtAddr >>= 12;
  P_i = virtAddr & 0x1ff;
  virtAddr >>= 9;
  PT_i = virtAddr & 0x1ff;
  virtAddr >>= 9;
  PD_i = virtAddr & 0x1ff;
  virtAddr >>= 9;
  PDP_i = virtAddr & 0x1ff;
}

pTableMan::pTableMan(table* LFAD) {
  this->PML4 = LFAD;
}

void pTableMan::map(void* virtualMemory, void* physicalMemory){
  pMapIndexer indexer = pMapIndexer((uint64_t)virtualMemory);
  dirEntry PDE;

  PDE = PML4->entries[indexer.PDP_i];
  table* PDP;
  if (!PDE.get(PTF::P)){
    PDP = (table*)allocator.getPage();
    set(PDP, 0, 0x1000);
    PDE.setAddr( (uint64_t)PDP >> 12);
    PDE.set(PTF::P, true);
    PDE.set(PTF::RW, true);
    PML4->entries[indexer.PDP_i] = PDE;
  }
  else
  {
    PDP = (table*)((uint64_t)PDE.getAddr() << 12);
  }
  
  
  PDE = PDP->entries[indexer.PD_i];
  table* PD;
  if (!PDE.get(PTF::P)){
    PD = (table*)allocator.getPage();
    set(PD, 0, 0x1000);
    PDE.setAddr((uint64_t)PD >> 12);
    PDE.set(PTF::P, true);
    PDE.set(PTF::RW, true);
    PDP->entries[indexer.PD_i] = PDE;
  }
  else
  {
    PD = (table*)((uint64_t)PDE.getAddr() << 12);
  }

  PDE = PD->entries[indexer.PT_i];
  table* PT;
  if (!PDE.get(PTF::P)){
    PT = (table*)allocator.getPage();
    set(PT, 0, 0x1000);
    PDE.setAddr((uint64_t)PT >> 12);
    PDE.set(PTF::P, true);
    PDE.set(PTF::RW, true);
    PD->entries[indexer.PT_i] = PDE;
  }
  else
  {
    PT = (table*)((uint64_t)PDE.getAddr() << 12);
  }

  PDE = PT->entries[indexer.P_i];
  PDE.setAddr((uint64_t)physicalMemory >> 12);
  PDE.set(PTF::P, true);
  PDE.set(PTF::RW, true);
  PT->entries[indexer.P_i] = PDE;
}

void* VAI;
void* pTableMan::getPool(uint64_t size) {
  void* ret = VAI;
  uint32_t pages = (uint32_t)((size / 0x1000) + 1);
  for (uint32_t i = 0; i < pages; i++) {
    map(VAI, allocator.getPage());
    //basicRender.printString(toString((double)(uint64_t)VAI / 4096, 3), 0xffffffff);
    cur.newLine();
    VAI = (void*)((uint64_t)VAI + 0x1000);
  }
  VAI = (void*)((uint64_t)VAI + 0x1000);
  return ret;
}