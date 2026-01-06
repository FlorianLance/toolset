
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

#include "points_drawers.hpp"

// base
#include "utility/logger.hpp"

// local
#include "points_renderer.hpp"

using namespace tool::gl;


auto CloudPointsDrawer::initialize(bool dynamic, std::span<const geo::Pt3f> vertices, std::span<const geo::Pt3f> colors, std::span<const geo::Pt3f> normals) -> void{
    auto pm = dynamic_cast<PointsRenderer*>(m_vaoRenderer.get());
    if(dynamic){
        pm->positionBufferUsage  = GL_DYNAMIC_STORAGE_BIT;
        pm->colorBufferUsage     = GL_DYNAMIC_STORAGE_BIT;
        pm->normalBufferUsage    = GL_DYNAMIC_STORAGE_BIT;
    }
    pm->initialize(!colors.empty(), !normals.empty());
    if(!pm->load_data(vertices, colors, normals)){
        Log::error("[CloudPointsDrawer::initialize] Error during loading.\n"sv);
    }
}

auto CloudPointsDrawer::initialize(bool dynamic, const geo::ColorCloud &cloud) -> void{
    initialize(dynamic, cloud.vertices, cloud.colors, cloud.normals);
}

auto CloudPointsDrawer::update(std::span<const geo::Pt3f> vertices, std::span<const geo::Pt3f> colors, std::span<const geo::Pt3f> normals) -> void{
    auto pm = dynamic_cast<PointsRenderer*>(m_vaoRenderer.get());
    if(!pm->update_data(vertices, 0, colors, 0, normals, 0)){
        Log::error("[CloudPointsDrawer::update] Error during update.\n"sv);
        return;
    }
}

auto CloudPointsDrawer::update(const geo::ColorCloud &cloud) -> void{
    update(cloud.vertices, cloud.colors, cloud.normals);
}



