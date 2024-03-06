
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

#include "mesh_drawer.hpp"

// local
#include "opengl/gl_functions.hpp"
#include "triangles_mesh_vao.hpp"

using namespace tool::gl;
using namespace tool::geo;

MeshDrawer2::MeshDrawer2(){
    BaseDrawer::vaoRenderer = std::make_unique<TriangleMeshVAO>();
}

auto MeshDrawer2::init_and_load(const graphics::Mesh &mesh) -> void{
    auto tm = dynamic_cast<TriangleMeshVAO*>(vaoRenderer.get());
    tm->clean();

    auto triData = reinterpret_cast<const Pt3<GLuint>*>(mesh.triIds.get_data());
    tm->init_and_load_3d_mesh(
        std::span<const Pt3<GLuint>>(triData, mesh.triIds.size()),
        mesh.vertices.span(),
        mesh.normals.span(),
        mesh.tCoords.span(),
        mesh.tangents.span(),
        mesh.bones.span(),
        mesh.colors.span()
    );
}

auto MeshDrawer2::draw() -> void{

    auto tm = dynamic_cast<TriangleMeshVAO*>(vaoRenderer.get());
    tm->bind();
    GL::draw_arrays_instance_base_instance(GL_TRIANGLES, 0, tm->indices_count(), 1, 0);
    tm->unbind();

}
