
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

// local
#include "opengl/gl_functions.hpp"

namespace tool::gl {


[[maybe_unused]] static inline auto draw_lines(GLsizei count, GLsizei instancecount = 1, GLuint baseinstance = 0) -> void{
    GL::draw_arrays_instance_base_instance(GL_LINES, 0, count, instancecount, baseinstance);
}

[[maybe_unused]] static inline auto draw_triangles(GLsizei count, GLsizei instancecount = 1, GLuint baseinstance = 0) -> void{
    GL::draw_arrays_instance_base_instance(GL_TRIANGLES, 0, count, instancecount, baseinstance);
}

[[maybe_unused]] static inline auto draw_patches(GLsizei count, GLsizei instancecount = 1, GLuint baseinstance = 0) -> void{
    GL::draw_arrays_instance_base_instance(GL_PATCHES, 0, count, instancecount, baseinstance);
}

[[maybe_unused]] static inline auto draw_points(GLsizei count, GLsizei instancecount = 1, GLuint baseinstance = 0) -> void{
    GL::draw_arrays_instance_base_instance(GL_POINTS, 0, count, instancecount, baseinstance);
}

[[maybe_unused]] static inline auto draw_points_with_ebo(GLsizei count, GLsizei instancecount = 1, GLint basevertex = 0, GLuint baseinstance = 0) -> void{
    GL::draw_elements_instanced_base_vertex_base_instance(GL_POINTS, count, GL_UNSIGNED_INT, nullptr, instancecount, basevertex, baseinstance);
}

[[maybe_unused]] static inline auto draw_lines_with_ebo(GLsizei count, GLsizei instancecount = 1, GLint basevertex = 0, GLuint baseinstance = 0) -> void{
    GL::draw_elements_instanced_base_vertex_base_instance(GL_LINES, count, GL_UNSIGNED_INT, nullptr, instancecount, basevertex, baseinstance);
}

[[maybe_unused]] static inline auto draw_line_strip_with_ebo(GLsizei count, GLsizei instancecount = 1, GLint basevertex = 0, GLuint baseinstance = 0) -> void{
    GL::draw_elements_instanced_base_vertex_base_instance(GL_LINE_STRIP, count, GL_UNSIGNED_INT, nullptr, instancecount, basevertex, baseinstance);
}

[[maybe_unused]] static inline auto draw_triangles_with_ebo(GLsizei count, GLsizei instancecount = 1, GLint basevertex = 0, GLuint baseinstance = 0) -> void{
    GL::draw_elements_instanced_base_vertex_base_instance(GL_TRIANGLES, count, GL_UNSIGNED_INT, nullptr, instancecount, basevertex, baseinstance);
}

[[maybe_unused]] static inline auto draw_triangles_adjacency_with_ebo(GLsizei count, GLsizei instancecount = 1, GLint basevertex = 0, GLuint baseinstance = 0) -> void{
    GL::draw_elements_instanced_base_vertex_base_instance(GL_TRIANGLES_ADJACENCY, count, GL_UNSIGNED_INT, nullptr, instancecount, basevertex, baseinstance);
}

[[maybe_unused]] static inline auto draw_quads_with_ebo(GLsizei count, GLsizei instancecount = 1, GLint basevertex = 0, GLuint baseinstance = 0) -> void{
    GL::draw_elements_instanced_base_vertex_base_instance(GL_QUADS, count, GL_UNSIGNED_INT, nullptr, instancecount, basevertex, baseinstance);
}

[[maybe_unused]] static inline auto draw_polygon_with_ebo(GLsizei count, GLsizei instancecount = 1, GLint basevertex = 0, GLuint baseinstance = 0) -> void{
    GL::draw_elements_instanced_base_vertex_base_instance(GL_POLYGON, count, GL_UNSIGNED_INT, nullptr, instancecount, basevertex, baseinstance);
}
}

