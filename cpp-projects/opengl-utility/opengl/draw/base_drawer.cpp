

/*******************************************************************************
** Toolset-opengl-utility                                                     **
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

#include "base_drawer.hpp"

// base
#include "utility/logger.hpp"

// local
#include "opengl/gl_functions.hpp"
#include "points_renderer.hpp"
#include "lines_renderer.hpp"
#include "triangles_renderer.hpp"
#include "voxels_renderer.hpp"
#include "opengl/texture/texture_buffer_object.hpp"

using namespace tool::gl;

BaseDrawer::BaseDrawer(DrawerType type) : m_type(type){
    switch (m_type) {
    case DrawerType::Points:{
        m_vaoRenderer = std::make_unique<PointsRenderer>();
    }break;
    case DrawerType::Voxels:{
        m_vaoRenderer = std::make_unique<VoxelsRenderer>();
    }break;
    case DrawerType::Lines:{
        m_vaoRenderer = std::make_unique<LinesRenderer>();
    }break;
    case DrawerType::Triangles:{
        m_vaoRenderer = std::make_unique<TrianglesRenderer>();
    }break;
    case ::DrawerType::Invalid:
        Logger::error("[BaseDrawer] Invalid type.\n");
        break;
    }
}

auto BaseDrawer::draw() -> void{

    if(!m_vaoRenderer->initialized()){
        return;
    }

    m_vaoRenderer->bind();

    GLsizei count = m_vaoRenderer->indices_count();
    if(nIndicesToDraw.has_value()){
        count = std::min(nIndicesToDraw.value(), count);
    }

    switch (m_type) {
    case DrawerType::Points:{
        GL::draw_arrays_instance_base_instance(GL_POINTS, 0, count, 1, 0);
    }break;
    case DrawerType::Voxels:{
        GL::draw_arrays_instance_base_instance(GL_POINTS, 0, count, 1, 0);
    }break;
    case DrawerType::Lines:{
        GL::draw_elements_instanced_base_vertex_base_instance(GL_LINES, count, GL_UNSIGNED_INT, nullptr, 1, 0, 0);
    }break;
    case DrawerType::Triangles:{
        if(!texturesId.empty()){
            TBO::bind(texturesId.values, 0);
        }
        GL::draw_elements_instanced_base_vertex_base_instance(GL_TRIANGLES, count, GL_UNSIGNED_INT, nullptr, 1, 0, 0);
    }break;
    case ::DrawerType::Invalid:
        Logger::error("[BaseDrawer::draw] Invalid type.\n");
        break;
    }

    m_vaoRenderer->unbind();
}

auto HierarchyDrawer2::draw() -> void{
    for(const auto &drawer : drawers){
        drawer->draw();
    }
    for(const auto &child : children){
        child->draw();
    }
}
