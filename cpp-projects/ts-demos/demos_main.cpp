/*******************************************************************************
** Toolset-ts-demos                                                           **
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

// std
#include <iostream>
#include <filesystem>

// base
#include "utility/paths.hpp"

// local
#include "tests/samples/draw_samples_window.hpp"
#include "tests/biopac/biopac_control_window.hpp"


#include "geometry/vertices.hpp"

enum class DemoType{
    Samples,
    Biopac
};

float gen(){
    float v =rand()%1000*0.01f;
    if(rand()%2== 0){
        return v;
    }else{
        return -v;
    }
}


auto display(const tool::geo::Vertices3D &vertices) -> void{
    size_t id = 0;
    std::cout << "Size " << vertices.capacity() << " valid: " << vertices.size() << "\n";

    for(const auto v : vertices){
        if(id < vertices.size()){
            std::cout << id << " " << v;
            id++;
        }else{
            break;
        }
    }
}


int main(int, char *argv[]){

    tool::Paths::get()->initialize(argv);

    // init main window
    // # screen
    tool::geo::Screen screen(1600, 900, 0,0);
    // # gl context
    sf::ContextSettings context;
    context.depthBits         = 24;
    context.stencilBits       = 8;
    context.antialiasingLevel = 4;
    context.majorVersion      = 4;
    context.minorVersion      = 6;
    context.attributeFlags    = sf::ContextSettings::Attribute::Debug;

    DemoType demo = DemoType::Samples;

    std::unique_ptr<tool::graphics::BaseSfmlGlWindow> scene = nullptr;
    switch (demo) {
    case DemoType::Samples:
        scene = std::make_unique<tool::graphics::DrawSampleWindow>("Samples", screen, context);
        break;
    case DemoType::Biopac:
        scene = std::make_unique<tool::graphics::BiopacControlWindow>("Biopac controller", screen, context);
        break;
    default:
        return 0;
    }


    scene->init();
    scene->start();

    return 0;
}
