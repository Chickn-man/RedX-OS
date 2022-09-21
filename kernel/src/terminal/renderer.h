#pragma once

#include "../main.h"

class TerminalRenderer {
    private:
    void drawChar(char chr, unsigned int x, unsigned int y, unsigned int color);

    public:
    PSF_FONT *font;
    void render();
};