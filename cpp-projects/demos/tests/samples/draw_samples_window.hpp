
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

    void update() override;

    // gl
    bool initialize_gl() override;
    void draw_gl() override;    
    // imgui
    void draw_imgui() override;
    // # window
    void resize_windows() override;

    // managers
    bool init_textures();
    bool init_shaders();
    bool init_models();
    bool init_drawers();
    bool init_samples();

    geo::ColoredCloudData cloud;

protected:

    // imgui
    ImPlotContext *imPlotContext = nullptr;

    IdAliasMapSharedPtr<gl::Drawer> uiDrawers;
    IdAliasMapUniquePtr<Sample> uiSamples;

    bool m_showDemoWindow = false;
    bool m_showMetricsWindow = false;
};
}
