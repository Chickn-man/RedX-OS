#include "kernelFunc.h"

extern uint64_t _KernelStart;
extern uint64_t _KernelEnd;

void gdtInit() {
    GDTDesc gdtDesc;
    gdtDesc.size = sizeof(GDT) - 1;
    gdtDesc.offset = (uint64_t)&globalDescrptorTable;
    loadGDT(&gdtDesc);
}

void memoryInit(KernelParameters *params) {
    // Initialize allocator
    allocator = pageAllocator();
    allocator.readMap(params->map, params->mapSize, params->descSize);

    // Lock kernel pages so the page map doesnt overwrite it again.
    uint64_t kernelSize = (uint64_t)&_KernelEnd - (uint64_t)&_KernelStart;
    uint64_t kernelPages = (uint64_t)(kernelSize / 0x1000) + 1;
    allocator.locks(&_KernelStart, kernelPages + 1);
    allocator.locks(params->stack, params->stackSize / 0x1000 + 1);
    allocator.locks(params->pageDirectoryAddress, params->pageDirectorySize / 0x1000 + 1);
    allocator.locks(params->buffer->BaseAddr, params->buffer->Size / 0x1000 + 1);
    allocator.lock(params);
    allocator.lock(params->buffer);
    allocator.lock(params->font);
    allocator.lock(params->font->buffer);
    allocator.lock(params->font->header);

    // Initialize page table manager
    VAI = pageAlign((void*)(getMemorySize(params->map, params->mapSize / params->descSize, params->descSize) + params->buffer->Size));
    pageTableMan.init(params->pageDirectoryAddress, params->pageDirectorySize);

    /* Map Memory */

    // poopy fix 
    /*uint64_t mapEntries = params->mapSize / params->descSize;
    for (uint64_t i = 0; i < getMemorySize(params->map, mapEntries, params->descSize); i += 0x1000) {
        pageTableMan.map((void*)i, (void*)i);
    }*/

    // kernel
    for (uint64_t i = (uint64_t)&_KernelStart; i < (uint64_t)&_KernelStart + kernelSize + 0x1000; i += 0x1000) {
        pageTableMan.map((void*)(/*0xFFFF800000000000 + */i), (void*)i);
    }

    // stack
    for (uint64_t i = (uint64_t)params->stack; i < (uint64_t)params->stack + params->stackSize + 0x1000; i += 0x1000) {
        pageTableMan.map((void*)i, (void*)i);
    }

    // page directory
    for (uint64_t i = (uint64_t)params->pageDirectoryAddress; i < (uint64_t)params->pageDirectoryAddress + params->pageDirectorySize; i += 0x1000) {
        pageTableMan.map((void*)i, (void*)i);
    }

    // page bitmap
    for (uint64_t i = (uint64_t)allocator.pageBitmap.buffer; i < (uint64_t)allocator.pageBitmap.buffer + allocator.pageBitmap.size + 0x1000; i += 0x1000) {
        pageTableMan.map((void*)i, (void*)i);
    }

    // framebuffer
    for (uint64_t i = (uint64_t)params->buffer->BaseAddr; i < (uint64_t)params->buffer->BaseAddr + (uint64_t)params->buffer->Size + 0x1000; i += 0x1000) {
        pageTableMan.map((void*)i, (void*)i);
    }

    pageTableMan.map(params, params);
    pageTableMan.map(params->buffer, params->buffer);
    pageTableMan.map(params->font, params->font);
    pageTableMan.map(params->font->buffer, params->font->buffer);
    pageTableMan.map(params->font->header, params->font->header);

    asm volatile ("mov %0, %%cr3" :: "r" (params->pageDirectoryAddress) : "memory");

}

void interuptInit() {
    idtr.limit = 0x0fff;
    idtr.offset = (uint64_t)pageTableMan.getPool(0x1000);

    setGate((void*)pageFaultHandler, 0xe, intr, 0x08);
    setGate((void*)doubleFaultHandler, 0x8, intr, 0x08);
    setGate((void*)genProcFaultHandler, 0xd, intr, 0x08);
    setGate((void*)keyboardHandler, 0x21, intr, 0x08);

    asm ("lidt %0" : : "m" (idtr));

    remapPIC();
    outb(PIC1_DATA, 0b11111001);
    outb(PIC2_DATA, 0b11101111);

    asm ("sti");
}

void welcome() {
    basicRender.print("Welcome!", 0xFFFFFFFF);
    for (uint8_t i = 0; i < 51; i++) {screen.pixel(screen.doubleBuffer->Width + i - 50, i, 0xFFFF0000);}
    for (uint8_t i = 0; i < 51; i++) {screen.pixel(screen.doubleBuffer->Width - i, i, 0xFFFF0000);}
}

void panic(const char *message) {
    basicRender.rect(0, 0, 200, 16, 0x00000000);
    cur.reset();
    basicRender.printString( "[ ", 0xffffffff);
    basicRender.printString( "PANIC", 0xffff0000);
    basicRender.printString( " ] ", 0xffffffff);
    basicRender.printString(message, 0xffffffff);
    for (;;) {asm volatile ("hlt");}
}

void panic(char *message) {
    basicRender.rect(0, 0, 800, 16, 0x00000000);
    cur.reset();
    basicRender.printString( "[ ", 0xffffffff);
    basicRender.printString( "PANIC", 0xffff0000);
    basicRender.printString( " ] ", 0xffffffff);
    basicRender.printString(message, 0xffffffff);
    for (;;) {asm volatile ("hlt");}
}