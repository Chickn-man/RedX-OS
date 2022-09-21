#include "mouse.h"

void mWait() {
  uint32_t timeout = 100000;
  while (timeout--) {
    if ((inb(0x64) & 0b10) == 0) {
      return;
    }
  }
}

void mWaitIn() {
  uint32_t timeout = 100000;
  while (timeout--) {
    if (inb(0x64) & 0b1) {
      return;
    }
  }
}

void mWrite(uint8_t V) {
  mWait();
  outb(0x64, 0xd4);
  mWait();
  outb(0x60, V);
}

uint8_t mRead() {
  mWaitIn();
  return inb(0x60);
}

uint8_t mCycle = 0;
uint8_t mPacket[4];
bool mReady = false;
cursor mouse;
void handleMouse(uint8_t data) {
  switch (mCycle) {
    case 0:
      if (mReady) break;
      if (data & 0b00001000 == 0) break;
      mPacket[0] = data;
      mCycle++;
      break;
    case 1:
      if (mReady) break;
      mPacket[1] = data;
      mCycle++;
      break;
    case 2:
      if (mReady) break;
      mPacket[2] = data;
      mReady = true;
      mCycle = 0;
      break;
  }
}

void processMPacket() {
  if (!mReady) return;

  bool xNeg, yNeg, xOFlow, yOFlow;
  
  int32_t xMov, yMov;
  
  if (mPacket[0] & XSign) {
    xNeg = true;
  } else xNeg = false;
  if (mPacket[0] & YSign) {
    yNeg = true;
  } else yNeg = false;
  if (mPacket[0] & XOFlow) {
    xOFlow = true;
  } else xOFlow = false;
  if (mPacket[0] & YOFlow) {
    yOFlow = true;
  } else yOFlow = false;

  if (!xNeg) {
    xMov = mPacket[1];
    xMov += 255 * xOFlow;
  } else {
    xMov = -(256 - mPacket[1]);
    xMov -= 255 * xOFlow;
  }
  if (!yNeg) {
    yMov = mPacket[2];
    yMov += 255 * yOFlow;
  } else {
    yMov = -(256 - mPacket[2]);
    yMov -= 255 * yOFlow;
  }

  mouse.move(xMov, -yMov);

  mReady = false;
}

void ps2MouseInit() {
  outb(0x64, 0xa8); // enable device - mouse

  mWait();
  outb(0x64, 0x20);
  mWaitIn();

  uint8_t stat = inb(0x60);

  stat |= 0b10;

  mWait();
  outb(0x64, 0x60);
  mWait();
  outb(0x60, stat);

  mWrite(0xf6);
  mRead();

  mWrite(0xf4);
  mRead();


}