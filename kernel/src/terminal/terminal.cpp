#include "terminal.h"

void Terminal::init(Framebuffer *frameBuffer) {
    this->width = frameBuffer->Width / 8 - 1;
    this->height = frameBuffer->Height / 16 - 1;
    this->bufferSize = this->width * this->height;

    textBuffer = (char *)pageTableMan.getPool(this->bufferSize);
    commandBuffer = (char *)pageTableMan.getPool(this->bufferSize);
    in = (char *)pageTableMan.getPool(this->bufferSize);
    out = (char *)pageTableMan.getPool(this->bufferSize);

    for (uint64_t i = 0; i < bufferSize - 1; i++) textBuffer[i] = 0;
    for (uint64_t i = 0; i < bufferSize - 1; i++) commandBuffer[i] = 0;
    for (uint64_t i = 0; i < bufferSize - 1; i++) in[i] = 0;
    for (uint64_t i = 0; i < bufferSize - 1; i++) out[i] = 0;
    
}

bool work = false;
int ii = 0;
int io = 0;
void Terminal::handler() {
    work = false;
    int ti;
    int ci;
    if (in[ii]) {
        work = true;
        for (ti = 0; textBuffer[ti] != NULL; ti++) {}
        for (ci = 0; commandBuffer[ci] != NULL; ci++) {}
        if (in[ii] == '\n') {
            for (; textBuffer[ti - 1] != '\n' && ti % this->width; ti++) {textBuffer[ti] = ' ';}
            textBuffer[ti - 1] = '\n';
            switch (runCommand(commandBuffer)) {
                case 1:
                    break;
                case 2:
                    print("Shell: error, no such file or command\n");
                default:
                    break;
            }
            for (int i = 0; i < bufferSize || commandBuffer[i] != NULL; i++) {commandBuffer[i] = NULL;}
            ci = 0;
        } else {
            textBuffer[ti] = in[ii];
            commandBuffer[ci] = in[ii];
        }
        in[ii] = NULL;
        ii++;
    } else {ii = 0;}
    if (out[io]) {
        work = true;
        for (ti = 0; textBuffer[ti] != NULL; ti++) {}
        if (out[io] == '\n') {
            for (; textBuffer[ti - 1] != '\n' && ti % width; ti++) {textBuffer[ti] = ' ';}
            textBuffer[ti - 1] = '\n';
        } else {
            textBuffer[ti] = out[io];
        }
        out[io] = NULL;
        io++;
    } else {io = 0;}
    this->work = work;
    scheduler.work |= work;
}

int Terminal::runCommand(char *commandString) {
    return 2;
}

Terminal terminal;