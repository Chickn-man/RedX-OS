#pragma once
#include <stdint.h>
#include "../kernelFunc.h"
#include "../hid/io.h"
#include "../hid/keyboard.h"

#define intr      0b10001110
#define call      0b10001100
#define trap      0b10001111

#define PIC1_COMMAND 0x20
#define PIC1_DATA 0x21
#define PIC2_COMMAND 0xA0
#define PIC2_DATA 0xA1
#define PIC_EOI 0x20

#define ICW1_INIT 0x10
#define ICW1_ICW4 0x01
#define ICW4_8086 0x01

struct IDTEntry {
  uint16_t offset0;
  uint16_t selector;
  uint8_t ist;
  uint8_t type_attr;
  uint16_t offset1;
  uint32_t offset2;
  uint32_t ignore;
  void setOffset(uint64_t offset);
  uint64_t getOffset();
};

struct IDTR {
  uint16_t limit;
  uint64_t offset;
}__attribute__((packed));

struct interruptFrame;
__attribute__((interrupt)) void pageFaultHandler(struct interruptFrame* frame);
__attribute__((interrupt)) void doubleFaultHandler(struct interruptFrame* frame);
__attribute__((interrupt)) void genProcFaultHandler(struct interruptFrame* frame);
extern uint8_t scancode;
__attribute__((interrupt)) void keyboardHandler(struct interruptFrame* frame);
extern uint8_t mData;
__attribute__((interrupt)) void mouseHandler(struct interruptFrame* frame);

extern IDTR idtr;
void setGate(void* handler, uint8_t offset, uint8_t type_attr, uint8_t selector);

void remapPIC();
void endMaster();
void endSlave();