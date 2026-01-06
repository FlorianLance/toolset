

/*******************************************************************************
** Toolset-ts-depth-camera                                                    **
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

#include "femto_base_device.hpp"

// k4a
#include <k4a/k4a.hpp>

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
#include "azure_utility.hpp"


// debug
#include <stb/stb_image_write.h>

using namespace  std::string_literals;
using namespace tool;
using namespace tool::cam;
using namespace tool::img;

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

struct OrbbecDeviceManager{

    ob::Context context;

    OrbbecDeviceManager(){
        tool::LogG lg("[OrbbecDeviceManager::OrbbecDeviceManager]");
        try{
            context.setLoggerSeverity(OB_LOG_SEVERITY_ERROR);
            context.enableNetDeviceEnumeration(false);
            devicesList = context.queryDeviceList();
            validity.resize(devicesList->deviceCount());
            std::fill(validity.begin(), validity.end(), false);

        }catch(const ob::Error &e) {
            tool::Log::error(std::format("Orbbec query device list error: [{}]\n"sv, e.getMessage()));
        }catch(const std::exception &e){
            tool::Log::error(std::format("Error: [{}]\n"sv, e.what()));
        }
        
        tool::Log::message(std::format("Devices founds: [{}]\n", devicesList->deviceCount()));
        for(size_t idD = 0; idD < devicesList->deviceCount(); ++idD){
            try{
                auto dev  = devicesList->getDevice(static_cast<int>(idD));
                auto info = dev->getDeviceInfo();

                std::string deviceName = info->name();
                std::string connectionT = info->connectionType();
                validity[idD] = true;

                if(deviceName.contains("Femto Bolt")){
                    femtoBoltDevices.push_back(dev);
                    Log::message(std::format("[{}] Femto Bolt:\n\tSerial number: [{}]\n\tPID: [{}]\n\tConnection: [{}] \n", idD, info->serialNumber(), info->pid(), connectionT));
                    if(!connectionT.contains("USB3")){
                        Log::warning("Connection is not USB3, bandwitch will be too low!\n");
                    }
                }else if(deviceName.contains("Femto Mega")){
                    if(connectionT.contains("Ethernet")){
                        femtoMegaEthernetDevices.push_back(dev);
                        Log::message(std::format("[{}] Femto Mega:\n\tSerial number: [{}]\n\tPID: [{}]\n\tConnection: [{}] \n\tIP Address: [{}]\n",
                            idD, info->serialNumber(), info->pid(), connectionT, info->ipAddress()));

                    }else if(connectionT.contains("USB")){
                        femtoMegaUSBDevices.push_back(dev);
                        Log::message(std::format("[{}] Femto Mega:\n\tSerial number: [{}]\n\tPID: [{}]\n\tConnection: [{}]\n",
                            idD, info->serialNumber(), info->pid(), connectionT));
                        if(!connectionT.contains("USB3")){
                            Log::warning("Connection is not USB3, bandwitch will be too low!\n");
                        }
                    }
                }else if(deviceName.contains("Gemini 215")){
                    gemini215Devices.push_back(dev);
                    Log::message(std::format("[{}] Gemini 215:\n\tSerial number: [{}]\n\tPID: [{}]\n\tConnection: [{}] \n", idD, info->serialNumber(), info->pid(), connectionT));
                    if(!connectionT.contains("USB3")){
                        Log::warning("Connection is not USB3, bandwitch will be too low!\n");
                    }
                }else{
                    Log::fmessage("DEVICE UNKNOW: {} {}\n", deviceName, connectionT);
                }

            }catch(const ob::Error &) {
                Log::message(std::format("[{}] Unavailable device, may be used in another program\n",idD));
            }
        }
    }

    ~OrbbecDeviceManager(){
        tool::LogG lg("[OrbbecDeviceManager::~OrbbecDeviceManager]");
    }

    std::vector<std::shared_ptr<ob::Device>> femtoBoltDevices;
    std::vector<std::shared_ptr<ob::Device>> femtoMegaEthernetDevices;
    std::vector<std::shared_ptr<ob::Device>> femtoMegaUSBDevices;
    std::vector<std::shared_ptr<ob::Device>> gemini215Devices;


private:

    std::shared_ptr<ob::DeviceList> devicesList = nullptr;
    std::vector<bool> validity;

};

struct FemtoBaseDevice::Impl{

    static inline std::unique_ptr<OrbbecDeviceManager> devicesM = nullptr;

    // device
    DCType deviceType;

    std::shared_ptr<ob::Device> device          = nullptr;
    std::shared_ptr<ob::SensorList> sensorList  = nullptr;
    std::unique_ptr<ob::Pipeline> pipe          = nullptr;

    OBCameraParam cameraParam;
    OBCalibrationParam calibrationParam;
    k4a::calibration k4aCalibration;
    std::unique_ptr<k4a::transformation> k4aTransformation = nullptr;

    // body tracking
    k4abt_tracker_configuration_t k4aBtConfig = K4ABT_TRACKER_CONFIG_DEFAULT;
    std::unique_ptr<k4abt::tracker> bodyTracker = nullptr;
    std::vector<DCBody> bodies;
    Buffer<std::uint8_t> bodiesIndex;


    k4a::capture bodyCapture = nullptr;

    // profiles
    std::shared_ptr<ob::StreamProfile> colorProfile = nullptr;
    std::shared_ptr<ob::StreamProfile> depthProfile = nullptr;
    std::shared_ptr<ob::StreamProfile> infraProfile = nullptr;

    // frames data
    std::shared_ptr<ob::FrameSet> frameSet     = nullptr;
    std::shared_ptr<ob::ColorFrame> colorImage = nullptr;
    std::shared_ptr<ob::DepthFrame> depthImage = nullptr;
    std::shared_ptr<ob::IRFrame> infraredImage = nullptr;


    // processing data
    std::vector<std::int8_t> depthSizedColorData;
    std::vector<std::int8_t> cloudData;

    auto set_property_value(OBPropertyID pId, bool value) -> void;
    auto set_property_value(OBPropertyID pId, std::int32_t value) -> void;
    static auto k4a_convert_calibration(const DCModeInfos &mInfos, const OBCalibrationParam &calibrationParam) -> k4a::calibration;
    static auto k4a_convert_calibration(const DCModeInfos &mInfos, const OBCameraParam &cameraParam) -> k4a::calibration;
    static auto update_k4_body(DCBody &body, const k4abt_body_t &k4aBody) -> void{
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
};

auto FemtoBaseDevice::Impl::set_property_value(OBPropertyID pId, bool value) -> void{

    auto dev = device.get();
    if(!dev->isPropertySupported(pId, OB_PERMISSION_READ_WRITE)){
        return;
    }
    if(dev->getBoolProperty(pId) != value){
        dev->setBoolProperty(pId, value);
    }
}

auto FemtoBaseDevice::Impl::set_property_value(OBPropertyID pId, int32_t value) -> void{

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

auto FemtoBaseDevice::Impl::k4a_convert_calibration(const DCModeInfos &mInfos, const OBCameraParam &cameraParam) -> k4a::calibration{
    
    auto lg = LogG("OrbbecBaseDevice::Impl::k4a_convert_calibration"sv);

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


auto FemtoBaseDevice::Impl::k4a_convert_calibration(const DCModeInfos &mInfos, const OBCalibrationParam &calibrationParam) -> k4a::calibration{

    k4a_calibration_camera_t depth_camera_calibration; /**< Depth camera calibration. */
    // k4a_calibration_camera_t color_camera_calibration; /**< Color camera calibration. */
    // k4a_calibration_extrinsics_t extrinsics[K4A_CALIBRATION_TYPE_NUM][K4A_CALIBRATION_TYPE_NUM];
    // k4a_depth_mode_t depth_mode;             /**< Depth camera mode for which calibration was obtained. */
    // k4a_color_resolution_t color_resolution; /**< Color camera resolution for which calibration was obtained. */
    
    auto lg = LogG("OrbbecBaseDevice::Impl::k4a_convert_calibration"sv);

    const auto &obDepthIntrinsics = calibrationParam.intrinsics[OB_SENSOR_DEPTH];
    const auto &obDepthDistorsion = calibrationParam.distortion[OB_SENSOR_DEPTH];
    const auto &obDepthExtrinsics = calibrationParam.extrinsics[OB_SENSOR_DEPTH];

    const auto &obColorIntrinsics = calibrationParam.intrinsics[OB_SENSOR_COLOR];
    const auto &obColorDistorsion = calibrationParam.distortion[OB_SENSOR_COLOR];
    const auto &obColorExtrinsics = calibrationParam.extrinsics[OB_SENSOR_COLOR];

    k4a::calibration k4C;
    k4C.depth_mode                                                          = convert_to_k4a_depth_mode(mInfos.depth_resolution());
    k4C.color_resolution                                                    = convert_to_k4a_color_resolution(mInfos.color_resolution());

    // depth calibration
    // # resolutions
    k4C.depth_camera_calibration.resolution_width                           = static_cast<int>(mInfos.depth_width());
    k4C.depth_camera_calibration.resolution_height                          = static_cast<int>(mInfos.depth_height());
    // # metric_radius
    k4C.depth_camera_calibration.metric_radius                              = 1.7399998f;
    // # intrinsics
    // ## type
    k4C.depth_camera_calibration.intrinsics.type                            = K4A_CALIBRATION_LENS_DISTORTION_MODEL_BROWN_CONRADY;
    // ## parameters
    k4C.depth_camera_calibration.intrinsics.parameters.param.cx             = obDepthIntrinsics.cx; /**< Principal point in image, x */
    k4C.depth_camera_calibration.intrinsics.parameters.param.cy             = obDepthIntrinsics.cy; /**< Principal point in image, y */
    k4C.depth_camera_calibration.intrinsics.parameters.param.fx             = obDepthIntrinsics.fx; /**< Focal length x */
    k4C.depth_camera_calibration.intrinsics.parameters.param.fy             = obDepthIntrinsics.fy; /**< Focal length y */
    k4C.depth_camera_calibration.intrinsics.parameters.param.k1             = obDepthDistorsion.k1; /**< k1 radial distortion coefficient */
    k4C.depth_camera_calibration.intrinsics.parameters.param.k2             = obDepthDistorsion.k2; /**< k2 radial distortion coefficient */
    k4C.depth_camera_calibration.intrinsics.parameters.param.k3             = obDepthDistorsion.k3; /**< k3 radial distortion coefficient */
    k4C.depth_camera_calibration.intrinsics.parameters.param.k4             = obDepthDistorsion.k4; /**< k4 radial distortion coefficient */
    k4C.depth_camera_calibration.intrinsics.parameters.param.k5             = obDepthDistorsion.k5; /**< k5 radial distortion coefficient */
    k4C.depth_camera_calibration.intrinsics.parameters.param.k6             = obDepthDistorsion.k6; /**< k6 radial distortion coefficient */
    k4C.depth_camera_calibration.intrinsics.parameters.param.codx           = 0; // UNUSED          /**< Center of distortion in Z=1 plane, x (only used for Rational6KT) */
    k4C.depth_camera_calibration.intrinsics.parameters.param.cody           = 0; // UNUSED          /**< Center of distortion in Z=1 plane, y (only used for Rational6KT) */
    k4C.depth_camera_calibration.intrinsics.parameters.param.p1             = obDepthDistorsion.p1; /**< Tangential distortion coefficient 2 */
    k4C.depth_camera_calibration.intrinsics.parameters.param.p2             = obDepthDistorsion.p2; /**< Tangential distortion coefficient 1 */
    k4C.depth_camera_calibration.intrinsics.parameters.param.metric_radius  = 0;                    /**< Metric radius */
    k4C.depth_camera_calibration.intrinsics.parameter_count = 15;
    // # extrinsics
    // ## rotation
    k4C.depth_camera_calibration.extrinsics.rotation[0] = 1.f;
    k4C.depth_camera_calibration.extrinsics.rotation[1] = 0.f;
    k4C.depth_camera_calibration.extrinsics.rotation[2] = 0.f;
    k4C.depth_camera_calibration.extrinsics.rotation[3] = 0.f;
    k4C.depth_camera_calibration.extrinsics.rotation[4] = 1.f;
    k4C.depth_camera_calibration.extrinsics.rotation[5] = 0.f;
    k4C.depth_camera_calibration.extrinsics.rotation[6] = 0.f;
    k4C.depth_camera_calibration.extrinsics.rotation[7] = 0.f;
    k4C.depth_camera_calibration.extrinsics.rotation[8] = 1.f;
    // ## translation
    k4C.depth_camera_calibration.extrinsics.translation[0] = 0.f;
    k4C.depth_camera_calibration.extrinsics.translation[1] = 0.f;
    k4C.depth_camera_calibration.extrinsics.translation[2] = 0.f;

    // color calibration
    k4C.color_camera_calibration.resolution_width                           = static_cast<int>(mInfos.color_width());
    k4C.color_camera_calibration.resolution_height                          = static_cast<int>(mInfos.color_height());
    // # metric_radius
    k4C.color_camera_calibration.metric_radius                              = 1.7f;
    // # intrinsics
    // ## type
    k4C.color_camera_calibration.intrinsics.type                            = K4A_CALIBRATION_LENS_DISTORTION_MODEL_BROWN_CONRADY;
    // ## parameters
    k4C.color_camera_calibration.intrinsics.parameters.param.cx             = obColorIntrinsics.cx; /**< Principal point in image, x */
    k4C.color_camera_calibration.intrinsics.parameters.param.cy             = obColorIntrinsics.cy; /**< Principal point in image, y */
    k4C.color_camera_calibration.intrinsics.parameters.param.fx             = obColorIntrinsics.fx; /**< Focal length x */
    k4C.color_camera_calibration.intrinsics.parameters.param.fy             = obColorIntrinsics.fy; /**< Focal length y */
    k4C.color_camera_calibration.intrinsics.parameters.param.k1             = obColorDistorsion.k1; /**< k1 radial distortion coefficient */
    k4C.color_camera_calibration.intrinsics.parameters.param.k2             = obColorDistorsion.k2; /**< k2 radial distortion coefficient */
    k4C.color_camera_calibration.intrinsics.parameters.param.k3             = obColorDistorsion.k3; /**< k3 radial distortion coefficient */
    k4C.color_camera_calibration.intrinsics.parameters.param.k4             = obColorDistorsion.k4; /**< k4 radial distortion coefficient */
    k4C.color_camera_calibration.intrinsics.parameters.param.k5             = obColorDistorsion.k5; /**< k5 radial distortion coefficient */
    k4C.color_camera_calibration.intrinsics.parameters.param.k6             = obColorDistorsion.k6; /**< k6 radial distortion coefficient */
    k4C.color_camera_calibration.intrinsics.parameters.param.codx           = 0.f; // UNUSED   /**< Center of distortion in Z=1 plane, x (only used for Rational6KT) */
    k4C.color_camera_calibration.intrinsics.parameters.param.cody           = 0.f; // UNUSED   /**< Center of distortion in Z=1 plane, y (only used for Rational6KT) */
    k4C.color_camera_calibration.intrinsics.parameters.param.p1             = obColorDistorsion.p1; /**< Tangential distortion coefficient 2 */
    k4C.color_camera_calibration.intrinsics.parameters.param.p2             = obColorDistorsion.p2; /**< Tangential distortion coefficient 1 */
    k4C.color_camera_calibration.intrinsics.parameters.param.metric_radius  = 0; /**< Metric radius */
    k4C.color_camera_calibration.intrinsics.parameter_count = 15;
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
    std::copy(obDepthExtrinsics[OB_SENSOR_COLOR].trans, obDepthExtrinsics[OB_SENSOR_COLOR].trans + 3,   k4C.color_camera_calibration.extrinsics.translation);
    std::copy(obDepthExtrinsics[OB_SENSOR_COLOR].rot,   obDepthExtrinsics[OB_SENSOR_COLOR].rot + 9,     k4C.color_camera_calibration.extrinsics.rotation);

    // extrinsics
    std::copy(obDepthExtrinsics[OB_SENSOR_COLOR].rot,   obDepthExtrinsics[OB_SENSOR_COLOR].rot + 9,     k4C.extrinsics[K4A_CALIBRATION_TYPE_DEPTH][K4A_CALIBRATION_TYPE_COLOR].rotation);
    std::copy(obDepthExtrinsics[OB_SENSOR_COLOR].trans, obDepthExtrinsics[OB_SENSOR_COLOR].trans + 3,   k4C.extrinsics[K4A_CALIBRATION_TYPE_DEPTH][K4A_CALIBRATION_TYPE_COLOR].translation);
    std::copy(obColorExtrinsics[OB_SENSOR_DEPTH].rot,   obDepthExtrinsics[OB_SENSOR_DEPTH].rot + 9,     k4C.extrinsics[K4A_CALIBRATION_TYPE_COLOR][K4A_CALIBRATION_TYPE_DEPTH].rotation);
    std::copy(obColorExtrinsics[OB_SENSOR_DEPTH].trans, obDepthExtrinsics[OB_SENSOR_DEPTH].trans + 3,   k4C.extrinsics[K4A_CALIBRATION_TYPE_COLOR][K4A_CALIBRATION_TYPE_DEPTH].translation);

    std::copy(obDepthExtrinsics[OB_SENSOR_DEPTH].rot,   obDepthExtrinsics[OB_SENSOR_DEPTH].rot + 9,     k4C.extrinsics[K4A_CALIBRATION_TYPE_DEPTH][K4A_CALIBRATION_TYPE_DEPTH].rotation);
    std::copy(obDepthExtrinsics[OB_SENSOR_DEPTH].trans, obDepthExtrinsics[OB_SENSOR_DEPTH].trans + 3,   k4C.extrinsics[K4A_CALIBRATION_TYPE_DEPTH][K4A_CALIBRATION_TYPE_DEPTH].translation);
    std::copy(obColorExtrinsics[OB_SENSOR_COLOR].rot,   obColorExtrinsics[OB_SENSOR_COLOR].rot + 9,     k4C.extrinsics[K4A_CALIBRATION_TYPE_COLOR][K4A_CALIBRATION_TYPE_COLOR].rotation);
    std::copy(obColorExtrinsics[OB_SENSOR_COLOR].trans, obColorExtrinsics[OB_SENSOR_COLOR].trans + 3,   k4C.extrinsics[K4A_CALIBRATION_TYPE_COLOR][K4A_CALIBRATION_TYPE_COLOR].translation);

    return k4C;
}



FemtoBaseDevice::FemtoBaseDevice(DCType deviceType) : i(std::make_unique<Impl>()){
    
    auto lg = LogG("OrbbecBaseDevice::OrbbecBaseDevice"sv);
    if(i->devicesM == nullptr){         
        i->devicesM = std::make_unique<OrbbecDeviceManager>();
    }
    i->deviceType = deviceType;    
}

FemtoBaseDevice::~FemtoBaseDevice(){
    auto lg = LogG("~OrbbecBaseDevice::OrbbecBaseDevice"sv);
}


auto FemtoBaseDevice::open(const DCModeInfos &mInfos, const DCConfigSettings &configS, const DCColorSettings &colorS) -> bool{
    
    auto lg = LogG("OrbbecBaseDevice::open"sv);
    
    Log::message("### Open device ###\n"sv);

    i->device = nullptr;

    try {

        if(i->deviceType == DCType::FemtoBolt){

            if(!configS.useSerialNumber){
                if(configS.idDevice >= i->devicesM->femtoBoltDevices.size()){
                    Log::error("[OrbbecDevice] Invalid id device.\n"sv);
                    return false;
                }
                Log::message("Retrieve from devices list.\n");
                i->device     = i->devicesM->femtoBoltDevices[configS.idDevice];
            }else{
                for(auto &dev : i->devicesM->femtoBoltDevices){
                    try{
                        if(configS.serialNumber == dev->getDeviceInfo()->serialNumber()){
                            i->device = dev;
                            break;
                        }
                    }catch(const ob::Error &) {}
                }
            }

        }else if(i->deviceType == DCType::FemtoMegaUSB){

            if(!configS.useSerialNumber){
                if(configS.idDevice >= i->devicesM->femtoMegaUSBDevices.size()){
                    Log::error("[OrbbecDevice] Invalid id device.\n"sv);
                    return false;
                }
                
                Log::message("Retrieve from devices list.\n");
                i->device     = i->devicesM->femtoMegaUSBDevices[configS.idDevice];
            }else{

                for(auto &dev : i->devicesM->femtoMegaUSBDevices){
                    try{
                        if(configS.serialNumber == dev->getDeviceInfo()->serialNumber()){
                            i->device = dev;
                            break;
                        }
                    }catch(const ob::Error &) {}
                }
            }

        }else if(i->deviceType == DCType::FemtoMegaEthernet){

            auto wantedAddress = std::format("{}.{}.{}.{}", configS.ipv4Address.x(), configS.ipv4Address.y(), configS.ipv4Address.z(), configS.ipv4Address.w());

            bool found = false;
            for(auto &dev : i->devicesM->femtoMegaEthernetDevices){
                Log::message(std::format("compare [{}] [{}]\n", dev->getDeviceInfo()->ipAddress(), wantedAddress));
                if(dev->getDeviceInfo()->ipAddress() == wantedAddress){
                    i->device = dev;
                    found = true;
                    break;
                }
            }

            if(!found){
                Log::message("Device not avaialble from list, create new net device.\n");
                Log::message(std::format("Open device from ip address: [{}]\n", wantedAddress));
                i->device = i->devicesM->context.createNetDevice(wantedAddress.c_str() , 8090);
            }
        }else if(i->deviceType == DCType::Gemini215){

            if(!configS.useSerialNumber){
                if(configS.idDevice >= i->devicesM->gemini215Devices.size()){
                    Log::error("[OrbbecDevice] Invalid id device.\n"sv);
                    return false;
                }
                Log::message("Retrieve from devices list.\n");
                i->device     = i->devicesM->gemini215Devices[configS.idDevice];
            }else{
                for(auto &dev : i->devicesM->gemini215Devices){
                    try{
                        if(configS.serialNumber == dev->getDeviceInfo()->serialNumber()){
                            i->device = dev;
                            break;
                        }
                    }catch(const ob::Error &) {}
                }
            }
        }


        if(i->device == nullptr){
            Log::error("Invalid device.\n");
            return false;
        }

        // Update the configuration items of the configuration file, and keep the original configuration for other items
        auto synchConfig = i->device->getMultiDeviceSyncConfig();
        Log::message(std::format("CURRENT SYNC CONFIG BEFORE:\n syncMode: {} \n trigger2ImageDelayUs: {} \n colorDelayUs: {} \n tdepthDelayUs: {} \n triggerOutEnable: {} \n triggerOutDelayUs: {} \n framesPerTrigger: {} \n",
            (int)synchConfig.syncMode, synchConfig.trigger2ImageDelayUs, synchConfig.colorDelayUs, synchConfig.depthDelayUs, synchConfig.triggerOutEnable, synchConfig.triggerOutDelayUs, synchConfig.framesPerTrigger));

        // // check if changes
        // // and if changes reboot device

        // switch (configS.synchMode) {
        // case DCSynchronisationMode::Standalone:

        //     Log::message("### STANDALONE ###\n");
        //     // The device does not synchronize with other devices.
        //     // The Color and Depth should be set to same frame rates, the Color and Depth will be synchronized.
        //     synchConfig.syncMode                = OB_MULTI_DEVICE_SYNC_MODE_STANDALONE;
        //     synchConfig.triggerOutEnable        = false;
        //     synchConfig.trigger2ImageDelayUs    = 0;
        //     break;
        // case DCSynchronisationMode::Main:

        //     Log::message("### PRIMARY ###\n");
        //     // The device is the primary device in the multi-device system, it will output the trigger signal via VSYNC_OUT pin on synchronization port by default.
        //     // The Color and Depth should be set to same frame rates, the Color and Depth will be synchronized and can be adjusted by @ref colorDelayUs, @ref depthDelayUs or @ref trigger2ImageDelayUs.
        //     synchConfig.syncMode                = OB_MULTI_DEVICE_SYNC_MODE_PRIMARY;
        //     synchConfig.trigger2ImageDelayUs    = 0;
        //     synchConfig.colorDelayUs            = 0;
        //     synchConfig.depthDelayUs            = 0;
        //     synchConfig.triggerOutEnable        = true;
        //     synchConfig.triggerOutDelayUs       = 0;
        //     break;

        // case DCSynchronisationMode::Subordinate:

        //     Log::message("### SUBORDINATE ###\n");
        //     // The device is the secondary device in the multi-device system, it will receive the trigger signal via VSYNC_IN pin on synchronization port. It
        //     // will out the trigger signal via VSYNC_OUT pin on synchronization port by default.
        //     // The Color and Depth should be set to same frame rates, the Color and Depth will be synchronized and can be adjusted by @ref colorDelayUs, @ref
        //     // depthDelayUs or @ref trigger2ImageDelayUs.
        //     // After starting the stream, the device will wait for the trigger signal to start capturing images, and will stop capturing images when the trigger
        //     // signal is stopped.
        //     // The frequency of the trigger signal should be same as the frame rate of the stream profile which is set when starting the stream.
        //     synchConfig.syncMode                = OB_MULTI_DEVICE_SYNC_MODE_SECONDARY;
        //     synchConfig.trigger2ImageDelayUs    = configS.subordinateDelayUsec;
        //     synchConfig.colorDelayUs            = configS.subordinateDelayUsec;
        //     synchConfig.depthDelayUs            = configS.subordinateDelayUsec;
        //     synchConfig.triggerOutEnable        = true;
        //     synchConfig.triggerOutDelayUs       = 0;

        //     break;
        // default:
        //     Log::message("### DEFAULT ###\n");
        //     // The device does not synchronize with other devices,
        //     // The Color and Depth can be set to different frame rates.
        //     synchConfig.syncMode                = OB_MULTI_DEVICE_SYNC_MODE_FREE_RUN;

        //     if(configS.synchronizeColorAndDepth){
        //         synchConfig.colorDelayUs            = 0;
        //         synchConfig.depthDelayUs            = 0;
        //     }else{
        //         synchConfig.colorDelayUs            = 0;
        //         synchConfig.depthDelayUs            = configS.delayBetweenColorAndDepthUsec;
        //     }

        //     synchConfig.triggerOutEnable        = false;
        //     synchConfig.trigger2ImageDelayUs    = 0;

        //     break;
        // }



        // i->device->setMultiDeviceSyncConfig(synchConfig);
        // // i->device->reboot();

        // std::this_thread::sleep_for(std::chrono::milliseconds(50));

        // synchConfig = i->device->getMultiDeviceSyncConfig();
        // Log::message(std::format("SYNC CONFIG AFTER:\n syncMode: {} \n trigger2ImageDelayUs: {} \n colorDelayUs: {} \n tdepthDelayUs: {} \n triggerOutEnable: {} \n triggerOutDelayUs: {} \n framesPerTrigger: {} \n",
        //                             (int)synchConfig.syncMode, synchConfig.trigger2ImageDelayUs, synchConfig.colorDelayUs, synchConfig.depthDelayUs, synchConfig.triggerOutEnable, synchConfig.triggerOutDelayUs, synchConfig.framesPerTrigger));
        
        
        Log::message("Retrieve sensor list.\n");
        i->sensorList = i->device->getSensorList();
        
        Log::message(std::format("[{}] sensors found.\n", i->sensorList->count()));

        for(uint32_t idS = 0; idS < i->sensorList->count(); idS++) {
            auto sensor = i->sensorList->getSensor(idS);
            switch(sensor->type()) {
            case OB_SENSOR_COLOR:
                Log::message("   Color sensor found.\n"sv);
                break;
            case OB_SENSOR_DEPTH:
                Log::message("   Depth sensor found.\n"sv);
                break;
            case OB_SENSOR_IR:
                Log::message("   Infrared sensor found.\n"sv);
                break;
            case OB_SENSOR_IR_LEFT:
                Log::message("   Infrared left sensor found.\n"sv);
                break;
            case OB_SENSOR_IR_RIGHT:
                Log::message("   Infrared right sensor found.\n"sv);
                break;
            case OB_SENSOR_GYRO:
                Log::message("   Gyro sensor found.\n"sv);
                break;
            case OB_SENSOR_ACCEL:
                Log::message("   Accel sensor found.\n"sv);
                break;
            default:
                break;
            }
        }

        auto dInfos     = i->device->getDeviceInfo();
        Log::message("Device infos:\n"sv);
        Log::message(std::format("  Name: {}\nsv", dInfos->name()));
        Log::message(std::format("  Chip type: {}\n"sv, dInfos->asicName()));
        Log::message(std::format("  Serialnum: {}\n"sv, dInfos->serialNumber()));
        Log::message(std::format("  PID: {}\n"sv, dInfos->pid()));
        Log::message(std::format("  VID: {}\n"sv, dInfos->vid()));
        Log::message(std::format("  UID: {}\n"sv, dInfos->uid()));
        
        Log::message("  Version:\n"sv);
        Log::message(std::format("      Firmware: {}\n"sv, dInfos->firmwareVersion()));
        Log::message(std::format("      Hardware: {}\n"sv, dInfos->hardwareVersion()));
        Log::message(std::format("      SDK minimum supported: {}\n"sv, dInfos->supportedMinSdkVersion()));
        // Log::message(std::format("      Extension info: {}\n"sv, dInfos->extensionInfo()));
        Log::message("  Types:\n"sv);
        std::string deviceT;
        switch(dInfos->deviceType()){
        case OB_STRUCTURED_LIGHT_MONOCULAR_CAMERA:
            deviceT = "Monocular structured light camera"sv;
            break;
        case OB_STRUCTURED_LIGHT_BINOCULAR_CAMERA:
            deviceT = "Binocular structured light camera"sv;
            break;
        case OB_TOF_CAMERA:
            deviceT = "Time-of-flight camera"sv;
            break;
        }
        Log::message(std::format("      Device: {}\n"sv, deviceT));
        Log::message(std::format("      Connection: {}\n"sv, dInfos->connectionType()));
        if(std::string(dInfos->connectionType()) == "Ethernet"sv){
            Log::message(std::format("  IP: {}\n"sv, dInfos->ipAddress()));
        }

    }catch(const ob::Error &e) {
        Log::error(std::format("Open error: [{}]\n"sv, e.getMessage()));
        i->device = nullptr;
        return false;
    }catch(const std::exception &e){
        Log::error(std::format("Error: [{}]\n"sv, e.what()));
        i->device = nullptr;
        return false;
    }

    return initialize(mInfos, configS, colorS);
}


auto FemtoBaseDevice::initialize(const DCModeInfos &mInfos, const DCConfigSettings &configS, const DCColorSettings &colorS) -> bool{

    static_cast<void>(configS);
    
    auto lg = LogG("OrbbecBaseDevice::initialize"sv);

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

    //i->deviceType = mInfos.device();
    
    Log::message("### Start pipeline ###.\n");
    Log::message("Create config.\n");
    std::shared_ptr<ob::Config> config = std::make_shared<ob::Config>();

    try {

        if(i->pipe != nullptr){
            Log::message("Close pipeline.\n");
            i->pipe->stop();
            Log::message("Destroy pipeline.\n");
            i->pipe = nullptr;
        }
        Log::message("Create new pipeline.\n");
        i->pipe = std::make_unique<ob::Pipeline>(i->device);
        
        Log::message("Retrieve sensors.\n");
        bool hasColorSensor = i->sensorList->getSensor(OBSensorType::OB_SENSOR_COLOR) != nullptr;
        Log::message(std::format("   Color sensor found [{}].\n", hasColorSensor));
        bool hasDepthSensor = i->sensorList->getSensor(OBSensorType::OB_SENSOR_DEPTH) != nullptr;
        Log::message(std::format("   Depth sensor found [{}].\n", hasDepthSensor));
        bool hasInfraSensor = i->sensorList->getSensor(OBSensorType::OB_SENSOR_IR)    != nullptr;
        Log::message(std::format("   Infra sensor found [{}].\n", hasInfraSensor));

        // retrieve color profile
        i->colorProfile = nullptr;
        if(hasColorSensor && mInfos.has_color()){
            Log::message("Retrieve color profile.\n");
            if(auto colorStreamProfileList = i->pipe->getStreamProfileList(OB_SENSOR_COLOR); colorStreamProfileList != nullptr){
                try {
                    i->colorProfile = colorStreamProfileList->getVideoStreamProfile(
                        static_cast<int>(mInfos.color_width()),
                        static_cast<int>(mInfos.color_height()),
                        convert_to_ob_image_format(mInfos.image_format()),
                        mInfos.framerate_value()
                    );
                    Log::message("Color profile found.\n");
                }catch(...) {
                    i->colorProfile = colorStreamProfileList->getProfile(OB_PROFILE_DEFAULT);
                }
                if(i->colorProfile != nullptr){
                    Log::message("Enable color stream.\n");
                    config->enableStream(i->colorProfile);
                }
            }
        }

        // retrieve depth profile
        i->depthProfile = nullptr;
        if(hasDepthSensor && mInfos.has_depth()){
            Log::message("Retrieve depth profile.\n");
            if(auto depthStreamProfileList = i->pipe->getStreamProfileList(OB_SENSOR_DEPTH); depthStreamProfileList != nullptr){
                try {
                    i->depthProfile = depthStreamProfileList->getVideoStreamProfile(
                        static_cast<int>(mInfos.depth_width()),
                        static_cast<int>(mInfos.depth_height()),
                        OB_FORMAT_Y16,
                        mInfos.framerate_value()
                        );
                    Log::message("Depth profile found.\n");
                }catch(...) {
                    i->depthProfile = depthStreamProfileList->getProfile(OB_PROFILE_DEFAULT);
                }

                if(i->depthProfile != nullptr){
                    Log::message("Enable depth stream.\n");
                    config->enableStream(i->depthProfile);
                }
            }
        }

        // retrieve infrared profile
        i->infraProfile = nullptr;

        if(configS.enableIRStream && hasInfraSensor && mInfos.has_infra()){
            Log::message("Retrieve infra profile.\n");
            if(auto infraStreamProfileList = i->pipe->getStreamProfileList(OB_SENSOR_IR); infraStreamProfileList != nullptr){
                try {
                    i->infraProfile = infraStreamProfileList->getVideoStreamProfile(
                        static_cast<int>(mInfos.infra_width()),
                        static_cast<int>(mInfos.infra_height()),
                        OB_FORMAT_Y16,
                        mInfos.framerate_value()
                        );
                    Log::message("Infra profile found.\n");
                }catch(...) {
                    i->infraProfile = infraStreamProfileList->getProfile(OB_PROFILE_DEFAULT);
                }

                if(i->infraProfile != nullptr){
                    Log::message("Enable infra stream.\n");
                    config->enableStream(i->infraProfile);
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



        // disable align mode
        Log::message("Disable align mode.\n");
        config->setAlignMode(ALIGN_DISABLE);

        // frame synch
        if(configS.synchronizeColorAndDepth){
            Log::message("Enable frame synch.\n");
            i->pipe->enableFrameSync();
        }else{
            Log::message("Disable frame synch.\n");
            i->pipe->disableFrameSync();
        }

        // other
        i->set_property_value(OB_PROP_INDICATOR_LIGHT_BOOL, !configS.disableLED);

        // start pipe with current config
        Log::message("Start pipeline with new config.\n");
        i->pipe->start(config);

        // get camera intrinsic and extrinsic parameters form pipeline and set to point cloud filter
        Log::message("Retrieve camera parameters.\n");
        i->cameraParam   = i->pipe->getCameraParam();
        // get calibration parameters
        Log::message("Retrieve calibration parameters.\n");
        i->calibrationParam = i->pipe->getCalibrationParam(config);
        
        Log::message("Generate k4a calibration.\n");
        i->k4aCalibration = i->k4a_convert_calibration(mInfos, i->calibrationParam);

        // specify color-depth alignment
        i->k4aCalibration.extrinsics[K4A_CALIBRATION_TYPE_DEPTH][K4A_CALIBRATION_TYPE_COLOR].translation[0] += configS.colorAlignmentTr.x();
        i->k4aCalibration.extrinsics[K4A_CALIBRATION_TYPE_DEPTH][K4A_CALIBRATION_TYPE_COLOR].translation[1] += configS.colorAlignmentTr.y();
        i->k4aCalibration.extrinsics[K4A_CALIBRATION_TYPE_DEPTH][K4A_CALIBRATION_TYPE_COLOR].translation[2] += configS.colorAlignmentTr.z();


        if((dc_depth_resolution(configS.mode) != DCDepthResolution::OFF) && configS.btEnabled){
            Log::message("[OrbbecBaseDevice::start_device] Start body tracker\n");

            i->k4aBtConfig.gpu_device_id       = configS.btGPUId;
            i->k4aBtConfig.processing_mode     = convert_to_k4a_body_tracking_processing_mode(configS.btProcessingMode);
            i->k4aBtConfig.sensor_orientation  = convert_to_k4a_sensor_orientation(configS.btOrientation);
            i->k4aBtConfig.model_path          = nullptr;

            i->bodyTracker = std::make_unique<k4abt::tracker>(k4abt::tracker::create(i->k4aCalibration, i->k4aBtConfig));
            // i->bodyTracker->set_temporal_smoothing(dataS.capture.btTemporalSmoothing);
        }

    }catch(ob::Error &e) {
        Log::error(std::format("[OrbbecDevice::start_reading] Start reading error: {}\n", e.getMessage()));
        i->device = nullptr;
        return false;
    }catch(const std::exception &e){
        Log::error(std::format("[OrbbecDevice::start_reading] Error: {}\nsv", e.what()));
        i->device = nullptr;
        return false;
    }catch(...){
        Log::error("[OrbbecDevice::start_reading] Unknow error\n");
        i->device = nullptr;
        return false;
    }

    display_calibration(i->k4aCalibration);
    
    Log::message("Generate k4a transformation.\n");
    i->k4aTransformation = std::make_unique<k4a::transformation>(i->k4aCalibration);
    
    Log::message("### Pipeline started ###.\n");
    
    Log::message("### Update colors settings ###.\n");
    update_from_colors_settings(colorS);


    return true;
}


auto FemtoBaseDevice::close() -> void{
    
    auto lg = LogG("OrbbecBaseDevice::close"sv);

    if(i->pipe != nullptr){

        Log::message("Stop pipeline\n");
        i->pipe->stop();
        
        // Log::message("Clean pipeline\n");
        // i->pipe = nullptr;
    }

    // if(i->bodyTracker != nullptr){
    //     Log::message("Stop body tracker\n");
    //     i->bodyTracker->shutdown();
    //     i->bodyTracker = nullptr;
    // }
    
    // Log::message("Clean sensor list\n");
    // i->sensorList = nullptr;

    Log::message("Close device\n");
    i->device = nullptr;
    // i->deviceList.clear();
    
    Log::message("Device closed\n");
}



auto FemtoBaseDevice::update_from_colors_settings(const DCColorSettings &colorS) -> void{

    auto lg = LogG("OrbbecBaseDevice::update_from_colors_settings"sv);
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
        Log::error(std::format("[OrbbecDevice] Error: {}\n", e.getMessage()));
    }
}


auto FemtoBaseDevice::is_opened() const noexcept -> bool {
    return i->device != nullptr;
}

auto FemtoBaseDevice::nb_devices() const noexcept -> size_t {
    if(i->deviceType == DCType::FemtoBolt){
        return i->devicesM->femtoBoltDevices.size();
    }else if(i->deviceType == DCType::FemtoMegaUSB){
        return i->devicesM->femtoMegaUSBDevices.size();
    }else if(i->deviceType == DCType::FemtoMegaEthernet){
        return 0;
    }else if(i->deviceType == DCType::Gemini215){
        return i->devicesM->gemini215Devices.size();
    }
    return 0;
}

auto FemtoBaseDevice::device_name() const noexcept -> std::string {
    if(is_opened()){
        // return std::format("{} {}", i->device->getDeviceInfo()->name(), i->device->getDeviceInfo()->serialNumber());
        return i->device->getDeviceInfo()->serialNumber();
    }
    return "unknow_device"s;
}

auto FemtoBaseDevice::capture_frame(int32_t timeoutMs) -> bool{

    if(i->pipe != nullptr){
        try{

            i->colorImage    = nullptr;
            i->depthImage    = nullptr;
            i->infraredImage = nullptr;
            i->frameSet      = nullptr;

            i->frameSet = i->pipe->waitForFrames(timeoutMs);
        }catch(ob::Error &e) {
            Log::error(std::format("[OrbbecDevice] Get capture error: {}\n", e.getMessage()));
        }

        return i->frameSet != nullptr;
    }
    return false;
}

auto FemtoBaseDevice::read_calibration() -> BinarySpan{
    return BinarySpan{
        reinterpret_cast<std::byte*>(&i->k4aCalibration),
        static_cast<size_t>(sizeof(k4a_calibration_t))
    };
}

auto FemtoBaseDevice::read_color_image() -> BinarySpan{

    if(i->frameSet != nullptr){

        // auto lg = LogGuard("OrbbecBaseDevice::read_color_image\n"sv);
        i->colorImage = i->frameSet->colorFrame();

        // std::cout << "READ COLOR IMAGE " << (i->colorImage != nullptr) << "\n";
        if(i->colorImage != nullptr){
            return BinarySpan{
                reinterpret_cast<std::byte*>(i->colorImage->data()),
                i->colorImage->width()*i->colorImage->height()*4
            };
        }
    }
    Log::message("invalid color.\n");
    return {};
}

auto FemtoBaseDevice::read_depth_image() -> std::span<std::uint16_t>{

    if(i->frameSet != nullptr){

        // auto lg = LogGuard("OrbbecBaseDevice::read_depth_image\n"sv);
        i->depthImage = i->frameSet->depthFrame();

        // std::cout << "READ DEPTH IMAGE " << (i->depthImage != nullptr) << "\n";
        if(i->depthImage != nullptr){
            return  std::span<std::uint16_t>{
                reinterpret_cast<std::uint16_t*>(i->depthImage->data()),
                i->depthImage->width()*i->depthImage->height()
            };
        }
    }
    Log::message("invalid depth.\n");
    return {};
}

auto FemtoBaseDevice::read_infra_image() -> std::span<std::uint16_t>{

    if(i->frameSet != nullptr){

        // auto lg = LogGuard("OrbbecBaseDevice::read_infra_image\n"sv);
        i->infraredImage = i->frameSet->irFrame();

        // std::cout << "READ INFRARED IMAGE " << (i->infraredImage != nullptr) << "\n";
        if(i->infraredImage != nullptr){
            return  std::span<std::uint16_t>{
                reinterpret_cast<std::uint16_t*>(i->infraredImage->data()),
                i->infraredImage->width()*i->infraredImage->height()
            };
        }
    }

    Log::message("invalid infra.\n");
    return {};
}

auto FemtoBaseDevice::read_body_tracking() -> std::tuple<std::span<uint8_t>, std::span<DCBody> >{

    if(i->bodyTracker == nullptr || i->depthImage == nullptr || i->infraredImage == nullptr){
        return {};
    }

    try{

        k4a::image k4DepthImage = k4a::image::create_from_buffer(
            K4A_IMAGE_FORMAT_DEPTH16,
            i->depthImage->width(),
            i->depthImage->height(),
            i->depthImage->width()*sizeof(std::uint16_t),
            reinterpret_cast<std::uint8_t*>(i->depthImage->data()),
            i->depthImage->dataSize(),
            nullptr,
            nullptr
        );

        k4a::image k4IRImage = k4a::image::create_from_buffer(
            K4A_IMAGE_FORMAT_DEPTH16,
            i->infraredImage->width(),
            i->infraredImage->height(),
            i->infraredImage->width()*sizeof(std::uint16_t),
            reinterpret_cast<std::uint8_t*>(i->infraredImage.get()->data()),
            i->infraredImage->dataSize(),
            nullptr,
            nullptr
        );

        if(i->bodyCapture == nullptr){
            k4a_capture_t bodyCaptureH = nullptr;
            k4a_capture_create(&bodyCaptureH);
            i->bodyCapture = k4a::capture(bodyCaptureH);
        }

        i->bodyCapture.set_depth_image(k4DepthImage);
        i->bodyCapture.set_ir_image(k4IRImage);

        if(!i->bodyTracker->enqueue_capture(i->bodyCapture, std::chrono::milliseconds(1))){
            return {};
        }

        if(k4abt::frame bodyFrame = i->bodyTracker->pop_result(std::chrono::milliseconds(1)); bodyFrame != nullptr){

            auto bodiesCount = bodyFrame.get_num_bodies();
            i->bodies.resize(bodiesCount);
            if(i->bodies.size() < bodiesCount){
                i->bodies.resize(bodiesCount);
            }
            for(size_t ii = 0; ii < bodiesCount; ++ii){
                i->update_k4_body(i->bodies[ii], bodyFrame.get_body(static_cast<int>(ii)));
            }

            k4a::image bodiesIndexImage = bodyFrame.get_body_index_map();

            std::span<ColorGray8> biISpan = {};
            if(bodiesIndexImage.is_valid()){

                // copy data
                i->bodiesIndex.resize(bodiesIndexImage.get_width_pixels()*bodiesIndexImage.get_height_pixels());
                std::copy(bodiesIndexImage.get_buffer(), bodiesIndexImage.get_buffer() + i->bodiesIndex.size(), i->bodiesIndex.begin());
            }else{
                i->bodiesIndex.clear();
            }
            biISpan = i->bodiesIndex.span();

            return std::make_tuple(biISpan, std::span<DCBody>{i->bodies});
        }

    }  catch (k4a::error error) {
        Log::error(std::format("[OrbbecBaseDevice::read_bodies] error: {}\n", error.what()));
    }  catch (std::runtime_error error) {
        Log::error(std::format("[OrbbecBaseDevice::read_bodies] error: {}\n", error.what()));
    }

    return {{},{}};
}

auto FemtoBaseDevice::release_frame() -> void{
    i->frameSet = nullptr;
}

auto FemtoBaseDevice::read_from_imu() -> tool::BinarySpan{
    return {};
}


auto FemtoBaseDevice::resize_color_image_to_depth_size(const DCModeInfos &mInfos, std::span<ColorRGBA8> colorData, std::span<uint16_t> depthData) -> std::span<ColorRGBA8>{

    size_t colorWidth;
    size_t colorHeight;
    size_t colorStride;
    size_t colorSizeBytes;
    std::uint8_t* colorDataBuffer;

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
        // auto t1 = Time::nanoseconds_since_epoch();
        i->k4aTransformation->color_image_to_depth_camera(
            k4aDepthImage,
            k4aColorImage,
            &k4aDepthSizedColorImage
        );
        // Log::fmessage("[r{}]", Time::now_difference_micro_s(t1));

    }catch(const std::runtime_error &error){
        Log::error(std::format("[OrbbecBaseDevice::k4a_resize_color_image_to_depth_size] Runtime error: {}", error.what()));
        return {};
    }

    return std::span<ColorRGBA8>{
        reinterpret_cast<ColorRGBA8*>(i->depthSizedColorData.data()),
        static_cast<size_t>(mInfos.depth_width() * mInfos.depth_height())
    };
}


auto FemtoBaseDevice::generate_cloud(const DCModeInfos &mInfos, std::span<uint16_t> depthData) -> std::span<geo::Pt3<std::int16_t>> {

    auto depthStride     = mInfos.depth_width() * 1 * sizeof(std::uint16_t);
    auto depthSizeBytes  = depthStride * mInfos.depth_height();
    auto depthDataBuffer = reinterpret_cast<std::uint8_t*>(depthData.data());

    auto cloudStride     = mInfos.depth_width() * 3 * sizeof(std::uint16_t);
    auto cloudSizeBytes  = cloudStride * mInfos.depth_height();
    auto cloudDataBuffer = reinterpret_cast<std::uint8_t*>(i->cloudData.data());

    auto k4aDepthImage = k4a::image::create_from_buffer(
        K4A_IMAGE_FORMAT_DEPTH16,
        static_cast<int>(mInfos.depth_width()), static_cast<int>(mInfos.depth_height()), static_cast<int>(depthStride), depthDataBuffer, depthSizeBytes,
        nullptr,nullptr
    );

    auto k4aPointCloudImage = k4a::image::create_from_buffer(
        K4A_IMAGE_FORMAT_CUSTOM,
        static_cast<int>(mInfos.depth_width()), static_cast<int>(mInfos.depth_height()), static_cast<int>(cloudStride), cloudDataBuffer, cloudSizeBytes,
        nullptr, nullptr
    );

    try{

        // auto t1 = Time::nanoseconds_since_epoch();
        i->k4aTransformation->depth_image_to_point_cloud(
            k4aDepthImage,
            K4A_CALIBRATION_TYPE_DEPTH,
            &k4aPointCloudImage
        );

        // invert cloud
        auto b = reinterpret_cast<geo::Pt3<std::int16_t>*>(i->cloudData.data());
        for(int ii = 0; ii < mInfos.depth_width() * mInfos.depth_height(); ++ii){
            b[ii].x() *= -1;
            b[ii].y() *= -1;
        }

    }catch(const std::runtime_error &error){
        Log::error(std::format("[OrbbecBaseDevice::k4a_generate_cloud] Runtime error: {}", error.what()));
        return {};
    }

    return std::span<geo::Pt3<std::int16_t>>{
        reinterpret_cast<geo::Pt3<std::int16_t>*>(i->cloudData.data()),
        mInfos.depth_width() * mInfos.depth_height()
    };
}



// auto init_context() -> void{

//     auto lg = LogGuard("[OrbbecBaseDevice::Impl::init_context]");

//     if(context == nullptr){
//         try{
//             context = std::make_unique<ob::Context>();
//             context->setLoggerSeverity(OB_LOG_SEVERITY_WARN);
//         }catch(const ob::Error &e) {
//             Log::error(std::format("Orbbec error: [{}]\n"sv, e.getMessage()));
//         }catch(const std::exception &e){
//             Log::error(std::format("Error: [{}]\n"sv, e.what()));
//         }
//     }

//     // context->setLoggerToCallback(OB_LOG_SEVERITY_WARN, [&](OBLogSeverity severity, const char *logMsg){
//     //     if((severity == OBLogSeverity::OB_LOG_SEVERITY_ERROR) || (severity == OBLogSeverity::OB_LOG_SEVERITY_FATAL)){
//     //         Log::error(logMsg);
//     //     }else if(severity == OBLogSeverity::OB_LOG_SEVERITY_WARN){
//     //         Log::warning(logMsg);
//     //     }else if(severity == OBLogSeverity::OB_LOG_SEVERITY_INFO){
//     //         Log::message(logMsg);
//     //     }else if(severity == OBLogSeverity::OB_LOG_SEVERITY_DEBUG){
//     //         //Log::log(logMsg);
//     //     }
//     // });
// }
