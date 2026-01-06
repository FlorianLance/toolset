
/*******************************************************************************
** Toolset-ts-demos                                                           **
** MIT License                                                                **
** Copyright (c) [2018] [Florian Lance]                                       **
**                                                                            **
** Permission is hereby granted, free of charge, to any person obtaining a    **
** copy of this software and associated documentation files (the "Software"), **
** to deal in the Software without restriction, including without limitation  **
** the rights to use, copy, modify, merge, publish, distribute, sublicense,   **
** and/or sell copies of the Software, and to permit persons to whom the      **
** Software is furnished to do so, subject to the following conditions:       **
**                                                                            **
** The above copyright notice and this permission notice shall be included in **
** all copies or substantial portions of the Software.                        **
**                                                                            **
** THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR **
** IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,   **
** FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL    **
** THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER **
** LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING    **
** FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER        **
** DEALINGS IN THE SOFTWARE.                                                  **
**                                                                            **
********************************************************************************/


#pragma once

// imgui
#include "imgui/extra/implot/implot.h"

// local
#include "windows/base_sfml_gl_window.hpp"
#include "biopac.hpp"

namespace tool::graphics {

class BiopacControlWindow : public BaseSfmlGlWindow{

public:

    BiopacControlWindow(std::string_view title, geo::Screen screen, std::optional<sf::ContextSettings> context) :
          BaseSfmlGlWindow(title, screen, context){

        std::fill(std::begin(serialBuffer), std::end(serialBuffer), ' ');
        serialBuffer[0] = 'a';
        serialBuffer[1] = 'u';
        serialBuffer[2] = 't';
        serialBuffer[3] = 'o';
    }

private:

    // gl
    bool initialize_gl() override;
    void post_update() override;
    // imgui
    void draw_imgui() override;


protected:

    std::array<char, 20> serialBuffer;
    BiopacDevice biopac;
    // imgui
    ImPlotContext *imPlotContext = nullptr;
};
}
