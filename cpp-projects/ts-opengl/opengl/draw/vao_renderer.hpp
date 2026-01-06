
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

#pragma once

// base
// #include "utility/unordered_map.hpp"

// local
#include "opengl/vao.hpp"
#include "opengl/buffer/vertex_buffer_object.hpp"


namespace tool::gl{

// enum class BufferType : std::int8_t{
//     Index, Vertex, Normal, TextureCoords, Tangent, Bones, Color
// };
// struct BufferInfo{
//     bool enabled = false;
//     GLuint bindindId = 0;
//     GLuint location = 0;
//     GLbitfield usage = 0;
//     VBO buffer;
// };


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
    
    [[nodiscard]] constexpr auto initialized()      const noexcept -> bool      {return m_buffersInitialized;}
    [[nodiscard]] constexpr auto data_loaded()      const noexcept -> bool      {return m_dataLoaded;}
    [[nodiscard]] constexpr auto ready_to_draw()    const noexcept -> bool      {return initialized() && data_loaded();}
    [[nodiscard]] constexpr auto vertices_count()   const noexcept -> GLsizei   {return m_nbVertices;}
    [[nodiscard]] constexpr auto indices_count()    const noexcept -> GLsizei   {return m_nbIndices;}

    // ### TODO: remove
    virtual auto render() const -> void{}
    virtual auto render_instances(int count, int baseId = 0) const -> void {
        static_cast<void>(count);
        static_cast<void>(baseId);
    }
    virtual auto render_adjacency() const -> void {}
    virtual auto render_patches() const -> void{}
    // ###

protected:

    VAO vao;
    VBO pointsB;
    GLsizei m_nbVertices = 0;
    GLsizei m_nbIndices = 0;
    
    bool m_buffersInitialized   = false;
    bool m_dataLoaded           = false;

    // umap<BufferType, BufferInfo> bInfos;
};

}
