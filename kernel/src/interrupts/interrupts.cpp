#include "interrupts.h"

void IDTEntry::setOffset(uint64_t offset){
    offset0 = (uint16_t)(offset & 0x000000000000ffff);
    offset1 = (uint16_t)((offset & 0x00000000ffff0000) >> 16);
    offset2 = (uint32_t)((offset & 0xffffffff00000000) >> 32);
}

uint64_t IDTEntry::getOffset(){ 
    uint64_t offset = 0;
    offset |= (uint64_t)offset0;
    offset |= (uint64_t)offset1 << 16;
    offset |= (uint64_t)offset2 << 32;
    return offset;
}
__attribute__((interrupt)) void pageFaultHandler(struct interruptFrame* frame) {
  panic("Page Fault");
  
  //halt cpu
  for (;;) {
    asm volatile("hlt");
  }
}

__attribute__((interrupt)) void doubleFaultHandler(struct interruptFrame* frame) {
  panic("Double Fault");

  //halt cpu
  for (;;) {
    asm volatile("hlt");
  }
}

__attribute__((interrupt)) void genProcFaultHandler(struct interruptFrame* frame) {
  panic("General Protection Fault");

  //halt cpu
  for (;;) {
    asm volatile("hlt");
  }
}

uint8_t scancode;
__attribute__((interrupt)) void keyboardHandler(interruptFrame* frame) {
  scancode = inb(0x60);
  
  handleKeyboard(scancode);

  endMaster();
}

IDTR idtr;
void setGate(void* handler, uint8_t offset, uint8_t type_attr, uint8_t selector) {
  IDTEntry* interrupt = (IDTEntry*)(idtr.offset + offset * sizeof(IDTEntry));
  interrupt->setOffset((uint64_t)handler);
  interrupt->type_attr = intr;
  interrupt->selector = 0x08;
}

void remapPIC(){
    uint8_t a1, a2; 

    a1 = inb(PIC1_DATA);
    ioWait();
    a2 = inb(PIC2_DATA);
    ioWait();

    outb(PIC1_COMMAND, ICW1_INIT | ICW1_ICW4);
    ioWait();
    outb(PIC2_COMMAND, ICW1_INIT | ICW1_ICW4);
    ioWait();

    outb(PIC1_DATA, 0x20);
    ioWait();
    outb(PIC2_DATA, 0x28);
    ioWait();

    outb(PIC1_DATA, 4);
    ioWait();
    outb(PIC2_DATA, 2);
    ioWait();

    outb(PIC1_DATA, ICW4_8086);
    ioWait();
    outb(PIC2_DATA, ICW4_8086);
    ioWait();

    outb(PIC1_DATA, a1);
    ioWait();
    outb(PIC2_DATA, a2);
}

void endMaster() {
  outb(PIC1_COMMAND, PIC_EOI);
}

void endSlave() {
  outb(PIC2_COMMAND, PIC_EOI);
  outb(PIC1_COMMAND, PIC_EOI);
}