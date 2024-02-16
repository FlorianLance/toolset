

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

// k4a
#include "k4a/k4a.hpp"

// orbbec
#include <libobsensor/hpp/Filter.hpp>
#include <libobsensor/hpp/Frame.hpp>
#include <libobsensor/hpp/Pipeline.hpp>
#include <libobsensor/hpp/Device.hpp>
#include <libobsensor/hpp/Context.hpp>
#include <libobsensor/hpp/Utils.hpp>
#include <libobsensor/hpp/Sensor.hpp>
#include <libobsensor/hpp/Error.hpp>
#include <libobsensor/hpp/StreamProfile.hpp>

// opencv
#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>

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

struct OrbbecBaseDevice::Impl{

    // device
    DCType deviceType;
    std::unique_ptr<ob::Context> context        = nullptr;
    std::shared_ptr<ob::Device> device          = nullptr;
    std::vector<std::shared_ptr<ob::Device>> deviceList;
    std::shared_ptr<ob::SensorList> sensorList  = nullptr;
    std::unique_ptr<ob::Pipeline> pipe          = nullptr;

    OBCameraParam cameraParam;
    OBCalibrationParam calibrationParam;
    k4a::calibration k4aCalibration;
    k4a::transformation k4aTransformation;

    // frames data
    std::shared_ptr<ob::FrameSet> frameSet     = nullptr;
    std::shared_ptr<ob::ColorFrame> colorImage = nullptr;
    std::shared_ptr<ob::DepthFrame> depthImage = nullptr;
    std::shared_ptr<ob::IRFrame> infraredImage = nullptr;

    // processing data
    std::vector<std::int8_t> depthSizedColorData;
    std::vector<std::int8_t> cloudData;

    // std::vector<ColorRGBA8> resizedColorImageFromDepthTransformation;

    auto set_property_value(OBPropertyID pId, bool value) -> void;
    auto set_property_value(OBPropertyID pId, std::int32_t value) -> void;
    static auto k4a_convert_calibration(const DCModeInfos &mInfos, const OBCalibrationParam &calibrationParam) -> k4a::calibration;
    static auto k4a_convert_calibration(const DCModeInfos &mInfos, const OBCameraParam &cameraParam) -> k4a::calibration;
};

auto OrbbecBaseDevice::Impl::set_property_value(OBPropertyID pId, bool value) -> void{

    auto dev = device.get();
    if(!dev->isPropertySupported(pId, OB_PERMISSION_READ_WRITE)){
        return;
    }
    if(dev->getBoolProperty(pId) != value){
        dev->setBoolProperty(pId, value);
    }
}

auto OrbbecBaseDevice::Impl::set_property_value(OBPropertyID pId, int32_t value) -> void{

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

auto OrbbecBaseDevice::Impl::k4a_convert_calibration(const DCModeInfos &mInfos, const OBCameraParam &cameraParam) -> k4a::calibration{

    const auto &obDepthIntrinsics = cameraParam.depthIntrinsic;
    const auto &obDepthDistorsion = cameraParam.depthDistortion;

    const auto &obColorIntrinsics = cameraParam.rgbIntrinsic;
    const auto &obColorDistorsion = cameraParam.rgbDistortion;

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
    cColorCamCal.resolution_width   = static_cast<int>(mInfos.color_width());
    cColorCamCal.resolution_height  = static_cast<int>(mInfos.color_height());
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
    // std::copy(obDepthExtrinsics[OB_SENSOR_COLOR].rot,   obDepthExtrinsics[OB_SENSOR_COLOR].rot + 9,     k4Calibration.extrinsics[K4A_CALIBRATION_TYPE_DEPTH][K4A_CALIBRATION_TYPE_COLOR].rotation);
    // std::copy(obDepthExtrinsics[OB_SENSOR_COLOR].trans, obDepthExtrinsics[OB_SENSOR_COLOR].trans + 3,   k4Calibration.extrinsics[K4A_CALIBRATION_TYPE_DEPTH][K4A_CALIBRATION_TYPE_COLOR].translation);
    // std::copy(obColorExtrinsics[OB_SENSOR_DEPTH].rot,   obDepthExtrinsics[OB_SENSOR_DEPTH].rot + 9,     k4Calibration.extrinsics[K4A_CALIBRATION_TYPE_COLOR][K4A_CALIBRATION_TYPE_DEPTH].rotation);
    // std::copy(obColorExtrinsics[OB_SENSOR_DEPTH].trans, obDepthExtrinsics[OB_SENSOR_DEPTH].trans + 3,   k4Calibration.extrinsics[K4A_CALIBRATION_TYPE_COLOR][K4A_CALIBRATION_TYPE_DEPTH].translation);

    return k4Calibration;
}


auto OrbbecBaseDevice::Impl::k4a_convert_calibration(const DCModeInfos &mInfos, const OBCalibrationParam &calibrationParam) -> k4a::calibration{

    const auto &obDepthIntrinsics = calibrationParam.intrinsics[OB_SENSOR_DEPTH];
    const auto &obDepthDistorsion = calibrationParam.distortion[OB_SENSOR_DEPTH];
    const auto &obDepthExtrinsics = calibrationParam.extrinsics[OB_SENSOR_DEPTH];

    const auto &obColorIntrinsics = calibrationParam.intrinsics[OB_SENSOR_COLOR];
    const auto &obColorDistorsion = calibrationParam.distortion[OB_SENSOR_COLOR];
    const auto &obColorExtrinsics = calibrationParam.extrinsics[OB_SENSOR_COLOR];

    k4a::calibration k4Calibration;
    k4Calibration.depth_mode       = convert_to_k4a_depth_mode(mInfos.depth_resolution());
    // if(mInfos.color_resolution() == DCColorResolution::R960P){
    //     k4Calibration.color_resolution = convert_to_k4a_color_resolution(DCColorResolution::R1536P);
    // }else{
        k4Calibration.color_resolution = convert_to_k4a_color_resolution(mInfos.color_resolution());
    // }

    // depth calibration
    auto &cdepthCamCal              = k4Calibration.depth_camera_calibration;
    // # resolutions
    cdepthCamCal.resolution_width   = static_cast<int>(mInfos.depth_width());
    cdepthCamCal.resolution_height  = static_cast<int>(mInfos.depth_height());
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
    // if(mInfos.color_resolution() == DCColorResolution::R960P){
    //     cColorCamCal.resolution_width   = 2048;
    //     cColorCamCal.resolution_height  = 1536;
    // }else{
        cColorCamCal.resolution_width   = static_cast<int>(mInfos.color_width());
        cColorCamCal.resolution_height  = static_cast<int>(mInfos.color_height());
    // }

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
    cIntrincisP.codx                = 0.f; // UNUSED   /**< Center of distortion in Z=1 plane, x (only used for Rational6KT) */
    cIntrincisP.cody                = 0.f; // UNUSED   /**< Center of distortion in Z=1 plane, y (only used for Rational6KT) */
    cIntrincisP.p1                  = obColorDistorsion.p1; /**< Tangential distortion coefficient 2 */
    cIntrincisP.p2                  = obColorDistorsion.p2; /**< Tangential distortion coefficient 1 */
    cIntrincisP.metric_radius       = 0; /**< Metric radius */
    cColorCamCal.intrinsics.parameter_count = 15;
    // # extrinsics
    // ## rotation
    // auto &cExtRot = cColorCamCal.extrinsics.rotation;
    // cExtRot[0] = 1.f;
    // cExtRot[1] = 0.f;
    // cExtRot[2] = 0.f;
    // cExtRot[3] = 0.f;
    // cExtRot[4] = 1.f;
    // cExtRot[5] = 0.f;
    // cExtRot[6] = 0.f;
    // cExtRot[7] = 0.f;
    // cExtRot[8] = 1.f;
    // // ## translation
    // auto &cExtTr = cColorCamCal.extrinsics.translation;
    // cExtTr[0] = 0.f;
    // cExtTr[1] = 0.f;
    // cExtTr[2] = 0.f;
    std::copy(obDepthExtrinsics[OB_SENSOR_COLOR].trans, obDepthExtrinsics[OB_SENSOR_COLOR].trans + 3,   cColorCamCal.extrinsics.translation);
    std::copy(obDepthExtrinsics[OB_SENSOR_COLOR].rot,   obDepthExtrinsics[OB_SENSOR_COLOR].rot + 9,     cColorCamCal.extrinsics.rotation);


    // extrinsics
    std::copy(obDepthExtrinsics[OB_SENSOR_COLOR].rot,   obDepthExtrinsics[OB_SENSOR_COLOR].rot + 9,     k4Calibration.extrinsics[K4A_CALIBRATION_TYPE_DEPTH][K4A_CALIBRATION_TYPE_COLOR].rotation);
    std::copy(obDepthExtrinsics[OB_SENSOR_COLOR].trans, obDepthExtrinsics[OB_SENSOR_COLOR].trans + 3,   k4Calibration.extrinsics[K4A_CALIBRATION_TYPE_DEPTH][K4A_CALIBRATION_TYPE_COLOR].translation);
    std::copy(obColorExtrinsics[OB_SENSOR_DEPTH].rot,   obDepthExtrinsics[OB_SENSOR_DEPTH].rot + 9,     k4Calibration.extrinsics[K4A_CALIBRATION_TYPE_COLOR][K4A_CALIBRATION_TYPE_DEPTH].rotation);
    std::copy(obColorExtrinsics[OB_SENSOR_DEPTH].trans, obDepthExtrinsics[OB_SENSOR_DEPTH].trans + 3,   k4Calibration.extrinsics[K4A_CALIBRATION_TYPE_COLOR][K4A_CALIBRATION_TYPE_DEPTH].translation);

    std::copy(obDepthExtrinsics[OB_SENSOR_DEPTH].rot,   obDepthExtrinsics[OB_SENSOR_DEPTH].rot + 9,     k4Calibration.extrinsics[K4A_CALIBRATION_TYPE_DEPTH][K4A_CALIBRATION_TYPE_DEPTH].rotation);
    std::copy(obDepthExtrinsics[OB_SENSOR_DEPTH].trans, obDepthExtrinsics[OB_SENSOR_DEPTH].trans + 3,   k4Calibration.extrinsics[K4A_CALIBRATION_TYPE_DEPTH][K4A_CALIBRATION_TYPE_DEPTH].translation);
    std::copy(obColorExtrinsics[OB_SENSOR_COLOR].rot,   obColorExtrinsics[OB_SENSOR_COLOR].rot + 9,     k4Calibration.extrinsics[K4A_CALIBRATION_TYPE_COLOR][K4A_CALIBRATION_TYPE_COLOR].rotation);
    std::copy(obColorExtrinsics[OB_SENSOR_COLOR].trans, obColorExtrinsics[OB_SENSOR_COLOR].trans + 3,   k4Calibration.extrinsics[K4A_CALIBRATION_TYPE_COLOR][K4A_CALIBRATION_TYPE_COLOR].translation);


    return k4Calibration;
}

OrbbecBaseDevice::OrbbecBaseDevice() : i(std::make_unique<Impl>()){
    ob::Context::setLoggerSeverity(OB_LOG_SEVERITY_WARN);
    i->context = std::make_unique<ob::Context>();
}

OrbbecBaseDevice::~OrbbecBaseDevice(){
}

auto OrbbecBaseDevice::query_devices(std::string_view deviceTypeName, bool ethernet) -> void{

    i->context->enableNetDeviceEnumeration(ethernet);

    auto devicesFound = i->context->queryDeviceList();

    i->deviceList.clear();
    for(std::uint32_t idDev = 0; idDev < devicesFound->deviceCount(); ++idDev){
        auto dev = devicesFound->getDevice(idDev);
        if(dev->getDeviceInfo()->name() == deviceTypeName){
            i->deviceList.push_back(std::move(dev));
        }
    }
    Logger::message(std::format("[OrbbecDevice] [{}] devices found of type [{}].\n", i->deviceList.size(), deviceTypeName));
}

auto OrbbecBaseDevice::initialize(const DCModeInfos &mInfos, const DCColorSettings &colorS) -> void{


    i->frameSet      = nullptr;
    i->colorImage    = nullptr;
    i->depthImage    = nullptr;
    i->infraredImage = nullptr;

    if(mInfos.has_color() && dc_has_depth(mInfos.mode())){
        i->depthSizedColorData.resize(mInfos.depth_size() * 4);
    }else{
        i->depthSizedColorData.clear();
    }

    if(mInfos.has_depth()){
        i->cloudData.resize(mInfos.depth_size() * 3 * sizeof(int16_t));
    }else{
        i->cloudData.clear();
    }

    // theses colors settings must be set before starting the pipeline
    i->set_property_value(OB_PROP_COLOR_HDR_BOOL, colorS.hdr);
}


auto OrbbecBaseDevice::open_device(uint32_t deviceId) -> bool{

    if(deviceId >= i->deviceList.size()){
        Logger::error("[OrbbecDevice] Invalid id device.\n"sv);
        return false;
    }

    try {

        // retrieve device
        i->device     = i->deviceList[deviceId];
        i->sensorList = i->device->getSensorList();

        for(uint32_t idS = 0; idS < i->sensorList->count(); idS++) {
            auto sensor = i->sensorList->getSensor(idS);
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

        auto dInfos     = i->device->getDeviceInfo();
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

    }catch(const ob::Error &e) {
        Logger::error(std::format("[OrbbecDevice] Open error: {}\nsv", e.getMessage()));
        i->device = nullptr;
        return false;
    }catch(const std::exception &e){
        Logger::error(std::format("[OrbbecDevice] Error: {}\nsv", e.what()));
        i->device = nullptr;
        return false;
    }

    return true;
}

auto OrbbecBaseDevice::start_pipeline(const DCModeInfos &mInfos, const DCConfigSettings &configS) -> bool{

    i->deviceType = mInfos.device();

    std::shared_ptr<ob::Config> config = std::make_shared<ob::Config>();

    try {

        if(i->pipe != nullptr){
            i->pipe->stop();
            i->pipe = nullptr;
        }
        i->pipe = std::make_unique<ob::Pipeline>(i->device);

        bool hasColorSensor = i->sensorList->getSensor(OBSensorType::OB_SENSOR_COLOR) != nullptr;
        bool hasDepthSensor = i->sensorList->getSensor(OBSensorType::OB_SENSOR_DEPTH) != nullptr;
        bool hasInfraSensor = i->sensorList->getSensor(OBSensorType::OB_SENSOR_IR)    != nullptr;

        // retrieve color profile
        if(hasColorSensor && mInfos.has_color()){
            if(auto colorStreamProfileList = i->pipe->getStreamProfileList(OB_SENSOR_COLOR); colorStreamProfileList != nullptr){
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
                if(colorProfile != nullptr){
                    config->enableStream(colorProfile);
                }
            }
        }

        // retrieve depth profile
        if(hasDepthSensor && mInfos.has_depth()){
            if(auto depthStreamProfileList = i->pipe->getStreamProfileList(OB_SENSOR_DEPTH); depthStreamProfileList != nullptr){
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

                if(depthProfile != nullptr){
                    config->enableStream(depthProfile);
                }
            }
        }

        // retrieve infrared profile
        if(hasInfraSensor && mInfos.has_infra()){
            if(auto infraStreamProfileList = i->pipe->getStreamProfileList(OB_SENSOR_IR); infraStreamProfileList != nullptr){
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

                if(infraProfile != nullptr){
                    config->enableStream(infraProfile);
                }
            }
        }

        // // accel
        // if(i->sensorList->getSensor(OBSensorType::OB_SENSOR_ACCEL)){
        //     if(auto accelStreamProfileList = i->pipe->getStreamProfileList(OB_SENSOR_ACCEL); accelStreamProfileList != nullptr){
        //         OBAccelFullScaleRange fullScaleRange    = OB_ACCEL_FS_2g;
        //         OBAccelSampleRate sampleRate            = OB_SAMPLE_RATE_50_HZ;
        //         auto accelProfile = accelStreamProfileList->getAccelStreamProfile(fullScaleRange, sampleRate);
        //         config->enableStream(accelProfile);
        //     }
        // }

        // // gyro
        // if(i->sensorList->getSensor(OBSensorType::OB_SENSOR_GYRO)){
        //     if(auto gyroStreamProfileList = i->pipe->getStreamProfileList(OB_SENSOR_GYRO); gyroStreamProfileList != nullptr){
        //         OBGyroFullScaleRange fullScaleRange = OB_GYRO_FS_250dps;
        //         OBGyroSampleRate sampleRate         = OB_SAMPLE_RATE_50_HZ;
        //         auto gyraProfile = gyroStreamProfileList->getGyroStreamProfile(fullScaleRange, sampleRate);
        //         config->enableStream(gyraProfile);
        //     }
        // }

        // Update the configuration items of the configuration file, and keep the original configuration for other items
        auto currSynchConfig = i->device->getMultiDeviceSyncConfig();

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
            i->pipe->enableFrameSync();
        }else{
            i->pipe->disableFrameSync();
        }

        // other
        i->set_property_value(OB_PROP_INDICATOR_LIGHT_BOOL, !configS.disableLED);

        // start pipe with current config
        i->pipe->start(config);

        // get camera intrinsic and extrinsic parameters form pipeline and set to point cloud filter
        i->cameraParam         = i->pipe->getCameraParam();
        // get calibration parameters
        i->calibrationParam    = i->pipe->getCalibrationParam(config);

        i->k4aCalibration      = i->k4a_convert_calibration(mInfos, i->calibrationParam);

    }catch(ob::Error &e) {
        Logger::error(std::format("[OrbbecDevice::start_reading] Start reading error: {}\n", e.getMessage()));
        i->device = nullptr;
        return false;
    }catch(const std::exception &e){
        Logger::error(std::format("[OrbbecDevice::start_reading] Error: {}\nsv", e.what()));
        i->device = nullptr;
        return false;
    }





    display_calibration(i->k4aCalibration);

    // auto &cccI = i->k4aCalibration.color_camera_calibration.intrinsics.parameters;
    // std::cout << "cci -> "
    //     << "cx: " <<cccI.param.cx << " "
    //     << "cy: " <<cccI.param.cy << " "
    //     << "fx: " <<cccI.param.fx << " "
    //     << "fy: " <<cccI.param.fy << " "
    //     << "k1: " <<cccI.param.k1 << " "
    //     << "k2: " <<cccI.param.k2 << " "
    //     << "k3: " <<cccI.param.k3 << " "
    //     << "k4: " <<cccI.param.k4 << " "
    //     << "k5: " <<cccI.param.k5 << " "
    //     << "k6: " <<cccI.param.k6 << " "
    //     << "codx: " <<cccI.param.codx << " "
    //     << "cody: " <<cccI.param.cody << " "
    //     << "p1: " <<cccI.param.p1 << " "
    //     << "p2: " <<cccI.param.p2 << " "
    //     << "metric_radius: " <<cccI.param.metric_radius << "\n";

    // auto &dccI = i->k4aCalibration.depth_camera_calibration.intrinsics.parameters;
    // std::cout << "dccI -> "
    //     << "cx: " <<dccI.param.cx << " "
    //     << "cy: " <<dccI.param.cy << " "
    //     << "fx: " <<dccI.param.fx << " "
    //     << "fy: " <<dccI.param.fy << " "
    //     << "k1: " <<dccI.param.k1 << " "
    //     << "k2: " <<dccI.param.k2 << " "
    //     << "k3: " <<dccI.param.k3 << " "
    //     << "k4: " <<dccI.param.k4 << " "
    //     << "k5: " <<dccI.param.k5 << " "
    //     << "k6: " <<dccI.param.k6 << " "
    //     << "codx: " <<dccI.param.codx << " "
    //     << "cody: " <<dccI.param.cody << " "
    //     << "p1: " <<dccI.param.p1 << " "
    //     << "p2: " <<dccI.param.p2 << " "
    //     << "metric_radius: " <<dccI.param.metric_radius << "\n";

    // std::cout << "COLOR RES: " <<(int)i->k4aCalibration.color_resolution << "\n";
    // std::cout << "DEPTH MODE: " <<(int)i->k4aCalibration.depth_mode << "\n";
    // std::cout << "COL HEIGHT " << i->k4aCalibration.color_camera_calibration.resolution_height<< "\n";
    // std::cout << "COL WIDTH " << i->k4aCalibration.color_camera_calibration.resolution_width<< "\n";
    // std::cout << "COL MRAD " << i->k4aCalibration.color_camera_calibration.metric_radius<< "\n";

    // std::cout << "DEP HEIGHT " << i->k4aCalibration.depth_camera_calibration.resolution_height<< "\n";
    // std::cout << "DEP WIDTH " << i->k4aCalibration.depth_camera_calibration.resolution_width<< "\n";
    // std::cout << "DEP MRAD " << i->k4aCalibration.depth_camera_calibration.metric_radius<< "\n";

    // auto &oci = i->calibrationParam.intrinsics[OB_SENSOR_COLOR];
    // std::cout << "ob color intrinsics cx: " << oci.cx << " cy: " << oci.cy << " fx: " << oci.fx << " fy: " << oci.fy << " h: " << oci.height << " w: " << oci.width << "\n";
    // auto &ocd = i->calibrationParam.intrinsics[OB_SENSOR_DEPTH];
    // std::cout << "ob depth intrinsics cx: " << ocd.cx << " cy: " << ocd.cy << " fx: " << ocd.fx << " fy: " << ocd.fy << " h: " << ocd.height << " w: " << ocd.width << "\n";

    // auto &v3 = i->calibrationParam.distortion[OB_SENSOR_COLOR];
    // std::cout << "ob color distorsions k1 : " << v3.k1 << " k2: " << v3.k2 << " k3: " << v3.k3 << " k4: " << v3.k4 << " k5 " << v3.k5 << " k6 " << v3.k6 << " p1 " << v3.p1 <<" p2 " << v3.p2 << "\n";

    // auto &v4 = i->calibrationParam.distortion[OB_SENSOR_DEPTH];
    // std::cout << "ob depth distorsions k1 : " << v4.k1 << " k2: " << v4.k2 << " k3: " << v4.k3 << " k4: " << v4.k4 << " k5 " << v4.k5 << " k6 " << v4.k6 << " p1 " << v4.p1 <<" p2 " << v4.p2 << "\n";


    i->k4aTransformation = k4a::transformation(i->k4aCalibration);

    std::cout << "end start_pipeline\n";

    return true;
}

auto OrbbecBaseDevice::close_device() -> void{

    if(i->pipe != nullptr){
        i->pipe->stop();
        i->pipe = nullptr;
    }

    i->sensorList = nullptr;
    i->device     = nullptr;
}

auto OrbbecBaseDevice::update_from_colors_settings(const DCColorSettings &colorS) -> void{

    if(!is_opened()){
        return;
    }

    try{
        i->set_property_value(OB_PROP_COLOR_AUTO_EXPOSURE_BOOL,           colorS.autoExposureTime);
        i->set_property_value(OB_PROP_COLOR_AUTO_WHITE_BALANCE_BOOL,      colorS.autoWhiteBalance);
        i->set_property_value(OB_PROP_COLOR_EXPOSURE_INT,                 colorS.exposureTime);
        i->set_property_value(OB_PROP_COLOR_WHITE_BALANCE_INT,            colorS.whiteBalance);
        i->set_property_value(OB_PROP_COLOR_BRIGHTNESS_INT,               colorS.brightness);
        i->set_property_value(OB_PROP_COLOR_CONTRAST_INT,                 colorS.contrast);
        i->set_property_value(OB_PROP_COLOR_SHARPNESS_INT,                colorS.sharpness);
        i->set_property_value(OB_PROP_COLOR_SATURATION_INT,               colorS.saturation);
        i->set_property_value(OB_PROP_COLOR_BACKLIGHT_COMPENSATION_INT,   colorS.backlightCompensation ? 1 : 0); // NOT AVAILABLE
        i->set_property_value(OB_PROP_COLOR_GAIN_INT,                     colorS.gain);
        i->set_property_value(OB_PROP_COLOR_POWER_LINE_FREQUENCY_INT,     static_cast<int>(colorS.powerlineFrequency));

    }catch(ob::Error &e) {
        Logger::error(std::format("[OrbbecDevice] Error: {}\n", e.getMessage()));
    }
}

auto OrbbecBaseDevice::is_opened() const noexcept -> bool {
    return i->device != nullptr;
}

auto OrbbecBaseDevice::nb_devices() const noexcept -> size_t {
    return i->deviceList.size();
}

auto OrbbecBaseDevice::device_name() const noexcept -> std::string {
    if(is_opened()){
        return i->device->getDeviceInfo()->name();
    }
    return "unknow_device"s;
}

auto OrbbecBaseDevice::capture_frame(int32_t timeoutMs) -> bool{

    if(i->pipe != nullptr){
        Bench::start("[OrbbecDevice::capture_frame]");
        try{
            i->frameSet = i->pipe->waitForFrames(timeoutMs);
        }catch(ob::Error &e) {
            Logger::error(std::format("[OrbbecDevice] Get capture error: {}\n", e.getMessage()));
        }

        Bench::stop();
        return i->frameSet != nullptr;
    }
    return false;
}

auto OrbbecBaseDevice::read_calibration() -> BinarySpan{
    return BinarySpan{
        reinterpret_cast<std::byte*>(&i->k4aCalibration),
        static_cast<size_t>(sizeof(k4a_calibration_t))
    };
}

auto OrbbecBaseDevice::read_color_image() -> BinarySpan{

    if(i->frameSet != nullptr){

        Bench::start("[OrbbecDevice::read_color_image]");
        i->colorImage = i->frameSet->colorFrame();
        Bench::stop();

        if(i->colorImage != nullptr){
            return BinarySpan{
                reinterpret_cast<std::byte*>(i->colorImage->data()),
                i->colorImage->width()*i->colorImage->height()*4
            };
        }
    }
    return {};
}

auto OrbbecBaseDevice::read_depth_image() -> std::span<std::uint16_t>{

    if(i->frameSet != nullptr){

        Bench::start("[OrbbecDevice::read_depth_image]");
        i->depthImage = i->frameSet->depthFrame();
        Bench::stop();

        if(i->depthImage != nullptr){
            return  std::span<std::uint16_t>{
                reinterpret_cast<std::uint16_t*>(i->depthImage->data()),
                i->depthImage->width()*i->depthImage->height()
            };
        }
    }
    return {};
}

auto OrbbecBaseDevice::read_infra_image() -> std::span<std::uint16_t>{

    if(i->frameSet != nullptr){

        Bench::start("[OrbbecDevice::read_infra_image]");
        i->infraredImage = i->frameSet->irFrame();
        Bench::stop();

        if(i->infraredImage != nullptr){
            return  std::span<std::uint16_t>{
                reinterpret_cast<std::uint16_t*>(i->infraredImage->data()),
                i->infraredImage->width()*i->infraredImage->height()
            };
        }
    }

    return {};
}

auto OrbbecBaseDevice::read_bodies() -> std::tuple<std::span<uint8_t>, std::span<DCBody> >{

    // k4aBtConfig.gpu_device_id       = settings.config.btGPUId;
    // k4aBtConfig.processing_mode     = static_cast<k4abt_tracker_processing_mode_t>(settings.config.btProcessingMode);
    // k4aBtConfig.sensor_orientation  = static_cast<k4abt_sensor_orientation_t>(settings.config.btOrientation);
    // k4aBtConfig.model_path          = nullptr;
    // if(!depthImage || ! infraredImage){
    //     return;
    // }

    // if((depth_resolution(settings.config.mode) != DCDepthResolution::K4_OFF) && settings.config.enableBodyTracking && (bodyTracker != nullptr)){


    //     try{

    //         k4a_capture_t captureH = nullptr;
    //         k4a_capture_create(&captureH);
    //         k4a::capture capture(captureH);

    //         // std::cout << "create d image\n";
    //         // std::cout << depthImage->width() << " " << depthImage->height() << " "<< depthImage->width()*depthImage->height() << " " << depthImage->dataSize() << "\n";
    //         k4a::image k4DepthImage = k4a::image::create_from_buffer(
    //             K4A_IMAGE_FORMAT_DEPTH16,
    //             depthImage->width(),
    //             depthImage->height(),
    //             depthImage->width()*sizeof(std::uint16_t),
    //             reinterpret_cast<std::uint8_t*>(depthImage.get()->data()),
    //             depthImage->dataSize(),
    //             nullptr,
    //             nullptr
    //         );

    //         // std::cout << "create ir image\n";
    //         // std::cout << infraredImage->width() << " " << infraredImage->height() << " "<< infraredImage->width()*infraredImage->height() << " " << infraredImage->dataSize() << "\n";
    //         k4a::image k4IRImage = k4a::image::create_from_buffer(
    //             K4A_IMAGE_FORMAT_DEPTH16,
    //             infraredImage->width(),
    //             infraredImage->height(),
    //             infraredImage->width()*sizeof(std::uint16_t),
    //             reinterpret_cast<std::uint8_t*>(infraredImage.get()->data()),
    //             infraredImage->dataSize(),
    //             nullptr,
    //             nullptr
    //         );

    //         // std::cout << "set depth image\n";
    //         capture.set_depth_image(k4DepthImage);

    //         // std::cout << "set ir image\n";
    //         capture.set_ir_image(k4IRImage);


    //         // std::cout << "enqueue capture\n";
    //         if(bodyTracker->enqueue_capture(capture, std::chrono::milliseconds(1))){
    //             if(k4abt::frame bodyFrame = bodyTracker->pop_result(std::chrono::milliseconds(1)); bodyFrame != nullptr){
    //                 auto bodiesCount = bodyFrame.get_num_bodies();
    //                 if(data.bodies.size() < bodiesCount){
    //                     data.bodies.resize(bodiesCount);
    //                 }
    //                 // std::cout << "bodiesCount " << bodiesCount << "\n";
    //                 for(size_t ii = 0; ii < bodiesCount; ++ii){
    //                     update_body(data.bodies[ii], bodyFrame.get_body(static_cast<int>(ii)));
    //                 }
    //                 timing.bodiesTS = bodyFrame.get_system_timestamp();
    //                 bodiesIndexImage = bodyFrame.get_body_index_map();
    //             }
    //         }

    //     }  catch (k4a::error error) {
    //         Logger::error("[FemtoOrbbecDeviceImpl::read_bodies] error: {}\n", error.what());
    //     }  catch (std::runtime_error error) {
    //         Logger::error("[FemtoOrbbecDeviceImpl::read_bodies] error: {}\n", error.what());
    //     }

    // }

    return {};
}

auto OrbbecBaseDevice::read_from_imu() -> tool::BinarySpan{
    return {};
}

auto OrbbecBaseDevice::resize_color_image_to_depth_size(const DCModeInfos &mInfos, std::span<ColorRGBA8> colorData, std::span<uint16_t> depthData) -> std::span<ColorRGBA8>{
    
    size_t colorWidth;
    size_t colorHeight;
    size_t colorStride;
    size_t colorSizeBytes;
    std::uint8_t* colorDataBuffer;

    // if(mInfos.color_resolution() == DCColorResolution::R960P){

    //     int newWidth = 2048;
    //     int newHeight = 1536;
    //     int newColorStride = newWidth *4 * sizeof(std::uint8_t);

    //     if(i->resizedColorImageFromDepthTransformation.size() != newWidth*newHeight){
    //         i->resizedColorImageFromDepthTransformation.resize(newWidth*newHeight);
    //     }

    //     cv::Mat output(newHeight, newWidth, CV_8UC4, i->resizedColorImageFromDepthTransformation.data());
    //     cv::Mat input(mInfos.color_height(), mInfos.color_width(), CV_8UC4, colorData.data());
    //     cv::resize(input, output, output.size(),0.0,0.0, cv::InterpolationFlags::INTER_NEAREST);

    //     cv::imwrite("D:/input.png", input);
    //     cv::imwrite("D:/output.png", output);


    //     colorWidth      = newWidth;
    //     colorHeight     = newHeight;
    //     colorStride     = newColorStride;
    //     colorSizeBytes  = colorStride * colorHeight;
    //     colorDataBuffer = reinterpret_cast<std::uint8_t*>(output.data);

    // }else{

    // }
    colorWidth      = mInfos.color_width();
    colorHeight     = mInfos.color_height();
    colorStride     = colorWidth * 4 * sizeof(std::uint8_t);
    colorSizeBytes  = colorStride * colorHeight;
    colorDataBuffer = reinterpret_cast<std::uint8_t*>(colorData.data());

    auto depthStride     = mInfos.depth_width() * 1 * sizeof(std::uint16_t);
    auto depthSizeBytes  = depthStride * mInfos.depth_height();
    auto depthDataBuffer = reinterpret_cast<std::uint8_t*>(depthData.data());

    auto depthSizedColorStride    = mInfos.depth_width() * 4 * sizeof(std::uint8_t);
    auto depthSizedColorSizeBytes = depthSizedColorStride * mInfos.depth_height();
    auto depthSizedColorDataBuffer= reinterpret_cast<std::uint8_t*>(i->depthSizedColorData.data());

    auto k4aColorImage = k4a::image::create_from_buffer(
        K4A_IMAGE_FORMAT_COLOR_BGRA32,
        static_cast<int>(colorWidth), static_cast<int>(colorHeight), static_cast<int>(colorStride), colorDataBuffer, colorSizeBytes,
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

        i->k4aTransformation.color_image_to_depth_camera(
            k4aDepthImage,
            k4aColorImage,
            &k4aDepthSizedColorImage
        );

        // cv::Mat output2(static_cast<int>(mInfos.depth_height()), static_cast<int>(mInfos.depth_width()), CV_8UC4, depthSizedColorDataBuffer);
        // cv::imwrite("D:/output2.png", output2);

        // std::int64_t count1=0;
        // std::int64_t count2=0;
        // std::int64_t count3=0;
        // for(const auto &d : depthData){
        //     count1 += d;
        // }
        // for(const auto &d : colorData){
        //     count2 += d.r();
        // }
        // for(const auto &d : i->depthSizedColorData){
        //     count3 += d;
        // }
        // std::cout << "resize: " << count1 << " " << count2 << " "<< count3 << " " << 1.f*count1/depthData.size() << " " << 1.f*count2/colorData.size() << " " << 1.f*count3/i->depthSizedColorData.size() << "\n";

        Bench::stop();

    }catch(const std::runtime_error &error){
        Logger::error(std::format("[OrbbecBaseDevice::k4a_resize_color_image_to_depth_size] Runtime error: {}", error.what()));
        return {};
    }

    return std::span<tool::ColorRGBA8>{
        reinterpret_cast<tool::ColorRGBA8*>(i->depthSizedColorData.data()),
        static_cast<size_t>(mInfos.depth_width() * mInfos.depth_height())
    };
}

auto OrbbecBaseDevice::generate_cloud(const DCModeInfos &mInfos, std::span<uint16_t> depthData) -> std::span<geo::Pt3<std::int16_t>> {

    auto depthStride     = mInfos.depth_width() * 1 * sizeof(std::uint16_t);
    auto depthSizeBytes  = depthStride * mInfos.depth_height();
    auto depthDataBuffer = reinterpret_cast<std::uint8_t*>(depthData.data());

    auto cloudStride     = mInfos.depth_width() * 3 * sizeof(std::uint16_t);
    auto cloudSizeBytes  = cloudStride * mInfos.depth_height();
    auto cloudDataBuffer = reinterpret_cast<std::uint8_t*>(i->cloudData.data());

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

        i->k4aTransformation.depth_image_to_point_cloud(
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
        reinterpret_cast<geo::Pt3<std::int16_t>*>(i->cloudData.data()),
        mInfos.depth_width() * mInfos.depth_height()
    };
}

