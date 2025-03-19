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

// std
#include <format>

// local
#include "geometry/matrix.hpp"

namespace tool {

template <typename ElementT>
static auto to_string(const geo::Matrix<ElementT, 3,3> &m) noexcept -> std::string{
    return std::format("[{:.4f} {:.4f} {:.4f}][{:.4f} {:.4f} {:.4f}][{:.4f} {:.4f} {:.4f}]",
        m.at(0,0),m.at(0,1),m.at(0,2),
        m.at(1,0),m.at(1,1),m.at(1,2),
        m.at(2,0),m.at(2,1),m.at(2,2)
    );
}

template <typename ElementT>
static auto to_string(const geo::Matrix<ElementT, 4,4> &m) noexcept -> std::string{
    return std::format("[{:.4f} {:.4f} {:.4f} {:.4f}][{:.4f} {:.4f} {:.4f} {:.4f}][{:.4f} {:.4f} {:.4f} {:.4f}][{:.4f} {:.4f} {:.4f} {:.4f}]",
        m.at(0,0),m.at(0,1),m.at(0,2),m.at(0,3),
        m.at(1,0),m.at(1,1),m.at(1,2),m.at(1,3),
        m.at(2,0),m.at(2,1),m.at(2,2),m.at(2,3),
        m.at(3,0),m.at(3,1),m.at(3,2),m.at(3,3)
    );
}
}

