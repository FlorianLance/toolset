
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

#include "quad_triangles_drawer.hpp"

// local
#include "opengl/gl_functions.hpp"
#include "opengl/texture/texture_buffer_object.hpp"
#include "triangles_mesh_vao.hpp"

using namespace tool::gl;
using namespace tool::geo;

QuadTrianglesDrawer::QuadTrianglesDrawer(){
    BaseDrawer::vaoRenderer = std::make_unique<TriangleMeshVAO>();
}

auto QuadTrianglesDrawer::init_and_load() -> void{

    constexpr static std::array<tool::geo::Pt3f,4> vertices = {
        tool::geo::Pt3f{-1.f, -1.f, 0.f},
        tool::geo::Pt3f{ 1.f, -1.f, 0.f},
        tool::geo::Pt3f{ 1.f,  1.f, 0.f},
        tool::geo::Pt3f{-1.f,  1.f, 0.f}
    };

    constexpr static std::array<tool::geo::Pt2f,4> texCoords = {
        tool::geo::Pt2f{0.f, 1.f},
        tool::geo::Pt2f{1.f, 1.f},
        tool::geo::Pt2f{1.f, 0.f},
        tool::geo::Pt2f{0.f, 0.f},
        // tool::geo::Pt2f{0.f, 0.f},
        // tool::geo::Pt2f{1.f, 0.f},
        // tool::geo::Pt2f{1.f, 1.f},
        // tool::geo::Pt2f{0.f, 1.f},
    };

    constexpr static std::array<tool::geo::Pt3f,4> normals = {
        tool::geo::Pt3f{0.f, 0.f, 1.f},
        tool::geo::Pt3f{0.f, 0.f, 1.f},
        tool::geo::Pt3f{0.f, 0.f, 1.f},
        tool::geo::Pt3f{0.f, 0.f, 1.f}
    };

    constexpr static std::array<tool::geo::Pt3<GLuint>,2> elements = {
        tool::geo::Pt3<GLuint>{0,1,2},
        tool::geo::Pt3<GLuint>{0,2,3}
    };

    auto tm = dynamic_cast<TriangleMeshVAO*>(vaoRenderer.get());
    tm->clean();
    tm->init_and_load_3d_mesh(elements, vertices, normals, texCoords);
}

auto QuadTrianglesDrawer::draw() -> void{

    auto tm = dynamic_cast<TriangleMeshVAO*>(vaoRenderer.get());
    if(!tm->initialized()){
        return;
    }

    if(!texturesId.empty()){
        TBO::bind(texturesId.values, 0);
    }

    tm->bind();
    GL::draw_elements_instanced_base_vertex_base_instance(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr, 1, 0, 0);
    tm->unbind();
}
