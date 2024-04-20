
#pragma once

// imgui
#include "imgui/extra/implot/implot.h"

// base
#include "utility/unordered_map.hpp"

// local
#include "windows/base_sfml_gl_window.hpp"
#include "samples.hpp"


namespace tool::graphics {

class DrawSampleWindow : public BaseSfmlGlWindow{

public:

    DrawSampleWindow(std::string_view title, graphics::Screen screen, std::optional<sf::ContextSettings> context);

private:


    // gl
    auto initialize_gl() -> bool override;
    void draw_gl() override;    
    // imgui
    void draw_imgui() override;
    // # window
    auto resize_windows() -> void override;

    void update() override;

    // managers
    auto init_models() -> bool;
    auto init_textures() -> bool;
    auto init_shaders() -> bool;
    auto init_drawers() -> bool;
    auto init_samples() -> bool;

protected:

    // imgui
    ImPlotContext *imPlotContext = nullptr;

    IdAliasMapSharedPtr<gl::BaseDrawer> uiDrawers2;
    IdAliasMapUniquePtr<Sample> uiSamples;

    bool m_showDemoWindow = false;
    bool m_showMetricsWindow = false;
};
}
