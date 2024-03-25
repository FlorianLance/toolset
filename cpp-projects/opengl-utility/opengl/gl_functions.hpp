
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

// glew
#include <GL/glew.h>

namespace tool::gl{

struct GL{

    // glew
    static auto init_glew() -> bool;
    static auto display_glew_info() -> void;

    [[deprecated]] static auto clear(GLbitfield mask) -> void;
    [[deprecated]] static auto clear_color(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha) -> void;
    static auto clear_named_framebuffer_fv(GLuint framebuffer, GLenum buffer, GLint drawbuffer, GLfloat *values) -> void;
    static auto enable(GLenum cap) -> void;
    static auto disable(GLenum cap) -> void;
    static auto enable_i(GLenum cap, GLuint index) -> void;
    static auto disable_i(GLenum cap, GLuint index) -> void;

    static auto create_buffers(GLsizei n, GLuint *buffers) -> void;
    static auto delete_buffers(GLsizei n, GLuint *buffers) -> void;

    // blend
    static auto depth_mask(GLboolean flag) -> void;

    // framebuffer
    static auto create_framebuffers(GLsizei n, GLuint *framebuffers) -> void;
    static auto bind_framebuffer(GLenum target, GLuint framebuffer) -> void;
    static auto named_framebuffer_texture(GLuint framebuffer, GLenum attachment, GLuint texture, GLint level) -> void;
    static auto named_framebuffer_renderbuffer(GLuint framebuffer, GLenum attachment, GLenum renderbuffertarget, GLuint renderbuffer) -> void;
    static auto named_framebuffer_read_buffer(GLuint framebuffer, GLenum mode) -> void;
    static auto named_framebuffer_draw_buffer(GLuint framebuffer, GLenum buf) -> void;
    static auto named_framebuffer_draw_buffers(GLuint framebuffer, GLsizei n, const GLenum *bufs) -> void;
    static auto check_named_framebuffer_status(GLuint framebuffer, GLenum target) -> GLenum;

    // texture
    static auto create_textures(GLenum target, GLsizei n, GLuint *textures) -> void;
    static auto delete_textures(GLsizei n, GLuint *textures) -> void;
    static auto bind_textures(GLuint first, GLsizei count, const GLuint *textures) -> void;
    static auto bind_image_texture(GLuint unit, GLuint texture, GLint level, GLboolean layered, GLint layer, GLenum access, GLenum format) -> void;
    static auto bind_image_textures(GLuint first, GLsizei count, const GLuint *textures) -> void;
    static auto texture_parameter_i(GLuint texture, GLenum pname, GLint param) -> void;
    static auto texture_parameter_fv(GLuint texture, GLenum pname, const GLfloat *params) -> void;
    static auto texture_storage_2d_multisample(GLuint texture, GLsizei samples, GLenum internalformat, GLsizei width, GLsizei height, GLboolean fixedsamplelocations) -> void;
    static auto texture_storage_2d(GLuint texture, GLsizei levels, GLenum internalformat, GLsizei width, GLsizei height) -> void;
    static auto texture_storage_3d(GLuint texture, GLsizei levels, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth) -> void;
    static auto texture_sub_image_2d(GLuint texture, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const void *pixels) -> void;

    static auto named_buffer_storage(GLuint buffer, GLsizeiptr size, const void *data, GLbitfield flags) -> void;
    static auto named_buffer_sub_data(GLuint buffer, GLintptr offset, GLsizeiptr size, const void *data) -> void;
    static auto bind_buffer_range(GLenum target, GLuint index, GLuint buffer, GLintptr offset, GLsizeiptr size) -> void;
    static auto bind_buffers_range(GLenum target, GLuint index, GLsizei count, const GLuint *buffers, const GLintptr *offsets, const GLintptr *sizes) -> void;

    static auto vertex_array_vertex_buffer(GLuint vaobj, GLuint bindingindex, GLuint buffer, GLintptr offset, GLsizei stride) -> void;
    static auto enable_vertex_array_attrib(GLuint vaobj, GLuint index) -> void;
    static auto vertex_array_attrib_format(GLuint vaobj, GLuint attribindex, GLint size, GLenum type, GLboolean normalized, GLuint relativeoffset) -> void;
    static auto vertex_array_attrib_binding(GLuint vaobj, GLuint attribindex, GLuint bindingindex) -> void;

    // shader
    static auto use_program(GLuint program) -> void;

    // draw
    static auto draw_arrays_instance_base_instance(GLenum mode, GLint first, GLsizei count, GLsizei instancecount, GLuint baseinstance) -> void;
    static auto draw_elements_instanced_base_vertex_base_instance(GLenum mode, GLsizei count, GLenum type, void *indices, GLsizei instancecount, GLint basevertex, GLuint baseinstance) -> void;


    // deprecated
    static auto bind_buffer(GLenum target, GLuint buffer) -> void;
    [[deprecated]] static auto enable_vertex_attrib_array(GLuint index) -> void;
    [[deprecated]] static auto vertex_attrib_i_pointer(GLuint index, GLint size, GLenum type, GLsizei stride, const void * pointer) -> void;
    [[deprecated]] static auto vertex_attrib_pointer(GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const void * pointer) -> void;
};
}

// Examples:

// glVertexArrayVertexBuffer(m_VAO, 0, m_Buffers[VERTEX_BUFFER], 0, sizeof(Vertex));
// size_t NumFloats = 0;
// glEnableVertexArrayAttrib(m_VAO, POSITION_LOCATION);
// glVertexArrayAttribFormat(m_VAO, POSITION_LOCATION, 3, GL_FLOAT, GL_FALSE, (GLuint)(NumFloats * sizeof(float)));
// glVertexArrayAttribBinding(m_VAO, POSITION_LOCATION, 0);
// NumFloats += 3;
