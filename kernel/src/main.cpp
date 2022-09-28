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

    basicRender.printString("Stack addr: ", 0xFFFFFFFF);
    basicRender.printString(toHString((uint64_t)parameters->stack), 0xFFFFFFFF);
    cur.newLine();
        
    gdtInit();

    memoryInit(parameters);

    interuptInit();

    welcome();

    cur.reset();

    terminalRenderer.font = parameters->font;

    terminal.init(parameters->buffer);

    basicRender.cls();

    for (;;) {
        terminal.handler();

        terminalRenderer.render();

        if (!scheduler.work) {asm volatile ("hlt");}
    }
}