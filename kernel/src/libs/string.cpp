#include "string.h"

/*
** Concats two strings into one.
*/
char catOutput[128];
const char* cat(const char* so, const char* st) {
  for (int i = 0; catOutput[i] != 0; i++) {
    catOutput[i] = 0;
  }

  uint8_t index = 0;
  for (char* chr = (char*)so; *chr != 0; chr++) {
    catOutput[index] = *chr;
    index++;
  }

  for (char* chr = (char*)st; *chr != 0; chr++) {
    catOutput[index] = *chr;
    index++;
  }
  
  return catOutput;
}

char uintOutput[128];
const char* toString(uint64_t number) {
  uint8_t size = 0;
  for (uint64_t sizeCheck = number; sizeCheck / 10 > 0; sizeCheck /= 10) {
    size++;
  }

  uint8_t index = 0;
  while (number / 10 > 0) {
    uint8_t remainder = number % 10;
    number /= 10;
    uintOutput[size - index] = remainder + '0';
    index++;
  }

  uint8_t remainder = number % 10;
  uintOutput[size - index] = remainder + '0';
  uintOutput[size + 1] = 0;

  return uintOutput;
}

char intOutput[128];
const char* toString(int64_t number) {
  uint8_t isNegative = 0;

  if (number < 0) {
    isNegative = 1;
    number *= -1;
    intOutput[0] = '-';
  }

  uint8_t size = 0;
  for (uint64_t sizeCheck = number; sizeCheck / 10 > 0; sizeCheck /= 10) {
    size++;
  }

  uint8_t index = 0;
  while (number / 10 > 0) {
    uint8_t remainder = number % 10;
    number /= 10;
    intOutput[size - index + isNegative] = remainder + '0';
    index++;
  }

  uint8_t remainder = number % 10;
  intOutput[size - index + isNegative] = remainder + '0';
  intOutput[size + 1 + isNegative] = 0;

  return intOutput;
}

char hexOutput64[128];
const char* toHString(uint64_t number) {
  uint64_t* valPtr = &number;
  uint8_t* ptr;
  uint8_t tmp;
  uint8_t size = 15;

  for (uint8_t i = 0; i < size; i++) {
    ptr = ((uint8_t*)valPtr + i);
    tmp = ((*ptr & 0xf0) >> 4);
    hexOutput64[size - (i * 2 + 1)] = tmp + (tmp > 9 ? 55 : '0');
    tmp = ((*ptr & 0x0f));
    hexOutput64[size - (i * 2)] = tmp + (tmp > 9 ? 55 : '0');
  }

  hexOutput64[size + 1] = 0;

  return hexOutput64;
}

char hexOutput32[128];
const char* toHString(uint32_t number) {
  uint32_t* valPtr = &number;
  uint8_t* ptr;
  uint8_t tmp;
  uint8_t size = 7;

  for (uint8_t i = 0; i < size; i++) {
    ptr = ((uint8_t*)valPtr + i);
    tmp = ((*ptr & 0xf0) >> 4);
    hexOutput32[size - (i * 2 + 1)] = tmp + (tmp > 9 ? 55 : '0');
    tmp = ((*ptr & 0x0f));
    hexOutput32[size - (i * 2)] = tmp + (tmp > 9 ? 55 : '0');
  }

  hexOutput32[size + 1] = 0;

  return hexOutput32;
}

char hexOutput16[128];
const char* toHString(uint16_t number) {
  uint16_t* valPtr = &number;
  uint8_t* ptr;
  uint8_t tmp;
  uint8_t size = 3;

  for (uint8_t i = 0; i < size; i++) {
    ptr = ((uint8_t*)valPtr + i);
    tmp = ((*ptr & 0xf0) >> 4);
    hexOutput16[size - (i * 2 + 1)] = tmp + (tmp > 9 ? 55 : '0');
    tmp = ((*ptr & 0x0f));
    hexOutput16[size - (i * 2)] = tmp + (tmp > 9 ? 55 : '0');
  }

  hexOutput16[size + 1] = 0;

  return hexOutput16;
}

char hexOutput8[128];
const char* toHString(uint8_t number) {
  uint8_t* valPtr = &number;
  uint8_t* ptr;
  uint8_t tmp;
  uint8_t size = 1;

  for (uint8_t i = 0; i < size; i++) {
    ptr = ((uint8_t*)valPtr + i);
    tmp = ((*ptr & 0xf0) >> 4);
    hexOutput8[size - (i * 2 + 1)] = tmp + (tmp > 9 ? 55 : '0');
    tmp = ((*ptr & 0x0f));
    hexOutput8[size - (i * 2)] = tmp + (tmp > 9 ? 55 : '0');
  }

  hexOutput8[size + 1] = 0;

  return hexOutput8;
}

char doubleOutput[128];
const char* toString(double number, uint8_t deciPlaces) {
  char* intPtr = (char*)toString((int64_t)number);
  char* doubPtr = doubleOutput;

  if (number < 0) {
    number *= -1;
  }

  while (*intPtr != 0) {
    *doubPtr = *intPtr;
    intPtr++;
    doubPtr++;
  }

  *doubPtr = '.';
  doubPtr++;

  double deci = number - (int)number;

  for (uint8_t i = 0; i < deciPlaces; i++) {
    deci *= 10;
    *doubPtr = (int)deci + '0';
    deci -= (int)deci;
    doubPtr++;
  }

  *doubPtr = 0;

  return doubleOutput;
}