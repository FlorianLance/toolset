
/*******************************************************************************
** Toolset-ts-opengl                                                          **
** MIT License                                                                **
** Copyright (c) [2024] [Florian Lance]                                       **
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

// base
#include "geometry/obb3.hpp"

// local
#include "base_drawer.hpp"

namespace tool::gl {

class LineDrawer2 : public BaseDrawer{
public:
    LineDrawer2() : BaseDrawer(DrawerType::Lines){}
    auto initialize(bool dynamic) -> void;
    auto update(std::span<const geo::Pt3f,2> vertices, std::span<const geo::Pt3f> colors = {}) -> void;
};

class LinesDrawer2 : public BaseDrawer{
public:
    LinesDrawer2() : BaseDrawer(DrawerType::Lines){}
    auto initialize(bool dynamic,  std::span<const GLuint> indices, std::span<const geo::Pt3f> vertices, std::span<const geo::Pt3f> colors) -> void;
    auto update(std::span<const GLuint> indices, std::span<const geo::Pt3f> vertices, std::span<const geo::Pt3f> colors) -> void;
};



class AxesLinesDrawer : public BaseDrawer{
public:
    AxesLinesDrawer() : BaseDrawer(DrawerType::Lines){}
    auto initialize(float length = 1.f) -> void;
};

class TriangleLinesDrawer : public BaseDrawer{
public:
    TriangleLinesDrawer() : BaseDrawer(DrawerType::Lines){}
    auto initialize(bool dynamic) -> void;
    auto update(std::span<const geo::Pt3f,3> vertices, std::span<const geo::Pt3f> colors = {}) -> void;
};

class QuadLinesDrawer : public BaseDrawer{
public:
    QuadLinesDrawer() : BaseDrawer(DrawerType::Lines){}
    auto initialize(bool dynamic) -> void;
    auto update(std::span<const geo::Pt3f,4> vertices, std::span<const geo::Pt3f> colors = {}) -> void;
};

class FrustumDrawerLinesDrawer : public BaseDrawer{
public:
    FrustumDrawerLinesDrawer() : BaseDrawer(DrawerType::Lines){}
    auto initialize(bool dynamic, float fovy = 50.0f, float ar = 1.0f, float nearDist = 0.5f, float farDist = 100.0f) -> void;
    auto update(float fovy, float ar, float nearDist, float farDist) -> void;
};

class GridLinesDrawer : public BaseDrawer{
public:
    GridLinesDrawer() : BaseDrawer(DrawerType::Lines){}
    auto initialize(float width = 0.2f, float height = 0.2f, int nbX = 100, int nbY = 100, bool addDiagonals = false) -> void;
};

class OrientedBoundingBoxLinesDrawer : public BaseDrawer{
public:
    OrientedBoundingBoxLinesDrawer() : BaseDrawer(DrawerType::Lines){}
    auto initialize(bool dynamic, const geo::OBB3<float> &obb) -> void;
    auto update(const geo::OBB3<float> &obb) -> void;
};

}

