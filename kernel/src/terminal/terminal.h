#pragma once

#include <stddef.h>
#include "../libs/io.h"
#include "../scheduler/scheduler.h"
#include "../memory/paging.h"

class Terminal {
    private:
    char *commandBuffer;
    int runCommand(char *commandString);
    bool work;

    public:
    size_t bufferSize;
    size_t width;
    size_t height;
    char *textBuffer;
    char *in;
    char *out;
    void init(Framebuffer *frameBuffer); // allocate buffers
    void handler(); // Handle in and out
};

extern Terminal terminal;