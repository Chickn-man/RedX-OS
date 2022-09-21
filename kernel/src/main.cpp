#include "main.h"

extern uint64_t _KernelStart;
extern uint64_t _KernelEnd;

extern "C" void _start (KernelParameters *parameters) {
    screen.doubleBuffer = parameters->buffer;

    cur.xm = roundd(parameters->buffer->ppsl / 8) - 1;
    cur.ym = roundd(parameters->buffer->Height / 16) - 1; 

    basicRender.font = parameters->font;
    basicRender.buffer = parameters->buffer;

    gdtInit();

    memoryInit(parameters);

    interuptInit();

    welcome();

    cur.reset();

    uint64_t mapEntries = parameters->mapSize / parameters->descSize;

    for (int i = 0; i < mapEntries; i++) { //debug
        EFI_MEMORY_DESCRIPTOR* desc = (EFI_MEMORY_DESCRIPTOR*)((uint64_t)parameters->map + (i * parameters->descSize));
    }
    
    for (;;) {
        //terminal.handler();

        if (!scheduler.work) {asm volatile ("hlt");}
    }
}