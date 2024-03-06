

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

#include "object_drawer.hpp"

// local
#include "opengl/texture/texture_buffer_object.hpp"
#include "opengl/draw/triangles_mesh_vao.hpp"

using namespace tool::gl;
using namespace tool::geo;

auto ObjectDrawer::draw(ShaderProgram *shader) -> void{

    static_cast<void>(shader);

    if(vaoRenderer != nullptr){
        if(!texturesNames.empty()){
            TBO::bind(texturesNames.span(),0);
        }
        vaoRenderer->render();
    }
}


FullscreenQuadDrawer2::FullscreenQuadDrawer2(){

    static std::vector<GLfloat> points = {
        -1.0f, -1.0f, 0.0f,
        1.0f, -1.0f, 0.0f,
        1.0f, 1.0f, 0.0f,
        -1.0f, 1.0f, 0.0f
    };

    static std::vector<GLfloat> textCoords = {
        0.0f, 0.0f,
        1.0f, 0.0f,
        1.0f, 1.0f,
        0.0f, 1.0f,
    };

    static std::vector<GLuint> trianglesIndices = {
        0,1,2,0,2,3
    };
    
    auto tmd = std::make_unique<TriangleMeshVAO>();
    tmd->init_buffers(&trianglesIndices, &points, nullptr, &textCoords);
    vaoRenderer = std::move(tmd);
}

