
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

// glew
#include <GL/glew.h>

// base
#include "geometry/point3.hpp"
#include "geometry/point4.hpp"


namespace tool::gl {

class BufferUtility{

public:

    static void copy(float &value, GLint offset, std::vector<GLubyte> &blockBuffer){
        std::memcpy(blockBuffer.data() + offset, &value, sizeof(GLfloat));
    }
    static void copy(geo::Pt3f &value, GLint offset, std::vector<GLubyte> &blockBuffer){
        std::copy(reinterpret_cast<GLubyte*>(value.array.data()), reinterpret_cast<GLubyte*>(value.array.data()) + sizeof (value), std::begin(blockBuffer) +  offset);
    }
    static void copy(geo::Pt4f &value, GLint offset, std::vector<GLubyte> &blockBuffer){
        std::copy(reinterpret_cast<GLubyte*>(value.array.data()), reinterpret_cast<GLubyte*>(value.array.data()) + sizeof (value), std::begin(blockBuffer) +  offset);
    }
};
}
