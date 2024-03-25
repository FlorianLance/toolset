
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

#pragma once



// local
#include "opengl/vao.hpp"
#include "opengl/buffer/vertex_buffer_object.hpp"

namespace tool::gl{

class VAORenderer{
public:

    virtual ~VAORenderer(){}
    virtual auto clean() -> void = 0;

    auto bind() const -> void{
        vao.bind();
    }

    auto unbind() const -> void{
        VAO::unbind();
    }

    [[nodiscard]] constexpr auto initialized() const noexcept -> bool{ return buffersInitialized;}
    [[nodiscard]] constexpr auto allocated_indices_count() const noexcept -> GLsizei{return nIndicesAllocated;}
    // [[nodiscard]] constexpr auto nb_indices_to_draw() const noexcept -> GLsizei{return nIndicesToDraw;}

    // TODO: remove
    virtual auto render() const -> void{}
    virtual auto render_instances(int count, int baseId = 0) const -> void {
        static_cast<void>(count);
        static_cast<void>(baseId);
    }
    virtual auto render_adjacency() const -> void {}
    virtual auto render_patches() const -> void{}

protected:

    VAO vao;
    VBO pointsB;
    GLsizei nIndicesAllocated = 0;
    // GLsizei nIndicesToDraw = 0;
    bool buffersInitialized = false;
};

}
