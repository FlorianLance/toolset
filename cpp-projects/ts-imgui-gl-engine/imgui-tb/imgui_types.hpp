

/*******************************************************************************
** Toolset-ts-imgui-gl-engine                                                 **
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
#include <string>

namespace tool{

using ImGuiId = unsigned int;

struct ImGuiDragS{
    float widthDrag      = 100.f;
    bool decButton       = true;
    bool incButton       = true;
    bool defaultButton   = true;
    bool displayText     = true;
    bool displayTextLeft = false;
};

struct ImGuiIntS{
    int defaultValue = 0;
    int min          = 0;
    int max          = 100;
    float speedDrag  = 0.1f;
    int speedInc     = 1;
};

struct ImGuiFloatS{
    float defaultValue = 0.f;
    float min          = 0.f;
    float max          = 1.f;
    float speedDrag    = 0.01f;
    float speedInc     = 0.1f;
    std::string format = "%.3f";
};
}
