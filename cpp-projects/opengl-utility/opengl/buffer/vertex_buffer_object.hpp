
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

struct VertexBufferObject{

    VertexBufferObject() = default;
    VertexBufferObject(const VertexBufferObject&) = delete;
    VertexBufferObject& operator=(const VertexBufferObject&) = delete;
    VertexBufferObject(VertexBufferObject&& other) = delete;
    VertexBufferObject& operator=(VertexBufferObject&& other) = delete;
    ~VertexBufferObject();

    [[nodiscard]] constexpr auto id() const noexcept -> GLuint{return m_handle;};

    auto generate() -> void;
    auto clean() -> void;

    auto load_data(const GLfloat *data, GLsizeiptr size, GLenum usage = defaultUsage) -> void;
    auto load_data(const GLint *data, GLsizeiptr size, GLenum usage = defaultUsage) -> void;

    auto attrib(AttriIndex index, AttriSize size, AttriType type, Stride stride, AttribOffset offset = {nullptr}) -> bool;
    // TODO: DSA ATRIB
    auto dsa_attrib(GLuint vao, GLint index, GLint size, GLsizei stride, GLenum type, GLuint relativeoffset, GLuint bindingIndex = 0, GLintptr offset = 0, GLboolean normalized = GL_FALSE) -> void;

private:

    static inline GLenum defaultUsage = GL_DYNAMIC_STORAGE_BIT | GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT;
    // bit combination of: GL_DYNAMIC_STORAGE_BIT, GL_MAP_READ_BIT GL_MAP_WRITE_BIT, GL_MAP_PERSISTENT_BIT, GL_MAP_COHERENT_BIT, and GL_CLIENT_STORAGE_BIT.

    // GL_DYNAMIC_STORAGE_BIT
    // The contents of the data store may be updated after creation through calls to glBufferSubData.
    // If this bit is not set, the buffer content may not be directly updated by the client.
    // The data argument may be used to specify the initial content of the buffer's data store regardless of the presence of the GL_DYNAMIC_STORAGE_BIT.
    // Regardless of the presence of this bit, buffers may always be updated with server-side calls such as glCopyBufferSubData and glClearBufferSubData.

    // GL_MAP_READ_BIT
    // The data store may be mapped by the client for read access and a pointer in the client's address space obtained that may be read from.

    // GL_MAP_WRITE_BIT
    // The data store may be mapped by the client for write access and a pointer in the client's address space obtained that may be written through.

    // GL_MAP_PERSISTENT_BIT
    // The client may request that the server read from or write to the buffer while it is mapped.
    // The client's pointer to the data store remains valid so long as the data store is mapped, even during execution of drawing or dispatch commands.

    // GL_MAP_COHERENT_BIT
    // Shared access to buffers that are simultaneously mapped for client access and are used by the server will be coherent,
    // so long as that mapping is performed using glMapBufferRange.
    // That is, data written to the store by either the client or server will be immediately visible
    // to the other with no further action taken by the application. In particular,
    //  * If GL_MAP_COHERENT_BIT is not set and the client performs a write followed by a call to the glMemoryBarrier command with the GL_CLIENT_MAPPED_BUFFER_BARRIER_BIT set,
    //    then in subsequent commands the server will see the writes.
    //  * If GL_MAP_COHERENT_BIT is set and the client performs a write, then in subsequent commands the server will see the writes.
    //  * If GL_MAP_COHERENT_BIT is not set and the server performs a write, the application must call glMemoryBarrier with the GL_CLIENT_MAPPED_BUFFER_BARRIER_BIT set
    //    and then call glFenceSync with GL_SYNC_GPU_COMMANDS_COMPLETE (or glFinish). Then the CPU will see the writes after the sync is complete.
    //  * If GL_MAP_COHERENT_BIT is set and the server does a write, the app must call glFenceSync with GL_SYNC_GPU_COMMANDS_COMPLETE (or glFinish).
    //    Then the CPU will see the writes after the sync is complete.

    // GL_CLIENT_STORAGE_BIT
    // When all other criteria for the buffer storage allocation are met, this bit may be used by an implementation
    // to determine whether to use storage that is local to the server or to the client to serve as the backing store for the buffer.



    GLuint m_handle = 0;
};


}
