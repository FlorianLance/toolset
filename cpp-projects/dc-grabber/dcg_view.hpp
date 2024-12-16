
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


#pragma once

// local
#include "ui/dcg_gl_window.hpp"
#include "ui/dcg_main_w.hpp"

namespace tool{

class DCGView {
public:

    DCGView(size_t id);
    auto initialize() -> void;

    auto start() -> void;
    auto update() -> void;
    auto draw(geo::Pt2f size, DCGModel *model) -> void;
    auto exit() -> void;

    auto gl() -> graphics::DCGGlWindow*{return m_glW.get();}

    graphics::DCGMainW mainW;

    auto update_selected_color(size_t idCloud, size_t idButton, geo::Pt2<int> coords, ColorRGBA8 color) -> void;

private:

    std::unique_ptr<graphics::DCGGlWindow> m_glW = nullptr;
};
}
