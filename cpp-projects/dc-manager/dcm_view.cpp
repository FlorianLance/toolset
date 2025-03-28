
/*******************************************************************************
** Toolset-dc-manager                                                         **
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

#include "dcm_view.hpp"

// base
#include "utility/logger.hpp"

using namespace tool;

DCMView::DCMView(size_t id){

    // init main window
    // # screen
    graphics::Screen screen(1920, 1080, 0,0);
    // # gl context
    sf::ContextSettings context;
    context.depthBits         = 24;
    context.stencilBits       = 8;
    context.antialiasingLevel = 4;
    context.majorVersion      = 4;
    context.minorVersion      = 6;
    context.attributeFlags    = sf::ContextSettings::Attribute::Default;
    m_glW = std::make_unique<graphics::DCMGlWindow>(std::format("DC manager id[{}] v[{}]", id, cam::dc_version_name(cam::current_dc_version())), screen,context);
    m_glW->init();
}

DCMView::~DCMView(){
}

auto DCMView::initialize(int nbDevices) -> void{
    auto lg = LogG("DCMView::initialize");
    mainW.initialize(nbDevices);
}

auto DCMView::start() -> void{
    m_glW->start();
}

auto DCMView::update() -> void{
    mainW.m_leftPanelD.uiFramerateMS = m_glW->main_thread_duration_ms();
    mainW.update();
}

auto DCMView::draw(geo::Pt2f size, DCMModel *model) -> void{
    mainW.draw(size, model);
}

auto DCMView::exit() -> void{
    m_glW->quit();
}

auto DCMView::update_selected_color(size_t idCloud, size_t idButton, geo::Pt2f coordsR, geo::Pt2<int> coords, ColorRGBA8 color) -> void{
    mainW.m_leftPanelD.update_selected_color(color.conv<float>()/255.f);
}
