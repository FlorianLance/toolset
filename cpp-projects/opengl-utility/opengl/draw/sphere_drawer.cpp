

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

#include "sphere_drawer.hpp"

// local
#include "opengl/gl_functions.hpp"
#include "triangles_mesh_vao.hpp"

using namespace tool::gl;

SphereDrawer2::SphereDrawer2(){
    BaseDrawer::vaoRenderer = std::make_unique<TriangleMeshVAO>();
}

auto SphereDrawer2::init_and_load(float radius, size_t nSlices, size_t nStacks) -> void{

    GLuint nVerts   = (nSlices+1) * (nStacks + 1);
    GLuint elements = (nSlices * 2 * (nStacks-1) );

    std::vector<geo::Pt3f> p(nVerts);
    std::vector<geo::Pt3f> n(nVerts);
    std::vector<geo::Pt2f> tex(nVerts);
    std::vector<geo::Pt3<GLuint>> el(elements);

    // generate positions and normals
    GLfloat thetaFac = two_PI<GLfloat>/ nSlices;
    GLfloat phiFac   = PI<GLfloat>/ nStacks;
    GLuint idx = 0;
    for( GLuint ii = 0; ii <= nSlices; ii++ ) {

        GLfloat theta = ii * thetaFac;
        GLfloat s = static_cast<GLfloat>(ii) / nSlices;

        for( GLuint jj = 0; jj <= nStacks; jj++ ) {

            GLfloat phi = jj * phiFac;
            GLfloat t = static_cast<GLfloat>(jj) / nStacks;
            GLfloat nx = sinf(phi) * cosf(theta);
            GLfloat ny = sinf(phi) * sinf(theta);
            GLfloat nz = cosf(phi);

            p[idx] = {radius * nx, radius * ny, radius * nz};
            n[idx] = {nx, ny, nz};
            tex[idx] = {s,t};
            ++idx;
        }
    }

    // generate the element list
    idx = 0;
    for( GLuint ii = 0; ii < nSlices; ii++ ) {
        GLuint stackStart = ii * (nStacks + 1);
        GLuint nextStackStart = (ii+1) * (nStacks+1);
        for( GLuint jj = 0; jj < nStacks; jj++ ) {
            if( jj == 0 ) {
                el[idx] = {stackStart, stackStart + 1, nextStackStart + 1};
                ++idx;
            } else if( jj == nStacks - 1) {
                el[idx] = {stackStart + jj, stackStart + jj + 1, nextStackStart + jj};
                ++idx;
            } else {
                el[idx] = {stackStart + jj, stackStart + jj + 1, nextStackStart + jj + 1};
                ++idx;
                el[idx] = {nextStackStart + jj, stackStart + jj, nextStackStart + jj + 1};
                ++idx;
            }
        }
    }

    auto tm = dynamic_cast<TriangleMeshVAO*>(vaoRenderer.get());
    tm->init_and_load_3d_mesh(
        el, p, n, tex
    );
}

auto SphereDrawer2::draw() -> void{

    auto tm = dynamic_cast<TriangleMeshVAO*>(vaoRenderer.get());
    if(!tm->initialized()){
        return;
    }
    tm->bind();
    GL::draw_elements_instanced_base_vertex_base_instance(GL_TRIANGLES, tm->allocated_indices_count(), GL_UNSIGNED_INT, nullptr, 0, 0, 0);
    tm->unbind();
}
