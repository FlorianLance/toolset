
/*******************************************************************************
** Toolset-ts-global-app                                                      **
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

#include "tests.hpp"

auto raw_kinect2() -> void{
    // ...
}

auto kinect2() -> void{

    //    using namespace std::chrono_literals;
    //    cam::K2Device kinect;
    //    if(!kinect.open(cam::K2FrameRequest::compressed_color_mesh)){
    //        std::cerr << "Cannot init kinect 2\n";
    //        return;
    //    }

    //    std::cout << "init\n";
    //    std::this_thread::sleep_for(1000ms);
    //    std::cout << "try to grab\n";

    //    for(int ii = 0; ii < 10000; ++ii){
    //        if(auto newFrame = kinect.get_kinect_data(); newFrame.has_value()){
    //            std::cout << "-";
    //        }else{
    //            std::cout << "E";
    //        }
    //        std::this_thread::sleep_for((1000/33)*1ms);
    //    }

    //    std::cout << "close\n";
    //    kinect.close();
}
