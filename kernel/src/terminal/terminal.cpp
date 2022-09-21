#include "terminal.h"

void Terminal::init(Framebuffer *frameBuffer) {
    this->width = frameBuffer->Width / 8 - 1;
    this->height = frameBuffer->Height / 16 - 1;
    this->bufferSize = this->width * this->height;

    textBuffer = (char *)/*malloc*/(this->bufferSize);
    commandBuffer = (char *)/*malloc*/(this->bufferSize);
    in = (char *)/*malloc*/(this->bufferSize);
    out = (char *)/*malloc*/(this->bufferSize);
    
}

bool work = false;
int ii = 0;
int io = 0;
void Terminal::handler() {
    work = false;
    int i;
    if (in[ii]) {
        work = true;
        for (i = 0; textBuffer[i] != NULL; i++) {}
        textBuffer[i] = in[ii];
        if (in[ii] == '\n') {
            i++;
            for (; i < this->width; i++) {textBuffer[i] = ' ';}
            switch (runCommand(commandBuffer)) {
            case 1:
                break;
            case 2:
                print("Shell: error, no such file or command");
            default:
                break;
            }
        }
        ii++;
    } else {ii = 0;}
    if (out[io]) {
        work = true;
        for (i = 0; textBuffer[i] != NULL; i++) {}
        textBuffer[i] = out[io];
        if (out[io] == '\n') {
            i++;
            for (; i < this->width; i++) {textBuffer[i] = ' ';}
        }
        io++;
    } else {io = 0;}
    scheduler.work |= work;
    this->work = work;
}

int Terminal::runCommand(char *commandString) {
    return 2;
}

Terminal terminal;