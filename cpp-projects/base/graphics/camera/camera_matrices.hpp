
/*******************************************************************************
** Toolset-base                                                               **
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
#include "geometry/matrix4.hpp"

namespace tool::graphics{

struct CameraMatrices{

    geo::Mat4d m;         // model matrix
    geo::Mat4d v;         // view matrix
    geo::Mat4d p;         // projection matrix

    geo::Mat3d normal;    // normal matrix
    geo::Mat4d mv;        // model*view matrix
    geo::Mat4d mvp;       // model*view*projection matrix

    auto update() -> void{
        mv     = m * v;
        normal = geo::rotation_m3x3(mv);
        mvp    = mv * p;
    }

    auto update_m(const geo::Mat4d &model) -> void{
        m = model;
        update();
    }

    auto update_v(const geo::Mat4d &view) -> void{
        v = view;
        update();
    }

    auto update_p(const geo::Mat4d &proj) -> void{
        p = proj;
        update();
    }

    auto update_vp(const geo::Mat4d &view, const geo::Mat4d &proj) -> void{
        v = view;
        p = proj;
        update();
    }

    auto update_mvp(const geo::Mat4d &model, const geo::Mat4d &view, const geo::Mat4d &proj) -> void{
        m = model;
        v = view;
        p = proj;
        update();
    }
};

}
