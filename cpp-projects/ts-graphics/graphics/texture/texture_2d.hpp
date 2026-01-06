

/*******************************************************************************
** Toolset-ts-graphics                                                        **
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
#include "texture.hpp"
#include "texture_options.hpp"
#include "geometry/point2.hpp"

namespace tool::graphics{

struct Texture2D : public Texture{

    Texture2D() = default;
    explicit Texture2D(const std::string &pathTexture, bool flip = true, int targetNbChannels = 4);

    constexpr auto size() const noexcept -> geo::Pt2<int>{
        return geo::Pt2<int>{m_sizes[0], m_sizes[1]};
    }

    [[nodiscard]] constexpr auto width() const noexcept -> int{return m_sizes[0];}
    [[nodiscard]] constexpr auto height() const noexcept -> int{return m_sizes[1];}

private :

    bool m_flipped = false;
};

struct TextureInfo {
    Texture2D *texture = nullptr;
    TextureOptions options;
};
using TexturesInfo = std::vector<graphics::TextureInfo>;


}
