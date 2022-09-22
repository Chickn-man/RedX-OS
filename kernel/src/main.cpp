#include "main.h"

extern uint64_t _KernelStart;
extern uint64_t _KernelEnd;

extern "C" void _start (KernelParameters *parameters) {
    screen.doubleBuffer = parameters->buffer;

    cur.xm = roundd(parameters->buffer->ppsl / 8) - 1;
    cur.ym = roundd(parameters->buffer->Height / 16) - 1; 

    basicRender.font = parameters->font;
    basicRender.buffer = parameters->buffer;

    void *stack;
    asm volatile ("mov %%rsp, %0" :: "rw" (stack) : "memory");

    uint64_t mapEntries = parameters->mapSize / parameters->descSize;

    for (int i = 0; i < mapEntries; i++) { //debug
        EFI_MEMORY_DESCRIPTOR* desc = (EFI_MEMORY_DESCRIPTOR*)((uint64_t)parameters->map + (i * parameters->descSize));
        basicRender.print(toString((uint64_t)i), 0xFFFFFFFF);
        basicRender.print(", ", 0xFF0000FF);
        basicRender.print(EFI_MEMORY_TYPE_STRINGS[desc->type], 0xFFFFFFFF);
        basicRender.print(", ", 0xFF0000FF);
        basicRender.print(toString((uint64_t)desc->pages), 0xFFFFFFFF);
        basicRender.print(" | ", 0xFFFF0000);
    }

    cur.newLine();

    int e;
    for (e = 0; e < mapEntries; e++) { //debug
        EFI_MEMORY_DESCRIPTOR* desc = (EFI_MEMORY_DESCRIPTOR*)((uint64_t)parameters->map + (e * parameters->descSize));
        if ((uint64_t)stack >= (uint64_t)desc->physAddr) {
            if ((uint64_t)stack <= (uint64_t)desc->physAddr + (desc->pages * 0x1000)) {
                basicRender.printString("--Stack--", 0xFF0000FF);
                cur.newLine();
                basicRender.printString("Stack addr: ", 0xFFFFFFFF);
                basicRender.printString(toHString((uint64_t)stack), 0xFFFFFFFF);
                cur.newLine();
                basicRender.printString("Entry #: ", 0xFFFFFFFF);
                basicRender.printString(toString((uint64_t)e), 0xFFFFFFFF);
                cur.newLine();
                goto FoundStack;
            }
        }
        if ((uint64_t)stack >= (uint64_t)desc->virtAddr) {
            if ((uint64_t)stack <= (uint64_t)desc->virtAddr + (desc->pages * 0x1000)) {
                basicRender.printString("--Stack--", 0xFF0000FF);
                cur.newLine();
                basicRender.printString("Stack addr: ", 0xFFFFFFFF);
                basicRender.printString(toHString((uint64_t)stack), 0xFFFFFFFF);
                cur.newLine();
                basicRender.printString("Entry #: ", 0xFFFFFFFF);
                basicRender.printString(toString((uint64_t)e), 0xFFFFFFFF);
                cur.newLine();
                goto FoundStack;
            }
        }
    }

    basicRender.printString("Did not find stack in efi memory map.", 0xFFFF0000);
    cur.newLine();
    panic("Could not find stack");

FoundStack:

    EFI_MEMORY_DESCRIPTOR *stackEntry = (EFI_MEMORY_DESCRIPTOR*)((uint64_t)parameters->map + (e * parameters->descSize));

    basicRender.printString("Type: ", 0xFFFFFFFF);
    basicRender.printString(EFI_MEMORY_TYPE_STRINGS[stackEntry->type], 0xFFFFFFFF);
    cur.newLine();

    basicRender.printString("Pages: ", 0xFFFFFFFF);
    basicRender.printString(toString((uint64_t)stackEntry->pages), 0xFFFFFFFF);
    cur.newLine();

    basicRender.printString("Start address: ", 0xFFFFFFFF);
    basicRender.printString(toHString((uint64_t)stackEntry->physAddr), 0xFFFFFFFF);
    cur.newLine();

    basicRender.printString("End address: ", 0xFFFFFFFF);
    basicRender.printString(toHString((uint64_t)stackEntry->physAddr + stackEntry->pages * 0x1000), 0xFFFFFFFF);
    cur.newLine();

    basicRender.printString("Virtual address: ", 0xFFFFFFFF);
    basicRender.printString(toHString((uint64_t)stackEntry->virtAddr), 0xFFFFFFFF);
    cur.newLine();

    gdtInit();

    memoryInit(parameters, stackEntry);

    interuptInit();

    welcome();

    cur.reset();

    for (;;) {
        //terminal.handler();

        if (!scheduler.work) {asm volatile ("hlt");}
    }
}