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

// base
#include "utility/logger.hpp"

// local
#include "tests.hpp"

using namespace tool;

int main(int argc, char *argv[]){

    std::puts(std::format("Argc: [{}]\n", argc).c_str());
    for(int ii = 0; ii < argc; ++ii){
        std::puts(std::format("\t->: [{}]\n", argv[ii]).c_str());
    }

    std::puts("Start tests.\n");
    
    auto logger = std::make_unique<Logger>();
    logger->init("./");
    // logger->message_signal.connect([&](std::string message){
    //     std::puts(message.c_str());
    // });
    // logger->warning_signal.connect([&](std::string warning){
    //     std::puts(warning.c_str());
    // });
    // logger->error_signal.connect([&](std::string error){
    //     std::puts(error.c_str());
    // });
    Logger::set_logger_instance(std::move(logger));

    simd();
    // time();
    // microphone();
    // dc_device_idle();
    // femto_mega();
    // raw_orbbec_femto();
    // raw_kinect2();
    // kinect2();
    // test_kinect4();
    // test_raw_k4device();
    // pcl_encoding();
    // process_kvid();
    // dc_player_test();
    // test_dc_player_merge("E:/_kvid/21-02-24-k4x4-3.kvid");
    // dc_settings();
    // encode1();
    // encode2();
    // convert_kvid("D:/test_kvid/thirdPartR.kvid", "D:/test_kvid/thirdPartR_conv.kvid");


    return 0;

}





