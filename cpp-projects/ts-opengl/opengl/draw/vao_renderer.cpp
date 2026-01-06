
/*******************************************************************************
** Toolset-ts-opengl                                                          **
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

#include "vao_renderer.hpp"

// local
#include "opengl/gl_functions.hpp"
#include "opengl/texture/texture_buffer_object.hpp"


// auto tool::gl::VAORenderer::render() const -> void{

//     if(!buffersInitialized){
//         return;
//     }

//     // if(!texturesId.empty()){
//     //     TBO::bind(texturesId.span(),0);
//     // }

//     vao.bind();
//     GL::draw_arrays_instance_base_instance(mode, 0, nIndices, 1, 0);
//     VAO::unbind();
// }

// auto tool::gl::VAORenderer::render_patches() const -> void {

//     if(!buffersInitialized){
//         return;
//     }

//     vao.bind();
//     GL::draw_arrays_instance_base_instance(GL_PATCHES, 0, nIndices, 1, 0);
//     VAO::unbind();
// }
