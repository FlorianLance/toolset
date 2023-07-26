
#pragma once

// imgui
#include "imgui/extra/implot/implot.h"

// local
#include "windows/base_sfml_gl_window.hpp"
#include "biopac.hpp"

namespace tool::graphics {

class BiopacControlWindow : public BaseSfmlGlWindow{

public:

    BiopacControlWindow(std::string_view title, graphics::Screen screen, std::optional<sf::ContextSettings> context) :
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
