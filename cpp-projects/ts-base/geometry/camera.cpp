
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

#include "camera.hpp"

using namespace tool::geo;

auto Camera::set_rotation(Pt3d eulerAngles) -> void{
    m_orientation = {};
    rotate(eulerAngles);
}

auto Camera::rotate(Pt3d eulerAngles) -> void{
    auto yawQuat   = geo::Quatd::from_axis(m_up,        eulerAngles.x()); // Rotate around world up (Y-axis)
    auto pitchQuat = geo::Quatd::from_axis(m_right,     eulerAngles.y()); // Rotate around camera's right (X-axis)
    auto rollQuat  = geo::Quatd::from_axis(m_direction, eulerAngles.z()); // Rotate around camera's front (Z-axis)
    m_orientation = normalize(rollQuat * pitchQuat * yawQuat * m_orientation);
    update_vectors();
}


