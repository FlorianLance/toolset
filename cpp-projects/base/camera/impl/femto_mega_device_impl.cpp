
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

#include "femto_mega_device_impl.hpp"

// std
#include <execution>

// local
#include "utility/logger.hpp"
#include "utility/benchmark.hpp"

using namespace  std::string_literals;
using namespace tool;
using namespace tool::geo;
using namespace tool::cam;


FemtoMegaDeviceImpl::FemtoMegaDeviceImpl(){
    orbbecD  = std::make_unique<OrbbecDevice>();
    orbbecD->query_devices("Femto Mega"sv, true);
}

auto FemtoMegaDeviceImpl::initialize_device_specific() -> void{

    // reset images
    orbbecD->frameSet             = nullptr;
    orbbecD->colorImage           = nullptr;
    orbbecD->depthImage           = nullptr;
    orbbecD->infraredImage        = nullptr;

    // init converted color image
    if(infos.colorResolution != DCColorResolution::OFF){
        auto colorWidth  = color_width(infos.colorResolution);
        auto colorHeight = color_height(infos.colorResolution);
        data.convertedColorData.resize(colorWidth*colorHeight*4);
    }else{
        data.convertedColorData.clear();
    }

    if(infos.imageFormat != DCImageFormat::MJPG){
        Logger::warning("Femto Mega must use MJPG image format, format ignored.\n");
        infos.imageFormat = DCImageFormat::MJPG;
    }
}

auto FemtoMegaDeviceImpl::update_camera_from_colors_settings() -> void{
    if(!is_opened()){
        return;
    }
    orbbecD->update_camera_from_colors_settings(readFramesFromCameras, settings.color);
}

auto FemtoMegaDeviceImpl::open(uint32_t deviceId) -> bool{
    return orbbecD->open_pipe(settings.config.idDevice = deviceId);
}

auto FemtoMegaDeviceImpl::start_reading(const DCConfigSettings &newConfigS) -> bool{

    settings.config = newConfigS;
    initialize();

    if(orbbecD->start_pipe(settings.config)){

        k4aCalibration    = orbbecD->convert_to_k4_calibration(settings.config.mode);
        k4aTransformation = k4a::transformation(k4aCalibration);

        auto depthRes = depth_resolution(settings.config.mode);
        k4aDepthSizedColorImage = k4a::image::create(
            K4A_IMAGE_FORMAT_COLOR_BGRA32,
            depth_width(depthRes),
            depth_height(depthRes),
            depth_width(depthRes) * 4 * sizeof(uint8_t)
        );

        k4aPointCloudImage = k4a::image::create(
            K4A_IMAGE_FORMAT_CUSTOM,
            depth_width(depthRes),
            depth_height(depthRes),
            depth_width(depthRes) * 3 * sizeof(int16_t)
        );

        start_reading_thread();
        return true;
    }

    return false;
}

auto FemtoMegaDeviceImpl::stop_reading() -> void{
    stop_reading_thread();
    orbbecD->stop_pipe();
}

auto FemtoMegaDeviceImpl::close() -> void {
    return orbbecD->close_pipe();
}

auto FemtoMegaDeviceImpl::is_opened() const noexcept -> bool{
    return orbbecD->is_opened();
}

auto FemtoMegaDeviceImpl::nb_devices() const noexcept -> uint32_t{
    return static_cast<std::uint32_t>(orbbecD->nb_devices());
}

auto FemtoMegaDeviceImpl::device_name() const noexcept -> std::string{
    return orbbecD->device_name();
}

auto FemtoMegaDeviceImpl::capture_frame(int32_t timeoutMs) -> bool{
    return orbbecD->capture_frame(timeoutMs);
}

auto FemtoMegaDeviceImpl::read_color_image() -> bool{

    if(infos.colorResolution != DCColorResolution::OFF){
        data.rawColor = orbbecD->read_color_image();
    }else{
        data.rawColor = {};
    }
    return !data.rawColor.empty();
}

auto FemtoMegaDeviceImpl::read_depth_image() -> bool{

    if(infos.depthResolution != DCDepthResolution::OFF){
        data.depth = orbbecD->read_depth_image();
    }else{
        data.depth = {};
    }
    return !data.depth.empty();
}

auto FemtoMegaDeviceImpl::read_infra_image() -> bool{

    if(has_infrared(settings.config.mode)){
        data.infra = orbbecD->read_infra_image();
    }else{
        data.infra = {};
    }
    return !data.infra.empty();
}

auto FemtoMegaDeviceImpl::convert_color_image() -> void{

    if((infos.colorResolution == DCColorResolution::OFF) || data.rawColor.empty()){
        std::fill(std::begin(data.convertedColorData), std::end(data.convertedColorData), 0);
        data.color = {};
        return;
    }

    size_t colorWidth  = color_width(infos.colorResolution);
    size_t colorHeight = color_height(infos.colorResolution);

    if(infos.imageFormat == DCImageFormat::MJPG){

        if(frames.frameUncompressor.uncompress_jpeg_data(
            colorWidth,
            colorHeight,
            DCFrameUncompressor::ColorFormat::BGRA,
            orbbecD->colorImage->dataSize(),
            reinterpret_cast<uint8_t*>(data.rawColor.data()),
            reinterpret_cast<uint8_t*>(data.convertedColorData.data())
        )){

            data.color =  std::span<tool::ColorRGBA8>{
                reinterpret_cast<tool::ColorRGBA8*>(data.convertedColorData.data()),
                colorWidth * colorHeight
            };

            return;

        }else{
            Logger::error("FemtoMegaDeviceImpl::convert_color_image: Uncompression error.\n");
        }
    }else{
        Logger::error("FemtoMegaDeviceImpl::convert_color_image: Invalid color format.\n");
    }

    orbbecD->colorImage = nullptr;
    data.color =  {};
}

auto FemtoMegaDeviceImpl::resize_color_image_to_depth_size() -> void {

    if((orbbecD->colorImage != nullptr) && (orbbecD->depthImage != nullptr)){

        auto k4aRGBAColorImage = k4a::image::create_from_buffer(
            K4A_IMAGE_FORMAT_COLOR_BGRA32,
            orbbecD->colorImage->width(),
            orbbecD->colorImage->height(),
            orbbecD->colorImage->width() * 4 * sizeof(uint8_t),
            reinterpret_cast<std::uint8_t*>(orbbecD->colorImage->data()),
            orbbecD->colorImage->dataSize(),
            nullptr,
            nullptr
        );

        auto k4aDepthImage = k4a::image::create_from_buffer(
            K4A_IMAGE_FORMAT_DEPTH16,
            orbbecD->depthImage->width(),
            orbbecD->depthImage->height(),
            orbbecD->depthImage->width() * 1 * sizeof(std::uint16_t),
            reinterpret_cast<std::uint8_t*>(orbbecD->depthImage->data()),
            orbbecD->depthImage->dataSize(),
            nullptr,
            nullptr
        );

        k4aTransformation.color_image_to_depth_camera(
            k4aDepthImage,
            k4aRGBAColorImage,
            &k4aDepthSizedColorImage
        );

        data.dephtSizedColor = std::span<tool::ColorRGBA8>{
            reinterpret_cast<tool::ColorRGBA8*>(k4aDepthSizedColorImage.get_buffer()),
            static_cast<size_t>(k4aDepthSizedColorImage.get_width_pixels() * k4aDepthSizedColorImage.get_height_pixels())
        };

    }else{
        k4aDepthSizedColorImage = nullptr;
        data.dephtSizedColor    = {};
    }
}

auto FemtoMegaDeviceImpl::generate_cloud() -> void{

    if(has_cloud(settings.config.mode) && (orbbecD->depthImage != nullptr)){
        if(validDepthValues > 0){

            auto k4aDepthImage = k4a::image::create_from_buffer(
                K4A_IMAGE_FORMAT_DEPTH16,
                orbbecD->depthImage->width(),
                orbbecD->depthImage->height(),
                orbbecD->depthImage->width() * 1 * sizeof(std::uint16_t),
                reinterpret_cast<std::uint8_t*>(orbbecD->depthImage->data()),
                orbbecD->depthImage->dataSize(),
                nullptr,
                nullptr
            );

            k4aTransformation.depth_image_to_point_cloud(
                k4aDepthImage,
                K4A_CALIBRATION_TYPE_DEPTH,
                &k4aPointCloudImage
            );
        }
    }

}

auto FemtoMegaDeviceImpl::create_local_frame(const DCDataSettings &dataS) -> std::unique_ptr<DCFrame>{

    // write frame
    tool::Bench::start("[FemtoMegaDeviceImpl::create_local_frame] Write display data frame");

    auto dFrame             = std::make_unique<DCFrame>();
    dFrame->idCapture       = static_cast<std::int32_t>(infos.idCapture);
    dFrame->afterCaptureTS  = timing.get_local("after_capture"sv).count();
    dFrame->receivedTS      = dFrame->afterCaptureTS;  // default init received TS with after capture TS
    dFrame->mode            = settings.config.mode;

    const auto dRange = depth_range(dFrame->mode)*1000.f;
    const auto diff   = dRange.y() - dRange.x();

    static constexpr std::array<Pt3f,5> depthGradient ={
        Pt3f{0.f,0.f,1.f},
        {0.f,1.f,1.f},
        {0.f,1.f,0.f},
        {1.f,1.f,0.f},
        {1.f,0.f,0.f},
    };

    // color frame
    if(dataS.generateRGBLocalFrame){

        tool::Bench::start("[FemtoMegaDeviceImpl::create_local_frame] color");

        if(auto colorData = color_rgba_data(); !colorData.empty()){

            dFrame->colorWidth  = color_width(infos.colorResolution);
            dFrame->colorHeight = color_height(infos.colorResolution);
            dFrame->imageColorData.resize(color_size(infos.colorResolution));

            std::for_each(std::execution::par_unseq, std::begin(indices.colors1D), std::end(indices.colors1D), [&](size_t id){
                dFrame->imageColorData[id] = {
                    colorData[id].b(),
                    colorData[id].g(),
                    colorData[id].r(),
                    255
                };
            });
        }

        if(auto depthSizedColorData = data.dephtSizedColor; !depthSizedColorData.empty()){

            dFrame->depthSizedColorWidth  = depth_width(infos.depthResolution);
            dFrame->depthSizedColorHeight = depth_height(infos.depthResolution);
            dFrame->depthSizedImageColorData.resize(depth_size(infos.depthResolution));

            std::for_each(std::execution::par_unseq, std::begin(indices.depths1D), std::end(indices.depths1D), [&](size_t id){
                dFrame->depthSizedImageColorData[id] = {
                    depthSizedColorData[id].b(),
                    depthSizedColorData[id].g(),
                    depthSizedColorData[id].r(),
                    255
                };
            });
        }
        tool::Bench::stop();
    }

    // depth frame
    if(dataS.generateDepthLocalFrame){

        tool::Bench::start("[FemtoMegaDeviceImpl::create_local_frame] depth");

        if(auto depthData = data.depth; !depthData.empty()){

            dFrame->depthWidth  = depth_width(infos.depthResolution);
            dFrame->depthHeight = depth_height(infos.depthResolution);
            dFrame->imageDepthData.resize(depth_size(infos.depthResolution));

            std::for_each(std::execution::par_unseq, std::begin(indices.depths1D), std::end(indices.depths1D), [&](size_t id){

                if(depthData[id] == dc_invalid_depth_value){
                    dFrame->imageDepthData[id] = geo::Pt3<std::uint8_t>{};
                    return;
                }

                float vF = (static_cast<float>(depthData[id]) - dRange.x())/diff;
                float intPart;
                float decPart = std::modf((vF*(depthGradient.size()-1)), &intPart);
                size_t idG = static_cast<size_t>(intPart);

                auto col = depthGradient[idG]*(1.f-decPart) + depthGradient[idG+1]*decPart;
                dFrame->imageDepthData[id] = {
                    static_cast<std::uint8_t>(255*col.x()),
                    static_cast<std::uint8_t>(255*col.y()),
                    static_cast<std::uint8_t>(255*col.z())
                };
            });
        }

        tool::Bench::stop();
    }

    // infrared frame
    if(dataS.generateInfraLocalFrame){

        tool::Bench::start("[FemtoMegaDeviceImpl::create_local_frame] display_infrared");

        if(auto infraData = data.infra; !infraData.empty()){

            dFrame->infraWidth  = depth_width(infos.depthResolution);
            dFrame->infraHeight = depth_height(infos.depthResolution);
            dFrame->imageInfraData.resize(depth_size(infos.depthResolution));

            const float max = 2000;
            std::for_each(std::execution::par_unseq, std::begin(indices.depths1D), std::end(indices.depths1D), [&](size_t id){

                float vF = static_cast<float>(infraData[id]);
                if(vF > max){
                    vF = max;
                }
                vF/=max;
                dFrame->imageInfraData[id] = {
                    static_cast<std::uint8_t>(255*vF),
                    static_cast<std::uint8_t>(255*vF),
                    static_cast<std::uint8_t>(255*vF)
                };
            });
        }

        tool::Bench::stop();
    }

    std::cout << "CLOUD: " << has_cloud(dFrame->mode) << " validDepthValues " << validDepthValues << "\n";
    // cloud
    if(dataS.generateCloudLocal && has_cloud(dFrame->mode) && validDepthValues > 0){

        tool::Bench::start("[FemtoMegaDeviceImpl::create_local_frame] cloud");

        auto depthSizedColorData = data.dephtSizedColor;
        auto cloudBuffer = reinterpret_cast<geo::Pt3<int16_t>*>(k4aPointCloudImage.get_buffer());

        if(!data.depth.empty()){
            dFrame->cloud.vertices.resize(validDepthValues);
            if(!depthSizedColorData.empty()){
                dFrame->cloud.colors.resize(validDepthValues);
            }
            dFrame->cloud.normals.resize(validDepthValues);

            std::for_each(std::execution::par_unseq, std::begin(indices.depthVertexCorrrespondance), std::end(indices.depthVertexCorrrespondance), [&](auto idC){

                auto idD = std::get<0>(idC);
                if(data.depth[idD] == dc_invalid_depth_value){
                    return;
                }

                auto idV = std::get<1>(idC);
                dFrame->cloud.vertices[idV]= geo::Pt3f{
                    static_cast<float>(-cloudBuffer[idD].x()),
                    static_cast<float>(-cloudBuffer[idD].y()),
                    static_cast<float>( cloudBuffer[idD].z())
                }*0.001f;

                if(!depthSizedColorData.empty()){
                    dFrame->cloud.colors[idV] = geo::Pt3f{
                        static_cast<float>(depthSizedColorData[idD].b()),
                        static_cast<float>(depthSizedColorData[idD].g()),
                        static_cast<float>(depthSizedColorData[idD].r())
                    }/255.f;
                }else{

                    float vF = (static_cast<float>(data.depth[idD]) - dRange.x())/diff;
                    float intPart;
                    float decPart = std::modf((vF*(depthGradient.size()-1)), &intPart);
                    size_t idG = static_cast<size_t>(intPart);
                    dFrame->cloud.colors[idV] = depthGradient[idG]*(1.f-decPart) + depthGradient[idG+1]*decPart;
                }

                // A B C
                // D I E
                // F G H
                const auto &idN   = indices.neighbours8Depth1D[idD];
                const auto &idDVC = indices.depthVertexCorrrespondance;
                const auto &v     = dFrame->cloud.vertices;
                Vec3f normal{};

                const auto &vId = v[idV];
                if(idN[0] != -1 && std::get<1>(idDVC[idN[0]]) != -1){
                    if(idN[3] != -1 && std::get<1>(idDVC[idN[3]]) != -1){ // vId x vIA
                        normal += cross(v[std::get<1>(idDVC[idN[3]])] - vId, v[std::get<1>(idDVC[idN[0]])] - vId);
                    }
                    if(idN[1] != -1 && std::get<1>(idDVC[idN[1]]) != -1){ // vIA x vIB
                        normal += cross(v[std::get<1>(idDVC[idN[0]])] - vId, v[std::get<1>(idDVC[idN[1]])] - vId);
                    }
                }
                if(idN[2] != -1 && std::get<1>(idDVC[idN[2]]) != -1){
                    if(idN[1] != -1 && std::get<1>(idDVC[idN[1]]) != -1){ // vIB x vIC
                        normal += cross(v[std::get<1>(idDVC[idN[1]])] - vId, v[std::get<1>(idDVC[idN[2]])] - vId);
                    }
                    if(idN[4] != -1 && std::get<1>(idDVC[idN[4]]) != -1){ // vIC x vIE
                        normal += cross(v[std::get<1>(idDVC[idN[2]])] - vId, v[std::get<1>(idDVC[idN[4]])] - vId);
                    }
                }
                if(idN[7] != -1 && std::get<1>(idDVC[idN[7]]) != -1){
                    if(idN[4] != -1 && std::get<1>(idDVC[idN[4]]) != -1){ // vIE x vIH
                        normal += cross(v[std::get<1>(idDVC[idN[4]])] - vId, v[std::get<1>(idDVC[idN[7]])] - vId);
                    }
                    if(idN[6] != -1 && std::get<1>(idDVC[idN[6]]) != -1){ // vIH x vIG
                        normal += cross(v[std::get<1>(idDVC[idN[7]])] - vId, v[std::get<1>(idDVC[idN[6]])] - vId);
                    }
                }
                if(idN[5] != -1 && std::get<1>(idDVC[idN[5]]) != -1){
                    if(idN[6] != -1 && std::get<1>(idDVC[idN[6]]) != -1){ // vIG x vIF
                        normal += cross(v[std::get<1>(idDVC[idN[5]])] - vId, v[std::get<1>(idDVC[idN[6]])] - vId);
                    }
                    if(idN[3] != -1 && std::get<1>(idDVC[idN[3]]) != -1){ // vIF x vID
                        normal += cross(v[std::get<1>(idDVC[idN[6]])] - vId, v[std::get<1>(idDVC[idN[3]])] - vId);
                    }
                }
                dFrame->cloud.normals[idV] = normalize(normal);
            });
        }

        tool::Bench::stop();
    }


    // imu sample
    if(dataS.captureIMU){
        dFrame->imuSample = data.imuSample;
    }else{
        dFrame->imuSample = std::nullopt;
    }

    // send bodies
    if(dataS.captureBodies){
        dFrame->bodies = data.bodies;
    }

    tool::Bench::stop();

    std::cout << "end FemtoMegaDeviceImpl::create_local_frame\n";
    return dFrame;
}
