
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


#include "paths.hpp"

// boost
#include <boost/asio/ip/host_name.hpp>

// local
#include "utility/logger.hpp"

using namespace tool;
using namespace std::filesystem;
using namespace std::string_view_literals;

// https://stackoverflow.com/questions/1528298/get-path-of-executable

struct Global{
    static inline std::unique_ptr<Paths> instance = nullptr;
};

auto Paths::get() -> Paths*{
    if(Global::instance == nullptr){
        Global::instance = std::make_unique<Paths>();
    }
    return Global::instance.get();
}

auto Paths::get_hostname() -> std::string{
    return boost::asio::ip::host_name();
}


auto Paths::initialize(char *argv[]) -> void{

    if(m_initialized){
        Log::warning("[Paths::initialize] Already initialized.\n"sv);
        return;
    }

    // retrieve hostname
    hostName        = get_hostname();

    executable      = weakly_canonical(path(argv[0]));
    applicationDir  = executable.parent_path();
    resourcesDir    = applicationDir    / "resources"sv;
    shadersDir      = resourcesDir      / "shaders"sv;
    dataDir         = applicationDir    / "data"sv;
    logsDir         = applicationDir    / "logs"sv;
    outputDir       = applicationDir    / "output"sv;
    configDir       = applicationDir    / "config"sv;
    settingsDir     = configDir         / "settings"sv;
    networkDir      = configDir         / "network"sv;
    calibrationDir  = configDir         / "calibration"sv;

    m_initialized = true;
}

auto Paths::get_shader(std::string_view name, std::string_view ext) -> std::filesystem::path{
    return shadersDir / std::format("{}{}"sv, name, ext);
}




