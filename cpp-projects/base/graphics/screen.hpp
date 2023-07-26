

/*******************************************************************************
** Toolset-base                                                               **
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
#include "geometry/point2.hpp"

namespace tool::graphics{

class Screen{

public:

    constexpr Screen(unsigned int width = 800, unsigned int height = 800, int xPos = 0, int yPos = 0) : m_size(width,height), m_position(xPos,yPos){
    }    

    constexpr auto resize(unsigned int width, unsigned int height) noexcept -> void{
        m_size = {width, height};
    }

    constexpr auto width() const noexcept -> unsigned int{
        return m_size.x();
    }

    constexpr auto height() const noexcept -> unsigned int{
        return m_size.y();
    }

    constexpr auto size_pixels() const noexcept -> unsigned int{
        return m_size.x() * m_size.y();
    }

    constexpr auto size() const noexcept -> geo::Pt2<unsigned int>{
        return m_size;
    }

    constexpr auto x_pos() const noexcept -> int{
        return m_position.x();
    }

    constexpr auto y_pos() const noexcept -> int{
        return m_position.y();
    }

    constexpr auto position() const noexcept -> geo::Pt2<int>{
        return m_position;
    }

private:
    geo::Pt2<unsigned int> m_size = {800,800};
    geo::Pt2<int> m_position = {0,0};
};
}

