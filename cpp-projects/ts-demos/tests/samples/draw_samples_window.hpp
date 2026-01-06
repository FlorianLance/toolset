
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

// base
#include "utility/unordered_map.hpp"

// local
#include "windows/base_sfml_gl_window.hpp"
#include "samples.hpp"


namespace tool::graphics {

class DrawSampleWindow : public BaseSfmlGlWindow{

public:

    DrawSampleWindow(std::string_view title, geo::Screen screen, std::optional<sf::ContextSettings> context);

private:


    // gl
    auto initialize_gl() -> bool override;
    auto draw_gl() -> void override;
    // imgui
    auto draw_imgui() -> void override;
    // # window
    auto resize_windows() -> void override;

    auto update() -> void override;

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
