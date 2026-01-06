
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

namespace tool {

// names types
// # time
struct Seconds     {double v;}; // seconds
struct Milliseconds{double v;}; // milliseconds
struct SecondsTS     {double v;}; // seconds timestamp
struct MillisecondsTS{double v;}; // milliseconds timestamp
struct Ratio       {double v;}; // between 0 and 1
// # ui
struct Index       {int v;};
//struct WidthPix    {int v;}; // width in pixels
//struct HeightPix   {int v;}; // height in pixels
//struct Width       {double v;};
//struct Height      {double v;};

struct VoidV{};
struct UndefinedV{};


template <typename T>
constexpr static size_t to_size_t(T v) noexcept {
    return static_cast<size_t>(v);
}

template <typename T>
constexpr static int to_int(T v) noexcept {
    return static_cast<int>(v);
}
}
