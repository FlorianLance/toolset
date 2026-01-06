
/*******************************************************************************
** Toolset-ts-depth-camera                                                    **
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

// base
#include "utility/image_buffer.hpp"
#include "utility/unordered_map.hpp"
#include "image/frame.hpp"

// local
#include "depth-camera/dc_types.hpp"

namespace tool::cam{

struct DCDataFrame : img::Frame{

    DCMode mode;
    size_t validVerticesCount = 0;
    umap<DCInfoType, std::int64_t> infosB;
    umap<DCDataBufferType, std::tuple<DCCompressionMode, BinaryBuffer>> datasB;
    umap<DCImageBufferType, std::tuple<DCCompressionMode, BinaryImageBuffer>> imagesB;
    umap<DCVolumeBufferType, std::tuple<DCCompressionMode, BinaryBuffer>> volumesB;

    // insert buffers
    auto insert_data_buffer(DCDataBufferType type, DCCompressionMode cm, float data) -> BinaryBuffer*{
        return insert_data_buffer(type, cm, std::span<const std::byte>(reinterpret_cast<const std::byte*>(&data), 4));
    }
    auto insert_data_buffer(DCDataBufferType type, DCCompressionMode cm, const std::span<const float> iData) -> BinaryBuffer*{
        return insert_data_buffer(type, cm, std::span<const std::byte>(reinterpret_cast<const std::byte*>(iData.data()), iData.size_bytes()));
    }
    auto insert_data_buffer(DCDataBufferType type, DCCompressionMode cm, const std::span<const std::byte> iData) -> BinaryBuffer*{
        auto iterator = datasB.insert({type, {cm, BinaryBuffer{}}});
        if(iterator.second){
            auto &dataB = std::get<1>(iterator.first->second);
            dataB.resize(iData.size());
            std::copy(iData.begin(), iData.end(), dataB.begin());
            return &dataB;
        }
        return nullptr;
    }

    auto insert_image_buffer(DCImageBufferType type, DCCompressionMode cm) -> BinaryImageBuffer*{
        auto iterator = imagesB.insert({type, {cm, BinaryImageBuffer{}}});
        if(iterator.second){
            return &std::get<1>(iterator.first->second);
        }
        return nullptr;
    }

    auto insert_image_buffer(DCImageBufferType type, DCCompressionMode cm, size_t width, size_t height, size_t sizeType, const std::span<const std::byte> iData) -> BinaryImageBuffer*{
        auto iterator = imagesB.insert({type, {cm, BinaryImageBuffer{}}});
        if(iterator.second){
            auto &imageB = std::get<1>(iterator.first->second);
            imageB.resize_image(width, height, sizeType);
            std::copy(iData.begin(), iData.end(), imageB.begin());
            return &imageB;
        }
        return nullptr;
    }

    auto insert_volume_buffer(DCVolumeBufferType type, DCCompressionMode cm) -> BinaryBuffer*{
        auto iterator = volumesB.insert({type, {cm, BinaryBuffer{}}});
        if(iterator.second){
            return &std::get<1>(iterator.first->second);
        }
        return nullptr;
    }

    auto insert_volume_buffer(DCVolumeBufferType type, DCCompressionMode cm, const std::span<const std::byte> iData) -> BinaryBuffer*{
        auto iterator = volumesB.insert({type, {cm, BinaryBuffer{}}});
        if(iterator.second){
            auto &dataB = std::get<1>(iterator.first->second);
            dataB.resize(iData.size());
            std::copy(iData.begin(), iData.end(), dataB.begin());
            return &dataB;
        }
        return nullptr;
    }

    // get buffers
    auto data_buffer(DCDataBufferType type) -> BinaryBuffer*{
        if(datasB.contains(type)){
            return &std::get<1>(datasB[type]);
        }
        return nullptr;
    }

    auto image_buffer(DCImageBufferType type) -> BinaryImageBuffer*{
        if(imagesB.contains(type)){
            return &std::get<1>(imagesB[type]);
        }
        return nullptr;
    }

    auto volume_buffer(DCVolumeBufferType type) -> BinaryBuffer*{
        if(volumesB.contains(type)){
            return &std::get<1>(volumesB[type]);
        }
        return nullptr;
    }

    // get buffer size
    auto data_buffer_size(DCDataBufferType type) -> size_t{
        if(auto dataB = data_buffer(type); dataB != nullptr){
            return dataB->size();
        }
        return 0;
    }

    auto image_buffer_size(DCImageBufferType type) -> size_t{
        if(auto imageB = image_buffer(type); imageB != nullptr){
            return imageB->size();
        }
        return 0;
    }

    auto volume_buffer_size(DCVolumeBufferType type) -> size_t{
        if(auto volumeB = volume_buffer(type); volumeB != nullptr){
            return volumeB->size();
        }
        return 0;
    }

    // init
    auto init_from_file_stream(std::ifstream &file) -> void override;    
    auto init_from_data(std::span<const std::byte> data, size_t &offset) -> void override;
    // write
    auto write_to_file_stream(std::ofstream &file) -> void override;
    auto write_to_data(std::span<std::byte> data, size_t &offset) -> void override;

    auto data_size() const noexcept -> size_t override;
    
    auto init_from_file_stream_legacy3(std::ifstream &file) -> void;
    auto init_from_file_stream_legacy2(std::ifstream &file) -> void;
};


// deprecated
struct DCCompressedFrame : img::Frame{

    // infos
    DCMode mode;    
    size_t validVerticesCount = 0;

    // jpeg encoding
    BinaryImageBuffer jpegRGBA8Color;
    BinaryImageBuffer jpegRGBA8DepthSizedColor;
    BinaryImageBuffer jpegG8BodiesIdMap;

    // fast PFor encoding
    BinaryImageBuffer fpfDepth;
    BinaryImageBuffer fpfInfra;
    BinaryBuffer fpfColoredCloud;

    // binary encoding
    BinaryBuffer calibration;
    BinaryBuffer imu;

    // no encoding
    Buffer<DCBody> bodyTracking;
    std::vector<std::array<float, 7>> audioFrames;

    // getters
    auto data_size() const noexcept -> size_t override;

    // init
    auto init_from_file_stream(std::ifstream &file) -> void override;
    auto init_from_data(std::span<const std::byte> data, size_t &offset) -> void override;
    // write
    auto write_to_file_stream(std::ofstream &file) -> void override;
    auto write_to_data(std::span<std::byte> data, size_t &offset) -> void override;

    // ## super legacy
    auto init_legacy_cloud_frame_from_file_stream(std::ifstream &file) -> void;
    auto init_legacy_full_frame_from_file_stream(std::ifstream &file) -> void;

};
}


// type frame
//  -> data frame
//  -> display frame

// enum class DCTypeFrame : std::int8_t{
//     DataFrame = 0,
//     DisplayFrame,
//     Undefined
// };

// struct DCUFrame : Frame{
//     DCTypeFrame type = DCTypeFrame::Undefined;
//     umap<DCInfoType,                                        InfoV>          infosB;
//     umap<DCDataBufferType,   std::tuple<DCCompressionMode,  DataBufferV>>   datasB;
//     umap<DCImageBufferType,  std::tuple<DCCompressionMode,  ImageBufferV>>  imagesB;
//     umap<DCVolumeBufferType, std::tuple<DCCompressionMode,  VolumeBufferV>> volumesB;

//     // init
//     auto init_from_file_stream(std::ifstream &file) -> void override;
//     auto init_from_data(std::span<const std::byte> data, size_t &offset) -> void override;
//     // write
//     auto write_to_file_stream(std::ofstream &file) -> void override;
//     auto write_to_data(std::span<std::byte> data, size_t &offset) -> void override;
// };
