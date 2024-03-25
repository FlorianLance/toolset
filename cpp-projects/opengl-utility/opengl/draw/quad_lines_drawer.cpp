
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

#include "quad_lines_drawer.hpp"

// base
#include "utility/logger.hpp"

// local
#include "opengl/gl_functions.hpp"
#include "lines_mesh_vao.hpp"

using namespace tool::gl;

static inline std::array<GLuint,8> indices = {
    0, 1,
    1, 2,
    2, 3,
    3, 0
};

QuadLinesDrawer::QuadLinesDrawer(){
    BaseDrawer::vaoRenderer = std::make_unique<LinesMeshVAO>();
}

auto QuadLinesDrawer::init(bool initColors) -> void{
    auto lm = dynamic_cast<LinesMeshVAO*>(vaoRenderer.get());
    lm->clean();
    lm->indicesBufferUsage  = GL_DYNAMIC_STORAGE_BIT;
    lm->positionBufferUsage = GL_DYNAMIC_STORAGE_BIT;
    lm->colorBufferUsage    = GL_DYNAMIC_STORAGE_BIT;
    lm->init_data(indices.size(), 4, initColors);
    lm->update_indices(indices);
}

auto QuadLinesDrawer::init_and_load(std::span<const geo::Pt3f,4> points, std::span<const geo::Pt3f> colors) -> void{
    auto lm = dynamic_cast<LinesMeshVAO*>(vaoRenderer.get());
    lm->clean();
    lm->init_and_load_data(indices, points, colors);
}

auto QuadLinesDrawer::update(std::span<const geo::Pt3f, 4> points, std::span<const geo::Pt3f> colors) -> void{        
    auto lm = dynamic_cast<LinesMeshVAO*>(vaoRenderer.get());
    lm->update_data(points, colors);
}

auto QuadLinesDrawer::draw() -> void{
    auto lm = dynamic_cast<LinesMeshVAO*>(vaoRenderer.get());
    if(!lm->initialized()){
        return;
    }
    lm->bind();
    GL::draw_elements_instanced_base_vertex_base_instance(GL_LINES, indices.size(), GL_UNSIGNED_INT, nullptr, 1, 0, 0);
    lm->unbind();
}
