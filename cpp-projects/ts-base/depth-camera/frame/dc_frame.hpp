
/*******************************************************************************
** Toolset-ts-depth-base                                                      **
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

#pragma once

// base
#include "image/frame.hpp"
#include "utility/unordered_map.hpp"

// local
#include "depth-camera/dc_types.hpp"
// #include "utility/visit.hpp"

namespace tool::cam{

struct DCFrame : img::Frame{

    DCMode mode;
    umap<DCInfoType,         std::int64_t> infosB;
    umap<DCDataBufferType,   BinaryBuffer> datasB;
    umap<DCImageBufferType,  ImageBufferV> imagesB;
    umap<DCVolumeBufferType, VolumeBufferV> volumesB;

    auto infos() -> void;

    // insert buffer
    auto insert_data_buffer(DCDataBufferType type, const std::span<const std::byte> iData) -> std::span<std::byte>{

        if(!datasB.contains(type)){
            datasB.insert({type, BinaryBuffer{}});
        }

        auto &dataB = datasB[type];
        dataB.resize(iData.size());
        std::copy(iData.begin(), iData.end(), dataB.begin());
        return dataB.span();
    }

    template <class T>
    auto insert_image_buffer(DCImageBufferType type, size_t width, size_t height, const std::span<const T> iData) -> std::span<T>{

        if(!imagesB.contains(type)){
            imagesB.insert({type, ImageBuffer<T>{}});
        }

        auto &imageB = std::get<ImageBuffer<T>>(imagesB[type]);
        imageB.resize_image(width, height);
        std::copy(iData.begin(), iData.end(), imageB.begin());
        return imageB.span();
    }

    template <class T>
    auto insert_image_buffer(DCImageBufferType type, size_t width, size_t height, std::optional<T> defaultValue = std::nullopt) -> std::span<T>{

        if(!imagesB.contains(type)){
            imagesB.insert({type, ImageBuffer<T>{}});
        }

        auto &imageB = std::get<ImageBuffer<T>>(imagesB[type]);
        imageB.resize_image(width,height);
        if(defaultValue.has_value()){
            imageB.fill(defaultValue.value());
        }
        return imageB.span();
    }

    auto insert_volume_buffer(DCVolumeBufferType type, const geo::ColorCloud &cloud = {}) -> geo::ColorCloud*{

        if(!volumesB.contains(type)){
            volumesB.insert({type, cloud});
        }

        return &std::get<geo::ColorCloud>(volumesB[type]);
    }

    // get buffer
    auto data_buffer(DCDataBufferType type) -> BinaryBuffer*{
        if(datasB.contains(type)){
            return &datasB[type];
        }
        return nullptr;
    }

    template <class T>
    auto image_buffer(DCImageBufferType type) -> ImageBuffer<T>*{
        if(imagesB.contains(type)){
            return &std::get<ImageBuffer<T>>(imagesB[type]);
        }
        return nullptr;
    }

    template <class T>
    auto volume_buffer(DCVolumeBufferType type) -> T*{
        if(volumesB.contains(type)){
            return &std::get<T>(volumesB[type]);
        }
        return nullptr;
    }

    // get buffer size
    auto data_buffer_size(DCDataBufferType type) -> size_t{
        if(datasB.contains(type)){
            return datasB[type].size();
        }
        return 0;
    }

    // auto image_buffer_size(DCImageBufferType type) -> size_t{

    //     if(imagesB.contains(type)){

    //         const auto &imageB = imagesB[type];
    //         std::visit(Visitor{
    //             [](const ImageBuffer<ColorRGBA8> &sImg){
    //                 return sImg.size();
    //             },
    //             [](const ImageBuffer<ColorRGB8> &sImg){
    //                 return sImg.size();
    //             },
    //             [](const ImageBuffer<ColorGray8> &sImg){
    //                 return sImg.size();
    //             },
    //             [](const ImageBuffer<ColorGray16> &sImg){
    //                 return sImg.size();
    //             }
    //             }, imageB
    //         );
    //     }
    //     return 0;
    // }

    // auto volume_buffer_size(DCVolumeBufferType type) -> size_t{
    //     if(volumesB.contains(type)){
    //         auto &volumeB = volumesB[type];
    //         std::visit(Visitor{
    //             [&](geo::ColorCloud &cloud){
    //                 return cloud.size();
    //             }
    //         }, volumeB
    //         );
    //     }
    //     return 0;
    // }
};

struct DCDeprecatedFrame : img::Frame{

    // info
    DCMode mode;

    // images
    ImageBuffer<img::ColorRGBA8> rgbaColor;          // color-sized
    ImageBuffer<img::ColorRGBA8> rgbaDepthSizedColor;// depth-sized
    ImageBuffer<img::ColorRGB8>  rgbDepth;           // depth-sized
    ImageBuffer<img::ColorRGB8>  rgbInfra;           // depth-sized
    ImageBuffer<img::ColorGray8> grayBodiesIdMap;    // depth-sized
    ImageBuffer<std::uint16_t> depth;           // depth-sized
    ImageBuffer<std::uint16_t> infra;           // depth-sized

    geo::ColorCloud cloud;                // valid-vertices sized

    // binary encoding
    BinaryBuffer calibration;
    BinaryBuffer imu;

    // audio
    // TODO: add nbChannels variables and use a simple vector
    std::vector<std::array<float, 7>> audioFrames;

    // bodyTracking
    Buffer<DCBody> bodyTracking;

    // utilities
    auto compute_rgb_depth_image(ImageBuffer<img::ColorRGB8> &rgbDepth) -> void;
    auto compute_rgb_infra_image(ImageBuffer<img::ColorRGB8> &rgbInfra) -> void;
};



}
