
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

// k4a
#include "azure_utility.hpp"
#include "thirdparty/k4a/k4amicrophonelistener.h"
#include "thirdparty/k4a/k4aaudiomanager.h"
// #include "thirdparty/k4a/k4astaticimageproperties.h"

// local
#include "utility/logger.hpp"

using namespace  std::string_literals;
using namespace tool::cam;

struct AzureBaseDevice::Impl{

    // static inline std::vector<std::string> devicesList = {};

    // device
    size_t deviceCount = 0;
    std::string deviceName = "unknow_device"s;
    std::unique_ptr<k4a::device> device = nullptr;
    k4a::calibration calibration;
    k4a::transformation transformation;
    k4a_device_configuration_t k4aConfig = K4A_DEVICE_CONFIG_INIT_DISABLE_ALL;
    std::unique_ptr<k4a::capture> capture = nullptr;

    // audio
    std::shared_ptr<k4a::K4AMicrophone> microphone = nullptr;
    std::shared_ptr<k4a::K4AMicrophoneListener> audioListener = nullptr;
    size_t lastFrameCount = 0;
    std::vector<k4a::K4AMicrophoneFrame> audioFrames;

    // body tracking
    k4abt_tracker_configuration_t k4aBtConfig = K4ABT_TRACKER_CONFIG_DEFAULT;
    std::unique_ptr<k4abt::tracker> bodyTracker = nullptr;
    std::vector<DCBody> bodies;

    // frames data
    k4a::image colorImage         = nullptr;
    k4a::image depthImage         = nullptr;
    k4a::image infraredImage      = nullptr;
    k4a::image pointCloudImage    = nullptr;
    k4a::image bodiesIndexImage   = nullptr;
    k4a::image depthSizedColorImage = nullptr;

    static auto generate_config(bool synchInConnected, bool synchOutConnected, const DCConfigSettings &config) -> k4a_device_configuration_t;
    static auto generate_bt_config(const DCConfigSettings &config) -> k4abt_tracker_configuration_t;
    static auto set_property_value(k4a::device *dev, k4a_color_control_command_t pId, std::int32_t value, bool manual) -> void;
    static auto update_k4_body(DCBody &body, const k4abt_body_t &k4aBody) -> void;
};

auto AzureBaseDevice::Impl::generate_config(bool synchInConnected, bool synchOutConnected, const DCConfigSettings &config) -> k4a_device_configuration_t{

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

auto AzureBaseDevice::Impl::generate_bt_config(const DCConfigSettings &config) -> k4abt_tracker_configuration_t{
    k4abt_tracker_configuration_t ka4BtConfig;
    ka4BtConfig.gpu_device_id       = config.btGPUId;
    ka4BtConfig.processing_mode     = convert_to_k4a_body_tracking_processing_mode(config.btProcessingMode);
    ka4BtConfig.sensor_orientation  = convert_to_k4a_sensor_orientation(config.btOrientation);
    ka4BtConfig.model_path          = nullptr;
    return ka4BtConfig;
}

auto AzureBaseDevice::Impl::set_property_value(k4a::device *dev, k4a_color_control_command_t pId, int32_t value, bool manual) -> void{

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

auto AzureBaseDevice::Impl::update_k4_body(DCBody &body, const k4abt_body_t &k4aBody) -> void{
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



AzureBaseDevice::AzureBaseDevice() : i(std::make_unique<Impl>()){

    i->deviceCount = k4a::device::get_installed_count();
    // if(i->devicesList.empty()){

    //     std::cout << "list device\n";
    //     for(size_t idD = 0; idD < i->deviceCount; ++idD){
    //         try {
    //             auto device = k4a::device::open(i->devicesList.size());
    //             i->devicesList.push_back(device.get_serialnum());
    //             device.close();
    //             std::cout << "-> " << i->devicesList.back() << "\n";
    //         }catch(const k4a::error &e){
    //             std::cout << "cannot list device id " << idD << "\n";
    //         }
    //     }
    // }


    // init audio manager
    auto audioM = k4a::K4AAudioManager::get_instance();
    if(!audioM->is_initialized()){
        const int audioInitStatus = audioM->initialize();
        if (audioInitStatus != SoundIoErrorNone){
            Logger::error("[AzureBaseDevice] Failed to initialize audio backend: {}\n", soundio_strerror(audioInitStatus));
        }
    }
}

AzureBaseDevice::~AzureBaseDevice(){
}

auto AzureBaseDevice::initialize(const DCModeInfos &mInfos, const DCConfigSettings &configS) -> void{

    // reset images
    i->colorImage           = nullptr;
    i->depthImage           = nullptr;
    i->infraredImage        = nullptr;
    i->pointCloudImage      = nullptr;

    if(mInfos.has_depth()){
        i->depthSizedColorImage = k4a::image::create(
            K4A_IMAGE_FORMAT_COLOR_BGRA32,
            static_cast<int>(mInfos.depth_width()),
            static_cast<int>(mInfos.depth_height()),
            static_cast<int>(mInfos.depth_width() * 4)
        );
    }

    if(mInfos.has_depth()){

        i->pointCloudImage = k4a::image::create(
            K4A_IMAGE_FORMAT_CUSTOM,
            static_cast<int>(mInfos.depth_width()),
            static_cast<int>(mInfos.depth_height()),
            static_cast<int>(mInfos.depth_width() * 3 * sizeof(std::int16_t))
        );
    }

    // init k4a configs
    i->k4aConfig   = i->generate_config(i->device->is_sync_in_connected(), i->device->is_sync_out_connected(), configS);
    i->k4aBtConfig = i->generate_bt_config(configS);
}

auto AzureBaseDevice::update_from_colors_settings(const DCColorSettings &colorS) -> void{

    if(!is_opened()){
        return;
    }

    k4a_color_control_command_t type;
    try{
        i->set_property_value(i->device.get(), type = K4A_COLOR_CONTROL_EXPOSURE_TIME_ABSOLUTE,   colorS.exposureTime,                                                 !colorS.autoExposureTime);
        i->set_property_value(i->device.get(), type = K4A_COLOR_CONTROL_WHITEBALANCE,             colorS.whiteBalance,                                                 !colorS.autoWhiteBalance);
        i->set_property_value(i->device.get(), type = K4A_COLOR_CONTROL_BRIGHTNESS,               colorS.brightness,                                                   true);
        i->set_property_value(i->device.get(), type = K4A_COLOR_CONTROL_CONTRAST,                 colorS.contrast,                                                     true);
        i->set_property_value(i->device.get(), type = K4A_COLOR_CONTROL_SHARPNESS,                colorS.sharpness,                                                    true);
        i->set_property_value(i->device.get(), type = K4A_COLOR_CONTROL_SATURATION,               colorS.saturation,                                                   true);
        i->set_property_value(i->device.get(), type = K4A_COLOR_CONTROL_BACKLIGHT_COMPENSATION,   colorS.backlightCompensation ? 1 : 0,                                true);
        i->set_property_value(i->device.get(), type = K4A_COLOR_CONTROL_GAIN,                     colorS.gain,                                                         true);
        i->set_property_value(i->device.get(), type = K4A_COLOR_CONTROL_POWERLINE_FREQUENCY,      convert_to_k4a_powerline_frequency_value(colorS.powerlineFrequency), true);

    }  catch (std::runtime_error error) {
        Logger::error(std::format("[AzureBaseDevice::update_camera_from_colors_settings] Set color settings error: {} T:{}\n", error.what(), static_cast<int>(type)));
    }
}

auto AzureBaseDevice::update_from_data_settings(const DCDataSettings &dataS) -> void{
    if(i->bodyTracker != nullptr){
        i->bodyTracker->set_temporal_smoothing(dataS.client.capture.btTemporalSmoothing);
    }
}

auto AzureBaseDevice::open_device(uint32_t deviceId) -> bool{

    try {
        i->device     = std::make_unique<k4a::device>(k4a::device::open(deviceId));
        i->deviceName = i->device->get_serialnum();

        const auto version  = i->device->get_version();
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
        Logger::message(std::format("  Serialnum: {}\n", i->device->get_serialnum()));
        Logger::message("  Version:\n");
        Logger::message(std::format("      Firmware build: {}\n", (debugFB ? "[debug]" : "[release]")));
        Logger::message(std::format("      Firmware signature: {}\n", fsStr));
        Logger::message(std::format("      Color camera firmware version {}.{}.{}\n", version.rgb.major, version.rgb.minor, version.rgb.iteration));
        Logger::message(std::format("      Depth camera firmware version {}.{}.{}\n", version.depth.major, version.depth.minor, version.depth.iteration));
        Logger::message(std::format("      Audio device firmware version {}.{}.{}\n", version.audio.major, version.audio.minor, version.audio.iteration));
        Logger::message(std::format("      Depth device firmware version {}.{}.{}\n", version.depth_sensor.major, version.depth_sensor.minor, version.depth_sensor.iteration));
        Logger::message("  Synch:\n");
        Logger::message(std::format("      IN connected {}\n", i->device->is_sync_in_connected()));
        Logger::message(std::format("      OUT connected {}\n",i->device->is_sync_out_connected()));


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

            i->microphone = audioM->get_microphone_for_device(deviceName);
            if(i->microphone == nullptr){
                Logger::error(std::format("[AzureBaseDevice::open_device] Cannot retrieve microphone.\n"));
                i->audioListener = nullptr;
                return false;
            }

            Logger::message("Start microphone. \n");
            if(i->microphone->Start() != SoundIoErrorNone){
                Logger::error(std::format("[AzureBaseDevice] Cannot start microphone.\n"));
                return false;
            }

            if(i->microphone->IsStarted()){
                Logger::message("Create listener. \n");
                i->audioListener = i->microphone->CreateListener();
                if(i->audioListener == nullptr){
                    Logger::error(std::format("[AzureBaseDevice] Cannot init audio listener.\n"));
                    return false;
                }
            }
        }
    }

    if(i->audioListener != nullptr){
        Logger::message("Microphone listener created. \n");
    }

    return true;
}

auto AzureBaseDevice::start_device(const DCConfigSettings &configS) -> bool{

    i->capture = std::make_unique<k4a::capture>();

    try {

        Logger::message("Start cameras\n");
        Logger::message("[Config]\n");
        Logger::message(std::format("   color format: {}\n",    static_cast<int>(i->k4aConfig.color_format)));
        Logger::message(std::format("   depth mode: {}\n",      static_cast<int>(i->k4aConfig.depth_mode)));
        i->device->start_cameras(&i->k4aConfig);

        Logger::message("Retrieve calibration\n");
        i->calibration     = i->device->get_calibration(i->k4aConfig.depth_mode, i->k4aConfig.color_resolution);
        i->transformation  = k4a::transformation(i->calibration);

        display_calibration(i->calibration);

        const auto &c = i->calibration;
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

        Logger::message("Start imu\n");
        i->device->start_imu();

        if((dc_depth_resolution(configS.mode) != DCDepthResolution::OFF) && configS.btEnabled){
            Logger::message("[AzureBaseDevice::start_device] Start body tracker\n");
            i->bodyTracker = std::make_unique<k4abt::tracker>(k4abt::tracker::create(i->calibration, i->k4aBtConfig));
        }

    }  catch (k4a::error error) {
        Logger::error("[AzureBaseDevice::start_device] start_cameras k4a error: {}\n", error.what());
        i->k4aConfig = K4A_DEVICE_CONFIG_INIT_DISABLE_ALL;
        return false;
    }  catch (std::runtime_error error) {
        Logger::error("[AzureBaseDevice::start_device] start_cameras runtime error: {}\n", error.what());
        i->k4aConfig = K4A_DEVICE_CONFIG_INIT_DISABLE_ALL;
        return false;
    }

    return true;
}

auto AzureBaseDevice::stop_device() -> void{

    if(i->bodyTracker != nullptr){
        Logger::message("[AzureBaseDevice::stop_device] Stop body tracker\n");
        i->bodyTracker->shutdown();
        i->bodyTracker = nullptr;
    }

    if(is_opened()){
        Logger::message("Stop IMU\n");
        i->device->stop_imu();
        Logger::message("Stop cameras\n");
        i->device->stop_cameras();
    }

}

auto AzureBaseDevice::close_device() -> void{

    if(i->microphone != nullptr){
        if(i->microphone->IsStarted()){
            i->microphone->Stop();
        }
        i->audioListener = nullptr;
        i->microphone = nullptr;
    }

    if(is_opened()){
        i->device->close();
        i->device = nullptr;
    }    
}

auto AzureBaseDevice::is_opened() const noexcept -> bool{
    return i->device != nullptr;
}

auto AzureBaseDevice::nb_devices() const noexcept -> size_t{    
    return i->deviceCount;
}

auto AzureBaseDevice::device_name() const noexcept -> std::string{
    return i->deviceName;
}

auto AzureBaseDevice::read_calibration() -> BinarySpan{
    return BinarySpan{
        reinterpret_cast<std::byte*>(&i->calibration),
        static_cast<size_t>(sizeof(k4a_calibration_t))
    };
}

auto AzureBaseDevice::capture_frame(int32_t timeoutMs) -> bool{

    if(!is_opened()){
        return false;
    }

    bool success = false;
    try{
        success = i->device->get_capture(i->capture.get(), std::chrono::milliseconds(timeoutMs));
    }catch(const std::runtime_error &e){
        Logger::error(std::format("[AzureBaseDevice::read_frames] Get capture runtime error: {}\n", e.what()));
    }
    return success;
}

auto AzureBaseDevice::read_color_image() -> BinarySpan{

    if(i->capture->is_valid()){

        i->colorImage = i->capture->get_color_image();

        if (i->colorImage.is_valid()){
            return BinarySpan{
                reinterpret_cast<std::byte*>(i->colorImage.get_buffer()),
                i->colorImage.get_size()
            };
        }
    }
    return {};
}

auto AzureBaseDevice::read_depth_image() -> std::span<std::uint16_t>{

    if(i->capture->is_valid()){

        i->depthImage = i->capture->get_depth_image();

        if (i->depthImage.is_valid()){
            return std::span<std::uint16_t>{
                reinterpret_cast<std::uint16_t*>(i->depthImage.get_buffer()),
                static_cast<size_t>(i->depthImage.get_width_pixels()*i->depthImage.get_height_pixels())
            };
        }
    }
    return {};
}

auto AzureBaseDevice::read_infra_image() -> std::span<std::uint16_t>{

    if(i->capture->is_valid()){

        i->infraredImage = i->capture->get_ir_image();

        if (i->infraredImage.is_valid()){
            return std::span<std::uint16_t>{
                reinterpret_cast<std::uint16_t*>(i->infraredImage.get_buffer()),
                static_cast<size_t>(i->infraredImage.get_width_pixels()*i->infraredImage.get_height_pixels())
            };
        }
    }
    return {};
}

auto AzureBaseDevice::read_from_microphones() -> std::pair<size_t, std::span<float>> {

    i->lastFrameCount = 0;
    if(i->audioListener != nullptr){

        // process audio frame
        i->audioListener->ProcessFrames([&](k4a::K4AMicrophoneFrame *frame, const size_t frameCount) {

            // store last count
            i->lastFrameCount = frameCount;
            if(i->lastFrameCount == 0){
                return i->lastFrameCount;
            }

            // resize audio buffer
            if(i->audioFrames.size() < i->lastFrameCount){
                i->audioFrames.resize(i->lastFrameCount);
            }

            // copy data
            std::copy(frame, frame + i->lastFrameCount, i->audioFrames.begin());

            return i->lastFrameCount;
        });

        if (i->audioListener->GetStatus() != SoundIoErrorNone){
            Logger::error(std::format("[AzureBaseDevice::read_from_microphones] Error while recording {}\n", soundio_strerror(i->audioListener->GetStatus())));
            return {};
        }else if (i->audioListener->Overflowed()){
            Logger::warning(std::format("[AzureBaseDevice::read_from_microphones] Warning: sound overflow detected!\n"));
            i->audioListener->ClearOverflowed();
            return {};
        }

        return {7, std::span<float>{
            reinterpret_cast<float*>(i->audioFrames.data()),
            static_cast<size_t>(7* i->lastFrameCount)
        }};
    }
    return {0, {}};
}

auto AzureBaseDevice::read_from_imu() -> BinarySpan{

    k4a_imu_sample_t sample;
    if(i->device->get_imu_sample(&sample, std::chrono::milliseconds(1))){

        return BinarySpan{
            reinterpret_cast<std::byte*>(&sample),
            static_cast<size_t>(sizeof(k4a_imu_sample_t))
        };

        // DCImuSample imuS;
        // imuS.temperature = sample.temperature;
        // const auto &dAcc = sample.acc_sample.xyz;
        // imuS.acc ={dAcc.x,dAcc.y,dAcc.z};
        // imuS.accTsMs = sample.acc_timestamp_usec;
        // const auto &dGyr = sample.gyro_sample.xyz;
        // imuS.gyr = {dGyr.x,dGyr.y,dGyr.z};
        // imuS.gyrTsMs = sample.gyro_timestamp_usec;
        // return imuS;

    }
    return {};
}

auto AzureBaseDevice::read_body_tracking() -> std::tuple<std::span<ColorGray8>, std::span<DCBody>>{

    if(i->bodyTracker == nullptr){
        return {};
    }

    if(!i->bodyTracker->enqueue_capture(*i->capture.get(), std::chrono::milliseconds(1))){
        return {};
    }

    if(k4abt::frame bodyFrame = i->bodyTracker->pop_result(std::chrono::milliseconds(1)); bodyFrame != nullptr){

        auto bodiesCount = bodyFrame.get_num_bodies();
        if(i->bodies.size() < bodiesCount){
            i->bodies.resize(bodiesCount);
        }
        for(size_t ii = 0; ii < bodiesCount; ++ii){
            i->update_k4_body(i->bodies[ii], bodyFrame.get_body(static_cast<int>(ii)));
        }
        i->bodiesIndexImage = bodyFrame.get_body_index_map();

        std::span<ColorGray8> biISpan = {};
        if(i->bodiesIndexImage.is_valid()){
            biISpan = std::span<ColorGray8>{
                reinterpret_cast<ColorGray8*>(i->bodiesIndexImage.get_buffer()),
                static_cast<size_t>(i->bodiesIndexImage.get_width_pixels() * i->bodiesIndexImage.get_height_pixels())
            };
        }
        return std::make_tuple(biISpan, std::span<DCBody>{i->bodies});
    }

    return {{},{}};
}




auto AzureBaseDevice::resize_color_image_to_depth_size(const DCModeInfos &mInfos, std::span<ColorRGBA8> colorData) -> std::span<tool::ColorRGBA8>{
    
    auto colorStride     = mInfos.color_width() * 4 * sizeof(std::uint8_t);
    auto colorSizeBytes  = colorStride * mInfos.color_height();
    auto colorDataBuffer = reinterpret_cast<std::uint8_t*>(colorData.data());

    auto k4aColorImage = k4a::image::create_from_buffer(
        K4A_IMAGE_FORMAT_COLOR_BGRA32,
        static_cast<int>(mInfos.color_width()),
        static_cast<int>(mInfos.color_height()),
        static_cast<int>(colorStride),
        colorDataBuffer,
        colorSizeBytes,
        nullptr, nullptr
    );

    try{

        i->transformation.color_image_to_depth_camera(
            i->depthImage,
            k4aColorImage,
            &i->depthSizedColorImage
        );

    }catch(const std::runtime_error &error){
        Logger::error(std::format("[AzureBaseDevice::resize_color_image_to_depth_size] Error: {}\n", error.what()));
        return {};
    }

    return std::span<tool::ColorRGBA8>{
        reinterpret_cast<tool::ColorRGBA8*>(i->depthSizedColorImage.get_buffer()),
        static_cast<size_t>(i->depthSizedColorImage.get_width_pixels() * i->depthSizedColorImage.get_height_pixels())
    };
}

auto AzureBaseDevice::generate_cloud() -> std::span<geo::Pt3<std::int16_t>>{

    try{

        i->transformation.depth_image_to_point_cloud(
            i->depthImage,
            K4A_CALIBRATION_TYPE_DEPTH,
            &i->pointCloudImage
        );

    }catch(const std::runtime_error &error){
        // ...
        return {};
    }

    return std::span<geo::Pt3<std::int16_t>>{
        reinterpret_cast<geo::Pt3<std::int16_t>*>(i->pointCloudImage.get_buffer()),
        static_cast<size_t>(i->pointCloudImage.get_width_pixels()*i->pointCloudImage.get_height_pixels())
    };
}

