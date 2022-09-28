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
  address &= 0x000000fffffff000;
  V &= 0xfff0000000000fff;
  V |= address;
}

uint64_t dirEntry::getAddr() {
  return (V & 0x000ffffffffff000);
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

void pTableMan::init(void *directoryAddress, size_t directorySize) {
  this->dirAddr = (table *)directoryAddress;
  this->dirSize = directorySize;
}

int PDI = 0;
void *pTableMan::allocateEntry() {
  PDI++;
  if (PDI > ((uint64_t)dirAddr + dirSize) / 0x1000) {
    // call panic interupt, msg = "page directory full"
    return NULL;
  }
  return (void *)((uint64_t)dirAddr + (PDI * 0x1000));
}

void pTableMan::map(void* virtualMemory, void* physicalMemory){
  if (!allocator.pageBitmap[(uint64_t)pageAlign(physicalMemory) / 0x1000]) {
    basicRender.print("Mapped unlocked page ", 0xFFFFFFFF);
    basicRender.print(toHString((uint64_t)physicalMemory), 0xFFFFFFFF);
    basicRender.print(" to ", 0xFFFFFFFF);
    basicRender.print(toHString((uint64_t)virtualMemory), 0xFFFFFFFF);
    basicRender.print(",", 0xFF0000FF);
  }
  pMapIndexer indexer = pMapIndexer((uint64_t)virtualMemory);
  dirEntry PDE;

  PDE = dirAddr->entries[indexer.PDP_i];
  table* PDP;
  if (!PDE.get(PTF::P)){
    PDP = (table*)allocateEntry();
    set(PDP, 0, 0x1000);
    PDE.setAddr((uint64_t)PDP);
    PDE.set(PTF::P, true);
    PDE.set(PTF::RW, true);
    dirAddr->entries[indexer.PDP_i] = PDE;
  }
  else
  {
    PDP = (table*)((uint64_t)PDE.getAddr());
  }
  
  
  PDE = PDP->entries[indexer.PD_i];
  table* PD;
  if (!PDE.get(PTF::P)) {
    PD = (table*)allocateEntry();
    set(PD, 0, 0x1000);
    PDE.setAddr((uint64_t)PD);
    PDE.set(PTF::P, true);
    PDE.set(PTF::RW, true);
    PDP->entries[indexer.PD_i] = PDE;
  }
  else
  {
    PD = (table*)((uint64_t)PDE.getAddr());
  }

  PDE = PD->entries[indexer.PT_i];
  table* PT;
  if (!PDE.get(PTF::P)){
    PT = (table*)allocateEntry();
    set(PT, 0, 0x1000);
    PDE.setAddr((uint64_t)PT);
    PDE.set(PTF::P, true);
    PDE.set(PTF::RW, true);
    PD->entries[indexer.PT_i] = PDE;
  }
  else
  {
    PT = (table*)((uint64_t)PDE.getAddr());
  }

  PDE = PT->entries[indexer.P_i];
  PDE.setAddr((uint64_t)physicalMemory);
  PDE.set(PTF::P, true);
  PDE.set(PTF::RW, true);
  PT->entries[indexer.P_i] = PDE;

  /*basicRender.print(toHString((uint64_t)physicalMemory), 0xFFFFFFFF);
  basicRender.print(",", 0xFFFFFFFF);
  basicRender.print(toHString((uint64_t)PDE.getAddr()), 0xFFFFFFFF);
  basicRender.print("|", 0xFF0000FF);*/
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

pTableMan pageTableMan;