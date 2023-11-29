
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

#pragma once

// local
#include "k4_frame.hpp"
#include "k4_compressed_frame.hpp"

namespace tool::camera{


struct K4FrameUncompressor{

    enum ColorFormat : std::int8_t {
        RGB = 0,RGBA = 7,BGR = 1,BGRA = 8
    };

    K4FrameUncompressor();
    ~K4FrameUncompressor();

    // uncompress data
    auto uncompress_jpeg_8_bits_data(size_t width, size_t height, ColorFormat format, size_t jpegSize, std::uint8_t *jpegData, std::uint8_t *data) -> bool; // data not resized
    auto uncompress_jpeg_8_bits_data(size_t width, size_t height, ColorFormat format, std::vector<uint8_t> &jpegData, std::vector<std::uint8_t> &data) -> bool;
    auto uncompress_jpeg_8_bits_data(size_t width, size_t height, ColorFormat format, std::vector<uint8_t> &jpegData, std::vector<geo::Pt3<uint8_t>> &data) -> bool;
    auto uncompress_jpeg_8_bits_data(size_t width, size_t height, ColorFormat format, std::vector<uint8_t> &jpegData, std::vector<geo::Pt4<uint8_t>> &data) -> bool;

    auto uncompress_lossless_16_bits_128padded_data(size_t validVerticesCount, std::vector<std::uint8_t> &encoded, std::vector<std::uint16_t> &decodedVertices) -> bool;

    // convert decoded data
    auto convert_to_depth_image(K4Mode mode, size_t depthWidth, size_t depthHeight, const std::vector<uint16_t> &uncompressedDepth, std::vector<geo::Pt3<uint8_t>> &imageDepth) -> void;
    auto convert_to_infra_image(size_t infraWidth, size_t infraHeight, const std::vector<uint16_t> &uncompressedInfra, std::vector<geo::Pt3<uint8_t>> &imageInfra) -> void;
    auto generate_cloud(K4Mode mode, size_t dephtWidth, size_t depthHeight, k4a_calibration_t &calibration, const std::vector<uint16_t> &uncompressedDepth) -> void;


    // convert to cloud
    // # colored cloud data
    // ## from vertices
    auto convert_to_cloud(K4Mode mode, size_t validVerticesCount, geo::ColoredCloudData &cloudData, std::vector<geo::Pt3<uint8_t>> &uncompressedColor, std::vector<uint16_t> &uncompressedVertices) -> void;
    // ## from cloud buffer
    auto convert_to_cloud(size_t validVerticesCount, geo::ColoredCloudData &cloudData, std::vector<geo::Pt4<uint8_t>> &uncompressedColor, std::vector<std::uint16_t> &uncompressedDepth, geo::Pt3<int16_t> *cloudBuffer) -> void;
    // # vertices,colors pointers
    // ## from vertices
    auto convert_to_cloud(K4Mode mode, size_t validVerticesCount, geo::Pt3f *vertices, geo::Pt3f *colors,          std::vector<geo::Pt3<uint8_t>> &uncompressedColor, std::vector<std::uint16_t> &uncompressedVertices) -> void;
    auto convert_to_cloud(K4Mode mode, size_t validVerticesCount, geo::Pt3f *vertices, geo::Pt4f *colors,          std::vector<geo::Pt3<uint8_t>> &uncompressedColor, std::vector<std::uint16_t> &uncompressedVertices) -> void;
    auto convert_to_cloud(K4Mode mode, size_t validVerticesCount, geo::Pt3f *vertices, geo::Pt3<uint8_t> *colors,  std::vector<geo::Pt3<uint8_t>> &uncompressedColor, std::vector<std::uint16_t> &uncompressedVertices) -> void;
    auto convert_to_cloud(K4Mode mode, size_t validVerticesCount, geo::Pt3f *vertices, geo::Pt4<uint8_t> *colors,  std::vector<geo::Pt3<uint8_t>> &uncompressedColor, std::vector<std::uint16_t> &uncompressedVertices) -> void;
    // ## from cloud bgeo::uffer
    auto convert_to_cloud(geo::Pt3f *vertices, geo::Pt3f *colors,          std::vector<geo::Pt3<uint8_t>> &uncompressedColor, std::vector<std::uint16_t> &uncompressedDepth, geo::Pt3<int16_t> *cloudBuffer) -> void;
    auto convert_to_cloud(geo::Pt3f *vertices, geo::Pt4f *colors,          std::vector<geo::Pt3<uint8_t>> &uncompressedColor, std::vector<std::uint16_t> &uncompressedDepth, geo::Pt3<int16_t> *cloudBuffer) -> void;
    auto convert_to_cloud(geo::Pt3f *vertices, geo::Pt3<uint8_t> *colors,  std::vector<geo::Pt3<uint8_t>> &uncompressedColor, std::vector<std::uint16_t> &uncompressedDepth, geo::Pt3<int16_t> *cloudBuffer) -> void;
    auto convert_to_cloud(geo::Pt3f *vertices, geo::Pt4<uint8_t> *colors,  std::vector<geo::Pt3<uint8_t>> &uncompressedColor, std::vector<std::uint16_t> &uncompressedDepth, geo::Pt3<int16_t> *cloudBuffer) -> void;
    // # K4 vertex mesh data
    // ## from vertices
    auto convert_to_cloud(K4Mode mode, size_t validVerticesCount, K4VertexMeshData *vertices, std::vector<geo::Pt3<uint8_t>> &uncompressedColor, std::vector<std::uint16_t> &uncompressedVertices) -> void;
    // ## from cloud buffer
    auto convert_to_cloud(K4VertexMeshData *vertices, std::vector<geo::Pt3<uint8_t>> &uncompressedColor, std::vector<std::uint16_t> &uncompressedDepth, geo::Pt3<int16_t> *cloudBuffer) -> void;

    // uncompress to frame
    auto uncompress(K4CompressedFrame *cFrame, K4Frame &frame) -> bool;
    auto uncompress(K4CompressedFrame *cFrame, geo::Pt3f *vertices, geo::Pt3f *colors) -> bool;
    auto uncompress(K4CompressedFrame *cFrame, geo::Pt3f *vertices, geo::Pt4f *colors) -> bool;
    auto uncompress(K4CompressedFrame *cFrame, geo::Pt3f *vertices, geo::Pt3<std::uint8_t> *colors) -> bool;
    auto uncompress(K4CompressedFrame *cFrame, geo::Pt3f *vertices, geo::Pt4<std::uint8_t> *colors) -> bool;
    auto uncompress(K4CompressedFrame *cFrame, K4VertexMeshData *vertices) -> int;



private:

    struct Impl;
    std::unique_ptr<Impl> i;
};
}









