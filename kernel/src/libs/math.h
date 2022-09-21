#pragma once
#include <stdint.h>
#include <stddef.h>

double sqrt(double number);
double rand();
int roundd(double x);

class bitmap {
  public:
  size_t size;
  uint8_t* buffer;
  bool operator[](uint64_t index);
  bool set(uint64_t index, bool val);
};