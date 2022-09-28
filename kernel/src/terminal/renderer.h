#pragma once

#include "../screen.h"
#include "terminal.h"
#include "../psf.h"

class TerminalRenderer {
    private:
    void drawChar(char chr, unsigned int x, unsigned int y, unsigned int color);
    void clearChar(unsigned int x, unsigned int y, unsigned int color);

    public:
    PSF_FONT *font;
    void render();
};

extern TerminalRenderer terminalRenderer;