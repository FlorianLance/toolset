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

// local
#include "test_ak_device.hpp"
#include "test_ob_device.hpp"

#include "geometry/matrix4.hpp"

using namespace tool::geo;

int main(){

    // auto id = Mat4f::identity();
    // id.at(0) = 2;
    // id.at(1,1) = 3;
    // std::cout << "mat " << id << "\n";

    // auto mat3 = cut(id,3,3);
    // auto inv = inverse(mat3);

    // std::cout << mat3 << "\n";
    // std::cout << inv << "\n";
    // std::cout << inv * mat3 << "\n";

    // auto tp = transpose(inv);
    // std::cout << tp << "\n";


    // test_raw_orbbec_femto();
    // test_raw_orbbec_femto();
    // test_raw_k4device();
    // test_kinect4();
    test_femto_mega();
    return 0;
}



