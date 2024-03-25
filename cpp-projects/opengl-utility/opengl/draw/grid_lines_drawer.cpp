
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

#include "grid_lines_drawer.hpp"

// local
#include "opengl/gl_functions.hpp"
#include "lines_mesh_vao.hpp"

using namespace tool::gl;

GridLinesDrawer::GridLinesDrawer(){
    BaseDrawer::vaoRenderer = std::make_unique<LinesMeshVAO>();
}

auto GridLinesDrawer::init_and_load(GLfloat width, GLfloat height, GLuint nbX, GLuint nbY) -> void{

    std::vector<GLuint> indices;
    indices.reserve(nbX*2+nbY*2);

    std::vector<GLfloat> points;
    points.reserve((nbX*2+nbY*2)*3);

    const GLfloat lX = nbX*width;
    const GLfloat lY = nbY*height;

    const GLfloat minX = -lX*0.5f;
    const GLfloat maxX = +lX*0.5f;
    for(GLuint ii = 0; ii < nbX; ++ii){
        indices.emplace_back(static_cast<GLuint>(points.size()));
        points.emplace_back(minX);
        points.emplace_back(0.f);
        points.emplace_back(minX+ii*width);

        indices.emplace_back(static_cast<GLuint>(points.size()));
        points.emplace_back(maxX);
        points.emplace_back(0.f);
        points.emplace_back(minX+ii*width);
    }

    const GLfloat minY = -lY*0.5f;
    const GLfloat maxY = +lY*0.5f;
    for(GLuint ii = 0; ii < nbY; ++ii){

        indices.emplace_back(static_cast<GLuint>(points.size()));
        points.emplace_back(minY+ii*height);
        points.emplace_back(0.f);
        points.emplace_back(minY);

        indices.emplace_back(static_cast<GLuint>(points.size()));
        points.emplace_back(minY+ii*height);
        points.emplace_back(0.f);
        points.emplace_back(maxY);
    }

    auto lm = dynamic_cast<LinesMeshVAO*>(vaoRenderer.get());
    lm->clean();
    lm->init_and_load_data(
        std::span<GLuint>(indices.data(),indices.size()),
        std::span<geo::Pt3f>(reinterpret_cast<geo::Pt3f*>(points.data()), points.size()/3),
        {}
    );
}

auto GridLinesDrawer::draw() -> void{
    auto lm = dynamic_cast<LinesMeshVAO*>(vaoRenderer.get());
    if(!lm->initialized()){
        return;
    }
    lm->bind();
    GL::draw_elements_instanced_base_vertex_base_instance(GL_LINES, lm->allocated_indices_count(), GL_UNSIGNED_INT, nullptr, 1, 0, 0);
    lm->unbind();
}
