#include "kernelFunc.h"

extern uint64_t _KernelStart;
extern uint64_t _KernelEnd;

void gdtInit() {
    GDTDesc gdtDesc;
    gdtDesc.size = sizeof(GDT) - 1;
    gdtDesc.offset = (uint64_t)&globalDescrptorTable;
    loadGDT(&gdtDesc);
}

pTableMan pageTableMan = NULL;
void memoryInit(KernelParameters *params) {
    // Initialize allocator
    allocator = pageAllocator();
    allocator.readMap(params->map, params->mapSize, params->descSize);

    // Initialize page table manager
    table* PML4 = (table*)allocator.getPage();
    set(PML4, 0, 0x1000);
    pageTableMan = pTableMan(PML4);

    /* Map Memory */

    // kernel
    uint64_t kernelSize = (uint64_t)&_KernelEnd - (uint64_t)&_KernelStart;
    uint64_t kernelPages = (uint64_t)(kernelSize / 4096) + 1;
    allocator.locks(&_KernelStart, kernelPages);
    for (uint64_t i = (uint64_t)&_KernelStart + kernelSize + 0x1000; i < kernelSize + 0x1000; i += 0x1000) {
        pageTableMan.map((void*)(/*0xFFFF800000000000*/ + i), (void*)i);
    }

    // stack


    // framebuffer
    allocator.locks(params->buffer->BaseAddr, ((params->buffer->Size + 0x1000) / 0x1000) + 1);
    for (uint64_t i = (uint64_t)params->buffer->BaseAddr; i < (uint64_t)params->buffer->BaseAddr + (uint64_t)params->buffer->Size + 0x1000; i += 0x1000) {
        pageTableMan.map((void*)i, (void*)i);
    }

    asm ("mov %0, %%cr3" : : "r" (PML4));
}

void interuptInit() {
    idtr.limit = 0x0fff;
    idtr.offset = (uint64_t)allocator.getPage();

    setGate((void*)pageFaultHandler, 0xe, intr, 0x08);
    setGate((void*)doubleFaultHandler, 0x8, intr, 0x08);
    setGate((void*)genProcFaultHandler, 0xd, intr, 0x08);
    setGate((void*)keyboardHandler, 0x21, intr, 0x08);

    asm ("lidt %0" : : "m" (idtr));

    remapPIC();
    outb(PIC1_DATA, 0b11111001);
    outb(PIC2_DATA, 0b11101111);
}

void welcome() {
    for (uint8_t i = 0; i < 51; i++) {screen.pixel(screen.doubleBuffer->Width + i - 50, i, 0xFFFF0000);}
    for (uint8_t i = 0; i < 51; i++) {screen.pixel(screen.doubleBuffer->Width - i, i, 0xFFFF0000);}
}

void panic(const char *message) {
    for (;;) {asm volatile ("hlt");}
}

void panic(char *message) {
    basicRender.rect(0, 0, 200, 16, 0x00000000);
    cur.reset();
    basicRender.printString( "[ ", 0xffffffff);
    basicRender.printString( "PANIC", 0xffff0000);
    basicRender.printString( " ] ", 0xffffffff);
    basicRender.printString(message, 0xffffffff);
    for (;;) {asm volatile ("hlt");}
}