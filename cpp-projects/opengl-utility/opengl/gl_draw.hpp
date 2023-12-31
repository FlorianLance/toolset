
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
#include "opengl/utility/gl_utility.hpp"

namespace tool::gl {

static inline void draw(DrawMode mode, VerticesCount count, GLsizei instancecount, GLuint baseinstance){

    // glDrawArrays(GL_LINES, 0, count.v);
    glDrawArraysInstancedBaseInstance(
        mode.v,         // GLenum mode,           : Specifies what kind of primitives to render
        0,              // GLint first,           : Specifies the starting index in the enabled arrays.
        count.v,        // GLsizei count,         : Specifies the number of indices to be rendered.
        instancecount,  // GLsizei instancecount, : Specifies the number of instances of the specified range of indices to be rendered.
        baseinstance    // GLuint baseinstance    : Specifies the base instance for use in fetching instanced vertex attributes.
    );
}

static inline  void draw_with_ebo(DrawMode mode, VerticesCount count, GLsizei instancecount, GLint basevertex, GLuint baseinstance){

    // glDrawElements(mode.v, count.v, GL_UNSIGNED_INT, nullptr);
    glDrawElementsInstancedBaseVertexBaseInstance(
        mode.v,
        count.v,
        GL_UNSIGNED_INT,
        nullptr,                // void *indices,         : Specifies a pointer to the location where the indices are stored. (not used with VBO)
        instancecount,          // GLsizei instancecount, : Specifies the number of instances of the indexed geometry that should be drawn.
        basevertex,             // GLint basevertex,      : Specifies a constant that should be added to each element of indices when chosing elements from the enabled vertex arrays.
        baseinstance            // GLuint baseinstance    : Specifies the base instance for use in fetching instanced vertex attributes.
    );
}

[[maybe_unused]] static inline void draw_lines(VerticesCount count, GLsizei instancecount = 1, GLuint baseinstance = 0){
    draw(DrawMode{GL_LINES}, count, instancecount, baseinstance);
}

[[maybe_unused]] static inline  void draw_triangles(VerticesCount count, GLsizei instancecount = 1, GLuint baseinstance = 0){
    draw(DrawMode{GL_TRIANGLES}, count, instancecount, baseinstance);
}

[[maybe_unused]] static inline void draw_patches(VerticesCount count, GLsizei instancecount = 1, GLuint baseinstance = 0){
    draw(DrawMode{GL_PATCHES}, count, instancecount, baseinstance);
}

[[maybe_unused]] static inline void draw_points(VerticesCount count, GLsizei instancecount = 1, GLuint baseinstance = 0){
    draw(DrawMode{GL_POINTS}, count, instancecount, baseinstance);
}

[[maybe_unused]] static inline  void draw_points_with_ebo(VerticesCount count, GLsizei instancecount = 1, GLint basevertex = 0, GLuint baseinstance = 0){
    draw_with_ebo(DrawMode{GL_POINTS}, count, instancecount, basevertex, baseinstance);
}

[[maybe_unused]] static inline  void draw_lines_with_ebo(VerticesCount count, GLsizei instancecount = 1, GLint basevertex = 0, GLuint baseinstance = 0){
    draw_with_ebo(DrawMode{GL_LINES}, count, instancecount, basevertex, baseinstance);
}

[[maybe_unused]] static inline  void draw_line_strip_with_ebo(VerticesCount count, GLsizei instancecount = 1, GLint basevertex = 0, GLuint baseinstance = 0){
    draw_with_ebo(DrawMode{GL_LINE_STRIP}, count, instancecount, basevertex, baseinstance);
}

[[maybe_unused]] static inline  void draw_triangles_with_ebo(VerticesCount count, GLsizei instancecount = 1, GLint basevertex = 0, GLuint baseinstance = 0){
    draw_with_ebo(DrawMode{GL_TRIANGLES}, count, instancecount, basevertex, baseinstance);
}

[[maybe_unused]] static inline  void draw_triangles_adjacency_with_ebo(VerticesCount count, GLsizei instancecount = 1, GLint basevertex = 0, GLuint baseinstance = 0){
    draw_with_ebo(DrawMode{GL_TRIANGLES_ADJACENCY}, count, instancecount, basevertex, baseinstance);
}

[[maybe_unused]] static inline  void draw_quads_with_ebo(VerticesCount count, GLsizei instancecount = 1, GLint basevertex = 0, GLuint baseinstance = 0){
    draw_with_ebo(DrawMode{GL_QUADS}, count, instancecount, basevertex, baseinstance);
}

[[maybe_unused]] static inline  void draw_polygon_with_ebo(VerticesCount count, GLsizei instancecount = 1, GLint basevertex = 0, GLuint baseinstance = 0){
    draw_with_ebo(DrawMode{GL_POLYGON}, count, instancecount, basevertex, baseinstance);
}
}

