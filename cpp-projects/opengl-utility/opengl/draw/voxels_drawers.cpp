

/*******************************************************************************
** Toolset-opengl-utility                                                     **
** MIT License                                                                **
** Copyright (c) [2024] [Florian Lance]                                       **
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

#include "voxels_drawers.hpp"

// base
#include "utility/logger.hpp"

// local
#include "voxels_renderer.hpp"

using namespace tool::geo;
using namespace tool::gl;

auto VoxelsDrawer2::initialize(bool dynamic, std::span<const geo::Pt3<int>> vertices, std::span<const geo::Pt3f> colors) -> void{

    auto vm = dynamic_cast<VoxelsRenderer*>(m_vaoRenderer.get());
    if(dynamic){
        vm->positionBufferUsage  = GL_DYNAMIC_STORAGE_BIT;
        vm->colorBufferUsage     = GL_DYNAMIC_STORAGE_BIT;
    }
    vm->initialize(!colors.empty());
    if(!vm->load_data(vertices, colors)){
        Log::error("[VoxelsDrawer::initialize] Error during loading.\n"sv);
    }
}

auto VoxelsDrawer2::update(std::span<const geo::Pt3<int>> vertices, std::span<const geo::Pt3f> colors) -> void{

    auto vm = dynamic_cast<VoxelsRenderer*>(m_vaoRenderer.get());
    if(!vm->update_data(vertices, 0, colors, 0)){
        Log::error("[VoxelsDrawer::update] Error during update.\n"sv);
        return;
    }
}
