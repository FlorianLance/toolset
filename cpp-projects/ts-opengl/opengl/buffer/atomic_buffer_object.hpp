
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

// glew
#include <GL/glew.h>

namespace tool::gl{

struct ABO{

    ABO() = default;
    ABO(const ABO&) = delete;
    ABO& operator=(const ABO&) = delete;
    ABO(ABO&& other) = default;
    ABO& operator=(ABO&& other) = default;
    ~ABO();

    [[nodiscard]] constexpr auto id()               const noexcept -> GLuint     {return m_handle;}
    [[nodiscard]] constexpr auto loaded_data_size() const noexcept -> GLsizeiptr {return m_loadedDataSize;}
    [[nodiscard]] constexpr auto is_initialized()   const noexcept -> bool       {return id() != 0;}
    [[nodiscard]] constexpr auto is_data_loaded()   const noexcept -> bool       {return loaded_data_size() != 0;}
    [[nodiscard]] constexpr auto buffer_usage()     const noexcept -> GLbitfield {return m_usage;}
    [[nodiscard]] constexpr auto is_dynamic()       const noexcept -> bool       {return buffer_usage() & GL_DYNAMIC_STORAGE_BIT;}

    auto initialize() -> void;
    auto clean() -> void;

    auto load_data(GLsizeiptr size, GLenum usage = defaultUsage) -> bool;
    auto update_data(const GLuint *data, GLsizeiptr size, GLintptr offset) -> bool;

    auto bind_to_index(GLuint index) const -> bool;

private:

    GLuint     m_handle = 0;
    GLsizeiptr m_loadedDataSize = 0;
    GLbitfield m_usage = 0;

    static constexpr GLbitfield defaultUsage = 0;
};
}

