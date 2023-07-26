

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

#include "k4_frame_uncompressor.hpp"

// std
#include <execution>

// kinect
#include "k4a/k4a.hpp"

// turbojpg
#include <turbojpeg.h>

// turbopfor
#include "TurboPFor/vp4.h"

// local
// # utility
#include "utility/logger.hpp"
#include "utility/types.hpp"

using namespace tool;
using namespace tool::geo;
using namespace tool::camera;

struct K4FrameUncompressor::Impl{

    Impl(){
        jpegUncompressor = tjInitDecompress();

        const size_t defaultWidth = 640;
        const size_t defaultHeight = 576;
        const size_t defaultResolution = defaultWidth*defaultHeight;
        indicesDepths1D.reserve(defaultResolution);
        indicesValid1D.reserve(defaultResolution);
        decodedColorData.reserve(defaultResolution);
        decodedDepthData.reserve(defaultResolution);
        decodedVerticesData.reserve(defaultResolution);

        // generate k4a image for storing depth values
        depthImage = k4a::image::create(
            k4a_image_format_t::K4A_IMAGE_FORMAT_DEPTH16,
            to_int(defaultWidth),
            to_int(defaultHeight),
            static_cast<int32_t>(defaultWidth * 1 * sizeof(uint16_t))
        );

        // generate k4a image for storing cloud values
        pointCloudImage = k4a::image::create(K4A_IMAGE_FORMAT_CUSTOM,
            to_int(defaultWidth),
            to_int(defaultHeight),
            static_cast<int32_t>(defaultWidth * 3 * sizeof(int16_t))
        );
    }

    ~Impl(){
        tjDestroy(jpegUncompressor);
    }

    tjhandle jpegUncompressor = nullptr;
    std::vector<size_t> indicesDepths1D;

    std::vector<size_t> indicesValid1D;    
    std::vector<Pt3<uint8_t>> decodedColorData;
    std::vector<std::uint16_t> decodedDepthData;
    std::vector<std::uint16_t> decodedVerticesData;

    k4a::image depthImage;
    k4a::image pointCloudImage;
    std::tuple<K4Mode, std::optional<k4a_transformation_t>> modeTr;

    static constexpr std::array<Pt3f, 5> depthGradient ={
        Pt3f{0.f,0.f,1.f},
        {0.f,1.f,1.f},
        {0.f,1.f,0.f},
        {1.f,1.f,0.f},
        {1.f,0.f,0.f},
    };

    // utility
    auto cloud_image_data() -> geo::Pt3<int16_t>*;
    auto update_id_array(size_t idV) -> void;

};

auto K4FrameUncompressor::Impl::cloud_image_data() -> Pt3<int16_t>*{
    if(pointCloudImage.is_valid()){
        return reinterpret_cast<Pt3<int16_t>*>(pointCloudImage.get_buffer());
    }
    return nullptr;
}
auto K4FrameUncompressor::Impl::update_id_array(size_t idV) -> void{
    if(indicesValid1D.size() < idV){
        indicesValid1D.resize(idV);
        std::iota(std::begin(indicesValid1D), std::end(indicesValid1D), 0);
    }
}

K4FrameUncompressor::K4FrameUncompressor() : i(std::make_unique<Impl>()){}

K4FrameUncompressor::~K4FrameUncompressor(){}


auto K4FrameUncompressor::uncompress_jpeg_8_bits_data(size_t width, size_t height, ColorFormat format, size_t jpegSize, std::uint8_t *jpegData, std::uint8_t *data) -> bool{

    // uncompress
    const int decompressStatus = tjDecompress2(
        i->jpegUncompressor,
        jpegData,
        static_cast<unsigned long>(jpegSize),
        data,
        static_cast<int>(width),
        0, // pitch
        static_cast<int>(height),
        format,
        TJFLAG_FASTDCT
    );
    if(decompressStatus == -1){
        Logger::error("[K4FrameUncompressor:uncompress_jpeg_8_bits_data] Error uncompress color.\n");
        return false;
    }
    return true;
}

auto K4FrameUncompressor::uncompress_jpeg_8_bits_data(size_t width, size_t height, ColorFormat format, std::vector<uint8_t> &encodedData, std::vector<std::uint8_t> &data) -> bool{

    size_t dim = 4;
    if(format == ColorFormat::BGR || format == ColorFormat::RGB){
        dim = 3;
    }

    // resize uncompressed buffer
    const size_t colorSize = width * height * dim;
    if(data.size() != colorSize){
        data.resize(colorSize);
    }

    // uncompress
    const int decompressStatus = tjDecompress2(
        i->jpegUncompressor,
        encodedData.data(),
        static_cast<unsigned long>(encodedData.size()),
        reinterpret_cast<uint8_t*>(data.data()),
        static_cast<int>(width),
        0, // pitch
        static_cast<int>(height),
        format,
        TJFLAG_FASTDCT
    );
    if(decompressStatus == -1){
        Logger::error("[FrameUncompressor] Error uncompress color.\n");
        return false;
    }

    return true;
}

auto K4FrameUncompressor::uncompress_jpeg_8_bits_data(size_t width, size_t height, ColorFormat format, std::vector<uint8_t> &jpegData, std::vector<Pt3<uint8_t>> &uncompressedColor) -> bool{

    // resize uncompressed buffer
    const size_t colorSize = width * height;
    if(uncompressedColor.size() != colorSize){
        uncompressedColor.resize(colorSize);
    }
    return uncompress_jpeg_8_bits_data(width, height, format, jpegData.size(), jpegData.data(), reinterpret_cast<std::uint8_t*>(uncompressedColor.data()));
}

auto K4FrameUncompressor::uncompress_jpeg_8_bits_data(size_t width, size_t height, ColorFormat format, std::vector<uint8_t> &jpegData, std::vector<Pt4<uint8_t>> &uncompressedColor) -> bool{

    // resize uncompressed buffer
    const size_t colorSize = width * height;
    if(uncompressedColor.size() != colorSize){
        uncompressedColor.resize(colorSize);
    }
    return uncompress_jpeg_8_bits_data(width, height, format, jpegData.size(), jpegData.data(), reinterpret_cast<std::uint8_t*>(uncompressedColor.data()));
}

auto K4FrameUncompressor::uncompress_lossless_16_bits_128padded_data(size_t uncompressedSize, std::vector<std::uint8_t> &encodedData, std::vector<std::uint16_t> &data) -> bool{

    size_t rest = uncompressedSize % 128;
    size_t paddedUncompressedSize = rest == 0 ? uncompressedSize : uncompressedSize + 128-rest;

    if(data.size() < paddedUncompressedSize){
        data.resize(paddedUncompressedSize);
    }

    size_t decodedBytesNb = p4nzdec128v16(
        encodedData.data(),
        paddedUncompressedSize,
        data.data());
    if(decodedBytesNb == 0){
        Logger::error("[K4FrameUncompressor::uncompress_lossless_16_bits_128padded_data] Error decoding data.\n");
        return false;
    }

    data.resize(uncompressedSize);

    return true;
}

auto K4FrameUncompressor::convert_to_depth_image(K4Mode mode, size_t depthWidth, size_t depthHeight, const std::vector<uint16_t> &uncompressedDepth, std::vector<Pt3<uint8_t> > &imageDepth) -> void{

    // resize image buffer
    size_t imageDepthSize = depthWidth * depthHeight*3;
    if(imageDepth.size() != imageDepthSize){
        imageDepth.resize(imageDepthSize);
    }

    const auto dRange = range(mode)*1000.f;
    const auto diff = dRange(1) - dRange(0);

    // convert data
    for(size_t ii = 0; ii < uncompressedDepth.size(); ++ii){

        if(uncompressedDepth[ii] == k4_invalid_depth_value){
            imageDepth[ii] = {0,0,0};
            continue;;
        }

        float vF = (static_cast<float>(uncompressedDepth[ii]) - dRange(0))/diff;
        float intPart;
        float decPart = std::modf((vF*(i->depthGradient.size()-1)), &intPart);
        size_t idG = static_cast<size_t>(intPart);

        auto col = i->depthGradient[idG]*(1.f-decPart) + i->depthGradient[idG+1]*decPart;
        imageDepth[ii] = {
            static_cast<uint8_t>(255*col.x()),
            static_cast<uint8_t>(255*col.y()),
            static_cast<uint8_t>(255*col.z())
        };
    }
}

auto K4FrameUncompressor::convert_to_infra_image(size_t infraWidth, size_t infraHeight, const std::vector<uint16_t> &uncompressedInfra, std::vector<Pt3<uint8_t>> &imageInfra) -> void{

    // resize image buffer
    size_t imageInfraSize = infraWidth * infraHeight;
    if(imageInfra.size() != imageInfraSize){
        imageInfra.resize(imageInfraSize);
    }

    // convert data
    const float max = 2000;
    for(size_t ii = 0; ii < uncompressedInfra.size(); ++ii){

        float vF = static_cast<float>(uncompressedInfra[ii]);
        if(vF > max){
            vF = max;
        }
        vF/=max;

        imageInfra[ii] = {
            static_cast<uint8_t>(255*vF),
            static_cast<uint8_t>(255*vF),
            static_cast<uint8_t>(255*vF)
        };
    }
}

auto K4FrameUncompressor::generate_cloud(K4Mode mode, size_t dephtWidth, size_t depthHeight, k4a_calibration_t &calibration, const std::vector<uint16_t> &uncompressedDepth) -> void{


    // reset k4a transformation if necessary
    if(!std::get<1>(i->modeTr).has_value() || (mode != std::get<0>(i->modeTr))){
        std::get<0>(i->modeTr) = mode;
        std::get<1>(i->modeTr) = k4a_transformation_create(&calibration);
    }

    // reset k4a images if necessary
    bool resetK4AImages = false;
    if(i->depthImage.is_valid()){
        if(i->depthImage.get_width_pixels()    != to_int(dephtWidth) ||
            i->depthImage.get_height_pixels()  != to_int(depthHeight) ){
            resetK4AImages = true;
        }
    }else{
        resetK4AImages = true;
    }
    if(resetK4AImages){

        // generate k4a image for storing depth values
        i->depthImage = k4a::image::create(
            k4a_image_format_t::K4A_IMAGE_FORMAT_DEPTH16,
            to_int(dephtWidth),
            to_int(depthHeight),
            static_cast<int32_t>(dephtWidth * 1 * sizeof(uint16_t))
        );

        // generate k4a image for storing cloud values
        i->pointCloudImage = k4a::image::create(K4A_IMAGE_FORMAT_CUSTOM,
            to_int(dephtWidth),
            to_int(depthHeight),
            static_cast<int32_t>(dephtWidth * 3 * sizeof(int16_t))
        );
    }

    // copy depth values
    std::copy(uncompressedDepth.begin(), uncompressedDepth.end(), reinterpret_cast<std::uint16_t*>(i->depthImage.get_buffer()));

    // generate point cloud from depth image
    k4a_transformation_depth_image_to_point_cloud(
        std::get<1>(i->modeTr).value(),
        i->depthImage.handle(),
        K4A_CALIBRATION_TYPE_DEPTH,
        i->pointCloudImage.handle()
    );
}


auto K4FrameUncompressor::convert_to_cloud(
    K4Mode mode,
    size_t validVerticesCount,
    ColoredCloudData &cloud,
    std::vector<Pt3<uint8_t>> &uncompressedColor,
    std::vector<std::uint16_t> &uncompressedVertices
    ) -> void{

    const auto vvc = validVerticesCount;
    i->update_id_array(vvc);

    // resize cloud if necessary
    cloud.resize(vvc);

    if(mode != K4Mode::Merged){

        for_each(std::execution::par_unseq, std::begin(i->indicesValid1D), std::begin(i->indicesValid1D) + vvc, [&](size_t id){

            cloud.vertices[id] = Pt3f{
                -(static_cast<float>(uncompressedVertices[        id]) - 4096),
                -(static_cast<float>(uncompressedVertices[vvc   + id]) - 4096),
                static_cast<float>(uncompressedVertices  [2*vvc + id])
            }*0.001f;

            cloud.colors[id] = Pt3f{
                  static_cast<float>(uncompressedColor[id].x()),
                  static_cast<float>(uncompressedColor[id].y()),
                  static_cast<float>(uncompressedColor[id].z())
            }/255.f;
        });
    }else{

        for_each(std::execution::par_unseq, std::begin(i->indicesValid1D), std::begin(i->indicesValid1D) + vvc, [&](size_t id){

            cloud.vertices[id] = Pt3f{
                static_cast<float>(uncompressedVertices[        id])  -32768.f,
                static_cast<float>(uncompressedVertices[vvc   + id])  -32768.f,
                static_cast<float>(uncompressedVertices [2*vvc + id]) -32768.f,
            }*0.001f;

            cloud.colors[id] = Pt3f{
                  static_cast<float>(uncompressedColor[id].x()),
                  static_cast<float>(uncompressedColor[id].y()),
                  static_cast<float>(uncompressedColor[id].z())
            }/255.f;
        });
    }
}

auto K4FrameUncompressor::convert_to_cloud(
    size_t validVerticesCount,
    ColoredCloudData &cloud,
    std::vector<Pt4<uint8_t>> &uncompressedColor,
    std::vector<std::uint16_t> &uncompressedDepth,
    geo::Pt3<int16_t> *cloudBuffer
    ) -> void{

    const auto vvc = validVerticesCount;

    // resize cloud if necessary
    cloud.resize(vvc);

    // resize depth indices
    if(i->indicesDepths1D.size() != uncompressedDepth.size()){
        i->indicesDepths1D.resize(uncompressedDepth.size());
        std::iota(std::begin(i->indicesDepths1D), std::end(i->indicesDepths1D), 0);
    }

    // update cloud values
    size_t idV = 0;
    for_each(std::execution::unseq, std::begin(i->indicesDepths1D), std::end(i->indicesDepths1D), [&](size_t id){

        if(uncompressedDepth[id] == k4_invalid_depth_value){
            return;
        }

        cloud.vertices[idV]= Pt3f{
            static_cast<float>(-cloudBuffer[id].x()),
            static_cast<float>(-cloudBuffer[id].y()),
            static_cast<float>( cloudBuffer[id].z())
        }*0.001f;
        cloud.colors[idV] = Pt3f{
            static_cast<float>(uncompressedColor[id].x()),
            static_cast<float>(uncompressedColor[id].y()),
            static_cast<float>(uncompressedColor[id].z())
        }/255.f;

        ++idV;
    });
}

auto K4FrameUncompressor::convert_to_cloud(
    K4VertexMeshData* vertices,
    std::vector<Pt3<uint8_t>>& uncompressedColor,
    std::vector<uint16_t>& uncompressedDepth,
    geo::Pt3<int16_t> *cloudBuffer) -> void{

    // resize depth indices
    if(i->indicesDepths1D.size() != uncompressedDepth.size()){
        i->indicesDepths1D.resize(uncompressedDepth.size());
        std::iota(std::begin(i->indicesDepths1D), std::end(i->indicesDepths1D), 0);
    }

    // update cloud values
    size_t idV = 0;
    for_each(std::execution::unseq, std::begin(i->indicesDepths1D), std::end(i->indicesDepths1D), [&](size_t id){

        if(uncompressedDepth[id] == k4_invalid_depth_value){
            return;
        }
        vertices[idV].pos = Pt3f{
            static_cast<float>(-cloudBuffer[id].x()),
            static_cast<float>(-cloudBuffer[id].y()),
            static_cast<float>( cloudBuffer[id].z())
        }*0.001f;
        vertices[idV].col = Pt4<std::uint8_t>{
            uncompressedColor[id].x(),
            uncompressedColor[id].y(),
            uncompressedColor[id].z(),
            255
        };

        ++idV;
    });
}

auto K4FrameUncompressor::convert_to_cloud(
    K4Mode mode, size_t validVerticesCount, Pt3f *vertices, Pt3f *colors,
    std::vector<Pt3<uint8_t>> &uncompressedColor, std::vector<uint16_t> &uncompressedVertices) -> void{

    const auto idV = validVerticesCount;
    i->update_id_array(idV);

    if(mode != K4Mode::Merged){

        for_each(std::execution::par_unseq, std::begin(i->indicesValid1D), std::begin(i->indicesValid1D) + idV, [&](size_t id){

            vertices[id]= Pt3f{
                -(static_cast<float>(uncompressedVertices[        id])-4096.f),
                -(static_cast<float>(uncompressedVertices[idV   + id])-4096.f),
                static_cast<float>(uncompressedVertices[2*idV + id])
            }*0.001f;

            colors[id] = Pt3f{
                static_cast<float>(uncompressedColor[id].x()),
                static_cast<float>(uncompressedColor[id].y()),
                static_cast<float>(uncompressedColor[id].z())
            }/255.f;
        });
    }else{

        for_each(std::execution::par_unseq, std::begin(i->indicesValid1D), std::begin(i->indicesValid1D) + idV, [&](size_t id){

            vertices[id]= Pt3f{
                static_cast<float>(uncompressedVertices[        id])-32768.f,
                static_cast<float>(uncompressedVertices[idV   + id])-32768.f,
                static_cast<float>(uncompressedVertices[2*idV + id])-32768.f
            }*0.001f;

            colors[id] = Pt3f{
                static_cast<float>(uncompressedColor[id].x()),
                static_cast<float>(uncompressedColor[id].y()),
                static_cast<float>(uncompressedColor[id].z())
            }/255.f;
        });
    }
}

auto K4FrameUncompressor::convert_to_cloud(K4Mode mode, size_t validVerticesCount, Pt3f *vertices, Pt4f *colors, std::vector<Pt3<uint8_t>> &uncompressedColor, std::vector<uint16_t> &uncompressedVertices) -> void{

    const auto idV = validVerticesCount;
    i->update_id_array(idV);

    if(mode != K4Mode::Merged){

        for_each(std::execution::par_unseq, std::begin(i->indicesValid1D), std::begin(i->indicesValid1D) + idV, [&](size_t id){

            vertices[id]= Pt3f{
                -(static_cast<float>(uncompressedVertices[        id])-4096.f),
                -(static_cast<float>(uncompressedVertices[idV   + id])-4096.f),
                static_cast<float>(uncompressedVertices[2*idV + id])
            }*0.001f;

            colors[id] = Pt4f{
                static_cast<float>(uncompressedColor[id].x()),
                static_cast<float>(uncompressedColor[id].y()),
                static_cast<float>(uncompressedColor[id].z()),
                255.f
             }/255.f;
        });
    }else{

        for_each(std::execution::par_unseq, std::begin(i->indicesValid1D), std::begin(i->indicesValid1D) + idV, [&](size_t id){

            vertices[id]= Pt3f{
                static_cast<float>(uncompressedVertices[        id])-32768.f,
                static_cast<float>(uncompressedVertices[idV   + id])-32768.f,
                static_cast<float>(uncompressedVertices[2*idV + id])-32768.f
            }*0.001f;

            colors[id] = Pt4f{
                static_cast<float>(uncompressedColor[id].x()),
                static_cast<float>(uncompressedColor[id].y()),
                static_cast<float>(uncompressedColor[id].z()),
                255.f
            }/255.f;
        });
    }
}

auto K4FrameUncompressor::convert_to_cloud(K4Mode mode, size_t validVerticesCount, Pt3f *vertices, Pt3<uint8_t> *colors, std::vector<Pt3<uint8_t>> &uncompressedColor, std::vector<uint16_t> &uncompressedVertices) -> void{
    const auto idV = validVerticesCount;
    i->update_id_array(idV);

    if(mode != K4Mode::Merged){

        for_each(std::execution::par_unseq, std::begin(i->indicesValid1D), std::begin(i->indicesValid1D) + idV, [&](size_t id){

            vertices[id]= Pt3f{
                -(static_cast<float>(uncompressedVertices[        id])-4096.f),
                -(static_cast<float>(uncompressedVertices[idV   + id])-4096.f),
                static_cast<float>(uncompressedVertices[2*idV + id])
            }*0.001f;

            colors[id] = {
                uncompressedColor[id].x(),
                uncompressedColor[id].y(),
                uncompressedColor[id].z(),
            };
        });
    }else{

        for_each(std::execution::par_unseq, std::begin(i->indicesValid1D), std::begin(i->indicesValid1D) + idV, [&](size_t id){

            vertices[id]= Pt3f{
                static_cast<float>(uncompressedVertices[        id])-32768.f,
                static_cast<float>(uncompressedVertices[idV   + id])-32768.f,
                static_cast<float>(uncompressedVertices[2*idV + id])-32768.f
            }*0.001f;

            colors[id] = {
                uncompressedColor[id].x(),
                uncompressedColor[id].y(),
                uncompressedColor[id].z(),
            };
        });
    }
}

auto K4FrameUncompressor::convert_to_cloud(
    K4Mode mode,
    size_t validVerticesCount,
    K4VertexMeshData *vertices,
    std::vector<Pt3<uint8_t>> &uncompressedColor,
    std::vector<uint16_t> &uncompressedVertices) -> void{

    const auto idV = validVerticesCount;
    i->update_id_array(idV);

    if(mode != K4Mode::Merged){

        for_each(std::execution::par_unseq, std::begin(i->indicesValid1D), std::begin(i->indicesValid1D) + idV, [&](size_t id){

            vertices[id].pos = Pt3f{
                -(static_cast<float>(uncompressedVertices[        id])-4096.f),
                -(static_cast<float>(uncompressedVertices[idV   + id])-4096.f),
                static_cast<float>(uncompressedVertices[2*idV + id])
            }*0.001f;

            vertices[id].col = {
                uncompressedColor[id].x(),
                uncompressedColor[id].y(),
                uncompressedColor[id].z(),
                255
            };
        });

    }else{

        for_each(std::execution::par_unseq, std::begin(i->indicesValid1D), std::begin(i->indicesValid1D) + idV, [&](size_t id){

            vertices[id].pos = Pt3f{
                static_cast<float>(uncompressedVertices[        id])-32768.f,
                static_cast<float>(uncompressedVertices[idV   + id])-32768.f,
                static_cast<float>(uncompressedVertices[2*idV + id])-32768.f
            }*0.001f;

            vertices[id].col = {
                uncompressedColor[id].x(),
                uncompressedColor[id].y(),
                uncompressedColor[id].z(),
                255
            };
        });
    }
}

auto K4FrameUncompressor::convert_to_cloud(K4Mode mode, size_t validVerticesCount, Pt3f *vertices, Pt4<uint8_t> *colors, std::vector<Pt3<uint8_t>> &uncompressedColor, std::vector<uint16_t> &uncompressedVertices) -> void{

    const size_t idV = validVerticesCount;
    i->update_id_array(idV);

    if(mode != K4Mode::Merged){

        for_each(std::execution::par_unseq, std::begin(i->indicesValid1D), std::begin(i->indicesValid1D) + idV, [&](size_t id){

            vertices[id]= Pt3f{
                -(static_cast<float>(uncompressedVertices[        id])-4096),
                -(static_cast<float>(uncompressedVertices[idV   + id])-4096),
                static_cast<float>(uncompressedVertices[2*idV + id])
            }*0.001f;

            colors[id] = {
                uncompressedColor[id].x(),
                uncompressedColor[id].y(),
                uncompressedColor[id].z(),
                255
            };
        });

    }else{

        for_each(std::execution::par_unseq, std::begin(i->indicesValid1D), std::begin(i->indicesValid1D) + idV, [&](size_t id){

            vertices[id]= Pt3f{
                static_cast<float>(uncompressedVertices[        id])-32768.f,
                static_cast<float>(uncompressedVertices[idV   + id])-32768.f,
                static_cast<float>(uncompressedVertices[2*idV + id])-32768.f
            }*0.001f;

            colors[id] = {
                uncompressedColor[id].x(),
                uncompressedColor[id].y(),
                uncompressedColor[id].z(),
                255
            };
        });
    }
}

auto K4FrameUncompressor::convert_to_cloud(
    Pt3f *vertices, Pt3f *colors,
    std::vector<Pt3<uint8_t> > &uncompressedColor, std::vector<uint16_t> &uncompressedDepth, geo::Pt3<int16_t> *cloudBuffer) -> void{

    // resize depth indices
    if(i->indicesDepths1D.size() != uncompressedDepth.size()){
        i->indicesDepths1D.resize(uncompressedDepth.size());
        std::iota(std::begin(i->indicesDepths1D), std::end(i->indicesDepths1D), 0);
    }

    // update cloud values
    size_t idV = 0;
    for_each(std::execution::unseq, std::begin(i->indicesDepths1D), std::end(i->indicesDepths1D), [&](size_t id){

        if(uncompressedDepth[id] == k4_invalid_depth_value){
            return;
        }
        vertices[idV] = Pt3f{
            static_cast<float>(-cloudBuffer[id].x()),
            static_cast<float>(-cloudBuffer[id].y()),
            static_cast<float>( cloudBuffer[id].z())
        }*0.001f;
        colors[idV] = Pt3f{
            1.f*uncompressedColor[id].x(),
            1.f*uncompressedColor[id].y(),
            1.f*uncompressedColor[id].z()
        }/255.f;

        ++idV;
    });
}

auto K4FrameUncompressor::convert_to_cloud(
    Pt3f *vertices, Pt4f *colors,
    std::vector<Pt3<uint8_t> > &uncompressedColor, std::vector<uint16_t> &uncompressedDepth, geo::Pt3<int16_t> *cloudBuffer) -> void{

    // resize depth indices
    if(i->indicesDepths1D.size() != uncompressedDepth.size()){
        i->indicesDepths1D.resize(uncompressedDepth.size());
        std::iota(std::begin(i->indicesDepths1D), std::end(i->indicesDepths1D), 0);
    }

    // update cloud values
    size_t idV = 0;
    for_each(std::execution::unseq, std::begin(i->indicesDepths1D), std::end(i->indicesDepths1D), [&](size_t id){

        if(uncompressedDepth[id] == k4_invalid_depth_value){
            return;
        }
        vertices[idV] = Pt3f{
            static_cast<float>(-cloudBuffer[id].x()),
            static_cast<float>(-cloudBuffer[id].y()),
            static_cast<float>( cloudBuffer[id].z())
        }*0.001f;
        colors[idV] = Pt4f{
            1.f*uncompressedColor[id].x(),
            1.f*uncompressedColor[id].y(),
            1.f*uncompressedColor[id].z(),
            255.f
        }/255.f;

        ++idV;
    });
}

auto K4FrameUncompressor::convert_to_cloud(
    Pt3f *vertices, Pt3<uint8_t> *colors,
    std::vector<Pt3<uint8_t> > &uncompressedColor, std::vector<uint16_t> &uncompressedDepth, geo::Pt3<int16_t> *cloudBuffer) -> void{

    // resize depth indices
    if(i->indicesDepths1D.size() != uncompressedDepth.size()){
        i->indicesDepths1D.resize(uncompressedDepth.size());
        std::iota(std::begin(i->indicesDepths1D), std::end(i->indicesDepths1D), 0);
    }

    // update cloud values
    size_t idV = 0;
    for_each(std::execution::unseq, std::begin(i->indicesDepths1D), std::end(i->indicesDepths1D), [&](size_t id){

        if(uncompressedDepth[id] == k4_invalid_depth_value){
            return;
        }
        vertices[idV] = Pt3f{
            static_cast<float>(-cloudBuffer[id].x()),
            static_cast<float>(-cloudBuffer[id].y()),
            static_cast<float>( cloudBuffer[id].z())
        }*0.001f;
        colors[idV] = Pt3<std::uint8_t>{
            uncompressedColor[id].x(),
            uncompressedColor[id].y(),
            uncompressedColor[id].z()
        };

        ++idV;
    });

}

auto K4FrameUncompressor::convert_to_cloud(

    Pt3f *vertices, Pt4<uint8_t> *colors,
    std::vector<Pt3<uint8_t> > &uncompressedColor, std::vector<uint16_t> &uncompressedDepth, geo::Pt3<int16_t> *cloudBuffer) -> void{

    // resize depth indices
    if(i->indicesDepths1D.size() != uncompressedDepth.size()){
        i->indicesDepths1D.resize(uncompressedDepth.size());
        std::iota(std::begin(i->indicesDepths1D), std::end(i->indicesDepths1D), 0);
    }

    // update cloud values
    size_t idV = 0;
    for_each(std::execution::unseq, std::begin(i->indicesDepths1D), std::end(i->indicesDepths1D), [&](size_t id){

        if(uncompressedDepth[id] == k4_invalid_depth_value){
            return;
        }
        vertices[idV] = Pt3f{
            static_cast<float>(-cloudBuffer[id].x()),
            static_cast<float>(-cloudBuffer[id].y()),
            static_cast<float>( cloudBuffer[id].z())
        }*0.001f;
        colors[idV] = Pt4<std::uint8_t>{
            uncompressedColor[id].x(),
            uncompressedColor[id].y(),
            uncompressedColor[id].z(),
            255
        };

        ++idV;
    });
}

auto K4FrameUncompressor::uncompress(K4CompressedFrame *cFrame, K4Frame &frame) -> bool{

    // info
    frame.idCapture      = cFrame->idCapture;
    frame.afterCaptureTS = cFrame->afterCaptureTS;
    frame.mode           = cFrame->mode;

    // reset sizes
    frame.colorWidth  = 0;
    frame.colorHeight = 0;
    frame.depthWidth  = 0;
    frame.depthHeight = 0;
    frame.infraWidth  = 0;
    frame.infraHeight = 0;

    // color
    if(!cFrame->encodedColorData.empty()){

        if(!uncompress_jpeg_8_bits_data(cFrame->colorWidth, cFrame->colorHeight, ColorFormat::RGBA, cFrame->encodedColorData, frame.imageColorData)){
            return false;
        }
        frame.colorWidth  = cFrame->colorWidth;
        frame.colorHeight = cFrame->colorHeight;
    }

    // depth
    if(!cFrame->encodedDepthData.empty()){

        if(!uncompress_lossless_16_bits_128padded_data(cFrame->depthWidth*cFrame->depthHeight, cFrame->encodedDepthData, frame.depthData)){
            return false;
        }

        convert_to_depth_image(cFrame->mode, cFrame->depthWidth, cFrame->depthHeight, frame.depthData, frame.imageDepthData);
        frame.depthWidth  = cFrame->depthWidth;
        frame.depthHeight = cFrame->depthHeight;
    }

    // infra
    if(!cFrame->encodedInfraData.empty()){

        if(!uncompress_lossless_16_bits_128padded_data(cFrame->infraWidth*cFrame->infraHeight, cFrame->encodedInfraData, frame.infraData)){
            return false;
        }

        convert_to_infra_image(cFrame->infraWidth, cFrame->infraHeight, frame.infraData, frame.imageInfraData);
        frame.infraWidth  = cFrame->infraWidth;
        frame.infraHeight = cFrame->infraHeight;
    }

    // cloud
    if(cFrame->validVerticesCount > 0){        

        if(!cFrame->encodedCloudVerticesData.empty() && !cFrame->encodedCloudColorData.empty()){            

            if(!uncompress_lossless_16_bits_128padded_data(cFrame->validVerticesCount*3, cFrame->encodedCloudVerticesData, i->decodedVerticesData)){
                return false;
            }

            if(!uncompress_jpeg_8_bits_data(cFrame->cloudColorWidth, cFrame->cloudColorHeight, ColorFormat::RGB, cFrame->encodedCloudColorData, i->decodedColorData)){
                return false;
            }

            convert_to_cloud(cFrame->mode, cFrame->validVerticesCount, frame.cloud, i->decodedColorData, i->decodedVerticesData);

        }else if(cFrame->calibration.has_value() && !frame.imageColorData.empty() && !frame.depthData.empty()){

            generate_cloud(cFrame->mode, cFrame->depthWidth, cFrame->depthHeight, cFrame->calibration.value(), frame.depthData);

            convert_to_cloud(cFrame->validVerticesCount, frame.cloud, frame.imageColorData, frame.depthData, i->cloud_image_data());
        }
    }

    // imu
    if(cFrame->imuSample.has_value()){
        frame.imuSample = cFrame->imuSample;
    }else{
        frame.imuSample = std::nullopt;
    }

    // audio
    if(cFrame->audioFrames.size() > 0){
        frame.audioFrames = cFrame->audioFrames;
    }

    // bodies
    // ...

    return true;
}

auto K4FrameUncompressor::uncompress(K4CompressedFrame *cFrame, Pt3f *vertices, Pt3f *colors) -> bool{

    // cloud
    if(cFrame->validVerticesCount > 0){

        if(!cFrame->encodedCloudVerticesData.empty() && !cFrame->encodedCloudColorData.empty()){

            // decode processed colors
            if(!uncompress_jpeg_8_bits_data(cFrame->colorWidth, cFrame->colorHeight,  ColorFormat::RGB, cFrame->encodedCloudColorData, i->decodedColorData)){
                return false;
            }

            // decode vertices
            if(!uncompress_lossless_16_bits_128padded_data(cFrame->validVerticesCount, cFrame->encodedCloudVerticesData, i->decodedVerticesData)){
                return false;
            }

            // convert
            convert_to_cloud(cFrame->mode, cFrame->validVerticesCount, vertices, colors, i->decodedColorData, i->decodedVerticesData);

            return true;

        }else if(cFrame->calibration.has_value() && !cFrame->encodedColorData.empty() && !cFrame->encodedDepthData.empty()){

            // decode colors
            if(!uncompress_jpeg_8_bits_data(cFrame->colorWidth, cFrame->colorHeight, ColorFormat::RGB, cFrame->encodedColorData, i->decodedColorData)){
                return false;
            }

            // decode depth
            if(!uncompress_lossless_16_bits_128padded_data(cFrame->depthWidth*cFrame->depthHeight, cFrame->encodedDepthData, i->decodedDepthData)){
                return false;
            }

            // generate cloud
            generate_cloud(cFrame->mode, cFrame->depthWidth, cFrame->depthHeight, cFrame->calibration.value(), i->decodedDepthData);

            // convert
            convert_to_cloud(vertices, colors, i->decodedColorData, i->decodedDepthData, i->cloud_image_data());

            return true;
        }
    }
    return false;
}

auto K4FrameUncompressor::uncompress(K4CompressedFrame *cFrame, Pt3f *vertices, Pt4f *colors) -> bool{

    // cloud
    if(cFrame->validVerticesCount > 0){

        if(!cFrame->encodedCloudVerticesData.empty() && !cFrame->encodedCloudColorData.empty()){

            // decode processed colors
            if(!uncompress_jpeg_8_bits_data(cFrame->colorWidth, cFrame->colorHeight, ColorFormat::RGB, cFrame->encodedCloudColorData, i->decodedColorData)){
                return false;
            }

            // decode vertices
            if(!uncompress_lossless_16_bits_128padded_data(cFrame->validVerticesCount, cFrame->encodedCloudVerticesData, i->decodedVerticesData)){
                return false;
            }

            // convert
            convert_to_cloud(cFrame->mode, cFrame->validVerticesCount, vertices, colors, i->decodedColorData, i->decodedVerticesData);

            return true;

        }else if(cFrame->calibration.has_value() && !cFrame->encodedColorData.empty() && !cFrame->encodedDepthData.empty()){

            // decode colors
            if(!uncompress_jpeg_8_bits_data(cFrame->colorWidth, cFrame->colorHeight, ColorFormat::RGB, cFrame->encodedColorData, i->decodedColorData)){
                return false;
            }

            // decode depth
            if(!uncompress_lossless_16_bits_128padded_data(cFrame->depthWidth*cFrame->depthHeight, cFrame->encodedDepthData, i->decodedDepthData)){
                return false;
            }

            // generate cloud
            generate_cloud(cFrame->mode, cFrame->depthWidth, cFrame->depthHeight, cFrame->calibration.value(), i->decodedDepthData);

            // convert
            convert_to_cloud(vertices, colors, i->decodedColorData, i->decodedDepthData, i->cloud_image_data());

            return true;
        }
    }
    return false;
}

auto K4FrameUncompressor::uncompress(K4CompressedFrame *cFrame, Pt3f *vertices, Pt3<uint8_t> *colors) -> bool{

    // cloud
    if(cFrame->validVerticesCount > 0){

        if(!cFrame->encodedCloudVerticesData.empty() && !cFrame->encodedCloudColorData.empty()){

            // decode processed colors
            if(!uncompress_jpeg_8_bits_data(cFrame->colorWidth, cFrame->colorHeight, ColorFormat::RGB, cFrame->encodedCloudColorData, i->decodedColorData)){
                return false;
            }

            // decode vertices
            if(!uncompress_lossless_16_bits_128padded_data(cFrame->validVerticesCount, cFrame->encodedCloudVerticesData, i->decodedVerticesData)){
                return false;
            }

            // convert
            convert_to_cloud(cFrame->mode, cFrame->validVerticesCount, vertices, colors, i->decodedColorData, i->decodedVerticesData);

            return true;

        }else if(cFrame->calibration.has_value() && !cFrame->encodedColorData.empty() && !cFrame->encodedDepthData.empty()){

            // decode colors
            if(!uncompress_jpeg_8_bits_data(cFrame->colorWidth, cFrame->colorHeight, ColorFormat::RGB, cFrame->encodedColorData, i->decodedColorData)){
                return false;
            }

            // decode depth
            if(!uncompress_lossless_16_bits_128padded_data(cFrame->depthWidth*cFrame->depthHeight, cFrame->encodedDepthData, i->decodedDepthData)){
                return false;
            }

            // generate cloud
            generate_cloud(cFrame->mode, cFrame->depthWidth, cFrame->depthHeight, cFrame->calibration.value(), i->decodedDepthData);

            // convert
            convert_to_cloud(vertices, colors, i->decodedColorData, i->decodedDepthData, i->cloud_image_data());

            return true;
        }
    }
    return false;
}

auto K4FrameUncompressor::uncompress(K4CompressedFrame *cFrame, Pt3f *vertices, Pt4<uint8_t> *colors) -> bool{

    // cloud
    if(cFrame->validVerticesCount > 0){

        if(!cFrame->encodedCloudVerticesData.empty() && !cFrame->encodedCloudColorData.empty()){

            // decode processed colors
            if(!uncompress_jpeg_8_bits_data(cFrame->colorWidth, cFrame->colorHeight, ColorFormat::RGB, cFrame->encodedCloudColorData, i->decodedColorData)){
                return false;
            }

            // decode vertices
            if(!uncompress_lossless_16_bits_128padded_data(cFrame->validVerticesCount, cFrame->encodedCloudVerticesData, i->decodedVerticesData)){
                return false;
            }

            // convert
            convert_to_cloud(cFrame->mode, cFrame->validVerticesCount, vertices, colors, i->decodedColorData, i->decodedVerticesData);

            return true;

        }else if(cFrame->calibration.has_value() && !cFrame->encodedColorData.empty() && !cFrame->encodedDepthData.empty()){

            // decode colors
            if(!uncompress_jpeg_8_bits_data(cFrame->colorWidth, cFrame->colorHeight, ColorFormat::RGB, cFrame->encodedColorData, i->decodedColorData)){
                return false;
            }

            // decode depth
            if(!uncompress_lossless_16_bits_128padded_data(cFrame->depthWidth*cFrame->depthHeight, cFrame->encodedDepthData, i->decodedDepthData)){
                return false;
            }

            // generate cloud
            generate_cloud(cFrame->mode, cFrame->depthWidth, cFrame->depthHeight, cFrame->calibration.value(), i->decodedDepthData);

            // convert
            convert_to_cloud(vertices, colors, i->decodedColorData, i->decodedDepthData, i->cloud_image_data());

            return true;
        }
    }
    return false;
}


auto K4FrameUncompressor::uncompress(K4CompressedFrame *cFrame, K4VertexMeshData *vertices) -> bool{


    // cloud
    if(cFrame->validVerticesCount > 0){

        if(!cFrame->encodedCloudVerticesData.empty() && !cFrame->encodedCloudColorData.empty()){

            // decode vertices
            if(!uncompress_lossless_16_bits_128padded_data(cFrame->validVerticesCount*3, cFrame->encodedCloudVerticesData, i->decodedVerticesData)){
                return false;
            }

            // decode processed colors
            if(!uncompress_jpeg_8_bits_data(cFrame->cloudColorWidth, cFrame->cloudColorHeight, ColorFormat::RGB, cFrame->encodedCloudColorData, i->decodedColorData)){
                return false;
            }

            // convert
            convert_to_cloud(cFrame->mode, cFrame->validVerticesCount, vertices, i->decodedColorData, i->decodedVerticesData);

            return true;

        }else if(cFrame->calibration.has_value() && !cFrame->encodedColorData.empty() && !cFrame->encodedDepthData.empty()){

            // decode colors
            if(!uncompress_jpeg_8_bits_data(cFrame->colorWidth, cFrame->colorHeight, ColorFormat::RGB, cFrame->encodedColorData, i->decodedColorData)){
                return false;
            }

            // decode depth
            if(!uncompress_lossless_16_bits_128padded_data(cFrame->depthWidth*cFrame->depthHeight, cFrame->encodedDepthData, i->decodedDepthData)){
                return false;
            }

            // generate cloud
            generate_cloud(cFrame->mode, cFrame->depthWidth, cFrame->depthHeight, cFrame->calibration.value(), i->decodedDepthData);

            // convert
            convert_to_cloud(vertices, i->decodedColorData, i->decodedDepthData, i->cloud_image_data());

            return true;            
        }
    }
    return false;
}




