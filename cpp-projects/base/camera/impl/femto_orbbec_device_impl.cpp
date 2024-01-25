


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

#include "femto_orbbec_device_impl.hpp"

// std
#include <format>
#include <execution>

// stb
#include "thirdparty/stb/stb_image_resize.h"

// local
#include "utility/logger.hpp"
#include "utility/benchmark.hpp"

using namespace tool;
using namespace tool::geo;
using namespace tool::cam;

auto set_value(ob::Device *dev, OBPropertyID pId, bool value) -> void{

    if(!dev->isPropertySupported(pId, OB_PERMISSION_READ_WRITE)){
        return;
    }
    if(dev->getBoolProperty(pId) != value){
        dev->setBoolProperty(pId, value);
    }
}

auto set_value(ob::Device *dev, OBPropertyID pId, std::int32_t value) -> void{

    if(!dev->isPropertySupported(pId, OB_PERMISSION_READ_WRITE)){
        return;
    }

    auto range = dev->getIntPropertyRange(pId);
    if(value < range.min){
        value = range.min;
    }
    value += (value - range.min)%range.step;

    if(value > range.max){
        value = range.max;
    }

    if(range.cur != value){
        dev->setIntProperty(pId, value);
    }
}

FemtoOrbbecDeviceImpl::FemtoOrbbecDeviceImpl(){
    context      = std::make_unique<ob::Context>();
    context->enableNetDeviceEnumeration(true);
    context->setLoggerSeverity(OB_LOG_SEVERITY_WARN);
    deviceList   = context->queryDeviceList();
    Logger::message(std::format("FemtoOrbbecDeviceImpl: {} devices found.\n", deviceList->deviceCount()));
}

auto FemtoOrbbecDeviceImpl::open(uint32_t deviceId) -> bool{

    try {
        device     = deviceList->getDevice(settings.config.idDevice = deviceId);
        sensorList = device->getSensorList();

        for(uint32_t idS = 0; idS < sensorList->count(); idS++) {
            auto sensor = sensorList->getSensor(idS);
            switch(sensor->type()) {
            case OB_SENSOR_COLOR:
                Logger::message("[FemtoOrbbecDeviceImpl] Color sensor found.\n");
                break;
            case OB_SENSOR_DEPTH:
                Logger::message("[FemtoOrbbecDeviceImpl] Depth sensor found.\n");
                break;
            case OB_SENSOR_IR:
                Logger::message("[FemtoOrbbecDeviceImpl] Infrared sensor found.\n");
                break;
            case OB_SENSOR_IR_LEFT:
                Logger::message("[FemtoOrbbecDeviceImpl] Infrared left sensor found.\n");
                break;
            case OB_SENSOR_IR_RIGHT:
                Logger::message("[FemtoOrbbecDeviceImpl] Infrared right sensor found.\n");
                break;
            case OB_SENSOR_GYRO:
                Logger::message("[FemtoOrbbecDeviceImpl] Gyro sensor found.\n");
                break;
            case OB_SENSOR_ACCEL:
                Logger::message("[FemtoOrbbecDeviceImpl] Accel sensor found.\n");
                break;
            default:
                break;
            }
        }

        pipe = std::make_unique<ob::Pipeline>(device);

        // auto protocolV = device->getProtocolVersion(); // CRASH
        auto dInfos     = device->getDeviceInfo();
        Logger::message("[FemtoOrbbecDeviceImpl] Device opened:\n");
        Logger::message(std::format("  Name: {}\n", dInfos->name()));
        Logger::message(std::format("  Chip type: {}\n", dInfos->asicName()));
        Logger::message(std::format("  Serialnum: {}\n", dInfos->serialNumber()));
        Logger::message(std::format("  PID: {}\n", dInfos->pid()));
        Logger::message(std::format("  VID: {}\n", dInfos->vid()));
        Logger::message(std::format("  UID: {}\n", dInfos->uid()));
        Logger::message(std::format("  IP: {}\n", dInfos->ipAddress())); // ERROR
        Logger::message("  Version:\n");
        // Logger::message(std::format("      Protocol: {}.{}.{}\n", protocolV.major, protocolV.minor, protocolV.patch));
        Logger::message(std::format("      Firmware: {}\n", dInfos->firmwareVersion()));
        Logger::message(std::format("      Hardware: {}\n", dInfos->hardwareVersion()));
        Logger::message(std::format("      SDK minimum supported: {}\n", dInfos->supportedMinSdkVersion()));
        Logger::message(std::format("      Extension info: {}\n", dInfos->extensionInfo()));
        Logger::message("  Types:\n");
        std::string deviceT;
        switch(dInfos->deviceType()){
        case OB_STRUCTURED_LIGHT_MONOCULAR_CAMERA:
            deviceT = "Monocular structured light camera";
            break;
        case OB_STRUCTURED_LIGHT_BINOCULAR_CAMERA:
            deviceT = "Binocular structured light camera";
            break;
        case OB_TOF_CAMERA:
            deviceT = "Time-of-flight camera";
            break;
        }
        Logger::message(std::format("      Device: {}\n", deviceT));
        Logger::message(std::format("      Connection: {}\n", dInfos->connectionType()));
        ethernet = std::string(dInfos->connectionType()) == "Ethernet";



    }catch(ob::Error &e) {
        Logger::error(std::format("[FemtoOrbbecDeviceImpl] open error: {}\n", e.getMessage()));
        device = nullptr;
        return false;
    }
    return true;
}


auto FemtoOrbbecDeviceImpl::start_reading(const DCConfigSettings &newConfigS) -> bool{

    settings.config = newConfigS;
    initialize();

    try {

        Logger::message("[FemtoOrbbecDeviceImpl] Start reading:\n");

        // configure which streams to enable or disable for the Pipeline by creating a Config
        std::shared_ptr<ob::Config> config = std::make_shared<ob::Config>();

        // color
        std::shared_ptr<ob::StreamProfile> colorProfile = nullptr;
        if(sensorList->getSensor(OBSensorType::OB_SENSOR_COLOR) && (infos.initialColorResolution != DCColorResolution::OFF)){
            if(auto colorStreamProfileList = pipe->getStreamProfileList(OB_SENSOR_COLOR); colorStreamProfileList != nullptr){
                try {
                    colorProfile = colorStreamProfileList->getVideoStreamProfile(
                        static_cast<int>(color_width(infos.initialColorResolution)),
                        static_cast<int>(color_height(infos.initialColorResolution)),
                        static_cast<OBFormat>(get_ob_image_format(infos.imageFormat)),
                        framerate_value(infos.fps)
                    );
                }catch(...) {
                    colorProfile = colorStreamProfileList->getProfile(OB_PROFILE_DEFAULT);
                }
                if(colorProfile){
                    config->enableStream(colorProfile);
                }
            }
        }

        // depth
        std::shared_ptr<ob::StreamProfile> depthProfile = nullptr;
        OBAlignMode  alignMode = ALIGN_DISABLE;
        std::shared_ptr<ob::StreamProfileList> depthStreamProfileList = nullptr;
        if(sensorList->getSensor(OBSensorType::OB_SENSOR_DEPTH) && (infos.depthMode != DCDepthResolution::K4_OFF)){
            if(colorProfile){
                depthStreamProfileList = pipe->getD2CDepthProfileList(colorProfile, ALIGN_D2C_HW_MODE);
                if(depthStreamProfileList->count() > 0) {
                    alignMode = ALIGN_D2C_HW_MODE;
                }else{
                    depthStreamProfileList = pipe->getD2CDepthProfileList(colorProfile, ALIGN_D2C_SW_MODE);
                    if(depthStreamProfileList->count() > 0){
                        alignMode = ALIGN_D2C_SW_MODE;
                    }
                }
            }else{
                depthStreamProfileList = pipe->getStreamProfileList(OB_SENSOR_DEPTH);
            }

            if(depthStreamProfileList->count() > 0) {
                try {
                    depthProfile = depthStreamProfileList->getVideoStreamProfile(
                        static_cast<int>(depth_width(infos.initialDepthResolution)),
                        static_cast<int>(depth_height(infos.initialDepthResolution)),
                        OB_FORMAT_Y16,
                        framerate_value(infos.fps)
                    );
                }catch(...) {
                    depthProfile = depthStreamProfileList->getProfile(OB_PROFILE_DEFAULT);
                }
                config->enableStream(depthProfile);
            }
        }
        config->setAlignMode(alignMode);

        // infrared
        if(sensorList->getSensor(OBSensorType::OB_SENSOR_IR) && has_infrared(settings.config.mode)){
            if(auto infraStreamProfileList = pipe->getStreamProfileList(OB_SENSOR_IR); infraStreamProfileList != nullptr){
                std::shared_ptr<ob::StreamProfile> infraProfile = nullptr;
                try {
                    infraProfile = infraStreamProfileList->getVideoStreamProfile(
                        static_cast<int>(depth_width(infos.initialDepthResolution)),
                        static_cast<int>(depth_height(infos.initialDepthResolution)),
                        OB_FORMAT_Y16,
                        framerate_value(infos.fps)
                    );
                }catch(...) {
                    infraProfile = infraStreamProfileList->getProfile(OB_PROFILE_DEFAULT);
                }
                config->enableStream(infraProfile);
            }
        }

        // accel
        if(sensorList->getSensor(OBSensorType::OB_SENSOR_ACCEL)){
            if(auto accelStreamProfileList = pipe->getStreamProfileList(OB_SENSOR_ACCEL); accelStreamProfileList != nullptr){
                OBAccelFullScaleRange fullScaleRange    = OB_ACCEL_FS_2g;
                OBAccelSampleRate sampleRate            = OB_SAMPLE_RATE_50_HZ;
                auto accelProfile = accelStreamProfileList->getAccelStreamProfile(fullScaleRange, sampleRate);
                config->enableStream(accelProfile);
            }
        }

        // gyro
        if(sensorList->getSensor(OBSensorType::OB_SENSOR_GYRO)){
            if(auto gyroStreamProfileList = pipe->getStreamProfileList(OB_SENSOR_GYRO); gyroStreamProfileList != nullptr){
                OBGyroFullScaleRange fullScaleRange = OB_GYRO_FS_250dps;
                OBGyroSampleRate sampleRate         = OB_SAMPLE_RATE_50_HZ;
                auto gyraProfile = gyroStreamProfileList->getGyroStreamProfile(fullScaleRange, sampleRate);
                config->enableStream(gyraProfile);
            }
        }

        // Update the configuration items of the configuration file, and keep the original configuration for other items
        auto currSynchConfig = device->getMultiDeviceSyncConfig();

        switch (settings.config.synchMode) {
        case DCSynchronisationMode::Standalone:
            // The device does not synchronize with other devices.
            // The Color and Depth should be set to same frame rates, the Color and Depth will be synchronized.
            currSynchConfig.syncMode                = OB_MULTI_DEVICE_SYNC_MODE_STANDALONE;
            currSynchConfig.triggerOutEnable        = false;
            currSynchConfig.triggerOutDelayUs       = 0;
            break;
        case DCSynchronisationMode::Main:
            // The device is the primary device in the multi-device system, it will output the trigger signal via VSYNC_OUT pin on synchronization port by default.
            // The Color and Depth should be set to same frame rates, the Color and Depth will be synchronized and can be adjusted by @ref colorDelayUs, @ref depthDelayUs or @ref trigger2ImageDelayUs.
            currSynchConfig.syncMode                = OB_MULTI_DEVICE_SYNC_MODE_PRIMARY;
            currSynchConfig.triggerOutEnable        = true;
            currSynchConfig.triggerOutDelayUs       = settings.config.subordinateDelayUsec;
            break;
        case DCSynchronisationMode::Subordinate:
            // The device is the secondary device in the multi-device system, it will receive the trigger signal via VSYNC_IN pin on synchronization port. It
            // will out the trigger signal via VSYNC_OUT pin on synchronization port by default.
            // The Color and Depth should be set to same frame rates, the Color and Depth will be synchronized and can be adjusted by @ref colorDelayUs, @ref
            // depthDelayUs or @ref trigger2ImageDelayUs.
            // After starting the stream, the device will wait for the trigger signal to start capturing images, and will stop capturing images when the trigger
            // signal is stopped.
            // The frequency of the trigger signal should be same as the frame rate of the stream profile which is set when starting the stream.
            currSynchConfig.syncMode                = OB_MULTI_DEVICE_SYNC_MODE_SECONDARY;
            currSynchConfig.triggerOutEnable        = true;
            currSynchConfig.triggerOutDelayUs       = settings.config.subordinateDelayUsec;
            break;
        default:
            // The device does not synchronize with other devices,
            // The Color and Depth can be set to different frame rates.
            currSynchConfig.syncMode                = OB_MULTI_DEVICE_SYNC_MODE_FREE_RUN;
            currSynchConfig.triggerOutEnable        = false;
            currSynchConfig.triggerOutDelayUs       = 0;
            break;
        }

        currSynchConfig.colorDelayUs = 0;
        currSynchConfig.depthDelayUs = 0;

        // curConfig.trigger2ImageDelayUs = c.trigger2ImageDelayUs;
        // curConfig.framesPerTrigger     = c.framesPerTrigger;
        // device->setMultiDeviceSyncConfig(currSynchConfig);
        // Configure the alignment mode as hardware D2C alignment
        // ALIGN_DISABLE,     /**< Turn off alignment */
        // ALIGN_D2C_HW_MODE, /**< Hardware D2C alignment mode */
        // ALIGN_D2C_SW_MODE, /**< Software D2C alignment mode */
        // config->setDepthScaleRequire(true);
        // config->setD2CTargetResolution(512,512);
        // config->setD2CTargetResolution(uint32_t d2cTargetWidth, uint32_t d2cTargetHeight);

        // frame synch
        if(settings.config.synchronizeColorAndDepth){
            pipe->enableFrameSync();
        }else{
            pipe->disableFrameSync();
        }

        // other
        set_value(device.get(), OB_PROP_INDICATOR_LIGHT_BOOL, !settings.config.disableLED);

        // start pipe with current config
        pipe->start(config);

        // get camera intrinsic and extrinsic parameters form pipeline and set to point cloud filter
        cameraParam = pipe->getCameraParam();

        // Create a point cloud Filter object (the device parameters will be obtained inside the Pipeline when the point cloud filter is created, so try to
        // configure the device before creating the filter)
        pointCloud = std::make_unique<ob::PointCloudFilter>();
        pointCloud->setCameraParam(cameraParam);


        if((depth_resolution(settings.config.mode) != DCDepthResolution::K4_OFF) && settings.config.enableBodyTracking){

            k4a::calibration calibration;
            calibration.depth_mode       = K4A_DEPTH_MODE_NFOV_UNBINNED;
            calibration.color_resolution = K4A_COLOR_RESOLUTION_OFF;

            auto &cdi = calibration.depth_camera_calibration.intrinsics;

            auto &cc = calibration.color_camera_calibration;
            cc.metric_radius = 1.7f;
            cc.resolution_width = 1280;
            cc.resolution_width = 720;
            cc.intrinsics.type = K4A_CALIBRATION_LENS_DISTORTION_MODEL_BROWN_CONRADY;
            cc.intrinsics.parameter_count = 14;

            auto &cp = cc.intrinsics.parameters.param;
            cp.cx = 642.212f;
            cp.cy = 363.123f;
            cp.fx = 608.033f;
            cp.fy = 607.952f;
            cp.k1 = 0.718214f;
            cp.k2 = -2.7539f;
            cp.k3 = 1.53235f;
            cp.k4 = 0.598884f;
            cp.k5 = -2.59631f;
            cp.k6 = 1.47102f;
            cp.codx = 0.f;
            cp.cody = 0.f;
            cp.p2  = 6.46868e-05f;
            cp.p1  = 0.000889385f;
            cp.metric_radius = 0.f;

            auto &dc = calibration.depth_camera_calibration;
            dc.metric_radius = 1.74f;
            dc.resolution_width = 640;
            dc.resolution_height = 576;
            dc.intrinsics.type = K4A_CALIBRATION_LENS_DISTORTION_MODEL_BROWN_CONRADY;
            dc.intrinsics.parameter_count = 14;

            auto &dp = dc.intrinsics.parameters.param;
            dp.cx = 322.494f;
            dp.cy = 333.486f;
            dp.fx = 502.959f;
            dp.fy = 503.111f;
            dp.k1 = 1.416f;
            dp.k2 = 0.814468f;
            dp.k3 = 0.041968f;
            dp.k4 = 1.75103f;
            dp.k5 = 1.23649f;
            dp.k6 = 0.22332f;
            dp.codx = 0.f;
            dp.cody = 0.f;
            dp.p2  = 6.94212e-05f;
            dp.p1  = -9.19473e-05f;
            dp.metric_radius = 0.f;

            try{
                Logger::message("[FemtoOrbbecDeviceImpl::start_cameras] Start body tracker\n");
                bodyTracker = std::make_unique<k4abt::tracker>(k4abt::tracker::create(calibration, k4aBtConfig));
            }catch (k4a::error error) {
                Logger::error("[FemtoOrbbecDeviceImpl::start_reading] body tracker error: {}\n", error.what());
            }
        }

    }catch(ob::Error &e) {
        Logger::error(std::format("[OrbbecDevice::start_reading] Start reading error: {}\n", e.getMessage()));
        device = nullptr;
        return false;
    }

    start_reading_thread();

    return true;
}

auto FemtoOrbbecDeviceImpl::stop_reading() -> void{

    stop_reading_thread();

    if(bodyTracker != nullptr){
        Logger::message("[FemtoOrbbecDeviceImpl::start_cameras] Stop body tracker\n");
        bodyTracker->shutdown();
        bodyTracker = nullptr;
    }

    // stop pipe
    if(pipe != nullptr){
        try{
            pipe->stop();
        }catch(ob::Error &e) {
            Logger::error(std::format("[FemtoOrbbecDeviceImpl::stop_reading] Error: {}\n", e.getMessage()));
        }
    }
}

auto FemtoOrbbecDeviceImpl::close() -> void{
    pipe       = nullptr;
    sensorList = nullptr;
    device     = nullptr;    
}

auto FemtoOrbbecDeviceImpl::is_opened() const noexcept -> bool{
    return device != nullptr;
}

auto FemtoOrbbecDeviceImpl::nb_devices() const noexcept -> uint32_t{
    return deviceList->deviceCount();
}

auto FemtoOrbbecDeviceImpl::device_name() const noexcept -> std::string{
    if(device != nullptr){
        return device->getDeviceInfo()->name();
    }
    return "unknow_device";
}

auto FemtoOrbbecDeviceImpl::initialize_device_specific() -> void{

    // reset images
    frameSet             = nullptr;
    colorImage           = nullptr;
    depthImage           = nullptr;
    infraredImage        = nullptr;
    pointCloudImage      = nullptr;
    convertedColorImage  = nullptr;

    k4aBtConfig.gpu_device_id       = settings.config.btGPUId;
    k4aBtConfig.processing_mode     = static_cast<k4abt_tracker_processing_mode_t>(settings.config.btProcessingMode);
    k4aBtConfig.sensor_orientation  = static_cast<k4abt_sensor_orientation_t>(settings.config.btOrientation);
    k4aBtConfig.model_path          = nullptr;


    // init converted color image
    if(infos.initialColorResolution != DCColorResolution::OFF){

        auto colRes         = color_resolution(settings.config.mode);
        colorConvBuffer     = cv::Mat(static_cast<int>(color_width(colRes)), static_cast<int>(color_height(colRes)), CV_8UC4);
        convertedColorImage = std::dynamic_pointer_cast<ob::ColorFrame>(ob::FrameHelper::createFrame(
            OB_FRAME_COLOR, OB_FORMAT_BGRA,
            static_cast<std::uint32_t>(color_width(colRes)),
            static_cast<std::uint32_t>(color_height(colRes)),
            0)
        );
    }

    if(ethernet){
        if(infos.imageFormat != DCImageFormat::MJPG){
            Logger::warning("Ethernet connection detected, MJPG image format will be used.\n");
            infos.imageFormat = DCImageFormat::MJPG;
        }
    }
}

auto FemtoOrbbecDeviceImpl::update_camera_from_colors_settings() -> void{

    if(!is_opened()){
        return;
    }


    try{
        const auto &colorS = settings.color;
        set_value(device.get(), OB_PROP_COLOR_AUTO_EXPOSURE_BOOL, colorS.autoExposureTime);
        set_value(device.get(), OB_PROP_COLOR_AUTO_WHITE_BALANCE_BOOL, colorS.autoWhiteBalance);
        set_value(device.get(), OB_PROP_COLOR_EXPOSURE_INT, colorS.exposureTime);
        set_value(device.get(), OB_PROP_COLOR_WHITE_BALANCE_INT, colorS.whiteBalance);
        set_value(device.get(), OB_PROP_COLOR_BRIGHTNESS_INT, colorS.brightness);
        set_value(device.get(), OB_PROP_COLOR_CONTRAST_INT, colorS.contrast);
        set_value(device.get(), OB_PROP_COLOR_SHARPNESS_INT, colorS.sharpness);
        set_value(device.get(), OB_PROP_COLOR_SATURATION_INT, colorS.saturation);
        set_value(device.get(), OB_PROP_COLOR_BACKLIGHT_COMPENSATION_INT, colorS.backlightCompensation ? 1 : 0); // NOT AVAILABLE
        set_value(device.get(), OB_PROP_COLOR_GAIN_INT, colorS.gain);
        set_value(device.get(), OB_PROP_COLOR_POWER_LINE_FREQUENCY_INT, static_cast<int>(colorS.powerlineFrequency));

        // color HDR
        auto currHDR = device->getBoolProperty(OB_PROP_COLOR_HDR_BOOL);
        if(currHDR != colorS.hdr){
            if(!readFramesFromCameras){
                set_value(device.get(), OB_PROP_COLOR_HDR_BOOL, colorS.hdr);
            }else{
                Logger::warning("[FemtoOrbbecDeviceImpl::update_camera_from_colors_settings] Reading must be stopped before changing HDR settings.\n");
            }
        }
    }catch(ob::Error &e) {
        Logger::error(std::format("[FemtoOrbbecDeviceImpl::update_camera_from_colors_settings] Error: {}\n", e.getMessage()));
    }
}

auto FemtoOrbbecDeviceImpl::color_data() -> std::span<ColorRGBA8>{
    if(colorImage != nullptr){
        return std::span<tool::ColorRGBA8>{
            reinterpret_cast<tool::ColorRGBA8*>(colorImage->data()),
            colorImage->width()*colorImage->height()
        };
    }
    return {};
}

auto FemtoOrbbecDeviceImpl::depth_data() -> std::span<uint16_t>{
    if(depthImage != nullptr){
        return std::span<std::uint16_t>{
            reinterpret_cast<std::uint16_t*>(depthImage->data()),            
            depthImage->width()*depthImage->height()
        };
    }
    return{};
}

auto FemtoOrbbecDeviceImpl::infra_data() -> std::span<uint16_t>{
    if(infraredImage != nullptr){
        return std::span<std::uint16_t>{
            reinterpret_cast<std::uint16_t*>(infraredImage->data()),
            infraredImage->width()*infraredImage->height()
        };
    }
    return{};
}

auto FemtoOrbbecDeviceImpl::bodies_index_data() -> std::span<uint8_t> {
    if(bodiesIndexImage.has_value()){
        return std::span<std::uint8_t>{
            reinterpret_cast<std::uint8_t*>(bodiesIndexImage.value().get_buffer()),
            static_cast<size_t>(bodiesIndexImage.value().get_width_pixels() * bodiesIndexImage.value().get_height_pixels())
        };
    }
    return {};
}



auto FemtoOrbbecDeviceImpl::capture_frame(int32_t timeoutMs) -> bool{

    if(pipe != nullptr){
        Bench::start("[FemtoOrbbecDeviceImpl::capture_frame]");
        try{
            frameSet = pipe->waitForFrames(timeoutMs);
        }catch(ob::Error &e) {
            Logger::error(std::format("[FemtoOrbbecDeviceImpl::capture_frame] Get capture error: {}\n", e.getMessage()));
        }

        Bench::stop();
        return frameSet != nullptr;
    }
    return false;
}

auto FemtoOrbbecDeviceImpl::read_color_image() -> bool{

    if((infos.initialColorResolution != DCColorResolution::OFF) && (frameSet != nullptr)){
        Bench::start("[FemtoOrbbecDeviceImpl::read_color_image]");
        colorImage = frameSet->colorFrame();

        // TODO:
        // colorImage->timeStamp();
        Bench::stop();
        return true;
    }
    return false;
}

auto FemtoOrbbecDeviceImpl::read_depth_image() -> bool{

    if((infos.depthMode != DCDepthResolution::K4_OFF) && (frameSet != nullptr)){
        Bench::start("[FemtoOrbbecDeviceImpl::read_depth_image]");
        depthImage = frameSet->depthFrame();
        Bench::stop();
        return true;
    }
    return false;
}

auto FemtoOrbbecDeviceImpl::read_infra_image() -> bool{

    if(has_infrared(settings.config.mode) && (frameSet != nullptr)){
        Bench::start("[FemtoOrbbecDeviceImpl::read_infra_image]");
        infraredImage = frameSet->irFrame();
        Bench::stop();
        return true;
    }

    return false;
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


auto FemtoOrbbecDeviceImpl::read_bodies() -> void{

    if(!depthImage || ! infraredImage){
        return;
    }

    if((depth_resolution(settings.config.mode) != DCDepthResolution::K4_OFF) && settings.config.enableBodyTracking && (bodyTracker != nullptr)){


        try{

            k4a_capture_t captureH = nullptr;
            k4a_capture_create(&captureH);
            k4a::capture capture(captureH);

            // std::cout << "create d image\n";
            // std::cout << depthImage->width() << " " << depthImage->height() << " "<< depthImage->width()*depthImage->height() << " " << depthImage->dataSize() << "\n";
            k4a::image k4DepthImage = k4a::image::create_from_buffer(
                K4A_IMAGE_FORMAT_DEPTH16,
                depthImage->width(),
                depthImage->height(),
                depthImage->width()*sizeof(std::uint16_t),
                reinterpret_cast<std::uint8_t*>(depthImage.get()->data()),
                depthImage->dataSize(),
                nullptr,
                nullptr
            );

            // std::cout << "create ir image\n";
            // std::cout << infraredImage->width() << " " << infraredImage->height() << " "<< infraredImage->width()*infraredImage->height() << " " << infraredImage->dataSize() << "\n";
            k4a::image k4IRImage = k4a::image::create_from_buffer(
                K4A_IMAGE_FORMAT_DEPTH16,
                infraredImage->width(),
                infraredImage->height(),
                infraredImage->width()*sizeof(std::uint16_t),
                reinterpret_cast<std::uint8_t*>(infraredImage.get()->data()),
                infraredImage->dataSize(),
                nullptr,
                nullptr
            );

            // std::cout << "set depth image\n";
            capture.set_depth_image(k4DepthImage);

            // std::cout << "set ir image\n";
            capture.set_ir_image(k4IRImage);


            // std::cout << "enqueue capture\n";
            if(bodyTracker->enqueue_capture(capture, std::chrono::milliseconds(1))){
                if(k4abt::frame bodyFrame = bodyTracker->pop_result(std::chrono::milliseconds(1)); bodyFrame != nullptr){
                    auto bodiesCount = bodyFrame.get_num_bodies();
                    if(data.bodies.size() < bodiesCount){
                        data.bodies.resize(bodiesCount);
                    }
                    // std::cout << "bodiesCount " << bodiesCount << "\n";
                    for(size_t ii = 0; ii < bodiesCount; ++ii){
                        update_body(data.bodies[ii], bodyFrame.get_body(static_cast<int>(ii)));
                    }
                    timing.bodiesTS = bodyFrame.get_system_timestamp();
                    bodiesIndexImage = bodyFrame.get_body_index_map();
                }
            }

        }  catch (k4a::error error) {
            Logger::error("[FemtoOrbbecDeviceImpl::read_bodies] error: {}\n", error.what());
        }  catch (std::runtime_error error) {
            Logger::error("[FemtoOrbbecDeviceImpl::read_bodies] error: {}\n", error.what());
        }

    }
}

auto FemtoOrbbecDeviceImpl::convert_color_image() -> void{

     // colorFrame = formatConvertFilter.process(colorFrame)->as<ob::ColorFrame>();

    if(infos.initialColorResolution == DCColorResolution::OFF){
        return;
    }
    if(colorImage == nullptr || convertedColorImage == nullptr){
        return;
    }

    // auto t1 = Time::nanoseconds_since_epoch();

    if(infos.imageFormat == DCImageFormat::NV12){

        cv::Mat rawMat(colorImage->height() * 3 / 2, colorImage->width(), CV_8UC1, colorImage->data());
        cv::cvtColor(rawMat, colorConvBuffer, cv::COLOR_YUV2RGBA_NV12, 4);
        std::copy(
            colorConvBuffer.data,
            colorConvBuffer.data + 4*colorImage->width()*colorImage->height(),
            reinterpret_cast<std::int8_t*>(convertedColorImage->data())
        );

        colorImage = convertedColorImage;

    }else if(infos.imageFormat == DCImageFormat::MJPG){

        frames.frameUncompressor.uncompress_jpeg_data(
            colorImage->width(),
            colorImage->height(),
            DCFrameUncompressor::ColorFormat::RGBA,
            colorImage->dataSize(),
            reinterpret_cast<uint8_t*>(colorImage->data()),
            reinterpret_cast<uint8_t*>(convertedColorImage->data())
        );

        colorImage = convertedColorImage;

    }else if(infos.imageFormat == DCImageFormat::BGRA){

        cv::Mat rawMat(colorImage->height(), colorImage->width(), CV_8UC4, colorImage->data());
        cv::cvtColor(rawMat, colorConvBuffer, cv::COLOR_BGRA2RGBA);
        std::copy(
            colorConvBuffer.data,
            colorConvBuffer.data + 4*colorImage->width()*colorImage->height(),
            reinterpret_cast<std::int8_t*>(convertedColorImage->data())
        );

        colorImage = convertedColorImage;

        // } else if(infos.imageFormat == DCImageFormat::YUY2 ){

        //     // not supported by femto bolt

        //     cv::Mat rawMat(colorImage->height(), colorImage->width(), CV_8UC2, colorImage->data());
        //     cv::cvtColor(rawMat, colorConvBuffer, cv::COLOR_YUV2RGBA_YUY2);
        //     std::copy(
        //         colorConvBuffer.data,
        //         colorConvBuffer.data + 4*colorImage->width()*colorImage->height(),
        //         reinterpret_cast<std::int8_t*>(convertedColorImage->data())
        //     );

        //     colorImage = convertedColorImage;

    }else{
        Logger::error("FemtoOrbbecDeviceImpl::convert_color_image: Invalid color format.\n");
        colorImage = nullptr;
    }

    // auto t2 = Time::nanoseconds_since_epoch();
    // std::cout << "convert color : " << std::chrono::duration_cast<std::chrono::microseconds>(t2-t1).count() << "\n";

}

auto FemtoOrbbecDeviceImpl::resize_images() -> void{
    // no need to resize
}


auto FemtoOrbbecDeviceImpl::generate_cloud() -> void{

    if(has_cloud(settings.config.mode) && (depthImage != nullptr)){
        if(validDepthValues > 0){
            auto depthValueScale = depthImage->getValueScale()*0.001f;
            if(colorImage != nullptr){
                pointCloud->setCreatePointFormat(OB_FORMAT_RGB_POINT);
            }else{
                pointCloud->setCreatePointFormat(OB_FORMAT_POINT);
            }
            pointCloud->setPositionDataScaled(depthValueScale);
            pointCloudImage = pointCloud->process(frameSet);
        }
    }
}

auto FemtoOrbbecDeviceImpl::filter_cloud_image(const DCFiltersSettings &filtersS) -> void{

    if(pointCloudImage == nullptr){
        return;
    }

    if(filtersS.p1FMode != DCFiltersSettings::PlaneFilteringMode::None){

        auto p1     = filtersS.p1A;
        auto p2     = filtersS.p1B;
        auto p3     = filtersS.p1C;
        auto meanPt = (p1+p2+p3)/3.f;
        auto AB = vec(p2,p1);
        auto AC = vec(p3,p1);
        auto normalV = cross(AB,AC);
        normalV = normalize(normalV);

        auto depthBuffer  = reinterpret_cast<const uint16_t*>(depthImage->data());
        bool rgbCloud = colorImage != nullptr;
        auto cloudData = reinterpret_cast<float*>(pointCloudImage->data());
        int dataSize = rgbCloud ? 6 : 3;

        std::for_each(std::execution::par_unseq, std::begin(indices.depthVertexCorrrespondance), std::end(indices.depthVertexCorrrespondance), [&](auto idC){

            auto idD = std::get<0>(idC);
            if(depthBuffer[idD] == dc_invalid_depth_value){
                return;
            }

            float *cloudPoint = &cloudData[idD*dataSize];
            if(dot(normalV,vec(meanPt,geo::Pt3f{-cloudPoint[0], -cloudPoint[1], cloudPoint[2]})) < 0){
                if(filtersS.p1FMode == DCFiltersSettings::PlaneFilteringMode::Above){
                    depthMask[idD] = 0;
                    return;
                }
            }else{
                if(filtersS.p1FMode == DCFiltersSettings::PlaneFilteringMode::Below){
                    depthMask[idD] = 0;
                    return;
                }
            }
        });
    }

    update_valid_depth_values();
}

auto FemtoOrbbecDeviceImpl::compress_frame(const DCFiltersSettings &filtersS, const DCDataSettings &dataS) -> std::unique_ptr<DCCompressedFrame>{

    tool::Bench::start("[FemtoOrbbecDeviceImpl::compress_frame] Generate compressed frame");

    auto mode = settings.config.mode;
    auto cFrame                = std::make_unique<DCCompressedFrame>();
    cFrame->mode               = mode;
    cFrame->idCapture          = static_cast<std::int32_t>(infos.idCapture);
    cFrame->afterCaptureTS     = timing.get_local("after_capture"sv).count();
    cFrame->receivedTS         = cFrame->afterCaptureTS;  // default init received TS with after capture TS
    cFrame->validVerticesCount = validDepthValues;

    size_t offset = 0;
    cFrame->init_calibration_from_data(DCType::FemtoOrbbec, reinterpret_cast<std::int8_t*>(&cameraParam), offset, sizeof(cameraParam));

    // compressed color
    if((colorImage != nullptr) && dataS.sendColor){
        frames.frameCompressor.add_color(
            colorImage->width(),
            colorImage->height(),
            4,
            reinterpret_cast<uint8_t*>(colorImage->data()),
            filtersS.jpegCompressionRate,
            cFrame.get()
        );
    }
    // compressed depth
    if((depthImage != nullptr) && dataS.sendDepth){
        frames.frameCompressor.add_depth(
            depthImage->width(),
            depthImage->height(),
            reinterpret_cast<std::uint16_t*>(depthImage->data()),
            cFrame.get()
        );
    }
    // compressed infrared
    if((infraredImage != nullptr) && dataS.sendInfra){
        frames.frameCompressor.add_infra(
            infraredImage->width(),
            infraredImage->height(),
            reinterpret_cast<std::uint16_t*>(infraredImage->data()),
            cFrame.get()
        );
    }
    // compressed cloud
    if((colorImage != nullptr) && (depthImage != nullptr) && (pointCloudImage != nullptr) && dataS.sendCloud){
        frames.frameCompressor.add_cloud(
            colorImage->width(), colorImage->height(), reinterpret_cast<uint8_t*>(colorImage->data()),
            depthImage->width()*depthImage->height(), reinterpret_cast<std::uint16_t*>(depthImage->data()),
            filtersS.jpegCompressionRate, cFrame.get()
        );
    }

    // no audio

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

auto FemtoOrbbecDeviceImpl::create_local_frame(const DCDataSettings &dataS) -> std::unique_ptr<DCFrame>{

    // write frame
    tool::Bench::start("[FemtoOrbbecDeviceImpl::create_local_frame] Write display data frame");

    auto dFrame             = std::make_unique<DCFrame>();
    dFrame->idCapture       = static_cast<std::int32_t>(infos.idCapture);
    dFrame->afterCaptureTS  = timing.get_local("after_capture"sv).count();
    dFrame->receivedTS      = dFrame->afterCaptureTS;  // default init received TS with after capture TS
    dFrame->mode            = settings.config.mode;

    const auto dRange = depth_range(settings.config.mode)*1000.f;
    const auto diff   = dRange.y() - dRange.x();

    static constexpr std::array<Pt3f,5> depthGradient ={
        Pt3f{0.f,0.f,1.f},
        {0.f,1.f,1.f},
        {0.f,1.f,0.f},
        {1.f,1.f,0.f},
        {1.f,0.f,0.f},
    };

    // color frame
    if(dataS.generateRGBLocalFrame && colorImage != nullptr){

        tool::Bench::start("[FemtoOrbbecDeviceImpl::create_local_frame] color");

        dFrame->colorWidth  = colorImage->width();
        dFrame->colorHeight = colorImage->height();
        dFrame->imageColorData.resize(dFrame->colorWidth*dFrame->colorHeight);
        std::copy(
            reinterpret_cast<std::int8_t*>(colorImage->data()),
            reinterpret_cast<std::int8_t*>(colorImage->data()) + dFrame->imageColorData.size() * 4,
            reinterpret_cast<std::int8_t*>(dFrame->imageColorData.data())
        );

        tool::Bench::stop();
    }

    // depth frame
    if(dataS.generateDepthLocalFrame && depthImage != nullptr){

        tool::Bench::start("[FemtoOrbbecDeviceImpl::create_local_frame] depth");

        dFrame->depthWidth  = depthImage->width();
        dFrame->depthHeight = depthImage->height();
        dFrame->imageDepthData.resize(dFrame->depthWidth * dFrame->depthHeight);

        auto depthBuffer  = reinterpret_cast<const uint16_t*>(depthImage->data());


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
    if(dataS.generateInfraLocalFrame && infraredImage != nullptr){

        tool::Bench::start("[OrbbecDevice::create_local_frame] display_infrared");

        dFrame->infraWidth  = infraredImage->width();
        dFrame->infraHeight = infraredImage->height();
        dFrame->imageInfraData.resize(dFrame->infraWidth * dFrame->infraHeight);

        auto infraBuffer = reinterpret_cast<const uint16_t*>(infraredImage->data());
        const float max = 2000;
        std::for_each(std::execution::par_unseq, std::begin(indices.infras1D), std::end(indices.infras1D), [&](size_t id){
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
    if(dataS.generateCloudLocal && (pointCloudImage != nullptr) && (depthImage != nullptr)){

        tool::Bench::start("[OrbbecDevice::create_local_frame] cloud");

        dFrame->cloud.vertices.resize(validDepthValues);
        dFrame->cloud.colors.resize(validDepthValues);
        // dFrame->cloud.normals.resize(validDepthValues);

        auto depthBuffer  = reinterpret_cast<const uint16_t*>(depthImage->data());
        bool rgbCloud = colorImage != nullptr;
        auto cloudData = reinterpret_cast<float*>(pointCloudImage->data());
        int dataSize = rgbCloud ? 6 : 3;

        std::for_each(std::execution::par_unseq, std::begin(indices.depthVertexCorrrespondance), std::end(indices.depthVertexCorrrespondance), [&](auto idC){

            auto idD = std::get<0>(idC);
            if(depthBuffer[idD] == dc_invalid_depth_value){
                return;
            }

            auto idV = std::get<1>(idC);
            float *cloudPoint = &cloudData[idD*dataSize];

            // set vertex
            dFrame->cloud.vertices[idV]= geo::Pt3f{
                -cloudPoint[0], -cloudPoint[1], cloudPoint[2]
            };

            // set color
            if(rgbCloud){

                // if(zonesId[idD] != 0){
                //     if(zonesId[idD] == 1){
                //         dFrame->cloud.colors[idV] = geo::Pt3f{1,0,0};
                //     }else if(zonesId[idD] == 2){
                //         dFrame->cloud.colors[idV] = geo::Pt3f{0,1,0};
                //     }else if(zonesId[idD] == 3){
                //         dFrame->cloud.colors[idV] = geo::Pt3f{0,0,1};
                //     }else{
                //         dFrame->cloud.colors[idV] = geo::Pt3f{1,0,1};
                //     }

                // }else{
                //     dFrame->cloud.colors[idV] = geo::Pt3f{
                //         0,
                //         0,
                //         0
                //     };
                // }

                dFrame->cloud.colors[idV] = geo::Pt3f{
                    cloudPoint[3],
                    cloudPoint[4],
                    cloudPoint[5]
                }/255.f;

            }else{
                float vF = (static_cast<float>(depthBuffer[idD]) - dRange.x())/diff;
                float intPart;
                float decPart = std::modf((vF*(depthGradient.size()-1)), &intPart);
                size_t idG = static_cast<size_t>(intPart);
                dFrame->cloud.colors[idV] = depthGradient[idG]*(1.f-decPart) + depthGradient[idG+1]*decPart;
            }
        });

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


    return dFrame;
}
