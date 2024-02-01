

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

#include "orbbec_base_device.hpp"

// orbbec
#include <libobsensor/hpp/Utils.hpp>
#include <libobsensor/hpp/Sensor.hpp>
#include <libobsensor/hpp/Error.hpp>
#include <libobsensor/hpp/StreamProfile.hpp>

// local
#include "utility/logger.hpp"
#include "utility/benchmark.hpp"
#include "azure_utility.hpp"

// debug
#include "thirdparty/stb/stb_image_write.h"

using namespace  std::string_literals;
using namespace tool::cam;


[[maybe_unused]] [[nodiscard]] static auto convert_to_ob_image_format(DCImageFormat iFormat) noexcept -> OBFormat{
    switch (iFormat) {
    case DCImageFormat::NV12:
        /**< NV12 format */
        return OB_FORMAT_NV12;
    case DCImageFormat::YUY2:
        /**< YUY2 format (the actual format is the same as YUYV) */
        return OB_FORMAT_YUY2;
    case DCImageFormat::MJPG:
        /**< MJPEG encoding format */
        return OB_FORMAT_MJPG;
    case DCImageFormat::BGRA:
        /**< BGRA format */
        return OB_FORMAT_BGRA;
    case DCImageFormat::DEPTH16:
        return OB_FORMAT_Y16;
        /**< Y16 format, single channel 16-bit depth */
    case DCImageFormat::INFRA16:
        /**< Y16 format, single channel 16-bit depth */
        return OB_FORMAT_Y16;
    default:
        return OB_FORMAT_BGRA;
    }
}

OrbbecBaseDevice::OrbbecBaseDevice(){
    ob::Context::setLoggerSeverity(OB_LOG_SEVERITY_WARN);
    context = std::make_unique<ob::Context>();
}

auto OrbbecBaseDevice::query_devices(std::string_view deviceTypeName, bool ethernet) -> void{
    context->enableNetDeviceEnumeration(ethernet);

    auto devicesFound = context->queryDeviceList();

    deviceList.clear();
    for(std::uint32_t idDev = 0; idDev < devicesFound->deviceCount(); ++idDev){
        auto dev = devicesFound->getDevice(idDev);
        if(dev->getDeviceInfo()->name() == deviceTypeName){
            deviceList.push_back(std::move(dev));
        }
    }
    Logger::message(std::format("[OrbbecDevice] [{}] devices found of type [{}].\n", deviceList.size(), deviceTypeName));
}

auto OrbbecBaseDevice::initialize(const DCModeInfos &mInfos) -> void{

    frameSet      = nullptr;
    colorImage    = nullptr;
    depthImage    = nullptr;
    infraredImage = nullptr;   

    if(mInfos.has_color() && dc_has_depth(mInfos.mode())){
        depthSizedColorData.resize(mInfos.depth_size() * 4);
    }else{
        depthSizedColorData.clear();
    }

    if(mInfos.has_depth() && mInfos.has_cloud()){
        cloudData.resize(mInfos.depth_size() * 3 * sizeof(int16_t));
    }else{
        cloudData.clear();
    }
}

auto OrbbecBaseDevice::is_opened() const noexcept -> bool {
    return device != nullptr;
}

auto OrbbecBaseDevice::nb_devices() const noexcept -> size_t {
    return deviceList.size();
}

auto OrbbecBaseDevice::device_name() const noexcept -> std::string {
    if(is_opened()){
        return device->getDeviceInfo()->name();
    }
    return "unknow_device"s;
}

auto OrbbecBaseDevice::capture_frame(int32_t timeoutMs) -> bool{

    if(pipe != nullptr){
        Bench::start("[OrbbecDevice::capture_frame]");
        try{
            frameSet = pipe->waitForFrames(timeoutMs);
        }catch(ob::Error &e) {
            Logger::error(std::format("[OrbbecDevice] Get capture error: {}\n", e.getMessage()));
        }

        Bench::stop();
        return frameSet != nullptr;
    }
    return false;
}

auto OrbbecBaseDevice::read_color_image() -> std::span<std::int8_t>{

    if(frameSet != nullptr){

        Bench::start("[OrbbecDevice::read_color_image]");
        colorImage = frameSet->colorFrame();
        Bench::stop();

        if(colorImage != nullptr){
            return std::span<std::int8_t>{
                reinterpret_cast<std::int8_t*>(colorImage->data()),
                colorImage->width()*colorImage->height()*4
            };
        }
    }
    return {};
}

auto OrbbecBaseDevice::read_depth_image() -> std::span<std::uint16_t>{

    if(frameSet != nullptr){

        Bench::start("[OrbbecDevice::read_depth_image]");
        depthImage = frameSet->depthFrame();
        Bench::stop();

        if(depthImage != nullptr){
            return  std::span<std::uint16_t>{
                reinterpret_cast<std::uint16_t*>(depthImage->data()),
                depthImage->width()*depthImage->height()
            };
        }
    }
    return {};
}

auto OrbbecBaseDevice::read_infra_image() -> std::span<std::uint16_t>{

    if(frameSet != nullptr){

        Bench::start("[OrbbecDevice::read_infra_image]");
        infraredImage = frameSet->irFrame();
        Bench::stop();

        if(infraredImage != nullptr){
            return  std::span<std::uint16_t>{
                reinterpret_cast<std::uint16_t*>(infraredImage->data()),
                infraredImage->width()*infraredImage->height()
            };
        }
    }

    return {};
}

auto OrbbecBaseDevice::k4a_convert_calibration(const DCModeInfos &mInfos) -> k4a::calibration{

    const auto &obDepthIntrinsics = calibrationParam.intrinsics[OB_SENSOR_DEPTH];
    const auto &obDepthDistorsion = calibrationParam.distortion[OB_SENSOR_DEPTH];
    const auto &obDepthExtrinsics = calibrationParam.extrinsics[OB_SENSOR_DEPTH];

    const auto &obColorIntrinsics = calibrationParam.intrinsics[OB_SENSOR_COLOR];
    const auto &obColorDistorsion = calibrationParam.distortion[OB_SENSOR_COLOR];
    const auto &obColorExtrinsics = calibrationParam.extrinsics[OB_SENSOR_COLOR];

    k4a::calibration k4Calibration;
    k4Calibration.depth_mode       = convert_to_k4a_depth_mode(mInfos.depth_resolution());
    k4Calibration.color_resolution = convert_to_k4a_color_resolution(mInfos.color_resolution());

    // depth calibration
    auto &cdepthCamCal              = k4Calibration.depth_camera_calibration;
    // # resolutions
    cdepthCamCal.resolution_width   = mInfos.depth_width();
    cdepthCamCal.resolution_height  = mInfos.depth_height();
    // # metric_radius
    cdepthCamCal.metric_radius = 1.7399998f;
    // # intrinsics
    // ## type
    cdepthCamCal.intrinsics.type = K4A_CALIBRATION_LENS_DISTORTION_MODEL_BROWN_CONRADY;
    // ## parameters
    auto &dIntrincisP           = cdepthCamCal.intrinsics.parameters.param;
    dIntrincisP.cx              = obDepthIntrinsics.cx; /**< Principal point in image, x */
    dIntrincisP.cy              = obDepthIntrinsics.cy; /**< Principal point in image, y */
    dIntrincisP.fx              = obDepthIntrinsics.fx; /**< Focal length x */
    dIntrincisP.fy              = obDepthIntrinsics.fy; /**< Focal length y */
    dIntrincisP.k1              = obDepthDistorsion.k1; /**< k1 radial distortion coefficient */
    dIntrincisP.k2              = obDepthDistorsion.k2; /**< k2 radial distortion coefficient */
    dIntrincisP.k3              = obDepthDistorsion.k3; /**< k3 radial distortion coefficient */
    dIntrincisP.k4              = obDepthDistorsion.k4; /**< k4 radial distortion coefficient */
    dIntrincisP.k5              = obDepthDistorsion.k5; /**< k5 radial distortion coefficient */
    dIntrincisP.k6              = obDepthDistorsion.k6; /**< k6 radial distortion coefficient */
    // dIntrincisP.codx; // UNUSED                      /**< Center of distortion in Z=1 plane, x (only used for Rational6KT) */
    // dIntrincisP.cody; // UNUSED                      /**< Center of distortion in Z=1 plane, y (only used for Rational6KT) */
    dIntrincisP.p1              = obDepthDistorsion.p1; /**< Tangential distortion coefficient 2 */
    dIntrincisP.p2              = obDepthDistorsion.p2; /**< Tangential distortion coefficient 1 */
    dIntrincisP.metric_radius   = 0; /**< Metric radius */
    cdepthCamCal.intrinsics.parameter_count = 15;
    // # extrinsics
    // ## rotation
    auto &dExtRot = cdepthCamCal.extrinsics.rotation;
    dExtRot[0] = 1.f;
    dExtRot[1] = 0.f;
    dExtRot[2] = 0.f;
    dExtRot[3] = 0.f;
    dExtRot[4] = 1.f;
    dExtRot[5] = 0.f;
    dExtRot[6] = 0.f;
    dExtRot[7] = 0.f;
    dExtRot[8] = 1.f;
    // ## translation
    auto &dExtTr = cdepthCamCal.extrinsics.translation;
    dExtTr[0] = 0.f;
    dExtTr[1] = 0.f;
    dExtTr[2] = 0.f;

    // color calibration
    auto &cColorCamCal              = k4Calibration.color_camera_calibration;
    // # resolutions
    cColorCamCal.resolution_width   = mInfos.color_width();
    cColorCamCal.resolution_height  = mInfos.color_height();
    // # metric_radius
    cColorCamCal.metric_radius      = 1.7f;
    // # intrinsics
    // ## type
    cColorCamCal.intrinsics.type    = K4A_CALIBRATION_LENS_DISTORTION_MODEL_BROWN_CONRADY;
    // ## parameters
    auto &cIntrincisP               = cColorCamCal.intrinsics.parameters.param;
    cIntrincisP.cx                  = obColorIntrinsics.cx; /**< Principal point in image, x */
    cIntrincisP.cy                  = obColorIntrinsics.cy; /**< Principal point in image, y */
    cIntrincisP.fx                  = obColorIntrinsics.fx; /**< Focal length x */
    cIntrincisP.fy                  = obColorIntrinsics.fy; /**< Focal length y */
    cIntrincisP.k1                  = obColorDistorsion.k1; /**< k1 radial distortion coefficient */
    cIntrincisP.k2                  = obColorDistorsion.k2; /**< k2 radial distortion coefficient */
    cIntrincisP.k3                  = obColorDistorsion.k3; /**< k3 radial distortion coefficient */
    cIntrincisP.k4                  = obColorDistorsion.k4; /**< k4 radial distortion coefficient */
    cIntrincisP.k5                  = obColorDistorsion.k5; /**< k5 radial distortion coefficient */
    cIntrincisP.k6                  = obColorDistorsion.k6; /**< k6 radial distortion coefficient */
    // cIntrincisP.codx; // UNUSED                          /**< Center of distortion in Z=1 plane, x (only used for Rational6KT) */
    // cIntrincisP.cody; // UNUSED                          /**< Center of distortion in Z=1 plane, y (only used for Rational6KT) */
    cIntrincisP.p1                  = obColorDistorsion.p1; /**< Tangential distortion coefficient 2 */
    cIntrincisP.p2                  = obColorDistorsion.p2; /**< Tangential distortion coefficient 1 */
    cIntrincisP.metric_radius       = 0; /**< Metric radius */
    cColorCamCal.intrinsics.parameter_count = 15;
    // # extrinsics
    // ## rotation
    auto &cExtRot = cColorCamCal.extrinsics.rotation;
    cExtRot[0] = 1.f;
    cExtRot[1] = 0.f;
    cExtRot[2] = 0.f;
    cExtRot[3] = 0.f;
    cExtRot[4] = 1.f;
    cExtRot[5] = 0.f;
    cExtRot[6] = 0.f;
    cExtRot[7] = 0.f;
    cExtRot[8] = 1.f;
    // ## translation
    auto &cExtTr = cColorCamCal.extrinsics.translation;
    cExtTr[0] = 0.f;
    cExtTr[1] = 0.f;
    cExtTr[2] = 0.f;

    // extrinsics
    std::copy(obDepthExtrinsics[OB_SENSOR_COLOR].rot,   obDepthExtrinsics[OB_SENSOR_COLOR].rot + 9,     k4Calibration.extrinsics[K4A_CALIBRATION_TYPE_DEPTH][K4A_CALIBRATION_TYPE_COLOR].rotation);
    std::copy(obDepthExtrinsics[OB_SENSOR_COLOR].trans, obDepthExtrinsics[OB_SENSOR_COLOR].trans + 3,   k4Calibration.extrinsics[K4A_CALIBRATION_TYPE_DEPTH][K4A_CALIBRATION_TYPE_COLOR].translation);
    std::copy(obColorExtrinsics[OB_SENSOR_DEPTH].rot,   obDepthExtrinsics[OB_SENSOR_DEPTH].rot + 9,     k4Calibration.extrinsics[K4A_CALIBRATION_TYPE_COLOR][K4A_CALIBRATION_TYPE_DEPTH].rotation);
    std::copy(obColorExtrinsics[OB_SENSOR_DEPTH].trans, obDepthExtrinsics[OB_SENSOR_DEPTH].trans + 3,   k4Calibration.extrinsics[K4A_CALIBRATION_TYPE_COLOR][K4A_CALIBRATION_TYPE_DEPTH].translation);

    return k4Calibration;
}

auto OrbbecBaseDevice::k4a_resize_color_image_to_depth_size(const DCModeInfos &mInfos, std::span<ColorRGBA8> colorData, std::span<uint16_t> depthData) -> std::span<ColorRGBA8>{
    
    auto colorStride     = mInfos.color_width() * 4 * sizeof(std::uint8_t);
    auto colorSizeBytes  = colorStride * mInfos.color_height();
    auto colorDataBuffer = reinterpret_cast<std::uint8_t*>(colorData.data());

    auto depthStride     = mInfos.depth_width() * 1 * sizeof(std::uint16_t);
    auto depthSizeBytes  = depthStride * mInfos.depth_height();
    auto depthDataBuffer = reinterpret_cast<std::uint8_t*>(depthData.data());

    auto depthSizedColorStride    = mInfos.depth_width() * 4 * sizeof(std::uint8_t);
    auto depthSizedColorSizeBytes = depthSizedColorStride * mInfos.depth_height();
    auto depthSizedColorDataBuffer= reinterpret_cast<std::uint8_t*>(depthSizedColorData.data());

    auto k4aColorImage = k4a::image::create_from_buffer(
        K4A_IMAGE_FORMAT_COLOR_BGRA32,
        static_cast<int>(mInfos.color_width()), static_cast<int>(mInfos.color_height()), static_cast<int>(colorStride), colorDataBuffer, colorSizeBytes,
        nullptr, nullptr
    );

    auto k4aDepthImage = k4a::image::create_from_buffer(
        K4A_IMAGE_FORMAT_DEPTH16,
        static_cast<int>(mInfos.depth_width()), static_cast<int>(mInfos.depth_height()), static_cast<int>(depthStride), depthDataBuffer, depthSizeBytes,
        nullptr, nullptr
    );

    auto k4aDepthSizedColorImage = k4a::image::create_from_buffer(
        K4A_IMAGE_FORMAT_COLOR_BGRA32,
        static_cast<int>(mInfos.depth_width()), static_cast<int>(mInfos.depth_height()), static_cast<int>(depthSizedColorStride), depthSizedColorDataBuffer, depthSizedColorSizeBytes,
        nullptr, nullptr
    );

    try{
        Bench::start("[OrbbecBaseDevice::k4a_resize_color_image_to_depth_size]");

        k4aTransformation.color_image_to_depth_camera(
            k4aDepthImage,
            k4aColorImage,
            &k4aDepthSizedColorImage
        );

        Bench::stop();

    }catch(const std::runtime_error &error){
        Logger::error(std::format("[OrbbecBaseDevice::k4a_resize_color_image_to_depth_size] Runtime error: {}", error.what()));
        return {};
    }

    return std::span<tool::ColorRGBA8>{
        reinterpret_cast<tool::ColorRGBA8*>(depthSizedColorData.data()),
        static_cast<size_t>(mInfos.depth_width() * mInfos.depth_height())
    };
}

auto OrbbecBaseDevice::k4a_generate_cloud(const DCModeInfos &mInfos, std::span<uint16_t> depthData) -> std::span<geo::Pt3<std::int16_t>> {

    auto depthStride     = mInfos.depth_width() * 1 * sizeof(std::uint16_t);
    auto depthSizeBytes  = depthStride * mInfos.depth_height();
    auto depthDataBuffer = reinterpret_cast<std::uint8_t*>(depthData.data());

    auto cloudStride     = mInfos.depth_width() * 3 * sizeof(std::uint16_t);
    auto cloudSizeBytes  = cloudStride * mInfos.depth_height();
    auto cloudDataBuffer = reinterpret_cast<std::uint8_t*>(cloudData.data());

    auto k4aDepthImage = k4a::image::create_from_buffer(
        K4A_IMAGE_FORMAT_DEPTH16,
        mInfos.depth_width(), mInfos.depth_height(), depthStride, depthDataBuffer, depthSizeBytes,
        nullptr,nullptr
    );

    auto k4aPointCloudImage = k4a::image::create_from_buffer(
        K4A_IMAGE_FORMAT_CUSTOM,
        mInfos.depth_width(), mInfos.depth_height(), cloudStride, cloudDataBuffer, cloudSizeBytes,
        nullptr, nullptr
    );

    try{
        Bench::start("[OrbbecBaseDevice::k4a_generate_cloud]");

        k4aTransformation.depth_image_to_point_cloud(
            k4aDepthImage,
            K4A_CALIBRATION_TYPE_DEPTH,
            &k4aPointCloudImage
        );
        Bench::stop();

    }catch(const std::runtime_error &error){
        Logger::error(std::format("[OrbbecBaseDevice::k4a_generate_cloud] Runtime error: {}", error.what()));
        return {};
    }

    return std::span<geo::Pt3<std::int16_t>>{
        reinterpret_cast<geo::Pt3<std::int16_t>*>(cloudData.data()),
        mInfos.depth_width() * mInfos.depth_height()
    };
}

auto OrbbecBaseDevice::set_property_value(OBPropertyID pId, bool value) -> void{

    if(!is_opened()){
        return;
    }

    auto dev = device.get();
    if(!dev->isPropertySupported(pId, OB_PERMISSION_READ_WRITE)){
        return;
    }
    if(dev->getBoolProperty(pId) != value){
        dev->setBoolProperty(pId, value);
    }
}

auto OrbbecBaseDevice::set_property_value(OBPropertyID pId, int32_t value) -> void{

    if(!is_opened()){
        return;
    }

    auto dev = device.get();
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

auto OrbbecBaseDevice::open_device(uint32_t deviceId) -> bool{

    if(deviceId >= deviceList.size()){
        Logger::error("[OrbbecDevice] Invalid id device.\n"sv);
        return false;
    }

    try {

        // retrieve device
        device     = deviceList[deviceId];
        sensorList = device->getSensorList();

        for(uint32_t idS = 0; idS < sensorList->count(); idS++) {
            auto sensor = sensorList->getSensor(idS);
            switch(sensor->type()) {
            case OB_SENSOR_COLOR:
                Logger::message("[OrbbecDevice] Color sensor found.\n"sv);
                break;
            case OB_SENSOR_DEPTH:
                Logger::message("[OrbbecDevice] Depth sensor found.\n"sv);
                break;
            case OB_SENSOR_IR:
                Logger::message("[OrbbecDevice] Infrared sensor found.\n"sv);
                break;
            case OB_SENSOR_IR_LEFT:
                Logger::message("[OrbbecDevice] Infrared left sensor found.\n"sv);
                break;
            case OB_SENSOR_IR_RIGHT:
                Logger::message("[OrbbecDevice] Infrared right sensor found.\n"sv);
                break;
            case OB_SENSOR_GYRO:
                Logger::message("[OrbbecDevice] Gyro sensor found.\n"sv);
                break;
            case OB_SENSOR_ACCEL:
                Logger::message("[OrbbecDevice] Accel sensor found.\n"sv);
                break;
            default:
                break;
            }
        }

        auto dInfos     = device->getDeviceInfo();
        Logger::message("[OrbbecDevice] Device opened:\n");
        Logger::message(std::format("  Name: {}\n", dInfos->name()));
        Logger::message(std::format("  Chip type: {}\n", dInfos->asicName()));
        Logger::message(std::format("  Serialnum: {}\n", dInfos->serialNumber()));
        Logger::message(std::format("  PID: {}\n", dInfos->pid()));
        Logger::message(std::format("  VID: {}\n", dInfos->vid()));
        Logger::message(std::format("  UID: {}\n", dInfos->uid()));

        Logger::message("  Version:\n");
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
        if(std::string(dInfos->connectionType()) == "Ethernet"){
            Logger::message(std::format("  IP: {}\n", dInfos->ipAddress()));
        }

    }catch(ob::Error &e) {
        Logger::error(std::format("[OrbbecDevice] Open error: {}\nsv", e.getMessage()));
        device = nullptr;
        return false;
    }
    return true;
}

auto OrbbecBaseDevice::start_pipeline(const DCModeInfos &mInfos, const DCConfigSettings &configS) -> bool{

    std::shared_ptr<ob::Config> config = std::make_shared<ob::Config>();

    try {

        if(pipe != nullptr){
            pipe->stop();
            pipe = nullptr;
        }
        pipe = std::make_unique<ob::Pipeline>(device);

        // retrieve color profile
        if(sensorList->getSensor(OBSensorType::OB_SENSOR_COLOR) && mInfos.has_color()){
            if(auto colorStreamProfileList = pipe->getStreamProfileList(OB_SENSOR_COLOR); colorStreamProfileList != nullptr){
                std::shared_ptr<ob::StreamProfile> colorProfile = nullptr;
                try {
                    colorProfile = colorStreamProfileList->getVideoStreamProfile(
                        static_cast<int>(mInfos.color_width()),
                        static_cast<int>(mInfos.color_height()),
                        convert_to_ob_image_format(mInfos.image_format()),
                        mInfos.framerate_value()
                    );
                }catch(...) {
                    colorProfile = colorStreamProfileList->getProfile(OB_PROFILE_DEFAULT);
                }
                config->enableStream(colorProfile);
            }
        }

        // retrieve depth profile
        if(sensorList->getSensor(OBSensorType::OB_SENSOR_DEPTH) && mInfos.has_depth()){
            if(auto depthStreamProfileList = pipe->getStreamProfileList(OB_SENSOR_DEPTH); depthStreamProfileList != nullptr){
                std::shared_ptr<ob::StreamProfile> depthProfile = nullptr;
                try {
                    depthProfile = depthStreamProfileList->getVideoStreamProfile(
                        static_cast<int>(mInfos.depth_width()),
                        static_cast<int>(mInfos.depth_height()),
                        OB_FORMAT_Y16,
                        mInfos.framerate_value()
                    );
                }catch(...) {
                    depthProfile = depthStreamProfileList->getProfile(OB_PROFILE_DEFAULT);
                }
                config->enableStream(depthProfile);
            }
        }

        // retrieve infrared profile
        if(sensorList->getSensor(OBSensorType::OB_SENSOR_IR) && mInfos.has_infra()){
            if(auto infraStreamProfileList = pipe->getStreamProfileList(OB_SENSOR_IR); infraStreamProfileList != nullptr){
                std::shared_ptr<ob::StreamProfile> infraProfile = nullptr;
                try {
                    infraProfile = infraStreamProfileList->getVideoStreamProfile(
                        static_cast<int>(mInfos.infra_width()),
                        static_cast<int>(mInfos.infra_height()),
                        OB_FORMAT_Y16,
                        mInfos.framerate_value()
                    );
                }catch(...) {
                    infraProfile = infraStreamProfileList->getProfile(OB_PROFILE_DEFAULT);
                }
                if(infraProfile){
                    config->enableStream(infraProfile);
                }
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

        switch (configS.synchMode) {
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
            currSynchConfig.triggerOutDelayUs       = configS.subordinateDelayUsec;
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
            currSynchConfig.triggerOutDelayUs       = configS.subordinateDelayUsec;
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

        // disable align mode
        config->setAlignMode(ALIGN_DISABLE);

        // frame synch
        if(configS.synchronizeColorAndDepth){
            pipe->enableFrameSync();
        }else{
            pipe->disableFrameSync();
        }

        // other
        set_property_value(OB_PROP_INDICATOR_LIGHT_BOOL, !configS.disableLED);

        // start pipe with current config
        pipe->start(config);

        // get camera intrinsic and extrinsic parameters form pipeline and set to point cloud filter
        cameraParam         = pipe->getCameraParam();
        // get calibration parameters
        calibrationParam    = pipe->getCalibrationParam(config);

    }catch(ob::Error &e) {
        Logger::error(std::format("[OrbbecDevice::start_reading] Start reading error: {}\n", e.getMessage()));
        device = nullptr;
        return false;
    }

    k4aCalibration    = k4a_convert_calibration(mInfos);
    k4aTransformation = k4a::transformation(k4aCalibration);

    return true;
}

// auto OrbbecBaseDevice::stop_pipe() -> void{

//     if(pipe != nullptr){
//         try{

//             // std::cout << "STOP ALL SENSORS\n";
//             // for(size_t idS = 0; idS < sensorList->count(); ++idS){
//             //     sensorList->getSensor(idS)->stop();
//             // }
//             // pipe->stop();
//             // pipe = nullptr;

//             // sensorList->getSensor(OBSensorType::OB_SENSOR_COLOR)->stop();
//             // std::cout << "STOP\n";
//             // sensorList->getSensor(OBSensorType::OB_SENSOR_DEPTH)->stop();
//             // std::cout << "STOP PIPE\n";
//             // pipe->stop();
//             // std::cout << "END STOP\n";
//         }catch(ob::Error &e) {
//             Logger::error(std::format("[OrbbecDevice] Error: {}\n", e.getMessage()));
//         }
//     }
// }

auto OrbbecBaseDevice::close_device() -> void{

    if(pipe != nullptr){
        pipe->stop();
        pipe = nullptr;
    }

    sensorList = nullptr;
    device     = nullptr;
}

auto OrbbecBaseDevice::update_camera_from_colors_settings(bool isCameraReading, const DCColorSettings &colorS) -> void{

    try{
        set_property_value(OB_PROP_COLOR_AUTO_EXPOSURE_BOOL,           colorS.autoExposureTime);
        set_property_value(OB_PROP_COLOR_AUTO_WHITE_BALANCE_BOOL,      colorS.autoWhiteBalance);
        set_property_value(OB_PROP_COLOR_EXPOSURE_INT,                 colorS.exposureTime);
        set_property_value(OB_PROP_COLOR_WHITE_BALANCE_INT,            colorS.whiteBalance);
        set_property_value(OB_PROP_COLOR_BRIGHTNESS_INT,               colorS.brightness);
        set_property_value(OB_PROP_COLOR_CONTRAST_INT,                 colorS.contrast);
        set_property_value(OB_PROP_COLOR_SHARPNESS_INT,                colorS.sharpness);
        set_property_value(OB_PROP_COLOR_SATURATION_INT,               colorS.saturation);
        set_property_value(OB_PROP_COLOR_BACKLIGHT_COMPENSATION_INT,   colorS.backlightCompensation ? 1 : 0); // NOT AVAILABLE
        set_property_value(OB_PROP_COLOR_GAIN_INT,                     colorS.gain);
        set_property_value(OB_PROP_COLOR_POWER_LINE_FREQUENCY_INT,     static_cast<int>(colorS.powerlineFrequency));

        // color HDR
        auto currHDR = device->getBoolProperty(OB_PROP_COLOR_HDR_BOOL);
        if(currHDR != colorS.hdr){
            if(!isCameraReading){
                set_property_value(OB_PROP_COLOR_HDR_BOOL, colorS.hdr);
            }else{
                Logger::warning("[OrbbecDevice] Reading must be stopped before changing HDR settings.\n");
            }
        }
    }catch(ob::Error &e) {
        Logger::error(std::format("[OrbbecDevice] Error: {}\n", e.getMessage()));
    }
}
