
/*******************************************************************************
** Toolset-opengl-utility                                                     **
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

#include "cloud_drawer.hpp"

// local
#include "opengl/gl_functions.hpp"
#include "points_mesh_vao.hpp"

using namespace tool::gl;

CloudDrawer::CloudDrawer(){
    BaseDrawer::vaoRenderer = std::make_unique<PointsMeshVAO>();
}

auto CloudDrawer::init_and_load(const geo::ColoredCloudData &cloud) -> void{
    auto pm = dynamic_cast<PointsMeshVAO*>(vaoRenderer.get());
    pm->clean();
    init_and_load(cloud.vertices.span(), cloud.colors.span(), cloud.normals.span());
}

auto CloudDrawer::init_and_load(std::span<const geo::Pt2f> points, std::span<const geo::Pt3f> colors, std::span<const geo::Pt2f> normals) -> void{
    auto pm = dynamic_cast<PointsMeshVAO*>(vaoRenderer.get());
    pm->clean();
    pm->init_and_load_2d_points(points, colors, normals);
}

auto CloudDrawer::init_and_load(std::span<const geo::Pt3f> points, std::span<const geo::Pt3f> colors, std::span<const geo::Pt3f> normals) -> void{
    auto pm = dynamic_cast<PointsMeshVAO*>(vaoRenderer.get());
    pm->clean();
    pm->init_and_load_3d_points(points, colors, normals);
}

auto CloudDrawer::draw() -> void{
    auto pm = dynamic_cast<PointsMeshVAO*>(vaoRenderer.get());
    pm->bind();
    GL::draw_arrays_instance_base_instance(GL_POINTS, 0, pm->indices_count(), 1, 0);
    pm->unbind();
}

