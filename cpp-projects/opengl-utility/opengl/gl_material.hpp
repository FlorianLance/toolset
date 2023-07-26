
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
#include "graphics/material.hpp"

// local
#include "opengl/buffer/uniform_buffer_object.hpp"
#include "opengl/buffer/buffer-utility.hpp"

namespace tool::gl{

class MaterialUBO : public UBO{

public:

    void generate() override{

        if(m_id != 0){
            std::cerr << "[GL] UBO already generated: " << m_id << "\n";
            return;
        }
        glCreateBuffers(1, &m_id);
    }

    std::string get_block_name() const override{
        return nameStr;
    }

    std::vector<std::string> get_elements_name() const override{
        return elementsStr;
    }

    void update(graphics::MaterialInfo &data){
        BufferUtility::copy(data.Ka,           m_offsets[0], m_blockBuffer);
        BufferUtility::copy(data.Kd,           m_offsets[1], m_blockBuffer);
        BufferUtility::copy(data.Ks,           m_offsets[2], m_blockBuffer);
        BufferUtility::copy(data.Shininess,    m_offsets[3], m_blockBuffer);
        update_data(m_blockBuffer.data(), static_cast<GLint>(m_blockBuffer.size()));
    }

protected:

    static inline const std::string nameStr = "MaterialInfo";
    static inline const std::vector<std::string> elementsStr = {
        "MaterialInfo.Ka",
        "MaterialInfo.Kd",
        "MaterialInfo.Ks",
        "MaterialInfo.Shininess"
    };
};

class LightUBO : public UBO{

public:

    void generate() override{

        if(m_id != 0){
            std::cerr << "[GL] UBO already generated: " << m_id << "\n";
            return;
        }
        glCreateBuffers(1, &m_id);
    }

    std::string get_block_name() const override{
        return nameStr;
    }

    std::vector<std::string> get_elements_name() const override{
        return elementsStr;
    }

    void update(graphics::LightInfo &data){
        BufferUtility::copy(data.Position,  m_offsets[0], m_blockBuffer);
        BufferUtility::copy(data.La,        m_offsets[1], m_blockBuffer);
        BufferUtility::copy(data.Ld,        m_offsets[2], m_blockBuffer);
        BufferUtility::copy(data.Ls,        m_offsets[3], m_blockBuffer);
        update_data(m_blockBuffer.data(), static_cast<GLint>(m_blockBuffer.size()));
    }

protected:

    static inline const std::string nameStr = "LightInfo";
    static inline const std::vector<std::string> elementsStr = {
        "LightInfo.Position",
        "LightInfo.La",
        "LightInfo.Ld",
        "LightInfo.Ls",
    };
};


}


