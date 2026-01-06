
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


#pragma once

// local
#include "ui/dcp_gl_window.hpp"
#include "ui/dcp_main_window_drawer.hpp"
#include "dcp_model.hpp"

namespace tool{

class DCPView {
public:

    DCPView(size_t id);
    ~DCPView();

    auto initialize(int nbGrabbers) -> void;

    auto start() -> void;
    auto update() -> void;
    auto draw(geo::Pt2f size, DCPModel *model) -> void;
    auto exit() -> void;

    auto gl() -> graphics::DCPGlWindow*{return m_glW.get();}

    graphics::DCPMainWindowDrawer mainW;

    auto update_selected_color(size_t idCloud, size_t idButton, geo::Pt2<int> coords, img::ColorRGBA8 color) -> void;

private:
    std::unique_ptr<graphics::DCPGlWindow> m_glW = nullptr;
};
}
