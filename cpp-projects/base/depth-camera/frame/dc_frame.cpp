
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

#include "dc_frame.hpp"

using namespace tool::geo;
using namespace tool::cam;

auto DCDeprecatedFrame::compute_rgb_depth_image(ImageBuffer<ColorRGB8> &rgbDepth) -> void{

    if(!depth.is_valid_image()){
        return;
    }

    static constexpr std::array<Pt3f, 5> depthGradient ={
        Pt3f{0.f,0.f,1.f},
        {0.f,1.f,1.f},
        {0.f,1.f,0.f},
        {1.f,1.f,0.f},
        {1.f,0.f,0.f},
    };

    rgbDepth.width  = depth.width;
    rgbDepth.height = depth.height;
    rgbDepth.resize(depth.width*depth.height);

    const auto dRange = dc_depth_range(mode)*1000.f;
    const auto diff = dRange(1) - dRange(0);

    // convert data
    for(size_t id = 0; id < depth.size(); ++id){

        if(depth[id] == dc_invalid_depth_value){
            rgbDepth[id] = ColorRGB8{0,0,0};
            continue;;
        }

        float vF = (static_cast<float>(depth[id]) - dRange(0))/diff;
        float intPart;
        float decPart = std::modf((vF*(depthGradient.size()-1)), &intPart);
        size_t idG = static_cast<size_t>(intPart);

        auto col = depthGradient[idG]*(1.f-decPart) + depthGradient[idG+1]*decPart;
        rgbDepth[id] = ColorRGB8{
            static_cast<uint8_t>(255*col.x()),
            static_cast<uint8_t>(255*col.y()),
            static_cast<uint8_t>(255*col.z())
        };
    }
}

auto DCDeprecatedFrame::compute_rgb_infra_image(ImageBuffer<ColorRGB8> &rgbInfra) -> void{

    if(!infra.is_valid_image()){
        return;
    }

    rgbInfra.width  = infra.width;
    rgbInfra.height = infra.height;
    rgbInfra.resize(infra.size());

    const float max = 2000;
    for(size_t id = 0; id < infra.size(); ++id){

        float vF = static_cast<float>(infra[id]);
        if(vF > max){
            vF = max;
        }
        vF/=max;
        rgbInfra[id] = ColorRGB8{
            static_cast<uint8_t>(255*vF),
            static_cast<uint8_t>(255*vF),
            static_cast<uint8_t>(255*vF)
        };
    }
}

#include "utility/logger.hpp"

auto tool::cam::DCFrame::infos() -> void{

    Log::message(std::format("Infos; [{}]\n", infosB.size()));
    for(const auto &info : infosB){
        Log::message(std::format("\t[{}] : [{}]\n", static_cast<int>(info.first), info.second));
    }

    Log::message(std::format("Data; [{}]\n", datasB.size()));
    for(const auto &data : datasB){
        Log::message(std::format("\t[{}] : [{}]\n", static_cast<int>(data.first), data.second.size()));
    }

    Log::message(std::format("Image; [{}]\n", imagesB.size()));
    for(const auto &image : imagesB){

        if(auto img = std::get_if<ImageBuffer<ColorRGBA8>>(&image.second); img != nullptr){
            Log::message(std::format("\t[{}] : RGBA8 [{}] [{}]\n", static_cast<int>(image.first), img->width, img->height));
        }else if(auto img = std::get_if<ImageBuffer<ColorRGB8>>(&image.second); img != nullptr){
            Log::message(std::format("\t[{}] : RGB8 [{}] [{}]\n", static_cast<int>(image.first), img->width, img->height));
        }else if(auto img = std::get_if<ImageBuffer<ColorGray8>>(&image.second); img != nullptr){
            Log::message(std::format("\t[{}] : Gray8 [{}] [{}]\n", static_cast<int>(image.first), img->width, img->height));
        }else if(auto img = std::get_if<ImageBuffer<ColorGray16>>(&image.second); img != nullptr){
            Log::message(std::format("\t[{}] : Gray16 [{}] [{}]\n", static_cast<int>(image.first), img->width, img->height));
        }
    }

    Log::message(std::format("Volume; [{}]\n", volumesB.size()));
    for(const auto &volume : volumesB){
        if(auto vol = std::get_if<ColorCloud>(&volume.second); vol != nullptr){
            Log::message(std::format("\t[{}] : Color cloud [{}]\n", static_cast<int>(volume.first), vol->size()));
        }
    }
}
