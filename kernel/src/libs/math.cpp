#include "math.h"

// sqare root
double sqrt(double number) {
  double temp = 0;
  double root = number / 2;
  int i = 0;
  while (root != temp && i < 10000) {
    temp = root;
    root = (number / temp + temp) / 2;
    i++;
  }
  return root;
}

/*
** returns a random number between 1 and 0
*/
double rand() {
  return 0;
}

/*
** Rounds x to the nearest integer.
*/

int roundd(double x) {
  if (x < 0.0)
    return (int)(x - 0.5);
  else
    return (int)(x + 0.5);
}

bool bitmap::operator[](uint64_t index){
  if (index > size * 8) return false;
  uint64_t byteIndex = index / 8;
  uint8_t bitIndex = index % 8;
  uint8_t bitIndexer = 0b10000000 >> bitIndex;
  if ((buffer[byteIndex] & bitIndexer) > 0){
    return true;
  }
  return false;
}

bool bitmap::set(uint64_t index, bool value){
  if (index > size * 8) return false;
  uint64_t byteIndex = index / 8;
  uint8_t bitIndex = index % 8;
  uint8_t bitIndexer = 0b10000000 >> bitIndex;
    buffer[byteIndex] &= ~bitIndexer;
  if (value){
    buffer[byteIndex] |= bitIndexer;
  }
  return true;
}