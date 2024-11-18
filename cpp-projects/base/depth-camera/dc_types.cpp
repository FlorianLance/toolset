

/*******************************************************************************
** Toolset-base                                                               **
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


#include "dc_types.hpp"

using namespace tool::cam;

auto DCModeInfos::initialize(DCMode mode) -> void{

    m_mode = mode;

    // reset capture id
    m_idCapture      = 0;

    // reset formats/fps/resolution
    m_fps = dc_framerate(mode);
    // m_timeoutMs = 1;

    // m_timeoutMs = 10;
    // switch (m_fps) {
    // case DCFramerate::F30:
    //     m_timeoutMs = 100;//40;
    //     break;
    // case DCFramerate::F15:
    //     m_timeoutMs = 200;//70;
    //     break;
    // case DCFramerate::F5:
    //     m_timeoutMs = 500;//250;
    //     break;
    // default:
    //     m_timeoutMs = 100;//40;
    //     break;
    // }
    m_imageFormat       = dc_image_format(mode);
    m_colorResolution   = dc_color_resolution(mode);
    m_depthResolution   = dc_depth_resolution(mode);

    // reset flags
    m_hasColor          = dc_has_color(mode);
    m_hasDepth          = dc_has_depth(mode);
    m_hasInfra          = dc_has_infra(device());

    // reset sizes
    m_colorWidth        = dc_color_width(m_colorResolution);
    m_colorHeight       = dc_color_height(m_colorResolution);
    m_colorSize         = dc_color_size(m_colorResolution);
    m_depthWidth        = dc_depth_width(m_depthResolution);
    m_depthHeight       = dc_depth_height(m_depthResolution);
    m_depthSize         = dc_depth_size(m_depthResolution);
    m_depthRange        = dc_depth_range(mode);
    m_infraWidth        = m_hasInfra ? m_depthWidth   : 0;
    m_infraHeight       = m_hasInfra ? m_depthHeight  : 0;
    m_infraSize         = m_hasInfra ? m_depthSize    : 0;
}


