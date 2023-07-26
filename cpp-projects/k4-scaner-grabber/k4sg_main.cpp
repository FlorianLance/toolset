
/*******************************************************************************
** Toolset-k4-scaner-grabber                                                  **
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

// base
#include "utility/paths.hpp"
#include "utility/logger.hpp"

// local
#include "k4sg_controller.hpp"

using namespace tool;


int main(int argc, char *argv[]){

    // init paths
    tool::Paths::initialize(argv);

    // define grabber id from command line arg
    Logger::message("Start K4 scaner grabber.\n");
    Logger::message(std::format("Args {}.\n", argc));
    bool argsFound = false;
    size_t id = 0;
    for(int ii = 0; ii < argc; ++ii){
        std::string arg = argv[ii];
        if(arg.starts_with("-i")){
            argsFound = true;
            id = std::stoi(arg.substr(2,1));
        }
    }
    if(!argsFound){
        Logger::warning("No id argument found. ID set to '0'\n");
    }

//    using namespace std::chrono;
//    if(id == 0){
//        tool::Time::offsetNano = duration_cast<nanoseconds>(nanoseconds(3000000000));
//    }else if(id == 1){
//        tool::Time::offsetNano = duration_cast<nanoseconds>(nanoseconds(5000000000));
//    }else if(id == 2){
//        tool::Time::offsetNano = duration_cast<nanoseconds>(nanoseconds(7000000000));
//    }else if(id == 3){
//        tool::Time::offsetNano = duration_cast<nanoseconds>(nanoseconds(9000000000));
//    }
//    Logger::message(std::format("Offset {}.\n", tool::Time::offsetNano.count()));

    Logger::message(std::format("ID {}\n", id));
    K4SGSettings::idLocalGrabber = static_cast<unsigned int>(id);


    // init controller
    tool::K4SGController controller;
    if(controller.initialize()){
        controller.start();
        return 0;
    }

    return -1;
}
