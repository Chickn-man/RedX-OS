#pragma once
#include <stdint.h>

const char* cat(const char* so, const char* st);
const char* toString(uint64_t number);
const char* toString(int64_t number);
const char* toHString(uint64_t number);
const char* toHString(uint32_t number);
const char* toHString(uint16_t number);
const char* toHString(uint8_t number);
const char* toString(double number, uint8_t deciPlaces);