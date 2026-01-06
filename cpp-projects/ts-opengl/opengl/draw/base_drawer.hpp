
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

// base
#include "utility/buffer.hpp"


// local
#include "vao_renderer.hpp"
#include "opengl/shader/shader_program.hpp"

namespace tool::gl {

enum class DrawerType : std::int8_t{
    Points,
    Voxels,
    Lines,
    Triangles,
    Invalid
};

class BaseDrawer{
public:

    BaseDrawer(DrawerType type);
    virtual ~BaseDrawer(){
        if(m_vaoRenderer != nullptr){
            m_vaoRenderer->clean();
        }
    }    

    virtual auto draw() -> void;

    float scaleHint = 1.f;
    // gl::ShaderProgram *shader = nullptr;

    auto set_indice_count(size_t count) noexcept -> void{
        nIndicesToDraw = static_cast<GLsizei>(count);
    }

    Buffer<GLuint> texturesId;

protected:

    std::unique_ptr<VAORenderer> m_vaoRenderer = nullptr;
    DrawerType m_type = DrawerType::Invalid;
    std::optional<GLsizei> nIndicesToDraw = {};
};

class HierarchyDrawer2 : public BaseDrawer{

public:
    HierarchyDrawer2(DrawerType type) : BaseDrawer(type){}

    auto clean() -> void{
        children.clear();
        drawers.clear();
    }

    auto draw() -> void override;

protected:
    std::vector<std::unique_ptr<BaseDrawer>> drawers;
    std::vector<std::unique_ptr<HierarchyDrawer2>> children;
};


}
