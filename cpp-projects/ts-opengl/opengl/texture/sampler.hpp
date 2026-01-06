

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

// opengl
#include <GL/glew.h>

// base
#include "image/texture_options.hpp"

namespace tool::gl {

struct Sampler{

    Sampler() = default;
    Sampler(const Sampler&) = delete;
    Sampler& operator=(const Sampler&) = delete;
    Sampler(Sampler&& other) = delete;
    Sampler& operator=(Sampler&& other) = delete;
    ~Sampler();

    [[nodiscard]] constexpr auto id()               const noexcept -> GLuint     {return m_handle;}
    [[nodiscard]] constexpr auto is_initialized()   const noexcept -> bool       {return id() != 0;}

    auto initialize() -> void;
    auto clean()      -> void;

    auto update(const img::TextureOptions &textureOptions) -> bool;

    auto bind(GLuint unit) const -> void;
    static auto bind(std::span<const Sampler> samplers, GLuint first) -> void;
    static auto unbind(GLuint first=0, GLuint count=1) -> void;

private:

    GLuint m_handle = 0;
};

}
