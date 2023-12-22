
/*******************************************************************************
** Toolset-3d-engine                                                          **
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
#include <vector>
#include <string>

// local
#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui/imgui.h"


namespace tool::graphics {

struct ImguiLogs{

    ImGuiTextBuffer buffer;
    ImGuiTextFilter filter;
    ImVector<int> lineOffsets; // Index to lines offset. We maintain this with AddLog() calls.
    bool autoScroll = true;  // Keep scrolling if already at the bottom.

    ImguiLogs();
    void clear();

    void add_log(const char* fmt, ...){// IM_FMTARGS(2){
        int old_size = buffer.size();
        va_list args;
        va_start(args, fmt);
        buffer.appendfv(fmt, args);
        va_end(args);
        for (int new_size = buffer.size(); old_size < new_size; old_size++){
            if (buffer[old_size] == '\n'){
                lineOffsets.push_back(old_size + 1);
            }
        }
    }

    void draw(const char* name);
};

struct ImguiLogs2{


    std::vector<std::string> logs;
    ImguiLogs2(){}
    void draw(const char* name);
};
}
