
/*******************************************************************************
** Toolset-dc-grabber                                                         **
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

#include "dcg_view.hpp"

// base
#include "utility/logger.hpp"

using namespace tool;
using namespace std::string_view_literals;

DCGView::DCGView(size_t id){

    auto lg = LogGuard("DCGView::DCGView"sv);
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
    // # window
    const std::string numVersion = "2.8";
    m_glW = std::make_unique<graphics::DCGGlWindow>(std::format("DC grabber id[{}] v[{}]", id, numVersion), screen,context);
    m_glW->init();
}

auto DCGView::initialize() -> void{
    auto lg = LogGuard("DCGView::initialize"sv);
    mainW.initialize();
}

auto DCGView::start() -> void{
    auto lg = LogGuard("DCGView::start"sv);
    m_glW->start();
}

auto DCGView::update() -> void{
    mainW.deviceD.update();
}

auto DCGView::draw(geo::Pt2f size, DCGModel *model) -> void{
    mainW.draw(size, model);
}

auto DCGView::exit() -> void{
    auto lg = LogGuard("DCGView::exit"sv);
    m_glW->quit();
}
