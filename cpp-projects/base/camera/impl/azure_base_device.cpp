
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


#include "azure_base_device.hpp"


// local
#include "utility/logger.hpp"
#include "utility/benchmark.hpp"

using namespace  std::string_literals;
using namespace tool::cam;


AzureBaseDevice::AzureBaseDevice(){

    // init audio manager
    auto audioM = k4a::K4AAudioManager::get_instance();
    if(!audioM->is_initialized()){
        const int audioInitStatus = audioM->initialize();
        if (audioInitStatus != SoundIoErrorNone){
            Logger::error("[AzureBaseDevice] Failed to initialize audio backend: {}\n", soundio_strerror(audioInitStatus));
        }
    }
}

auto AzureBaseDevice::initialize(const DCModeInfos &mInfos, const DCConfigSettings &configS) -> void{

    // reset images
    colorImage           = nullptr;
    depthImage           = nullptr;
    infraredImage        = nullptr;
    pointCloudImage      = nullptr;   

    if(mInfos.has_depth()){
        depthSizedColorImage = k4a::image::create(
            K4A_IMAGE_FORMAT_COLOR_BGRA32,
            mInfos.depth_width(),
            mInfos.depth_height(),
            mInfos.depth_width() * 4
        );
    }

    if(mInfos.has_depth() && mInfos.has_cloud()){

        pointCloudImage = k4a::image::create(
            K4A_IMAGE_FORMAT_CUSTOM,
            mInfos.depth_width(),
            mInfos.depth_height(),
            mInfos.depth_width() * 3 * sizeof(std::int16_t)
        );
    }

    // init k4a configs
    k4aConfig   = generate_config(device->is_sync_in_connected(), device->is_sync_out_connected(), configS);
    k4aBtConfig = generate_bt_config(configS);
}

auto AzureBaseDevice::update_camera_from_colors_settings(bool isCameraReading, const DCColorSettings &colorS) -> void{

    k4a_color_control_command_t type;
    try{
        set_property_value(device.get(), type = K4A_COLOR_CONTROL_EXPOSURE_TIME_ABSOLUTE,   colorS.exposureTime,                                                 !colorS.autoExposureTime);
        set_property_value(device.get(), type = K4A_COLOR_CONTROL_WHITEBALANCE,             colorS.whiteBalance,                                                 !colorS.autoWhiteBalance);
        set_property_value(device.get(), type = K4A_COLOR_CONTROL_BRIGHTNESS,               colorS.brightness,                                                   true);
        set_property_value(device.get(), type = K4A_COLOR_CONTROL_CONTRAST,                 colorS.contrast,                                                     true);
        set_property_value(device.get(), type = K4A_COLOR_CONTROL_SHARPNESS,                colorS.sharpness,                                                    true);
        set_property_value(device.get(), type = K4A_COLOR_CONTROL_SATURATION,               colorS.saturation,                                                   true);
        set_property_value(device.get(), type = K4A_COLOR_CONTROL_BACKLIGHT_COMPENSATION,   colorS.backlightCompensation ? 1 : 0,                                true);
        set_property_value(device.get(), type = K4A_COLOR_CONTROL_GAIN,                     colorS.gain,                                                         true);
        set_property_value(device.get(), type = K4A_COLOR_CONTROL_POWERLINE_FREQUENCY,      convert_to_k4a_powerline_frequency_value(colorS.powerlineFrequency), true);

    }  catch (std::runtime_error error) {
        Logger::error(std::format("[AzureBaseDevice::update_camera_from_colors_settings] Set color settings error: {} T:{}\n", error.what(), static_cast<int>(type)));
    }
}

auto AzureBaseDevice::open_device(uint32_t deviceId) -> bool{

    try {
        device  = std::make_unique<k4a::device>(k4a::device::open(deviceId));

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
        Logger::error(std::format("[AzureBaseDevice::open_device] open error: {}\n", error.what()));
        return false;
    }

    auto audioM = k4a::K4AAudioManager::get_instance();
    audioM->refresh_devices();
    size_t nbDevices = audioM->get_devices_count();
    Logger::message(std::format("[AzureBaseDevice::open_device] Audio devices count: {}\n", nbDevices));

    for(size_t ii = 0; ii < nbDevices; ++ii){
        std::string deviceName = audioM->get_device_name(ii);
        Logger::message(std::format(" - {}\n", deviceName));
        if (deviceName.find("Azure Kinect Microphone Array") != std::string::npos) {
            Logger::message(std::format("Found Azure kinect microphones array.\n"));

            microphone = audioM->get_microphone_for_device(deviceName);
            if(microphone == nullptr){
                Logger::error(std::format("[AzureBaseDevice::open_device] Cannot retrieve microphone.\n"));
                audioListener = nullptr;
                return false;
            }

            Logger::message("Start microphone. \n");
            if(microphone->Start() != SoundIoErrorNone){
                Logger::error(std::format("[AzureBaseDevice] Cannot start microphone.\n"));
                return false;
            }

            if(microphone->IsStarted()){
                Logger::message("Create listener. \n");
                audioListener = microphone->CreateListener();
                if(audioListener == nullptr){
                    Logger::error(std::format("[AzureBaseDevice] Cannot init audio listener.\n"));
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

auto AzureBaseDevice::start_device(const DCConfigSettings &configS) -> bool{

    capture = std::make_unique<k4a::capture>();

    try {

        Logger::message("Start cameras\n");
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
        Logger::message(std::format("      metric radius: {}\n",    c.depth_camera_calibration.metric_radius));

        // auto rot = c.depth_camera_calibration.extrinsics.rotation;
        // Logger::message(std::format("      rot ext: {} {} {} {} {} {} {} {} {}\n",  rot[0], rot[1], rot[2], rot[3],  rot[4], rot[5], rot[6], rot[7], rot[8]));
        // auto tr = c.depth_camera_calibration.extrinsics.translation;
        // Logger::message(std::format("      tr ext: {} {} {}\n",  tr[0], tr[1], tr[2]));

        Logger::message("Start imu\n");
        device->start_imu();

        if((dc_depth_resolution(configS.mode) != DCDepthResolution::OFF) && configS.enableBodyTracking){
            Logger::message("[AzureBaseDevice::start_device] Start body tracker\n");
            bodyTracker = std::make_unique<k4abt::tracker>(k4abt::tracker::create(calibration, k4aBtConfig));
        }

    }  catch (k4a::error error) {
        Logger::error("[AzureBaseDevice::start_device] start_cameras k4a error: {}\n", error.what());
        k4aConfig = K4A_DEVICE_CONFIG_INIT_DISABLE_ALL;
        return false;
    }  catch (std::runtime_error error) {
        Logger::error("[AzureBaseDevice::start_device] start_cameras runtime error: {}\n", error.what());
        k4aConfig = K4A_DEVICE_CONFIG_INIT_DISABLE_ALL;
        return false;
    }

    return true;
}

auto AzureBaseDevice::stop_device() -> void{

    if(bodyTracker != nullptr){
        Logger::message("[AzureBaseDevice::stop_device] Stop body tracker\n");
        bodyTracker->shutdown();
        bodyTracker = nullptr;
    }

    if(is_opened()){
        Logger::message("Stop IMU\n");
        device->stop_imu();
        Logger::message("Stop cameras\n");
        device->stop_cameras();
    }

}

auto AzureBaseDevice::close_device() -> void{

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

auto AzureBaseDevice::is_opened() const noexcept -> bool{
    return device != nullptr;
}

auto AzureBaseDevice::nb_devices() const noexcept -> size_t{
    return k4a::device::get_installed_count();
}

auto AzureBaseDevice::device_name() const noexcept -> std::string{
    if(is_opened()){
        return device->get_serialnum();
    }else{
        return "unknow_device"s;
    }
}

auto AzureBaseDevice::capture_frame(int32_t timeoutMs) -> bool{

    if(!is_opened()){
        return false;
    }

    Bench::start("[AzureBaseDevice::capture_frame]");
    bool success = false;
    try{
        success = device->get_capture(capture.get(), std::chrono::milliseconds(timeoutMs));
    }catch(const std::runtime_error &e){
        Logger::error(std::format("[AzureBaseDevice::read_frames] Get capture runtime error: {}\n", e.what()));
    }
    Bench::stop();
    return success;

}

auto AzureBaseDevice::read_color_image() -> std::span<std::int8_t>{

    if(capture->is_valid()){

        Bench::start("[AzureBaseDevice::read_color_image]");
        colorImage = capture->get_color_image();
        Bench::stop();

        if (colorImage.is_valid()){
            return std::span<std::int8_t>{
                reinterpret_cast<std::int8_t*>(colorImage.get_buffer()),
                colorImage.get_size()
            };
        }
    }
    return {};
}

auto AzureBaseDevice::read_depth_image() -> std::span<std::uint16_t>{

    if(capture->is_valid()){

        Bench::start("[AzureBaseDevice::read_depth_image]");
        depthImage = capture->get_depth_image();
        Bench::stop();

        if (depthImage.is_valid()){
            return std::span<std::uint16_t>{
                reinterpret_cast<std::uint16_t*>(depthImage.get_buffer()),
                static_cast<size_t>(depthImage.get_width_pixels()*depthImage.get_height_pixels())
            };
        }
    }
    return {};
}

auto AzureBaseDevice::read_infra_image() -> std::span<std::uint16_t>{

    if(capture->is_valid()){

        Bench::start("[AzureBaseDevice::read_infra_image]");
        infraredImage = capture->get_ir_image();
        Bench::stop();

        if (infraredImage.is_valid()){
            return std::span<std::uint16_t>{
                reinterpret_cast<std::uint16_t*>(infraredImage.get_buffer()),
                static_cast<size_t>(infraredImage.get_width_pixels()*infraredImage.get_height_pixels())
            };
        }
    }
    return {};
}

auto AzureBaseDevice::read_from_microphones() -> std::pair<size_t, std::span<float>> {

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
            Logger::error(std::format("[AzureBaseDevice::read_from_microphones] Error while recording {}\n", soundio_strerror(audioListener->GetStatus())));
            return {};
        }else if (audioListener->Overflowed()){
            Logger::warning(std::format("[AzureBaseDevice::read_from_microphones] Warning: sound overflow detected!\n"));
            audioListener->ClearOverflowed();
            return {};
        }

        return {7, std::span<float>{
            reinterpret_cast<float*>(audioFrames.data()),
            static_cast<size_t>(7* lastFrameCount)
        }};
    }
    return {0, {}};
}

auto AzureBaseDevice::read_from_imu() -> std::optional<DCImuSample>{

    k4a_imu_sample_t sample;
    if(device->get_imu_sample(&sample, std::chrono::milliseconds(1))){

        DCImuSample imuS;
        imuS.temperature = sample.temperature;
        const auto &dAcc = sample.acc_sample.xyz;
        imuS.acc ={dAcc.x,dAcc.y,dAcc.z};
        imuS.accTsMs = sample.acc_timestamp_usec;
        const auto &dGyr = sample.gyro_sample.xyz;
        imuS.gyr = {dGyr.x,dGyr.y,dGyr.z};
        imuS.gyrTsMs = sample.gyro_timestamp_usec;
        return imuS;

    }
    return {};
}

auto AzureBaseDevice::read_bodies(std::vector<DCBody> &bodies) -> std::span<uint8_t>{

    if(bodyTracker != nullptr){
        if(bodyTracker->enqueue_capture(*capture.get(), std::chrono::milliseconds(1))){
            if(k4abt::frame bodyFrame = bodyTracker->pop_result(std::chrono::milliseconds(1)); bodyFrame != nullptr){
                auto bodiesCount = bodyFrame.get_num_bodies();
                if(bodies.size() < bodiesCount){
                    bodies.resize(bodiesCount);
                }
                for(size_t ii = 0; ii < bodiesCount; ++ii){
                    update_k4_body(bodies[ii], bodyFrame.get_body(static_cast<int>(ii)));
                }
                bodiesIndexImage = bodyFrame.get_body_index_map();

                if(bodiesIndexImage.is_valid()){
                    return std::span<std::uint8_t>{
                        reinterpret_cast<std::uint8_t*>(bodiesIndexImage.get_buffer()),
                        static_cast<size_t>(bodiesIndexImage.get_width_pixels() * bodiesIndexImage.get_height_pixels())
                    };
                }
            }
        }
    }
    return {};
}

auto AzureBaseDevice::resize_color_image_to_depth_size(const DCModeInfos &mInfos, std::span<ColorRGBA8> colorData) -> std::span<tool::ColorRGBA8>{
    
    auto colorStride     = mInfos.color_width() * 4 * sizeof(std::uint8_t);
    auto colorSizeBytes  = colorStride * mInfos.color_height();
    auto colorDataBuffer = reinterpret_cast<std::uint8_t*>(colorData.data());

    auto k4aColorImage = k4a::image::create_from_buffer(
        K4A_IMAGE_FORMAT_COLOR_BGRA32,
        mInfos.color_width(), mInfos.color_height(), colorStride, colorDataBuffer, colorSizeBytes,
        nullptr, nullptr
    );

    try{
        Bench::start("[AzureKinectDeviceImpl::resize_color_to_fit_depth]");
        transformation.color_image_to_depth_camera(
            depthImage,
            k4aColorImage,
            &depthSizedColorImage
        );
        Bench::stop();

    }catch(const std::runtime_error &error){
        // ...
        return {};
    }

    return std::span<tool::ColorRGBA8>{
        reinterpret_cast<tool::ColorRGBA8*>(depthSizedColorImage.get_buffer()),
        static_cast<size_t>(depthSizedColorImage.get_width_pixels() * depthSizedColorImage.get_height_pixels())
    };
}


auto AzureBaseDevice::generate_cloud() -> std::span<geo::Pt3<std::int16_t>>{

    try{
        Bench::start("[AzureBaseDevice::generate_cloud]");
        transformation.depth_image_to_point_cloud(depthImage, K4A_CALIBRATION_TYPE_DEPTH, &pointCloudImage);
        Bench::stop();

    }catch(const std::runtime_error &error){
        // ...
        return {};
    }

    return std::span<geo::Pt3<std::int16_t>>{
        reinterpret_cast<geo::Pt3<std::int16_t>*>(pointCloudImage.get_buffer()),
        static_cast<size_t>(pointCloudImage.get_width_pixels()*pointCloudImage.get_height_pixels())
    };
}

auto AzureBaseDevice::generate_config(bool synchInConnected, bool synchOutConnected, const DCConfigSettings &config) -> k4a_device_configuration_t{

    k4a_device_configuration_t ka4Config        = K4A_DEVICE_CONFIG_INIT_DISABLE_ALL;
    ka4Config.color_format                      = convert_to_k4a_image_format(dc_image_format(config.mode));
    ka4Config.color_resolution                  = convert_to_k4a_color_resolution(dc_color_resolution(config.mode));
    ka4Config.depth_mode                        = convert_to_k4a_depth_mode(dc_depth_resolution(config.mode));
    ka4Config.camera_fps                        = convert_to_k4a_fps(dc_framerate(config.mode));
    ka4Config.synchronized_images_only          = (ka4Config.depth_mode != K4A_DEPTH_MODE_OFF) ? config.synchronizeColorAndDepth : false;
    ka4Config.depth_delay_off_color_usec        = config.delayBetweenColorAndDepthUsec;
    ka4Config.disable_streaming_indicator       = config.disableLED;
    ka4Config.wired_sync_mode                   = convert_to_k4a_synch_mode(config.synchMode);
    ka4Config.subordinate_delay_off_master_usec = config.subordinateDelayUsec;

    // check modes
    if(ka4Config.wired_sync_mode == K4A_WIRED_SYNC_MODE_SUBORDINATE){
        if(!synchInConnected){
            ka4Config.wired_sync_mode = K4A_WIRED_SYNC_MODE_STANDALONE;
            Logger::warning("[AzureBaseDevice::generate_config] No input synchronisation cable found, switch from [Subordinate] to [Standalone] mode and set subordinate delay to [0].\n");
        }
    }else if(ka4Config.wired_sync_mode == K4A_WIRED_SYNC_MODE_MASTER){
        if(!synchOutConnected){
            ka4Config.wired_sync_mode = K4A_WIRED_SYNC_MODE_STANDALONE;
            Logger::warning("[AzureBaseDevice::generate_config] No output synchronisation cable found, switch from [Master] to [Standalone] mode.\n");
        }
    }

    if((ka4Config.wired_sync_mode == K4A_WIRED_SYNC_MODE_MASTER) && (config.subordinateDelayUsec != 0)){
        Logger::warning("[AzureBaseDevice::generate_config] Subordinate delay != 0 for mode [Master], subordinate delay is now set to [0].\n");
        ka4Config.subordinate_delay_off_master_usec = 0;
    }else if ((ka4Config.wired_sync_mode == K4A_WIRED_SYNC_MODE_STANDALONE) && (config.subordinateDelayUsec != 0)){
        Logger::warning("[AzureBaseDevice::generate_config] Subordinate delay != 0 for mode [Standalone], subordinate delay is now set to [0].\n");
        ka4Config.subordinate_delay_off_master_usec = 0;
    }

    return ka4Config;
}

auto AzureBaseDevice::generate_bt_config(const DCConfigSettings &config) -> k4abt_tracker_configuration_t{
    k4abt_tracker_configuration_t ka4BtConfig;
    ka4BtConfig.gpu_device_id       = config.btGPUId;
    ka4BtConfig.processing_mode     = convert_to_k4a_body_tracking_processing_mode(config.btProcessingMode);
    ka4BtConfig.sensor_orientation  = convert_to_k4a_sensor_orientation(config.btOrientation);
    ka4BtConfig.model_path          = nullptr;
    return ka4BtConfig;
}

auto AzureBaseDevice::set_property_value(k4a::device *dev, k4a_color_control_command_t pId, int32_t value, bool manual) -> void{

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

auto AzureBaseDevice::update_k4_body(DCBody &body, const k4abt_body_t &k4aBody) -> void{
    body.id = static_cast<std::int8_t>(k4aBody.id);
    for(const auto &jointD : dcJoints.data){
        const auto &kaKoint = k4aBody.skeleton.joints[static_cast<int>(std::get<0>(jointD))];
        auto &joint = body.skeleton.joints[static_cast<int>(std::get<0>(jointD))];
        joint.confidence = convert_to_confidence_level(kaKoint.confidence_level);
        const auto &p = kaKoint.position;
        joint.position = {-p.v[0],-p.v[1],p.v[2]};
        const auto &o = kaKoint.orientation;
        joint.orientation = {o.wxyz.x,o.wxyz.y,o.wxyz.z,o.wxyz.w};
    }
}

