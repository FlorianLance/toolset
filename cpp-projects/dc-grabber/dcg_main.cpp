
/*******************************************************************************
** Toolset-dc-grabber                                                         **
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
#include "utility/logger.hpp"
#include "utility/paths.hpp"
#include "depth-camera/settings/dc_settings_paths.hpp"

// local
#include "dcg_controller.hpp"
#include "depth-camera/settings/dc_settings_paths.hpp"

using namespace tool;
using namespace cam;

int main(int argc, char *argv[]){

    // check application arguments
    std::optional<size_t> id = std::nullopt;
    for(int ii = 0; ii < argc; ++ii){
        std::string arg = argv[ii];
        if(arg.starts_with("-i")){
            id = std::stoi(arg.substr(2,1));
        }
    }

    // init paths
    DCSettingsPaths::get()->initialize(argv, DCApplicationType::DCGrabber, id.has_value() ? id.value() : 0);
    
    auto logger = std::make_unique<Logger>();
    logger->init(Paths::get()->logsDir.string(), DCSettingsPaths::get()->logName);
    BaseLogger::set_logger_instance(std::move(logger));

    Log::message("Start DC grabber.\n"sv);
    Log::message(std::format("Args {}.\n"sv, argc));
    if(!id.has_value()){
        Log::warning("No id argument found, use 0 instead.\n"sv);
        id = 0;
    }

    // init controller
    DCGController controller;
    if(controller.initialize(id.value())){
        controller.start();
        Log::message("Exit DC grabber.\n"sv);
        return 1;
    }

    return -1;
}
