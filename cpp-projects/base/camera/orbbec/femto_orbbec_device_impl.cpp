


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
using namespace tool::camera;

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
    context->setLoggerSeverity(OB_LOG_SEVERITY_WARN);
    deviceList   = context->queryDeviceList();
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
        auto infos     = device->getDeviceInfo();
        Logger::message("[FemtoOrbbecDeviceImpl] Device opened:\n");
        Logger::message(std::format("  Name: {}\n", infos->name()));
        Logger::message(std::format("  Chip type: {}\n", infos->asicName()));
        Logger::message(std::format("  Serialnum: {}\n", infos->serialNumber()));
        Logger::message(std::format("  PID: {}\n", infos->pid()));
        Logger::message(std::format("  VID: {}\n", infos->vid()));
        Logger::message(std::format("  UID: {}\n", infos->uid()));
        Logger::message(std::format("  IP: {}\n", infos->ipAddress())); // ERROR
        Logger::message("  Version:\n");
        // Logger::message(std::format("      Protocol: {}.{}.{}\n", protocolV.major, protocolV.minor, protocolV.patch));
        Logger::message(std::format("      Firmware: {}\n", infos->firmwareVersion()));
        Logger::message(std::format("      Hardware: {}\n", infos->hardwareVersion()));
        Logger::message(std::format("      SDK minimum supported: {}\n", infos->supportedMinSdkVersion()));
        Logger::message(std::format("      Extension info: {}\n", infos->extensionInfo()));
        Logger::message("  Types:\n");
        std::string deviceT;
        switch(infos->deviceType()){
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
        Logger::message(std::format("      Connection: {}\n", infos->connectionType()));


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
            std::cout << "SUBORDINATE\n";
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

    // stop pipe
    if(pipe != nullptr){
        pipe->stop();
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
}

auto FemtoOrbbecDeviceImpl::update_camera_from_colors_settings() -> void{

    if(!is_opened()){
        return;
    }

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

    if(has_cloud(settings.config.mode) && (depthImage != nullptr) && (colorImage != nullptr)){
        auto depthValueScale = depthImage->getValueScale()*0.001f;
        pointCloud->setCreatePointFormat(OB_FORMAT_RGB_POINT);
        pointCloud->setPositionDataScaled(depthValueScale);                       
        pointCloudImage = pointCloud->process(frameSet);
    }
}

auto FemtoOrbbecDeviceImpl::compress_frame(const DCFiltersSettings &filtersS, const DCDataSettings &dataS) -> std::unique_ptr<DCCompressedFrame>{

    tool::Bench::start("[FemtoOrbbecDeviceImpl::compress_frame] Generate compressed frame");

    auto mode = settings.config.mode;
    auto cFrame                = std::make_unique<DCCompressedFrame>();
    cFrame->mode               = mode;
    cFrame->idCapture          = static_cast<std::int32_t>(infos.idCapture);
    cFrame->afterCaptureTS     = timing.get_local("after_capture"sv).count();
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

auto FemtoOrbbecDeviceImpl::create_local_frame(const DCDataSettings &d) -> std::unique_ptr<DCFrame>{

    // write frame
    tool::Bench::start("[FemtoOrbbecDeviceImpl::create_local_frame] Write display data frame");

    auto dFrame             = std::make_unique<DCFrame>();
    dFrame->idCapture       = static_cast<std::int32_t>(infos.idCapture);
    dFrame->afterCaptureTS  = timing.get_local("after_capture"sv).count();
    dFrame->mode            = settings.config.mode;

    static constexpr std::array<Pt3f,5> depthGradient ={
        Pt3f{0.f,0.f,1.f},
        {0.f,1.f,1.f},
        {0.f,1.f,0.f},
        {1.f,1.f,0.f},
        {1.f,0.f,0.f},
    };

    // color frame
    if(d.generateRGBLocalFrame && colorImage != nullptr){

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
    if(d.generateDepthLocalFrame && depthImage != nullptr){

        tool::Bench::start("[FemtoOrbbecDeviceImpl::create_local_frame] depth");

        dFrame->depthWidth  = depthImage->width();
        dFrame->depthHeight = depthImage->height();
        dFrame->imageDepthData.resize(dFrame->depthWidth * dFrame->depthHeight);

        auto depthBuffer  = reinterpret_cast<const uint16_t*>(depthImage->data());
        const auto dRange = depth_range(settings.config.mode)*1000.f;
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
    if(d.generateInfraLocalFrame && infraredImage != nullptr){

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
    if(d.generateCloudLocal && (pointCloudImage != nullptr) && (depthImage != nullptr) && (colorImage != nullptr)){

        tool::Bench::start("[OrbbecDevice::create_local_frame] cloud");

        dFrame->cloud.vertices.resize(validDepthValues);
        dFrame->cloud.colors.resize(validDepthValues);
        // dFrame->cloud.normals.resize(validDepthValues);

        auto depthBuffer  = reinterpret_cast<const uint16_t*>(depthImage->data());
        OBColorPoint *cloudBuffer = reinterpret_cast<OBColorPoint*>(pointCloudImage->data());
        std::for_each(std::execution::par_unseq, std::begin(indices.depthVertexCorrrespondance), std::end(indices.depthVertexCorrrespondance), [&](auto idC){

            auto idD = std::get<0>(idC);
            if(depthBuffer[idD] == dc_invalid_depth_value){
                return;
            }

            auto idV = std::get<1>(idC);
            dFrame->cloud.vertices[idV]= geo::Pt3f{
                static_cast<float>(-cloudBuffer[idD].x),
                static_cast<float>(-cloudBuffer[idD].y),
                static_cast<float>( cloudBuffer[idD].z)
            };

            dFrame->cloud.colors[idV] = geo::Pt3f{
                cloudBuffer[idD].r,
                cloudBuffer[idD].g,
                cloudBuffer[idD].b
            }/255.f;
        });

        tool::Bench::stop();
    }

    // imu sample
    if(d.captureIMU){
        dFrame->imuSample = data.imuSample;
    }else{
        dFrame->imuSample = std::nullopt;
    }

    // send bodies
    // ...

    tool::Bench::stop();


    return dFrame;
}
