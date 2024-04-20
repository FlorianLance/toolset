
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

// base
#include "graphics/material/material.hpp"
#include "graphics/light.hpp"

// local
#include "opengl/shader/uniform_buffer_object.hpp"


namespace tool::gl{

class BufferUtility{

public:

    // static auto copy(float &value, GLint offset, std::span<GLubyte> blockBuffer) -> void{
    //     std::memcpy(blockBuffer.data() + offset, &value, sizeof(GLfloat));
    // }

    template <typename T>
    static auto copy(const T &value, GLint offset, std::span<GLubyte> blockBuffer) -> void{
        std::copy(reinterpret_cast<const GLubyte*>(&value), reinterpret_cast<const GLubyte*>(&value) + sizeof (value), std::begin(blockBuffer) +  offset);
    }
    // static auto copy(const geo::Pt3f &value, GLint offset, std::span<GLubyte> blockBuffer) -> void{
    //     std::copy(reinterpret_cast<const GLubyte*>(&value), reinterpret_cast<const GLubyte*>(&value) + sizeof (value), std::begin(blockBuffer) +  offset);
    // }
    // static auto copy(const geo::Pt4f &value, GLint offset, std::span<GLubyte> blockBuffer) -> void{
    //     std::copy(reinterpret_cast<const GLubyte*>(&value), reinterpret_cast<const GLubyte*>(&value) + sizeof (value), std::begin(blockBuffer) +  offset);
    // }
};


class MaterialUBO : public UBO{

public:

    auto get_block_name() const -> const std::string_view override {
        return "MaterialInfo"sv;
    }

    auto get_elements_name() const -> std::span<const std::string_view> override{
        return std::span<const std::string_view>(elementsStr.data(), elementsStr.size());
    }

    auto update(graphics::MaterialInfo &data) -> void{
        BufferUtility::copy(data.Ka,           m_offsets[0], m_blockBuffer);
        BufferUtility::copy(data.Kd,           m_offsets[1], m_blockBuffer);
        BufferUtility::copy(data.Ks,           m_offsets[2], m_blockBuffer);
        BufferUtility::copy(data.Shininess,    m_offsets[3], m_blockBuffer);
        update_data(m_blockBuffer.data(), static_cast<GLint>(m_blockBuffer.size()));
    }

protected:

    static constexpr std::array<std::string_view,4> elementsStr = {
        "MaterialInfo.Ka"sv,
        "MaterialInfo.Kd"sv,
        "MaterialInfo.Ks"sv,
        "MaterialInfo.Shininess"sv
    };
};

class LightUBO : public UBO{

public:

    auto get_block_name() const -> const std::string_view override {
        return "LightInfo"sv;
    }

    auto get_elements_name() const -> std::span<const std::string_view> override{
        return std::span<const std::string_view>(elementsStr.data(), elementsStr.size());
    }

    auto update(graphics::LightInfo &data) -> void{
        BufferUtility::copy(data.Position,  m_offsets[0], m_blockBuffer);
        BufferUtility::copy(data.La,        m_offsets[1], m_blockBuffer);
        BufferUtility::copy(data.Ld,        m_offsets[2], m_blockBuffer);
        BufferUtility::copy(data.Ls,        m_offsets[3], m_blockBuffer);
        update_data(m_blockBuffer.data(), static_cast<GLint>(m_blockBuffer.size()));
    }

protected:

    static constexpr std::array<std::string_view,4> elementsStr = {
        "LightInfo.Position",
        "LightInfo.La",
        "LightInfo.Ld",
        "LightInfo.Ls",
    };
};


}


