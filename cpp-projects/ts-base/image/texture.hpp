

/*******************************************************************************
** Toolset-ts-base                                                            **
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
#include "geometry/point3.hpp"

namespace tool::img{

struct Texture{

    [[nodiscard]] constexpr auto nb_channels() const noexcept -> int{return m_nbChannels;}
    [[nodiscard]] constexpr auto data() noexcept -> unsigned char*{return m_data.data();}
    [[nodiscard]] constexpr auto data() const noexcept -> const unsigned char*{return m_data.data();}
    [[nodiscard]] constexpr auto is_hdr() const noexcept -> bool{return m_hdr;}

    auto copy_2d_data(int width, int height, int nbChannels, unsigned char *data) -> void;
    auto copy_2d_data(int width, int height, int nbChannels, const std::vector<unsigned char> &data) -> void;
    auto copy_2d_data(int width, int height, const std::vector<geo::Pt3<unsigned char>> &data) -> void;
    auto copy_2d_data(int width, int height, int nbChannels, const std::vector<float> &data) -> void;
    auto copy_2d_data(int width, int height, const std::vector<geo::Pt3<float>> &data) -> void;

    auto load_2d_image_file_data(const std::string &pathTexture, bool flip, int targetNbChannels = 3) -> bool;
    auto write_2d_image_file_data(const std::string &pathTexture) -> bool;

protected:

    bool m_hdr = false;
    std::vector<unsigned char> m_data;
    int m_nbChannels = 0;
    std::vector<int> m_sizes;
};





}
