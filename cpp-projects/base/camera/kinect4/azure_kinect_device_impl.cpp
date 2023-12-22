
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

#include "azure_kinect_device_impl.hpp"

// std
#include <execution>

// libyuv
#include "thirdparty/libyuv/libyuv.h"

// local
#include "utility/logger.hpp"
#include "utility/benchmark.hpp"

using namespace tool::geo;
using namespace tool::camera;

AzureKinectDeviceImpl::AzureKinectDeviceImpl(){

    // init audio manager
    auto audioM = k4a::K4AAudioManager::get_instance();
    if(!audioM->is_initialized()){
        const int audioInitStatus = audioM->initialize();
        if (audioInitStatus != SoundIoErrorNone){
            Logger::error("[K4DeviceImpl] Failed to initialize audio backend: {}\n", soundio_strerror(audioInitStatus));
        }
    }
}

auto AzureKinectDeviceImpl::open(uint32_t deviceId) -> bool {

    try {
        device = std::make_unique<k4a::device>(k4a::device::open(settings.config.idDevice = deviceId));

        const auto version  = device->get_version();
        const auto fb       = version.firmware_build;
        const auto fs       = version.firmware_signature;
        bool debugFB        = fb == K4A_FIRMWARE_BUILD_DEBUG;
        std::string fsStr;
        switch (fs) {
        case K4A_FIRMWARE_SIGNATURE_MSFT:
            fsStr = "Microsoft signed";
            break;
        case K4A_FIRMWARE_SIGNATURE_TEST:
            fsStr = "Test signed";
            break;
        case K4A_FIRMWARE_SIGNATURE_UNSIGNED:
            fsStr = "Unsigned";
            break;
        }

        Logger::message("Azure Kinect device opened:\n");
        Logger::message(std::format("  Serialnum: {}\n", device->get_serialnum()));
        Logger::message("  Version:\n");
        Logger::message(std::format("      Firmware build: {}\n", (debugFB ? "[debug]" : "[release]")));
        Logger::message(std::format("      Firmware signature: {}\n", fsStr));
        Logger::message(std::format("      Color camera firmware version {}.{}.{}\n", version.rgb.major, version.rgb.minor, version.rgb.iteration));
        Logger::message(std::format("      Depth camera firmware version {}.{}.{}\n", version.depth.major, version.depth.minor, version.depth.iteration));
        Logger::message(std::format("      Audio device firmware version {}.{}.{}\n", version.audio.major, version.audio.minor, version.audio.iteration));
        Logger::message(std::format("      Depth device firmware version {}.{}.{}\n", version.depth_sensor.major, version.depth_sensor.minor, version.depth_sensor.iteration));
        Logger::message("  Synch:\n");
        Logger::message(std::format("      IN connected {}\n", device->is_sync_in_connected()));
        Logger::message(std::format("      OUT connected {}\n",device->is_sync_out_connected()));


    }  catch (std::runtime_error error) {
        Logger::error(std::format("[AzureKinectDeviceImpl::open_device] open error: {}\n", error.what()));
        return false;
    }


    auto audioM = k4a::K4AAudioManager::get_instance();
    audioM->refresh_devices();
    size_t nbDevices = audioM->get_devices_count();
    Logger::message(std::format("[AzureKinectDeviceImpl::open_device] Audio devices count: {}\n", nbDevices));

    for(size_t ii = 0; ii < nbDevices; ++ii){
        std::string deviceName = audioM->get_device_name(ii);
        Logger::message(std::format(" - {}\n", deviceName));
        if (deviceName.find("Azure Kinect Microphone Array") != std::string::npos) {
            Logger::message(std::format("Found Azure kinect microphones array.\n"));

            microphone = audioM->get_microphone_for_device(deviceName);
            if(microphone == nullptr){
                Logger::error(std::format("[AzureKinectDeviceImpl::open_device] Cannot retrieve microphone.\n"));
                audioListener = nullptr;
                return false;
            }

            Logger::message("Start microphone. \n");
            if(microphone->Start() != SoundIoErrorNone){
                Logger::error(std::format("[K4DeviceImpl] Cannot start microphone.\n"));
                return false;
            }

            if(microphone->IsStarted()){
                Logger::message("Create listener. \n");
                audioListener = microphone->CreateListener();
                if(audioListener == nullptr){
                    Logger::error(std::format("[K4DeviceImpl] Cannot init audio listener.\n"));
                    return false;
                }
            }
        }
    }

    if(audioListener != nullptr){
        Logger::message("Microphone listener created. \n");
    }


    return true;
}

auto AzureKinectDeviceImpl::start_reading(const DCConfigSettings &newConfigS) -> bool {

    settings.config = newConfigS;
    initialize();

    try {

        Logger::message("Start reading cameras\n");
        Logger::message("[Config]\n");
        Logger::message(std::format("   color format: {}\n",    static_cast<int>(k4aConfig.color_format)));
        Logger::message(std::format("   depth mode: {}\n",      static_cast<int>(k4aConfig.depth_mode)));
        device->start_cameras(&k4aConfig);

        Logger::message("Retrieve calibration\n");
        calibration     = device->get_calibration(k4aConfig.depth_mode, k4aConfig.color_resolution);
        transformation  = k4a::transformation(calibration);

        const auto &c = calibration;
        Logger::message("[Calibration]\n");
        Logger::message(std::format("  color resolution: {}\n",     static_cast<int>(c.color_resolution)));
        Logger::message("  color camera:\n");
        Logger::message(std::format("      width: {}\n",            c.color_camera_calibration.resolution_width));
        Logger::message(std::format("      height: {}\n",           c.color_camera_calibration.resolution_height));
        Logger::message(std::format("      metric radius: {}\n",    c.color_camera_calibration.metric_radius));
        Logger::message("  depth mode:\n");
        Logger::message(std::format("      width: {}\n",            c.depth_camera_calibration.resolution_width));
        Logger::message(std::format("      height: {}\n",           c.depth_camera_calibration.resolution_height));

        Logger::message("Start imu\n");
        device->start_imu();
        
        if(depth_resolution(settings.config.mode) != DCDepthResolution::K4_OFF){
            // Logger::message("[AzureKinectDeviceImpl::start_cameras] start body tracker\n");
            // tracker = k4abt::tracker::create(calibration, k4aBtConfig);
        }

    }  catch (k4a::error error) {
        Logger::error("[AzureKinectDeviceImpl::start_reading] start_cameras error: {}\n", error.what());
        k4aConfig = K4A_DEVICE_CONFIG_INIT_DISABLE_ALL;
        return false;
    }  catch (std::runtime_error error) {
        Logger::error("[AzureKinectDeviceImpl::start_cameras] start_cameras error: {}\n", error.what());
        k4aConfig = K4A_DEVICE_CONFIG_INIT_DISABLE_ALL;
        return false;
    }

    start_reading_thread();

    return true;
}

auto AzureKinectDeviceImpl::stop_reading() -> void{

    stop_reading_thread();

    if(depth_resolution(settings.config.mode) != DCDepthResolution::K4_OFF){
        // Logger::message("Shutdown body tracker\n");
        // tracker.shutdown();
    }

    if(is_opened()){
        Logger::message("Stop IMU\n");
        device->stop_imu();
        Logger::message("Stop cameras\n");
        device->stop_cameras();
    }
}

auto AzureKinectDeviceImpl::close() -> void{

    if(microphone != nullptr){
        if(microphone->IsStarted()){
            microphone->Stop();
        }
        audioListener = nullptr;
        microphone = nullptr;
    }    

    if(is_opened()){
        device->close();
        device = nullptr;
    }
}

auto AzureKinectDeviceImpl::is_opened() const noexcept -> bool{
    return device != nullptr;
}

auto AzureKinectDeviceImpl::nb_devices() const noexcept -> uint32_t {
    return k4a::device::get_installed_count();
}

auto AzureKinectDeviceImpl::device_name() const noexcept -> std::string {
    if(is_opened()){
        return device->get_serialnum();
    }else{
        return "unknow_device";
    }
}

auto set_value(k4a::device *dev, k4a_color_control_command_t pId, std::int32_t value, bool manual) -> void{

    bool supportAuto;
    std::int32_t minV;
    std::int32_t maxV;
    std::int32_t stepV;
    std::int32_t defV;
    k4a_color_control_mode_t dMode;

    std::int32_t currV;
    k4a_color_control_mode_t cMode;

    k4a_device_get_color_control_capabilities(
        dev->handle(),
        pId,
        &supportAuto,
        &minV, &maxV, &stepV, &defV, &dMode
    );

    dev->get_color_control(pId, &cMode, &currV);

    if(value < minV){
        value = minV;
    }
    value += (value - minV) % stepV;

    if(value > maxV){
        value = maxV;
    }

    auto mode = manual ? K4A_COLOR_CONTROL_MODE_MANUAL : K4A_COLOR_CONTROL_MODE_AUTO;
    if(currV != value || cMode != mode){
        dev->set_color_control(pId, mode, value);
    }
}

auto AzureKinectDeviceImpl::update_camera_from_colors_settings() -> void {

    if(!is_opened()){
        return;
    }

    k4a_color_control_command_t type;
    try{        
        set_value(device.get(), type = K4A_COLOR_CONTROL_EXPOSURE_TIME_ABSOLUTE,   settings.color.exposureTime,                    !settings.color.autoExposureTime);
        set_value(device.get(), type = K4A_COLOR_CONTROL_WHITEBALANCE,             settings.color.whiteBalance,                    !settings.color.autoWhiteBalance);
        set_value(device.get(), type = K4A_COLOR_CONTROL_BRIGHTNESS,               settings.color.brightness,                      true);
        set_value(device.get(), type = K4A_COLOR_CONTROL_CONTRAST,                 settings.color.contrast,                        true);
        set_value(device.get(), type = K4A_COLOR_CONTROL_SHARPNESS,                settings.color.sharpness,                       true);
        set_value(device.get(), type = K4A_COLOR_CONTROL_SATURATION,               settings.color.saturation,                      true);
        set_value(device.get(), type = K4A_COLOR_CONTROL_BACKLIGHT_COMPENSATION,   settings.color.backlightCompensation ? 1 : 0,   true);
        set_value(device.get(), type = K4A_COLOR_CONTROL_GAIN,                     settings.color.gain,                            true);
        set_value(device.get(), type = K4A_COLOR_CONTROL_POWERLINE_FREQUENCY,      settings.color.powerlineFrequency,              true);

    }  catch (std::runtime_error error) {
        Logger::error(std::format("[AzureKinectDeviceImpl::update_camera_from_colors_settings] Set color settings error: {} T:{}\n", error.what(), static_cast<int>(type)));
    }
}

auto AzureKinectDeviceImpl::read_color_image() -> bool {
    
    if(infos.initialColorResolution != DCColorResolution::OFF){

        Bench::start("[AzureKinectDeviceImpl::read_color_image]");
        colorImage = capture->get_color_image();
        Bench::stop();

        if (!colorImage->is_valid()){
            Logger::error("[AzureKinectDeviceImpl::read_color_image] Failed to get color image from capture\n");
            return false;
        }

        timing.colorImageTS = colorImage->get_system_timestamp();
    }
    return true;
}

auto AzureKinectDeviceImpl::read_depth_image() -> bool {
    
    if(infos.depthMode != DCDepthResolution::K4_OFF){

        Bench::start("[AzureKinectDeviceImpl::read_depth_image]");
        depthImage = capture->get_depth_image();
        Bench::stop();

        if (!depthImage->is_valid()){
            Logger::error("[AzureKinectDeviceImpl::read_depth_image] Failed to get depth image from capture\n");
            return false;
        }

        timing.depthImageTS = depthImage->get_system_timestamp();
    }

    return true;
}

auto AzureKinectDeviceImpl::read_infra_image() -> bool {

    if(has_infrared(settings.config.mode)){

        Bench::start("[AzureKinectDeviceImpl::read_infra_image]");
        infraredImage = capture->get_ir_image();
        Bench::stop();

        if (!infraredImage->is_valid()){
            Logger::error("[AzureKinectDeviceImpl::read_infra_image] Failed to get infrared image from capture\n");
            return false;
        }

        timing.infraredImageTS = infraredImage->get_system_timestamp();
    }

    return true;
}

auto AzureKinectDeviceImpl::read_from_microphones() -> void{

    lastFrameCount = 0;
    if(audioListener != nullptr){

        // process audio frame
        audioListener->ProcessFrames([&](k4a::K4AMicrophoneFrame *frame, const size_t frameCount) {

            // store last count
            lastFrameCount = frameCount;
            if(lastFrameCount == 0){
                return lastFrameCount;
            }

            // resize audio buffer
            if(audioFrames.size() < lastFrameCount){
                audioFrames.resize(lastFrameCount);
            }

            // copy data
            std::copy(frame, frame + lastFrameCount, audioFrames.begin());

            return lastFrameCount;
        });

        if (audioListener->GetStatus() != SoundIoErrorNone){
            Logger::error(std::format("[AzureKinectDeviceImpl::read_from_microphones] Error while recording {}\n", soundio_strerror(audioListener->GetStatus())));
        }else if (audioListener->Overflowed()){
            Logger::warning(std::format("[AzureKinectDeviceImpl::read_from_microphones] Warning: sound overflow detected!\n"));
            audioListener->ClearOverflowed();
        }
    }
}

auto AzureKinectDeviceImpl::read_from_imu() -> void {

    k4a_imu_sample_t sample;
    if(device->get_imu_sample(&sample, std::chrono::milliseconds(1))){
        data.imuSample.temperature = sample.temperature;
        const auto &dAcc = sample.acc_sample.xyz;
        data.imuSample.acc ={dAcc.x,dAcc.y,dAcc.z};
        data.imuSample.accTsMs = sample.acc_timestamp_usec;
        const auto &dGyr = sample.gyro_sample.xyz;
        data.imuSample.gyr = {dGyr.x,dGyr.y,dGyr.z};
        data.imuSample.gyrTsMs = sample.gyro_timestamp_usec;
        dcDevice->new_imu_sample_signal(data.imuSample);
    }
}

auto update_body(DCBody &body, const k4abt_body_t &k4aBody) -> void{
    body.id = static_cast<std::int8_t>(k4aBody.id);
    for(const auto &jointD : dcJoints.data){
        const auto &kaKoint = k4aBody.skeleton.joints[static_cast<int>(std::get<0>(jointD))];
        auto &joint = body.skeleton.joints[static_cast<int>(std::get<0>(jointD))];
        joint.confidence = static_cast<DCJointConfidenceLevel>(kaKoint.confidence_level);
        const auto &p = kaKoint.position;
        joint.position = {-p.v[0],-p.v[1],p.v[2]};
        const auto &o = kaKoint.orientation;
        joint.orientation = {o.wxyz.x,o.wxyz.y,o.wxyz.z,o.wxyz.w};
    }
}

auto AzureKinectDeviceImpl::read_bodies() -> void{
    
    if((depth_resolution(settings.config.mode) != DCDepthResolution::K4_OFF)){
        // if(tracker.enqueue_capture(*capture.get(), std::chrono::milliseconds(1))){

        //     if(k4abt::frame bodyFrame = tracker.pop_result(std::chrono::milliseconds(1)); bodyFrame != nullptr){
        //         auto bodiesCount = bodyFrame.get_num_bodies();
        //         if(data.bodies.size() < bodiesCount){
        //             data.bodies.resize(bodiesCount);
        //         }
        //         for(size_t ii = 0; ii < bodiesCount; ++ii){
        //             update_body(data.bodies[ii], bodyFrame.get_body(static_cast<int>(ii)));
        //         }
        //         timing.bodiesTS = bodyFrame.get_system_timestamp();
        //     }

        //     //  k4a::image body_index_map = body_frame.get_body_index_map();
        //     //  if (body_index_map != nullptr){
        //     ////    print_body_index_map_middle_line(body_index_map);
        //     //  }else{
        //     //      Logger::error("Error: Failed to generate bodyindex map!\n");
        //     //  }
        // }
    }
}

auto AzureKinectDeviceImpl::generate_cloud() -> void{
    if(has_cloud(settings.config.mode) && pointCloudImage.has_value()){
        Bench::start("[AzureKinectDeviceImpl::generate_cloud]");
        transformation.depth_image_to_point_cloud(depthImage.value(), K4A_CALIBRATION_TYPE_DEPTH, &pointCloudImage.value());
        Bench::stop();
    }

    // image depth_image_to_color_camera(const image &depth_image) const
    // {
    //     image transformed_depth_image = image::create(K4A_IMAGE_FORMAT_DEPTH16,
    //                                                   m_color_resolution.width,
    //                                                   m_color_resolution.height,
    //                                                   m_color_resolution.width *
    //                                                       static_cast<int32_t>(sizeof(uint16_t)));
    //     depth_image_to_color_camera(depth_image, &transformed_depth_image);
    //     return transformed_depth_image;
    // }


}

auto AzureKinectDeviceImpl::convert_color_image() -> void{
    
    if(infos.initialColorResolution == DCColorResolution::OFF){
        return;
    }

    if(infos.imageFormat == DCImageFormat::NV12){

        // cv::Mat rawMat(colorImage->get_height_pixels() * 3 / 2, colorImage->get_width_pixels(), CV_8UC1, colorImage->get_height_pixels());
        // cv::cvtColor(rawMat, colorConvBuffer, cv::COLOR_YUV2RGBA_NV12, 4);
        // std::copy(
        //     colorConvBuffer.data,
        //     colorConvBuffer.data + 4*colorImage->get_width_pixels()*colorImage->get_height_pixels(),
        //     reinterpret_cast<std::int8_t*>(convertedColorImage->get_buffer())
        // );
        // colorImage = convertedColorImage;

        // libyuv refers to pixel order in system-endian order but OpenGL refers to
        // pixel order in big-endian order, which is why we create the OpenGL texture
        // as "RGBA" but then use the "ABGR" libyuv function here.
        const int luminanceStride  = colorImage->get_width_pixels();
        const int hueSatStride     = colorImage->get_width_pixels();
        const uint8_t *hueSatStart = colorImage->get_buffer() + luminanceStride * colorImage->get_height_pixels();

        // libyuv::NV12ToRGB

        Bench::start("[AzureKinectDeviceImpl::convert_color_image] NV12");
        int result = libyuv::NV12ToARGB(
            colorImage->get_buffer(),                                               // src_y
            luminanceStride,                                                        // src_stride_y
            hueSatStart,                                                            // src_vu
            hueSatStride,                                                           // src_stride_vu
            convertedColorImage->get_buffer(),                                      // dst_argb
            colorImage->get_width_pixels() * static_cast<int>(sizeof(geo::Pt4<std::uint8_t>)),  // dst_stride_argb
            colorImage->get_width_pixels(),                                                     // width
            colorImage->get_height_pixels()                                                     // height
        );

        // libyuv::ARGBToRGBA()

        Bench::stop();

        if (result != 0){
            Logger::error("[AzureKinectDeviceImpl::convert_color_image] Error during NV12 convertion.\n");
            return;
        }

        colorImage = convertedColorImage;

    } else if(infos.imageFormat == DCImageFormat::YUY2 ){ // //YUY2 is a 4:2:2 format, so there are 4 bytes per 'chunk' of data, and each 'chunk' represents 2 pixels.

        Bench::start("[AzureKinectDeviceImpl::convert_color_image] YUY2");
        const int stride = colorImage->get_width_pixels() * 4 / 2;
        int result = libyuv::YUY2ToARGB(
            colorImage->get_buffer(),                                                           // src_yuy2,
            stride,                                                                             // src_stride_yuy2,
            convertedColorImage->get_buffer(),                                                  // dst_argb,
            colorImage->get_width_pixels() * static_cast<int>(sizeof(geo::Pt4<std::uint8_t>)),  // dst_stride_argb
            colorImage->get_width_pixels(),                                                     // width,
            colorImage->get_height_pixels()                                                     // height
        );
        Bench::stop();

        if (result != 0){
            Logger::error("[AzureKinectDeviceImpl::convert_color_image] Error during YUY2 convertion.\n");
            return;
        }

        colorImage = convertedColorImage;

    }else if(infos.imageFormat == DCImageFormat::MJPG){

        Bench::start("[AzureKinectDeviceImpl::convert_color_image] MJPG");

        bool result = frames.frameUncompressor.uncompress_jpeg_data(
            colorImage->get_width_pixels(),
            colorImage->get_height_pixels(),
            DCFrameUncompressor::ColorFormat::BGRA,
            colorImage->get_size(),
            colorImage->get_buffer(),
            convertedColorImage->get_buffer()
        );
        Bench::stop();

        if(!result){
            Logger::error("[AzureKinectDeviceImpl::convert_color_image] Error during MJPG convertion.\n");
            return;
        }

        colorImage = convertedColorImage;

    }else if(infos.imageFormat == DCImageFormat::BGRA){
        // nothing to do
    }
}

auto AzureKinectDeviceImpl::resize_images() -> void{

    if(colorImage.has_value() && depthImage.has_value()){

        Bench::start("[AzureKinectDeviceImpl::resize_color_to_fit_depth]");
        transformation.color_image_to_depth_camera(
            depthImage.value(),
            colorImage.value(),
            &depthSizedColorImage.value());
        Bench::stop();
    }
}

auto AzureKinectDeviceImpl::compress_frame(const DCFiltersSettings &filtersS, const DCDataSettings &dataS) -> std::unique_ptr<DCCompressedFrame> {

    tool::Bench::start("[AzureKinectDeviceImpl::compress_frame] Generate compressed frame");

    auto mode = settings.config.mode;
    auto cFrame                = std::make_unique<DCCompressedFrame>();
    cFrame->mode               = mode;
    cFrame->idCapture          = static_cast<std::int32_t>(infos.idCapture);
    cFrame->afterCaptureTS     = timing.get_local("after_capture"sv).count();
    cFrame->validVerticesCount = validDepthValues;

    size_t offset = 0;
    cFrame->init_calibration_from_data(DCType::AzureKinect, reinterpret_cast<std::int8_t*>(&calibration), offset, sizeof(k4a_calibration_t));

    // compressed color
    if(depthSizedColorImage.has_value() && dataS.sendColor){
        frames.frameCompressor.add_color(
            depthSizedColorImage->get_width_pixels(),
            depthSizedColorImage->get_height_pixels(),
            4,
            depthSizedColorImage.value().get_buffer(),
            filtersS.jpegCompressionRate,
            cFrame.get()
        );
    }
    // compressed depth
    if(depthImage.has_value() && dataS.sendDepth){
        frames.frameCompressor.add_depth(
            depthImage->get_width_pixels(),
            depthImage->get_height_pixels(),
            reinterpret_cast<std::uint16_t*>(depthImage.value().get_buffer()),
            cFrame.get()
        );
    }
    // compressed infrared
    if(infraredImage.has_value() && dataS.sendInfra){
        frames.frameCompressor.add_infra(
            infraredImage->get_width_pixels(),
            infraredImage->get_height_pixels(),
            reinterpret_cast<std::uint16_t*>(infraredImage.value().get_buffer()),
            cFrame.get()
        );
    }
    // compressed cloud
    if(colorImage.has_value() && depthImage.has_value() && dataS.sendCloud){
        frames.frameCompressor.add_cloud(
            colorImage->get_width_pixels(), colorImage->get_height_pixels(), colorImage.value().get_buffer(),
            depthImage->get_width_pixels()*depthImage->get_height_pixels(), reinterpret_cast<std::uint16_t*>(depthImage.value().get_buffer()),
            filtersS.jpegCompressionRate, cFrame.get()
        );
    }
    // uncompressed audio
    float *audioData = dataS.sendAudio ? reinterpret_cast<float*>(audioFrames.data()) : nullptr;
    size_t audioSize = lastFrameCount;
    if(audioData != nullptr && audioSize > 0){
        cFrame->audioFrames.resize(audioSize);
        std::copy(audioData, audioData + 7*audioSize, reinterpret_cast<float*>(cFrame->audioFrames.data()));
    }

    // uncompressed imu
    if(dataS.sendIMU){
        cFrame->imuSample = data.imuSample;
    }

    // uncompressed bodies
    if(dataS.sendBodies){
        // ...
    }

    tool::Bench::stop();

    return cFrame;
}

auto AzureKinectDeviceImpl::create_local_frame(const DCDataSettings &dataS) -> std::unique_ptr<DCFrame> {

    auto mode = settings.config.mode;

    // write frame
    tool::Bench::start("[AzureKinectDeviceImpl::create_local_frame] Write display data frame");

    auto dFrame            = std::make_unique<DCFrame>();
    dFrame->idCapture      = static_cast<std::int32_t>(infos.idCapture);
    dFrame->afterCaptureTS = timing.get_local("after_capture"sv).count();
    dFrame->mode           = mode;


    static constexpr std::array<Pt3f,5> depthGradient ={
    Pt3f{0.f,0.f,1.f},
        {0.f,1.f,1.f},
        {0.f,1.f,0.f},
        {1.f,1.f,0.f},
        {1.f,0.f,0.f},
    };

    // color frame
    if(dataS.generateRGBLocalFrame && colorImage.has_value()){

        tool::Bench::start("[AzureKinectDeviceImpl::create_local_frame] color");

        if(depthSizedColorImage.has_value()){
            dFrame->depthSizedColorWidth  = depthSizedColorImage->get_width_pixels();
            dFrame->depthSizedColorHeight = depthSizedColorImage->get_height_pixels();
            dFrame->depthSizedImageColorData.resize(dFrame->depthSizedColorWidth*dFrame->depthSizedColorHeight);

            // std::copy(
            //     reinterpret_cast<std::int8_t*>(depthSizedColorImage->get_buffer()),
            //     reinterpret_cast<std::int8_t*>(depthSizedColorImage->get_buffer()) + dFrame->depthSizedImageColorData.size()*4,
            //     reinterpret_cast<std::int8_t*>(dFrame->depthSizedImageColorData.data())
            // );

            auto colorBuffer = reinterpret_cast<const geo::Pt4<uint8_t>*>(depthSizedColorImage->get_buffer());
            std::for_each(std::execution::par_unseq, std::begin(indices.depths1D), std::end(indices.depths1D), [&](size_t id){
                dFrame->depthSizedImageColorData[id] = {
                    colorBuffer[id].z(),
                    colorBuffer[id].y(),
                    colorBuffer[id].x(),
                    255
                };
            });
        }

        dFrame->colorWidth  = colorImage->get_width_pixels();
        dFrame->colorHeight = colorImage->get_height_pixels();
        dFrame->imageColorData.resize(dFrame->colorWidth*dFrame->colorHeight);        
        // std::copy(
        //     reinterpret_cast<std::int8_t*>(colorImage->get_buffer()),
        //     reinterpret_cast<std::int8_t*>(colorImage->get_buffer()) + dFrame->imageColorData.size() * 4,
        //     reinterpret_cast<std::int8_t*>(dFrame->imageColorData.data())
        // );

        auto colorBuffer = reinterpret_cast<const geo::Pt4<uint8_t>*>(colorImage->get_buffer());
        std::for_each(std::execution::par_unseq, std::begin(indices.colors1D), std::end(indices.colors1D), [&](size_t id){
            dFrame->imageColorData[id] = {
                colorBuffer[id].z(),
                colorBuffer[id].y(),
                colorBuffer[id].x(),
                255
            };
        });

        tool::Bench::stop();
    }

    // depth frame
    if(dataS.generateDepthLocalFrame && depthImage.has_value()){

        tool::Bench::start("[AzureKinectDeviceImpl::create_local_frame] depth");

        dFrame->depthWidth  = depthImage->get_width_pixels();
        dFrame->depthHeight = depthImage->get_height_pixels();
        dFrame->imageDepthData.resize(dFrame->depthWidth * dFrame->depthHeight);

        auto depthBuffer  = reinterpret_cast<const uint16_t*>(depthImage->get_buffer());
        const auto dRange = depth_range(mode)*1000.f;
        const auto diff   = dRange.y() - dRange.x();

        std::for_each(std::execution::par_unseq, std::begin(indices.depths1D), std::end(indices.depths1D), [&](size_t id){

            if(depthBuffer[id] == dc_invalid_depth_value){
                dFrame->imageDepthData[id] = geo::Pt3<std::uint8_t>{};
                return;
            }

            float vF = (static_cast<float>(depthBuffer[id]) - dRange.x())/diff;
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

        tool::Bench::stop();
    }

    // infrared frame
    if(dataS.generateInfraLocalFrame && infraredImage.has_value()){

        tool::Bench::start("[AzureKinectDeviceImpl::create_local_frame] display_infrared");

        dFrame->infraWidth  = infraredImage->get_width_pixels();;
        dFrame->infraHeight = infraredImage->get_height_pixels();;
        dFrame->imageInfraData.resize(dFrame->infraWidth * dFrame->infraHeight);

        auto infraBuffer = reinterpret_cast<const uint16_t*>(infraredImage->get_buffer());

        const float max = 2000;
        std::for_each(std::execution::par_unseq, std::begin(indices.depths1D), std::end(indices.depths1D), [&](size_t id){

            float vF = static_cast<float>(infraBuffer[id]);
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

        tool::Bench::stop();
    }

    // cloud
    if(dataS.generateCloudLocal && pointCloudImage.has_value() && depthSizedColorImage.has_value() && depthImage.has_value()){

        tool::Bench::start("[AzureKinectDeviceImpl::create_local_frame] cloud");

        dFrame->cloud.vertices.resize(validDepthValues);
        dFrame->cloud.colors.resize(validDepthValues);
        dFrame->cloud.normals.resize(validDepthValues);

        auto cloudBuffer = reinterpret_cast<geo::Pt3<int16_t>*>(pointCloudImage->get_buffer());
        auto colorBuffer = reinterpret_cast<const geo::Pt4<uint8_t>*>(depthSizedColorImage->get_buffer());
        auto depthBuffer = reinterpret_cast<const uint16_t*>(depthImage->get_buffer());

        std::for_each(std::execution::par_unseq, std::begin(indices.depthVertexCorrrespondance), std::end(indices.depthVertexCorrrespondance), [&](auto idC){

            auto idD = std::get<0>(idC);
            if(depthBuffer[idD] == dc_invalid_depth_value){
                return;
            }

            auto idV = std::get<1>(idC);
            dFrame->cloud.vertices[idV]= geo::Pt3f{
                static_cast<float>(-cloudBuffer[idD].x()),
                static_cast<float>(-cloudBuffer[idD].y()),
                static_cast<float>( cloudBuffer[idD].z())
            }*0.001f;
            dFrame->cloud.colors[idV] = geo::Pt3f{
                static_cast<float>(colorBuffer[idD].z()),
                static_cast<float>(colorBuffer[idD].y()),
                static_cast<float>(colorBuffer[idD].x())
            }/255.f;

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

        tool::Bench::stop();
    }

    // imu sample
    if(dataS.captureIMU){
        dFrame->imuSample = data.imuSample;
    }else{
        dFrame->imuSample = std::nullopt;
    }

    tool::Bench::stop();

    // send audio
    if(dataS.captureAudio && lastFrameCount != 0){

        tool::Bench::start("[AzureKinectDeviceImpl::create_local_frame] audio");

        // copy audio frames
        dFrame->audioFrames.resize(lastFrameCount);
        auto audioFrom = reinterpret_cast<float*>(audioFrames.data());
        std::copy(audioFrom, audioFrom + 7*lastFrameCount, reinterpret_cast<float*>(dFrame->audioFrames.data()));

        tool::Bench::stop();
    }

    // send bodies
    if(dataS.captureBodies){
        dFrame->bodies = data.bodies;
    }

    return dFrame;
}

auto AzureKinectDeviceImpl::initialize_device_specific() -> void {

    // reset images
    colorImage           = std::nullopt;
    depthImage           = std::nullopt;
    infraredImage        = std::nullopt;
    pointCloudImage      = std::nullopt;
    convertedColorImage  = std::nullopt;
    depthSizedColorImage = std::nullopt;

    // init k4a configs
    k4aConfig   = generate_config(device->is_sync_in_connected(), device->is_sync_out_connected(), settings.config);
    k4aBtConfig = generate_bt_config(settings.config);

    // init capture
    capture = std::make_unique<k4a::capture>();

    // init converted color image
    if(infos.initialColorResolution != DCColorResolution::OFF){

        auto colorRes   = color_resolution(settings.config.mode);
        convertedColorImage = k4a::image::create(
            K4A_IMAGE_FORMAT_COLOR_BGRA32,
            static_cast<int>(color_width(colorRes)),
            static_cast<int>(color_height(colorRes)),
            static_cast<int32_t>(color_width(colorRes) * 4 * sizeof(uint8_t))
        );
    }

    // init depth resized color image
    if(infos.depthMode != DCDepthResolution::K4_OFF){
        if(infos.initialColorResolution != DCColorResolution::OFF){
            auto depthRes   = depth_resolution(settings.config.mode);
            depthSizedColorImage = k4a::image::create(
                K4A_IMAGE_FORMAT_COLOR_BGRA32,
                static_cast<int>(depth_width(depthRes)),
                static_cast<int>(depth_height(depthRes)),
                static_cast<int32_t>(depth_width(depthRes) * 4 * sizeof(uint8_t))
            );            
        }
    }

    // init cloud image
    if(has_cloud(settings.config.mode)){

        auto depthRes   = depth_resolution(settings.config.mode);
        pointCloudImage = k4a::image::create(
            K4A_IMAGE_FORMAT_CUSTOM,
            static_cast<int>(depth_width(depthRes)),
            static_cast<int>(depth_height(depthRes)),
            static_cast<int32_t>(infos.depthWidth * 3 * sizeof(int16_t))
        );
    }
}

auto AzureKinectDeviceImpl::generate_config(bool synchInConnected, bool synchOutConnected, const DCConfigSettings &config) -> k4a_device_configuration_t{

    k4a_device_configuration_t ka4Config = K4A_DEVICE_CONFIG_INIT_DISABLE_ALL;
    ka4Config.color_format               = static_cast<k4a_image_format_t>(get_k4_image_format(image_format(config.mode)));
    ka4Config.color_resolution           = static_cast<k4a_color_resolution_t>(color_resolution(config.mode));
    ka4Config.depth_mode                 = static_cast<k4a_depth_mode_t>(depth_resolution(config.mode));
    ka4Config.camera_fps                 = static_cast<k4a_fps_t>(framerate(config.mode));

    ka4Config.synchronized_images_only = false;
    
    if(depth_resolution(config.mode) == DCDepthResolution::K4_OFF){
        ka4Config.synchronized_images_only = false;
    }else{
        ka4Config.synchronized_images_only = config.synchronizeColorAndDepth;
    }
    ka4Config.depth_delay_off_color_usec        = config.delayBetweenColorAndDepthUsec;
    ka4Config.disable_streaming_indicator       = config.disableLED;
    ka4Config.wired_sync_mode = static_cast<k4a_wired_sync_mode_t>(config.synchMode);
    ka4Config.subordinate_delay_off_master_usec = config.subordinateDelayUsec;

    // check modes
    if(config.synchMode == DCSynchronisationMode::Subordinate){
        if(!synchInConnected){
            ka4Config.wired_sync_mode = static_cast<k4a_wired_sync_mode_t>(DCSynchronisationMode::Standalone);
            Logger::warning("[AzureKinectDeviceImpl::generate_config] No input synchronisation cable found, switch from [Subordinate] to [Standalone] mode and set subordinate delay to [0].\n");
        }
    }else if(config.synchMode == DCSynchronisationMode::Main){
        if(!synchOutConnected){
            ka4Config.wired_sync_mode = static_cast<k4a_wired_sync_mode_t>(DCSynchronisationMode::Standalone);
            Logger::warning("[AzureKinectDeviceImpl::generate_config] No output synchronisation cable found, switch from [Master] to [Standalone] mode.\n");
        }
    }

    if(config.synchMode == DCSynchronisationMode::Main && config.subordinateDelayUsec != 0){
        Logger::warning("[AzureKinectDeviceImpl::generate_config] Subordinate delay != 0 for mode [Master], subordinate delay is now set to [0].\n");
        ka4Config.subordinate_delay_off_master_usec = 0;
    }else if (config.synchMode == DCSynchronisationMode::Standalone && config.subordinateDelayUsec != 0){
        Logger::warning("[AzureKinectDeviceImpl::generate_config] Subordinate delay != 0 for mode [Standalone], subordinate delay is now set to [0].\n");
        ka4Config.subordinate_delay_off_master_usec = 0;
    }

    return ka4Config;
}

auto AzureKinectDeviceImpl::generate_bt_config(const DCConfigSettings &config) -> k4abt_tracker_configuration_t{
    k4abt_tracker_configuration_t ka4BtConfig;
    ka4BtConfig.gpu_device_id       = config.btGPUId;
    ka4BtConfig.processing_mode     = static_cast<k4abt_tracker_processing_mode_t>(config.btProcessingMode);
    ka4BtConfig.sensor_orientation  = static_cast<k4abt_sensor_orientation_t>(config.btOrientation);
    ka4BtConfig.model_path          = nullptr;
    return ka4BtConfig;
}

auto AzureKinectDeviceImpl::color_data() -> std::span<tool::ColorRGBA8> {
    if(depthSizedColorImage.has_value()){
        return std::span<tool::ColorRGBA8>{
            reinterpret_cast<tool::ColorRGBA8*>(depthSizedColorImage.value().get_buffer()),
            static_cast<size_t>(depthSizedColorImage.value().get_width_pixels() * depthSizedColorImage.value().get_height_pixels())
        };
    }
    return {};
}

auto AzureKinectDeviceImpl::depth_data() -> std::span<uint16_t> {
    if(depthImage.has_value()){
        return std::span<std::uint16_t>{
            reinterpret_cast<std::uint16_t*>(depthImage.value().get_buffer()),
            static_cast<size_t>(depthImage.value().get_width_pixels() * depthImage.value().get_height_pixels())
        };
    }
    return {};
}

auto AzureKinectDeviceImpl::infra_data() -> std::span<uint16_t> {
    if(infraredImage.has_value()){
        return std::span<std::uint16_t>{
            reinterpret_cast<std::uint16_t*>(infraredImage.value().get_buffer()),
            static_cast<size_t>(infraredImage.value().get_width_pixels() * infraredImage.value().get_height_pixels())
        };
    }
    return {};
}

auto AzureKinectDeviceImpl::capture_frame(std::int32_t timeoutMs) -> bool{

    Bench::start("[AzureKinectDeviceImpl::capture_frame]");
    bool success = false;
    try{
        success   = device->get_capture(capture.get(), std::chrono::milliseconds(timeoutMs));
    }catch(const std::runtime_error &e){
        Logger::error(std::format("[DCDeviceData::read_frames] Get capture runtime error: {}\n", e.what()));
    }
    Bench::stop();
    return success;
}



// auto K4Device::Impl::filter_depth_image(const DCFiltersSettings &f) -> void{

//     if(!depthImage.has_value()){
//         return;
//     }

//     auto mode = configS.mode;

//     Bench::start("[K4DeviceImpl] Filter depth");

//     // retrieve buffers
//     auto depthBuffer = reinterpret_cast<int16_t*>(depthImage->get_buffer());
//     ColorRGBA8 *colorBuffer = depthSizedColorImage.has_value() ? reinterpret_cast<ColorRGBA8*>(depthSizedColorImage.value().get_buffer()) : nullptr;
//     uint16_t *infraredBuffer = infraredImage.has_value() ? reinterpret_cast<uint16_t*>(infraredImage.value().get_buffer()) : nullptr;

//     static_cast<void>(infraredBuffer);

//     const auto dRange = range(mode)*1000.f;
//     auto minD = f.minDepthValue < dRange.x() ? static_cast<std::int16_t>(dRange.x()) : f.minDepthValue;
//     auto maxD = f.maxDepthValue > dRange.y() ? static_cast<std::int16_t>(dRange.y()) : f.maxDepthValue;

//     auto hsvDiffColor = Convert::to_hsv(f.filterColor);

//     // reset depth mask
//     std::fill(depthMask.begin(), depthMask.end(), 1);

//     //    for(size_t id = 0; id < indicesDepths1DNoBorders.size(); ++id){
//     //        indicesDepthsSortedCorrespondanceNoBorders[id] = {indicesDepths1DNoBorders[id], depthBuffer[indicesDepths1DNoBorders[id]]};
//     //    }
//     //    auto sortDepth = [](const std::tuple<size_t, std::int16_t> &lhs, const std::tuple<size_t, std::int16_t> &rhs){
//     //        return std::get<1>(lhs) < std::get<1>(rhs);
//     //    };
//     //    std::sort(std::begin(indicesDepthsSortedCorrespondanceNoBorders), std::end(indicesDepthsSortedCorrespondanceNoBorders), sortDepth);

//     //    auto plane1 = f.plane1;
//     //    plane1.normal = normalize(plane1.normal);

//     // v' = q * v * conjugate(q)
//     // t = 2 * cross(q.xyz, v)
//     // v' = v + q.w * t + cross(q.xyz, t)

//     //    auto pl1Tr  = geo::transform(geo::Pt3f{1.f,1.f,1.f}, f.p1Rot, f.p1Pos);
//     //    auto pl1Dir  = (Vec4f(0,1,0,1)*pl1Tr).xyz();
//     auto pl1Dir = normalize(f.p1Rot);//pl1Tr.col(0).xyz();//*-1.f;

//     //    auto dir = Vec3f{0,1,0};
//     //    auto q = Quaternion<float>::from_euler(f.p1Rot);
//     //    auto t = 2.f * cross(Vec3f{q.x,q.y,q.z}, dir);
//     //    auto pl1Dir = dir + q.w * t + cross(Vec3f{q.x,q.y,q.z}, t);
//     //    auto pl1Dir = q * Vec3f(1,0,0);
//     //    auto pl1Dir = normalize(f.p1Rot);

//     // depth/width/height/mask/color/infra filtering
//     for_each(std::execution::par_unseq, std::begin(indicesDepths3D), std::end(indicesDepths3D), [&](const Pt3<size_t> &dIndex){

//         size_t id = dIndex.x();
//         size_t ii = dIndex.y();
//         size_t jj = dIndex.z();

//         const auto &currentDepth = depthBuffer[id];

//         // check validity
//         if(currentDepth == dc_invalid_depth_value){
//             depthMask[id] = 0;
//             return;
//         }

//         // depth filtering
//         if((ii < f.minWidth)  || (ii > f.maxWidth)  ||      // width
//             (jj < f.minHeight) || (jj > f.maxHeight) ||      // height
//             (currentDepth < minD) || (currentDepth > maxD) ){// depth
//             depthMask[id] = 0;
//             return;
//         }

//         // plane filtering
//         if(f.p1FMode != DCFiltersSettings::PlaneFilteringMode::None){
//             geo::Pt3<float> pt{0.001f * ii,0.001f * jj, 0.001f * currentDepth};

//             if(dot(pt - f.p1Pos, pl1Dir) < 0){
//                 if(f.p1FMode == DCFiltersSettings::PlaneFilteringMode::Above){
//                     depthMask[id] = 0;
//                     return;
//                 }
//             }else{
//                 if(f.p1FMode == DCFiltersSettings::PlaneFilteringMode::Below){
//                     depthMask[id] = 0;
//                     return;
//                 }
//             }
//         }

//         //        if(plane_equation(pt, f.plane1) < 0){
//         //            depthMask[id] = 0;
//         //            return;
//         //        }

//         // mask
//         //        if(!f.depthMask.at(id)){
//         //            depthMask[id] = 0;
//         //            return;
//         //        }

//         // color filtering
//         if(depthSizedColorImage.has_value() && f.filterDepthWithColor){

//             auto hsv = Convert::to_hsv(colorBuffer[id]);
//             if((std::abs(hsv.h()- hsvDiffColor.h()) > f.maxDiffColor.x()) ||
//                 (std::abs(hsv.s()- hsvDiffColor.s()) > f.maxDiffColor.y()) ||
//                 (std::abs(hsv.v()- hsvDiffColor.v()) > f.maxDiffColor.z())){
//                 depthMask[id] = 0;
//                 return;
//             }
//         }

//         // infrared filtering
//         // ...
//     });

//     //    for(size_t ii = 0; ii < 100; ++ii){
//     //        auto id = std::get<0>(indicesDepthsSortedCorrespondance[ii]);
//     //        depthMask[id] = 0;
//     //        std::cout << std::get<1>(indicesDepthsSortedCorrespondance[ii]) << " ";
//     ////        colorBuffer[id].rgba() = {255,0,0,255};
//     ////        std::cout << ii << " " << id << " | ";
//     //    }

//     if(f.doLocalDiffFiltering){
//         maximum_local_depth_difference(f.maxLocalDiff, Connectivity::Connectivity_4);
//     }

//     // minimum neighbours filtering
//     if(f.doMinNeighboursFiltering){
//         mininum_neighbours(f.minNeighboursLoops, f.nbMinNeighbours, Connectivity::Connectivity_4);
//     }

//     // erosion
//     if(f.doErosion){
//         erode(f.erosionLoops, Connectivity::Connectivity_8);
//     }

//     // remove edges
//     if(f.keepOnlyBiggestCluster){// && colorImage.has_value()){

//         //        std::fill(filteringMask.begin(),filteringMask.end(), 0);

//         //        for_each(std::execution::seq, std::begin(indicesDepthsSortedCorrespondanceNoBorders), std::end(indicesDepthsSortedCorrespondanceNoBorders), [&](const auto &t){

//         //            auto id = indicesDepths1DNoBorders[std::get<0>(t)];
//         //            if(depthMask[id] == 0){
//         //                return;
//         //            }

//         //            size_t count = 0;
//         //            float currDepth = depthBuffer[id];

//         //            for(auto cId : indicesNeighbours8Depth1D[id]){

//         //                if(depthMask[cId] == 0){
//         //                    continue;
//         //                }

//         //                if(currDepth > depthBuffer[cId]){
//         //                    continue;
//         //                }

//         //                if(filteringMask[cId] == 0){
//         //                    ++count;
//         //                    continue;
//         //                }

//         //                if(std::abs(currDepth - depthBuffer[cId]) > f.maxLocalDiff){
//         //                    ++count;
//         //                }
//         //            }

//         //            filteringMask[id] = count > f.nbMinNeighbours ? 1 : 0;
//         //        });

//         //        for(size_t ii = 0; ii < filteringMask.size(); ++ii){
//         //            if(filteringMask[ii] == 1){
//         //                depthMask[ii] = 0;
//         //            }
//         //        }



//         //        // detect edge
//         //        // si même couleur et depth > 0
//         //        for(size_t numLoop = 0; numLoop < f.minNeighboursLoops; ++numLoop){

//         //            std::fill(filteringMask.begin(),filteringMask.end(), 0);

//         //            for_each(std::execution::par_unseq, std::begin(indicesDepths1DNoBorders), std::end(indicesDepths1DNoBorders), [&](size_t id){

//         //                if(depthMask[id] == 0){
//         //                    return;
//         //                }

//         //                size_t count = 0;
//         //                float currDepth = depthBuffer[id];

//         //                for(auto cId : indicesNeighbours8Depth1D[id]){

//         //                    if(depthMask[cId] == 0){
//         //                        continue;
//         //                    }

//         ////                    if(filteringMask[cId] == 0){
//         ////                        ++count;
//         ////                        continue;
//         ////                    }

//         //                    if(std::abs(currDepth - depthBuffer[cId]) > f.maxLocalDiff){
//         //                        ++count;
//         //                    }
//         //                }

//         //                filteringMask[id] = count > f.nbMinNeighbours ? 1 : 0;
//         //            });

//         //            for(size_t ii = 0; ii < filteringMask.size(); ++ii){
//         //                if(filteringMask[ii] == 1){
//         //                    depthMask[ii] = 0;
//         //                }
//         //            }
//         //        }
//     }

//     // keep only biggest cluster
//     if(f.keepOnlyBiggestCluster){
//         keep_only_biggest_cluster();
//     }else{
//         meanBiggestZoneId = 0;
//     }

//     // count valid depth values
//     validDepthValues = 0;
//     for_each(std::execution::unseq, std::begin(indicesDepths1D), std::end(indicesDepths1D), [&](size_t id){
//         if(depthMask[id] == 0){
//             depthBuffer[id] = dc_invalid_depth_value;
//             //            colorBuffer[id].rgba() = {255,0,0,255};
//             indexDepthVertexCorrrespondance[id] = {id, -1};
//         }else{
//             indexDepthVertexCorrrespondance[id] = {id, validDepthValues};
//             validDepthValues++;
//         }
//     });
//     Bench::stop();


//     return;



//     //    // smooth filtering
//     //    if(f.keepOnlyBiggestCluster){

//     //        std::fill(depthFiltering.begin(), depthFiltering.end(), k4_invalid_depth_value);

//     //        float k = 1.f/9.f;

//     //        for_each(std::execution::par_unseq, std::begin(indicesDepths1DNoBorders), std::end(indicesDepths1DNoBorders), [&](size_t id){

//     //            if(depthMask[id] == 0){
//     //                return;
//     //            }

//     //            float total = 0.f;
//     //            for(auto cId : neighbours8Depth1D[id]){
//     //                if(depthMask[cId] == 1){
//     //                    total += depthBuffer[cId]*k;
//     //                }else{
//     //                    depthFiltering[id] = depthBuffer[id];
//     //                    return;
//     //                }
//     //            }
//     //            depthFiltering[id] = static_cast<std::int16_t>(total + depthBuffer[id]*k);f
//     //        });

//     //        std::copy(depthFiltering.begin(), depthFiltering.end(), depthBuffer);
//     //    }

// }
