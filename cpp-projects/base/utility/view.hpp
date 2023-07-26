
/*******************************************************************************
** Toolset-base                                                               **
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
#include <utility>
#include <functional>

namespace tool {

template<typename T>
class View2D{
public:

    View2D(T* d, size_t w, size_t h) : width(w), height(h), data(d){
    }

    T at(size_t id) const{
        return data[id];
    }

    T &at(size_t id) {
        return data[id];
    }

    T at(size_t i, size_t j) const{
        return data[j*width+i];
    }

    T &at(size_t i, size_t j) {
        return data[j*width+i];
    }

    bool edge(size_t i, size_t j) const{
        return (i == 0 || j == 0 || i == width-1 || j == height-1);
    }

    // coordinate not in edge
    std::pair<size_t, T> neighbours_8(size_t i, size_t j, std::function<bool(T value)> f) const{

        size_t count = 0;
        T mean;
        if(edge(i,j)){
            return std::make_pair(count, mean);
        }

        for(size_t kk = i-1; kk < i+2; ++kk){
            for(size_t ll = j-1; ll < j+2; ++ll){
                if(kk == i && ll == j){
                    continue;
                }
                if(f(at(kk,ll))){
                    mean += at(kk,ll);
                    ++count;
                }
            }
        }
        return std::make_pair(count, mean);
    }

    size_t width;
    size_t height;
private:
    T *data = nullptr;
};
}
