#include "io.h"

void print(char *string) {
    int o;
    for (o = 0; terminal.out[o] != NULL && o < terminal.bufferSize; o++) {}
    for (int i = 0; !string[i]; i++) {
        terminal.out[o + i] = string[i];
    }
}