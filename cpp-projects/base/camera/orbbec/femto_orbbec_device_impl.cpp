


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
        if(sensorList->getSensor(OBSensorType::OB_SENSOR_COLOR)){
            colorStreamProfileList = pipe->getStreamProfileList(OB_SENSOR_COLOR);
        }
        if(sensorList->getSensor(OBSensorType::OB_SENSOR_DEPTH)){
            depthStreamProfileList = pipe->getStreamProfileList(OB_SENSOR_DEPTH);
        }
        if(sensorList->getSensor(OBSensorType::OB_SENSOR_IR)){
            infraStreamProfileList = pipe->getStreamProfileList(OB_SENSOR_IR);
        }
        if(sensorList->getSensor(OBSensorType::OB_SENSOR_ACCEL)){
            accelStreamProfileList = pipe->getStreamProfileList(OB_SENSOR_ACCEL);
        }
        if(sensorList->getSensor(OBSensorType::OB_SENSOR_GYRO)){
            gyroStreamProfileList = pipe->getStreamProfileList(OB_SENSOR_GYRO);
        }

        // callbacks
        // device->setDeviceStateChangedCallback([&](OBDeviceState state, const char *message){
        //    // ...
        // });


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
        std::shared_ptr<ob::VideoStreamProfile> colorProfile = nullptr;
        std::shared_ptr<ob::VideoStreamProfile> depthProfile = nullptr;
        std::shared_ptr<ob::VideoStreamProfile> infraProfile = nullptr;
        std::shared_ptr<ob::AccelStreamProfile> accelProfile = nullptr;
        std::shared_ptr<ob::GyroStreamProfile> gyraProfile   = nullptr;

        auto mode    = settings.config.mode;
        auto fps     = framerate(mode);

        if(colorStreamProfileList && (infos.colorResolution != DCColorResolution::OFF)){
            colorProfile = colorStreamProfileList->getVideoStreamProfile(static_cast<int>(infos.colorWidth), static_cast<int>(infos.colorHeight), static_cast<OBFormat>(get_ob_image_format(image_format(mode))), framerate_value(fps));
        }
        if(depthStreamProfileList && (infos.depthMode != DCDepthMode::K4_OFF)){
            depthProfile = depthStreamProfileList->getVideoStreamProfile(static_cast<int>(infos.depthWidth), static_cast<int>(infos.depthHeight), OB_FORMAT_Y16, framerate_value(fps));
        }
        if(infraStreamProfileList && has_infrared(mode)){
            infraProfile = infraStreamProfileList->getVideoStreamProfile(static_cast<int>(infos.depthWidth), static_cast<int>(infos.depthHeight), OB_FORMAT_Y16, framerate_value(fps));
        }
        if(accelStreamProfileList){
            OBAccelFullScaleRange fullScaleRange    = OB_ACCEL_FS_2g;
            OBAccelSampleRate sampleRate            = OB_SAMPLE_RATE_50_HZ;
            accelProfile = accelStreamProfileList->getAccelStreamProfile(fullScaleRange, sampleRate);
        }
        if(gyroStreamProfileList){
            OBGyroFullScaleRange fullScaleRange = OB_GYRO_FS_250dps;
            OBGyroSampleRate sampleRate         = OB_SAMPLE_RATE_50_HZ;
            gyraProfile = gyroStreamProfileList->getGyroStreamProfile(fullScaleRange, sampleRate);
        }

        // configure which streams to enable or disable for the Pipeline by creating a Config
        std::shared_ptr<ob::Config> config = std::make_shared<ob::Config>();

        // enable streams depening parameters
        if(colorProfile){
            config->enableStream(colorProfile);
        }
        if(depthProfile){
            config->enableStream(depthProfile);
        }
        if(infraProfile){
            config->enableStream(infraProfile);
        }
        if(accelProfile){
            config->enableStream(accelProfile);
        }
        if(gyraProfile){
            config->enableStream(gyraProfile);
        }

        // Update the configuration items of the configuration file, and keep the original configuration for other items
        auto currSynchConfig = device->getMultiDeviceSyncConfig();

        switch (settings.config.synchMode) {
        case DCSynchronisationMode::K4_Standalone:
            // The device does not synchronize with other devices.
            // The Color and Depth should be set to same frame rates, the Color and Depth will be synchronized.
            currSynchConfig.syncMode                = OB_MULTI_DEVICE_SYNC_MODE_STANDALONE;
            currSynchConfig.triggerOutEnable        = false;
            currSynchConfig.triggerOutDelayUs       = 0;
            break;
        case DCSynchronisationMode::K4_Master:
            // The device is the primary device in the multi-device system, it will output the trigger signal via VSYNC_OUT pin on synchronization port by default.
            // The Color and Depth should be set to same frame rates, the Color and Depth will be synchronized and can be adjusted by @ref colorDelayUs, @ref depthDelayUs or @ref trigger2ImageDelayUs.
            currSynchConfig.syncMode                = OB_MULTI_DEVICE_SYNC_MODE_PRIMARY;
            currSynchConfig.triggerOutEnable        = true;
            currSynchConfig.triggerOutDelayUs       = settings.config.subordinateDelayUsec;
            break;
        case DCSynchronisationMode::K4_Subordinate:
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
        // config->setAlignMode(ALIGN_DISABLE);
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

        // start pipe with current config
        pipe->start(config);

        // Create a point cloud Filter object (the device parameters will be obtained inside the Pipeline when the point cloud filter is created, so try to
        // configure the device before creating the filter)
        pointCloud = std::make_unique<ob::PointCloudFilter>();
        pointCloud->setCreatePointFormat(OB_FORMAT_RGB_POINT);

        // get camera intrinsic and extrinsic parameters form pipeline and set to point cloud filter
        cameraParam = pipe->getCameraParam();
        pointCloud->setCameraParam(cameraParam);

        // other
        set_value(device.get(), OB_PROP_INDICATOR_LIGHT_BOOL, !settings.config.disableLED);

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
    depthSizedColorImage = nullptr;

    // init converted color image
    if(infos.colorResolution != DCColorResolution::OFF){
        colorConvBuffer     = cv::Mat(static_cast<int>(infos.colorWidth), static_cast<int>(infos.colorHeight), CV_8UC4);
        convertedColorImage = std::dynamic_pointer_cast<ob::ColorFrame>(ob::FrameHelper::createFrame(
            OB_FRAME_COLOR, OB_FORMAT_BGRA,
            static_cast<std::uint32_t>(infos.colorWidth), static_cast<std::uint32_t>(infos.colorHeight), 0)
        );
    }

    // init resized color image
    if(infos.depthMode != DCDepthMode::K4_OFF){
        if(infos.colorResolution != DCColorResolution::OFF){
            depthSizedColorImage = std::dynamic_pointer_cast<ob::ColorFrame>(ob::FrameHelper::createFrame(
                OB_FRAME_COLOR, OB_FORMAT_BGRA,
                static_cast<std::uint32_t>(infos.depthWidth), static_cast<std::uint32_t>(infos.depthHeight), 0)
            );
        }
    }
}

auto FemtoOrbbecDeviceImpl::update_camera_from_colors_settings() -> void{

    if(!is_opened()){
        return;
    }

    const auto &colorS = settings.color;

    // color auto exposure
    set_value(device.get(), OB_PROP_COLOR_AUTO_EXPOSURE_BOOL, colorS.autoExposureTime);
    // color auto white balance
    set_value(device.get(), OB_PROP_COLOR_AUTO_WHITE_BALANCE_BOOL, colorS.autoWhiteBalance);
    // color exposure
    {
        std::int32_t value = 0;
        switch (static_cast<K4ExposureTimesMicroS>(colorS.exposureTimeAbsolute)) {
        case K4ExposureTimesMicroS::t500:   value = 500; break;
        case K4ExposureTimesMicroS::t1250:  value = 1250; break;
        case K4ExposureTimesMicroS::t2500:  value = 2500; break;
        case K4ExposureTimesMicroS::t8330:  value = 8330; break;
        case K4ExposureTimesMicroS::t16670: value = 16670; break;
        case K4ExposureTimesMicroS::t33330: value = 33330; break;
        }
        set_value(device.get(), OB_PROP_COLOR_EXPOSURE_INT, value);
    }
    // color white balance
    set_value(device.get(), OB_PROP_COLOR_WHITE_BALANCE_INT, colorS.whiteBalance);
    // color brightness
    set_value(device.get(), OB_PROP_COLOR_BRIGHTNESS_INT, colorS.brightness);
    // color contrast
    set_value(device.get(), OB_PROP_COLOR_CONTRAST_INT, colorS.contrast);
    // color sharpness
    set_value(device.get(), OB_PROP_COLOR_SHARPNESS_INT, colorS.sharpness);
    // color saturation
    set_value(device.get(), OB_PROP_COLOR_SATURATION_INT, colorS.saturation);
    // color backlight compensation
    set_value(device.get(), OB_PROP_COLOR_BACKLIGHT_COMPENSATION_INT, colorS.backlightCompensation ? 1 : 0); // TODO: INSPECT
    // color gain
    set_value(device.get(), OB_PROP_COLOR_GAIN_INT, colorS.gain);
    // color powerline frequency
    {
        std::int32_t value = 0;
        switch (colorS.powerlineFrequency){
        case PowerlineFrequency::F50: value = 50; break;
        case PowerlineFrequency::F60: value = 60; break;
        }
        set_value(device.get(), OB_PROP_COLOR_POWER_LINE_FREQUENCY_INT, value); // TODO: INSPECT
    }
    // color HDR
    set_value(device.get(), OB_PROP_COLOR_HDR_BOOL, true); // TODO: INSPECT
}

auto FemtoOrbbecDeviceImpl::depth_sized_color_data() -> std::span<ColorRGBA8>{
    if(depthSizedColorImage != nullptr){
        return std::span<tool::ColorRGBA8>{
            reinterpret_cast<tool::ColorRGBA8*>(depthSizedColorImage->data()),
            static_cast<size_t>(depthSizedColorImage->width() * depthSizedColorImage->height())
        };
    }
    return {};
}

auto FemtoOrbbecDeviceImpl::depth_data() -> std::span<uint16_t>{
    if(depthImage != nullptr){
        return std::span<std::uint16_t>{
            reinterpret_cast<std::uint16_t*>(depthImage->data()),
            depthImage->dataSize()
        };
    }
    return{};
}

auto FemtoOrbbecDeviceImpl::infra_data() -> std::span<uint16_t>{
    if(infraredImage != nullptr){
        return std::span<std::uint16_t>{
            reinterpret_cast<std::uint16_t*>(infraredImage->data()),
            infraredImage->dataSize()
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

    if((infos.colorResolution != DCColorResolution::OFF) && (frameSet != nullptr)){
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

    if((infos.depthMode != DCDepthMode::K4_OFF) && (frameSet != nullptr)){
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

    if(infos.colorResolution == DCColorResolution::OFF){
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

auto FemtoOrbbecDeviceImpl::resize_color_to_fit_depth() -> void{

    if(colorImage != nullptr && depthImage != nullptr){

        Bench::start("[FemtoOrbbecDeviceImpl] resize_color_to_fit_depth");
        int res = stbir_resize_uint8(
            reinterpret_cast<const unsigned char *>(colorImage->data()),     // input_pixels
            colorImage->width(),                                             // input_w
            colorImage->height(),                                            // input_h
            colorImage->width()* sizeof(std::int8_t)*4,                      // input_stride_in_bytes
            reinterpret_cast<unsigned char *>(depthSizedColorImage->data()), // output_pixels
            depthSizedColorImage->width(),                                   // output_w
            depthSizedColorImage->height(),                                  // output_h
            depthSizedColorImage->width()* sizeof(std::int8_t)*4,            // output_stride_in_bytes
            4                                                                // num_channels
        );
        Bench::stop();
        if(!res){
            Logger::error("FemtoOrbbecDeviceImpl::resize_color_to_fit_depth: error during resizing.\n");
        }
    }
}


auto FemtoOrbbecDeviceImpl::generate_cloud() -> void{

    if(colorImage == nullptr || depthImage == nullptr){
        return;
    }

    if(has_cloud(settings.config.mode)){
        pointCloudImage = pointCloud->process(frameSet);
    }
}

auto FemtoOrbbecDeviceImpl::compress_frame(const DCFiltersSettings &filtersS, const DCDataSettings &dataS) -> std::unique_ptr<DCCompressedFrame>{
    return nullptr;
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

        if(depthSizedColorImage != nullptr){

            dFrame->depthSizedColorWidth  = depthSizedColorImage->width();
            dFrame->depthSizedColorHeight = depthSizedColorImage->height();
            dFrame->depthSizedImageColorData.resize(dFrame->depthSizedColorWidth*dFrame->depthSizedColorHeight);
            std::copy(
                reinterpret_cast<std::int8_t*>(depthSizedColorImage->data()),
                reinterpret_cast<std::int8_t*>(depthSizedColorImage->data()) + dFrame->depthSizedImageColorData.size()*4,
                reinterpret_cast<std::int8_t*>(dFrame->depthSizedImageColorData.data())
            );
        }

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
        const auto dRange = range(settings.config.mode)*1000.f;
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

        dFrame->infraWidth  = infraredImage->width();;
        dFrame->infraHeight = infraredImage->height();;
        dFrame->imageInfraData.resize(dFrame->infraWidth * dFrame->infraHeight);

        auto infraBuffer = reinterpret_cast<const uint16_t*>(infraredImage->data());

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
    if(d.generateCloudLocal && pointCloudImage != nullptr && depthSizedColorImage != nullptr && depthImage != nullptr){

        tool::Bench::start("[OrbbecDevice::create_local_frame] cloud");

        dFrame->cloud.vertices.resize(validDepthValues);
        dFrame->cloud.colors.resize(validDepthValues);
        dFrame->cloud.normals.resize(validDepthValues);

        // auto cloudBuffer = reinterpret_cast<geo::Pt3<int16_t>*>(pointCloudImage->get_buffer());
        auto colorBuffer = reinterpret_cast<const geo::Pt4<uint8_t>*>(depthSizedColorImage->data());
        auto depthBuffer = reinterpret_cast<const uint16_t*>(depthImage->data());


        OBColorPoint *cloudBuffer = reinterpret_cast<OBColorPoint*>(pointCloudImage->data());
        std::cout << "pt: " << pointCloudImage->dataSize() << " " <<  cloudBuffer->r << " " << cloudBuffer->g << " " << cloudBuffer->b << " " << cloudBuffer->x << " " << cloudBuffer->y << " " << cloudBuffer->z << "\n";
        // for(int i = 0; i < pointsSize; i++) {
        //     fprintf(fp, "%.3f %.3f %.3f\n", point->x, point->y, point->z);
        //     point++;
        // }


        std::for_each(std::execution::par_unseq, std::begin(indices.depthVertexCorrrespondance), std::end(indices.depthVertexCorrrespondance), [&](auto idC){

            // auto idD = std::get<0>(idC);
            // if(depthBuffer[idD] == dc_invalid_depth_value){
            //     return;
            // }

            // auto idV = std::get<1>(idC);
            // dFrame->cloud.vertices[idV]= geo::Pt3f{
            //     static_cast<float>(-cloudBuffer[idD].x()),
            //     static_cast<float>(-cloudBuffer[idD].y()),
            //     static_cast<float>( cloudBuffer[idD].z())
            // }*0.001f;
            // dFrame->cloud.colors[idV] = geo::Pt3f{
            //     static_cast<float>(colorBuffer[idD].z()),
            //     static_cast<float>(colorBuffer[idD].y()),
            //     static_cast<float>(colorBuffer[idD].x())
            // }/255.f;

            // // A B C
            // // D I E
            // // F G H
            // const auto &idN   = indicesNeighbours8Depth1D[idD];
            // const auto &idDVC = indices.depthVertexCorrrespondance;
            // const auto &v     = dFrame->cloud.vertices;
            // Vec3f normal{};

            // const auto &vId = v[idV];
            // if(idN[0] != -1 && std::get<1>(idDVC[idN[0]]) != -1){
            //     if(idN[3] != -1 && std::get<1>(idDVC[idN[3]]) != -1){ // vId x vIA
            //         normal += cross(v[std::get<1>(idDVC[idN[3]])] - vId, v[std::get<1>(idDVC[idN[0]])] - vId);
            //     }
            //     if(idN[1] != -1 && std::get<1>(idDVC[idN[1]]) != -1){ // vIA x vIB
            //         normal += cross(v[std::get<1>(idDVC[idN[0]])] - vId, v[std::get<1>(idDVC[idN[1]])] - vId);
            //     }
            // }
            // if(idN[2] != -1 && std::get<1>(idDVC[idN[2]]) != -1){
            //     if(idN[1] != -1 && std::get<1>(idDVC[idN[1]]) != -1){ // vIB x vIC
            //         normal += cross(v[std::get<1>(idDVC[idN[1]])] - vId, v[std::get<1>(idDVC[idN[2]])] - vId);
            //     }
            //     if(idN[4] != -1 && std::get<1>(idDVC[idN[4]]) != -1){ // vIC x vIE
            //         normal += cross(v[std::get<1>(idDVC[idN[2]])] - vId, v[std::get<1>(idDVC[idN[4]])] - vId);
            //     }
            // }
            // if(idN[7] != -1 && std::get<1>(idDVC[idN[7]]) != -1){
            //     if(idN[4] != -1 && std::get<1>(idDVC[idN[4]]) != -1){ // vIE x vIH
            //         normal += cross(v[std::get<1>(idDVC[idN[4]])] - vId, v[std::get<1>(idDVC[idN[7]])] - vId);
            //     }
            //     if(idN[6] != -1 && std::get<1>(idDVC[idN[6]]) != -1){ // vIH x vIG
            //         normal += cross(v[std::get<1>(idDVC[idN[7]])] - vId, v[std::get<1>(idDVC[idN[6]])] - vId);
            //     }
            // }
            // if(idN[5] != -1 && std::get<1>(idDVC[idN[5]]) != -1){
            //     if(idN[6] != -1 && std::get<1>(idDVC[idN[6]]) != -1){ // vIG x vIF
            //         normal += cross(v[std::get<1>(idDVC[idN[5]])] - vId, v[std::get<1>(idDVC[idN[6]])] - vId);
            //     }
            //     if(idN[3] != -1 && std::get<1>(idDVC[idN[3]]) != -1){ // vIF x vID
            //         normal += cross(v[std::get<1>(idDVC[idN[6]])] - vId, v[std::get<1>(idDVC[idN[3]])] - vId);
            //     }
            // }
            // dFrame->cloud.normals[idV] = normalize(normal);
        });

        tool::Bench::stop();
    }

    // imu sample
    if(d.captureIMU){
        dFrame->imuSample = data.imuSample;
    }else{
        dFrame->imuSample = std::nullopt;
    }

    tool::Bench::stop();

    // send audio
    // ...

    // send bodies
    // ...

    return dFrame;
}






//auto test() -> int {

//    // Create a Context.

//    ob::Context ctx;

//    // Query the list of connected devices
//    auto devList = ctx.queryDeviceList();

//    // Get the number of connected devices
//    if(devList->deviceCount() == 0) {
//        std::cerr << "Device not found!" << std::endl;
//        return -1;
//    }
//    // devList->pid(0) / devList->vid(0) / devList->uid(0)
//    // devList->serialNumber(0);
//    // devList->ipAddress(0); // for ethernet device
//    // devList->connectionType(0); // ："USB", "USB1.0", "USB1.1", "USB2.0", "USB2.1", "USB3.0", "USB3.1", "USB3.2", "Ethernet"
//    // devList->getDeviceBySN(..); // serial number
//    // devList->getDeviceByUid(..); // uid;

//    for(std::uint32_t ii = 0; ii < devList->deviceCount(); ++ii){
////        std::shared_ptr<ob::Device> dev = devList->getDevice(ii);
//    }

//    // Create a device, 0 means the index of the first device
//    std::shared_ptr<ob::Device> dev = devList->getDevice(0);

////    auto cpl = dev->getCalibrationCameraParamList();
////    dev->getDepthWorkModeList();


//    OBMultiDeviceSyncConfig c;
//    auto curConfig = dev->getMultiDeviceSyncConfig();
//    // Update the configuration items of the configuration file, and keep the original configuration for other items
//    curConfig.syncMode             = c.syncMode;
//    curConfig.depthDelayUs         = c.depthDelayUs;
//    curConfig.colorDelayUs         = c.colorDelayUs;
//    curConfig.trigger2ImageDelayUs = c.trigger2ImageDelayUs;
//    curConfig.triggerOutEnable     = c.triggerOutEnable;
//    curConfig.triggerOutDelayUs    = c.triggerOutDelayUs;
//    curConfig.framesPerTrigger     = c.framesPerTrigger;
//    dev->setMultiDeviceSyncConfig(curConfig);
////    dev->reboot();

//    // dev->getDeviceInfo();
//    // dev->deviceUpgrade();
//    // dev->deviceUpgradeFromData();
//    // auto state = dev->getDeviceState();

//    // Get device information
//    auto devInfo = dev->getDeviceInfo();
//    // Get the name of the device
//    std::cout << "Device name: " << devInfo->name() << std::endl;
//    // Get the pid, vid, uid of the device
//    std::cout << "Device pid: " << devInfo->pid() << " vid: " << devInfo->vid() << " uid: " << devInfo->uid() << std::endl;

//    // By getting the firmware version number of the device
//    auto fwVer = devInfo->firmwareVersion();
//    std::cout << "Firmware version: " << fwVer << std::endl;

//    // By getting the serial number of the device
//    auto sn = devInfo->serialNumber();
//    std::cout << "Serial number: " << sn << std::endl;

//    // By getting the connection type of the device
//    auto connectType = devInfo->connectionType();
//    std::cout << "ConnectionType: " << connectType << std::endl;

//    // Get the list of supported sensors
//    std::cout << "Sensor types: " << std::endl;
//    auto sensorList = dev->getSensorList();
//    for(uint32_t i = 0; i < sensorList->count(); i++) {
//        auto sensor = sensorList->getSensor(i);
//        switch(sensor->type()) {
//        case OB_SENSOR_COLOR:
//            std::cout << "\tColor sensor" << std::endl;
//            break;
//        case OB_SENSOR_DEPTH:
//            std::cout << "\tDepth sensor" << std::endl;
//            break;
//        case OB_SENSOR_IR:
//            std::cout << "\tIR sensor" << std::endl;
//            break;
//        case OB_SENSOR_IR_LEFT:
//            std::cout << "\tIR Left sensor" << std::endl;
//            break;
//        case OB_SENSOR_IR_RIGHT:
//            std::cout << "\tIR Right sensor" << std::endl;
//            break;
//        case OB_SENSOR_GYRO:
//            std::cout << "\tGyro sensor" << std::endl;
//            break;
//        case OB_SENSOR_ACCEL:
//            std::cout << "\tAccel sensor" << std::endl;
//            break;
//        default:
//            break;
//        }
//    }


//    // create a pipeline with default device
//    ob::Pipeline pipe;



//    // get profiles list from sensors
//    auto dProfilesL = pipe.getStreamProfileList(OB_SENSOR_DEPTH);
//    auto cProfilesL = pipe.getStreamProfileList(OB_SENSOR_COLOR);
//    auto irProfilesL = pipe.getStreamProfileList(OB_SENSOR_IR);

//    // retrieve video profiles
//    std::shared_ptr<ob::VideoStreamProfile> colorProfile = nullptr;
//    std::shared_ptr<ob::VideoStreamProfile> irProfile    = nullptr;
//    std::shared_ptr<ob::VideoStreamProfile> depthProfile = nullptr;

//    try {
//        colorProfile = cProfilesL->getVideoStreamProfile(1280, OB_HEIGHT_ANY, OB_FORMAT_RGB, 30);
//        depthProfile = dProfilesL->getVideoStreamProfile(640, OB_HEIGHT_ANY, OB_FORMAT_Y16, 30);
//        irProfile    = irProfilesL->getVideoStreamProfile(640, OB_HEIGHT_ANY, OB_FORMAT_Y16, 30);
//    }
//    catch(ob::Error &e) {
//        // If the specified format is not found, search for the default profile to open the stream
//        colorProfile = std::const_pointer_cast<ob::StreamProfile>(cProfilesL->getProfile(OB_PROFILE_DEFAULT))->as<ob::VideoStreamProfile>();
//        depthProfile = std::const_pointer_cast<ob::StreamProfile>(dProfilesL->getProfile(OB_PROFILE_DEFAULT))->as<ob::VideoStreamProfile>();
//        irProfile    = std::const_pointer_cast<ob::StreamProfile>(irProfilesL->getProfile(OB_PROFILE_DEFAULT))->as<ob::VideoStreamProfile>();
//    }

//    // configure which streams to enable or disable for the Pipeline by creating a Config
//    std::shared_ptr<ob::Config> config = std::make_shared<ob::Config>();
//    config->enableStream(colorProfile);
//    config->enableStream(irProfile);
//    config->enableStream(depthProfile);

//    // Configure the alignment mode as hardware D2C alignment
////    ALIGN_DISABLE,     /**< Turn off alignment */
////    ALIGN_D2C_HW_MODE, /**< Hardware D2C alignment mode */
////    ALIGN_D2C_SW_MODE, /**< Software D2C alignment mode */
//    config->setAlignMode(ALIGN_D2C_HW_MODE);
////    void setDepthScaleRequire(bool enable);
////    void setD2CTargetResolution(uint32_t d2cTargetWidth, uint32_t d2cTargetHeight);

//    // Start the pipeline with config
//    pipe.start(config);

////    // Start the pipeline and pass in the configuration
////    pipe->start(config, [i](std::shared_ptr<ob::FrameSet> frameSet) {
////        std::lock_guard<std::mutex> lock(frameMutex);
////        if(frameSet->colorFrame()) {
////            colorFrames[i] = frameSet->colorFrame();
////        }
////        if(frameSet->depthFrame()) {
////            depthFrames[i] = frameSet->depthFrame();
////        }
////    });

////    pipe.enableFrameSync();
////    pipe.disableFrameSync();

//    bool readData = true;
//    while(readData){
//        // Wait for up to 100ms for a frameset in blocking mode.
//        std::shared_ptr<ob::FrameSet> frameSet = pipe.waitForFrames(100);
//        if(frameSet == nullptr) {
//            continue;
//        }

//        // get per frame
////        frameSet->frameCount();
////        frameSet->getFrame(0);
////        frameSet->getFrame(OBFrameType::OB_FRAME_COLOR);
////        OB_FRAME_VIDEO     = 0,  /**< Video frame */
////        OB_FRAME_IR        = 1,  /**< IR frame */
////        OB_FRAME_COLOR     = 2,  /**< Color frame */
////        OB_FRAME_DEPTH     = 3,  /**< Depth frame */
////        OB_FRAME_ACCEL     = 4,  /**< Accelerometer data frame */
////        OB_FRAME_SET       = 5,  /**< Frame collection (internally contains a variety of data frames) */
////        OB_FRAME_POINTS    = 6,  /**< Point cloud frame */
////        OB_FRAME_GYRO      = 7,  /**< Gyroscope data frame */
////        OB_FRAME_IR_LEFT   = 8,  /**< Left IR frame */
////        OB_FRAME_IR_RIGHT  = 9,  /**< Right IR frame */
////        OB_FRAME_RAW_PHASE = 10, /**< Rawphase frame*/

//        // get specific
//        if(frameSet->colorFrame()) {
//            // https://en.wikipedia.org/wiki/Curiously_recurring_template_pattern
//            auto cf = frameSet->colorFrame();
//            // videoframe
////            cf->data();
////            uint32_t width();
////            uint32_t height();
////            void *metadata();
////            uint32_t metadataSize();
////            uint8_t pixelAvailableBitSize();

//            // frame
////            virtual OBFrameType type();
////            virtual OBFormat format();
////            OB_FORMAT_YUYV       = 0,    /**< YUYV format */
////            OB_FORMAT_YUY2       = 1,    /**< YUY2 format (the actual format is the same as YUYV) */
////            OB_FORMAT_UYVY       = 2,    /**< UYVY format */
////            OB_FORMAT_NV12       = 3,    /**< NV12 format */
////            OB_FORMAT_NV21       = 4,    /**< NV21 format */
////            OB_FORMAT_MJPG       = 5,    /**< MJPEG encoding format */
////            OB_FORMAT_H264       = 6,    /**< H.264 encoding format */
////            OB_FORMAT_H265       = 7,    /**< H.265 encoding format */
////            OB_FORMAT_Y16        = 8,    /**< Y16 format, single channel 16-bit depth */
////            OB_FORMAT_Y8         = 9,    /**< Y8 format, single channel 8-bit depth */
////            OB_FORMAT_Y10        = 10,   /**< Y10 format, single channel 10-bit depth (SDK will unpack into Y16 by default) */
////            OB_FORMAT_Y11        = 11,   /**< Y11 format, single channel 11-bit depth (SDK will unpack into Y16 by default) */
////            OB_FORMAT_Y12        = 12,   /**< Y12 format, single channel 12-bit depth (SDK will unpack into Y16 by default) */
////            OB_FORMAT_GRAY       = 13,   /**< GRAY (the actual format is the same as YUYV) */
////            OB_FORMAT_HEVC       = 14,   /**< HEVC encoding format (the actual format is the same as H265) */
////            OB_FORMAT_I420       = 15,   /**< I420 format */
////            OB_FORMAT_ACCEL      = 16,   /**< Acceleration data format */
////            OB_FORMAT_GYRO       = 17,   /**< Gyroscope data format */
////            OB_FORMAT_POINT      = 19,   /**< XYZ 3D coordinate point format */
////            OB_FORMAT_RGB_POINT  = 20,   /**< XYZ 3D coordinate point format with RGB information */
////            OB_FORMAT_RLE        = 21,   /**< RLE pressure test format (SDK will be unpacked into Y16 by default) */
////            OB_FORMAT_RGB        = 22,   /**< RGB format (actual RGB888)  */
////            OB_FORMAT_BGR        = 23,   /**< BGR format (actual BGR888) */
////            OB_FORMAT_Y14        = 24,   /**< Y14 format, single channel 14-bit depth (SDK will unpack into Y16 by default) */
////            OB_FORMAT_BGRA       = 25,   /**< BGRA format */
////            OB_FORMAT_COMPRESSED = 26,   /**< Compression format */
////            OB_FORMAT_RVL        = 27,   /**< RVL pressure test format (SDK will be unpacked into Y16 by default) */
////            OB_FORMAT_UNKNOWN    = 0xff, /**< unknown format */
////            virtual uint64_t index();
////            virtual void *data();
////            virtual uint32_t dataSize();
////            uint64_t timeStamp();
////            uint64_t timeStampUs();
////            uint64_t systemTimeStamp();
////            template <typename T> bool is();
////            template <typename T> std::shared_ptr<T> as();
//        }


//        frameSet->depthFrame();
//        frameSet->irFrame();
//        frameSet->pointsFrame();

//        readData = false;
//    }

//    pipe.stop();

////        while(app) {


////            // Render frameset in the window, only color frames are rendered here.
////            app.addToRender(frameSet->colorFrame());

////        // for Y16 format depth frame, print the distance of the center pixel every 30 frames
////        if(depthFrame->index() % 30 == 0 && depthFrame->format() == OB_FORMAT_Y16) {
////            uint32_t  width  = depthFrame->width();
////            uint32_t  height = depthFrame->height();
////            float     scale  = depthFrame->getValueScale();
////            uint16_t *data   = (uint16_t *)depthFrame->data();

////            // pixel value multiplied by scale is the actual distance value in millimeters
////            float centerDistance = data[width * height / 2 + width / 2] * scale;

////            // attention: if the distance is 0, it means that the depth camera cannot detect the object（may be out of detection range）
////            std::cout << "Facing an object " << centerDistance << " mm away. " << std::endl;
////        }
////        }

////        // Stop the Pipeline, no frame data will be generated
////        pipe.stop();

//    // ############################ MULTIFRAME

////        // create frame resource
////        std::mutex                 videoFrameMutex;
////        std::shared_ptr<ob::Frame> colorFrame;
////        std::shared_ptr<ob::Frame> depthFrame;
////        std::shared_ptr<ob::Frame> irFrame;

////        std::mutex                 accelFrameMutex;
////        std::shared_ptr<ob::Frame> accelFrame;

////        std::mutex                 gyroFrameMutex;
////        std::shared_ptr<ob::Frame> gyroFrame;

////        std::vector<std::shared_ptr<ob::Frame>> frames;

////        // Create a pipeline with default device
////        ob::Pipeline pipe;

////        // Configure which streams to enable or disable for the Pipeline by creating a Config
////        std::shared_ptr<ob::Config> config = std::make_shared<ob::Config>();

////        try {
////            auto colorProfiles = pipe.getStreamProfileList(OB_SENSOR_COLOR);
////            auto colorProfile  = colorProfiles->getProfile(OB_PROFILE_DEFAULT);
////            config->enableStream(colorProfile->as<ob::VideoStreamProfile>());
////        }
////        catch(...) {
////            std::cout << "color stream not found!" << std::endl;
////        }

////        try {
////            auto depthProfiles = pipe.getStreamProfileList(OB_SENSOR_DEPTH);
////            auto depthProfile  = depthProfiles->getProfile(OB_PROFILE_DEFAULT);
////            config->enableStream(depthProfile->as<ob::VideoStreamProfile>());
////        }
////        catch(...) {
////            std::cout << "depth stream not found!" << std::endl;
////        }

////        try {
////            auto irProfiles = pipe.getStreamProfileList(OB_SENSOR_IR);
////            auto irProfile  = irProfiles->getProfile(OB_PROFILE_DEFAULT);
////            config->enableStream(irProfile->as<ob::VideoStreamProfile>());
////        }
////        catch(...) {
////            std::cout << "ir stream not found!" << std::endl;
////        }

////        // Configure the alignment mode as hardware D2C alignment
////        config->setAlignMode(ALIGN_D2C_HW_MODE);


////        pipe.start(config, [&](std::shared_ptr<ob::FrameSet> frameset) {
////            std::unique_lock<std::mutex> lk(videoFrameMutex);
////            colorFrame = frameset->colorFrame();
////            depthFrame = frameset->depthFrame();
////            irFrame    = frameset->irFrame();
////        });
//}





//        // register device callback
//        ctx.setDeviceChangedCallback([](std::shared_ptr<ob::DeviceList> removedList, std::shared_ptr<ob::DeviceList> addedList) {
//            handleDeviceDisconnected(removedList);
//            handleDeviceConnected(addedList);
//        });
//        ctx.setDeviceChangedCallback([](std::shared_ptr<ob::DeviceList> removedList, std::shared_ptr<ob::DeviceList> addedList) {
//            if(isWaitRebootComplete_) {
//                if(addedList && addedList->deviceCount() > 0) {
//                    auto device = addedList->getDevice(0);
//                    if(isDeviceRemoved_ && deviceSN_ == std::string(device->getDeviceInfo()->serialNumber())) {
//                        rebootedDevice_       = device;
//                        isWaitRebootComplete_ = false;

//                        std::unique_lock<std::mutex> lk(waitRebootMutex_);
//                        waitRebootCondition_.notify_all();
//                    }
//                }

//                if(removedList && removedList->deviceCount() > 0) {
//                    if(deviceUid_ == std::string(removedList->uid(0))) {
//                        isDeviceRemoved_ = true;
//                    }
//                }
//            }  // if isWaitRebootComplete_
//        });




// OrbbecDevice::refresh_devices_list();

