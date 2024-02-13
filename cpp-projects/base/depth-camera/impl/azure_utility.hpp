
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

#pragma once

#include <iostream>

// k4a
// #include "k4a/k4a.h"
#include <k4a/k4atypes.h>

// kabt
#include <kabt/k4abt.hpp>

// local
#include "depth-camera/dc_enums.hpp"

namespace tool::cam{

[[maybe_unused]]  [[nodiscard]] static auto convert_to_k4a_powerline_frequency_value(DCPowerlineFrequency pFreq) noexcept -> int{
    switch(pFreq){
    case DCPowerlineFrequency::F50:
        return 1;
    case DCPowerlineFrequency::F60:
        return 2;
    case DCPowerlineFrequency::Undefined:
        return 1;
    default:
        return 1;
    }
}

[[maybe_unused]] [[nodiscard]] static auto convert_to_k4a_fps(DCFramerate fps) noexcept -> k4a_fps_t{
    switch(fps){
    case DCFramerate::F30:
        return K4A_FRAMES_PER_SECOND_30;
    case DCFramerate::F15:
        return K4A_FRAMES_PER_SECOND_15;
    case DCFramerate::F5:
        return K4A_FRAMES_PER_SECOND_5;
    default:
        return K4A_FRAMES_PER_SECOND_30;
    }
}

[[maybe_unused]] [[nodiscard]] static auto convert_to_k4a_depth_mode(DCDepthResolution dRes) noexcept -> k4a_depth_mode_t{
    switch (dRes) {
    case DCDepthResolution::K4A_320x288:
        return K4A_DEPTH_MODE_NFOV_2X2BINNED;
    case DCDepthResolution::K4A_640x576:
        return K4A_DEPTH_MODE_NFOV_UNBINNED;
    case DCDepthResolution::K4A_512x512:
        return K4A_DEPTH_MODE_WFOV_2X2BINNED;
    case DCDepthResolution::K4A_1024x1024:
        return K4A_DEPTH_MODE_WFOV_UNBINNED;
    default:
        return K4A_DEPTH_MODE_OFF;
    }
}

[[maybe_unused]] [[nodiscard]] static auto convert_to_k4a_color_resolution(DCColorResolution dRes) noexcept -> k4a_color_resolution_t{
    switch (dRes) {
    case DCColorResolution::R720P:
        return K4A_COLOR_RESOLUTION_720P;
    case DCColorResolution::R1080P:
        return K4A_COLOR_RESOLUTION_1080P;
    case DCColorResolution::R1440P:
        return K4A_COLOR_RESOLUTION_1440P;
    case DCColorResolution::R1536P:
        return K4A_COLOR_RESOLUTION_1536P;
    case DCColorResolution::R2160P:
        return K4A_COLOR_RESOLUTION_2160P;
    case DCColorResolution::R3072P:
        return K4A_COLOR_RESOLUTION_3072P;
    default:
        return K4A_COLOR_RESOLUTION_OFF;
    }
}

[[maybe_unused]] [[nodiscard]] static auto convert_to_k4a_image_format(DCImageFormat iFormat) noexcept -> k4a_image_format_t{
    switch (iFormat) {
    case DCImageFormat::NV12: // NV12 images separate the luminance and chroma data such that all the luminance is at the beginning of the buffer, and the chroma lines follow immediately after.
        return K4A_IMAGE_FORMAT_COLOR_NV12;
    case DCImageFormat::YUY2: // YUY2 stores chroma and luminance data in interleaved pixels.
        return K4A_IMAGE_FORMAT_COLOR_YUY2;
    case DCImageFormat::MJPG: //  The buffer for each image is encoded as a JPEG and can be decoded by a JPEG decoder.
        return K4A_IMAGE_FORMAT_COLOR_MJPG;
    case DCImageFormat::BGRA: // Each pixel of BGRA32 data is four bytes. The first three bytes represent Blue, Green, and Red data. The fourth byte is the alpha channel and is unused in the Azure Kinect APIs.
        return K4A_IMAGE_FORMAT_COLOR_BGRA32;
    case DCImageFormat::DEPTH16: // Each pixel of DEPTH16 data is two bytes of little endian unsigned depth data. The unit of the data is in millimeters from the origin of the camera.
        return K4A_IMAGE_FORMAT_DEPTH16;
    case DCImageFormat::INFRA16: // Each pixel of IR16 data is two bytes of little endian unsigned depth data. The value of the data represents brightness.
        return K4A_IMAGE_FORMAT_IR16;
    default:
        return K4A_IMAGE_FORMAT_COLOR_BGRA32;
    }
}

[[maybe_unused]] [[nodiscard]] static auto convert_to_k4a_synch_mode(DCSynchronisationMode synchM) noexcept -> k4a_wired_sync_mode_t{

    switch (synchM) {
    case DCSynchronisationMode::Standalone:
        /** Neither 'Sync In' or 'Sync Out' connections are used. */
        return K4A_WIRED_SYNC_MODE_STANDALONE;
    case DCSynchronisationMode::Main:
        /**< The 'Sync Out' jack is enabled and synchronization data it driven out the
        connected wire. While in master mode the color camera must be enabled as part of the
        multi device sync signalling logic. Even if the color image is not needed, the color
        camera must be running.*/
        return K4A_WIRED_SYNC_MODE_MASTER;
    case DCSynchronisationMode::Subordinate:
        /**< The 'Sync In' jack is used for synchronization and 'Sync Out' is driven for the
        next device in the chain. 'Sync Out' is a mirror of 'Sync In' for this mode.
        */
        return K4A_WIRED_SYNC_MODE_SUBORDINATE;
    default:
        return K4A_WIRED_SYNC_MODE_STANDALONE;
    }
}

[[maybe_unused]] [[nodiscard]] static auto convert_to_k4a_body_tracking_processing_mode(DCBTProcessingMode pMode) noexcept -> k4abt_tracker_processing_mode_t{
    switch(pMode){
    case DCBTProcessingMode::GPU:
        /**< SDK will use the most appropriate GPU mode for the operating system to run the tracker.
         * Currently this is ONNX DirectML EP for Windows and ONNX Cuda EP for Linux. ONNX TensorRT EP is experimental */
        return K4ABT_TRACKER_PROCESSING_MODE_GPU;
    case DCBTProcessingMode::CPU:
        /**< SDK will use CPU only mode to run the tracker */
        return K4ABT_TRACKER_PROCESSING_MODE_CPU;
    case DCBTProcessingMode::GPU_CUDA:
        /**< SDK will use ONNX Cuda EP to run the tracker */
        return K4ABT_TRACKER_PROCESSING_MODE_GPU_CUDA;
    case DCBTProcessingMode::GPU_TENSORRT:
        /**< SDK will use ONNX TensorRT EP to run the tracker */
        return K4ABT_TRACKER_PROCESSING_MODE_GPU_TENSORRT;
    case DCBTProcessingMode::GPU_DIRECTML:
        /**< SDK will use ONNX DirectML EP to run the tracker (Windows only) */
        return K4ABT_TRACKER_PROCESSING_MODE_GPU_DIRECTML;
    default:
        return K4ABT_TRACKER_PROCESSING_MODE_CPU;
    }
}

[[maybe_unused]] [[nodiscard]] static auto convert_to_k4a_sensor_orientation(DCBTSensorOrientation sOrientation) noexcept -> k4abt_sensor_orientation_t{
    switch(sOrientation){
    case DCBTSensorOrientation::default_orientation:
        /**< Mount the sensor at its default orientation */
        return K4ABT_SENSOR_ORIENTATION_DEFAULT;
    case DCBTSensorOrientation::clockwise_90:
        /**< Clockwisely rotate the sensor 90 degree */
        return K4ABT_SENSOR_ORIENTATION_CLOCKWISE90;
    case DCBTSensorOrientation::counter_clockwise_90:
        /**< Counter-clockwisely rotate the sensor 90 degrees */
        return K4ABT_SENSOR_ORIENTATION_COUNTERCLOCKWISE90;
    case DCBTSensorOrientation::flip_180:
        /**< Mount the sensor upside-down */
        return K4ABT_SENSOR_ORIENTATION_FLIP180;
    default:
        return K4ABT_SENSOR_ORIENTATION_DEFAULT;
    }
}

[[maybe_unused]] [[nodiscard]] static auto convert_to_confidence_level(k4abt_joint_confidence_level_t cLevel) noexcept -> DCJointConfidenceLevel{
    switch(cLevel){
    case K4ABT_JOINT_CONFIDENCE_NONE:
        /** The joint is out of range (too far from depth camera) */
        return DCJointConfidenceLevel::None;
    case K4ABT_JOINT_CONFIDENCE_LOW:
        /**< The joint is not observed (likely due to occlusion), predicted joint pose */
        return DCJointConfidenceLevel::Low;
    case K4ABT_JOINT_CONFIDENCE_MEDIUM:
        /**< Medium confidence in joint pose. Current SDK will only provide joints up to this confidence level */
        return DCJointConfidenceLevel::Medium;
    case K4ABT_JOINT_CONFIDENCE_HIGH:
        /**< High confidence in joint pose. Placeholder for future SDK */
        return DCJointConfidenceLevel::Hight;
    default:
        return DCJointConfidenceLevel::None;
    }
}


static auto display_calibration(const k4a::calibration &calibration) -> void{

    const auto &c = calibration;
    const auto &color = c.color_camera_calibration;
    const auto &depth = c.depth_camera_calibration;

    std::cout << "CALIBRATION:\n";
    std::cout << "DEPTH MODE: " << (int)c.depth_mode << "\n";
    std::cout << "COLOR RESOLUTION: " << (int)c.color_resolution << "\n";

    std::cout << "### COLOR:\n";
    std::cout << "resolution_height: " << color.resolution_height << "\n";
    std::cout << "resolution_width: " << color.resolution_width << "\n";
    std::cout << "metric_radius: " << color.metric_radius << "\n";
    std::cout << "###### INTRINSIC:\n";
    const auto &ci = color.intrinsics.parameters.param;
    std::cout
              << "cx: "     <<ci.cx << " "
              << "cy: "     <<ci.cy << " "
              << "fx: "     <<ci.fx << " "
              << "fy: "     <<ci.fy << " "
              << "k1: "     <<ci.k1 << " "
              << "k2: "     <<ci.k2 << " "
              << "k3: "     <<ci.k3 << " "
              << "k4: "     <<ci.k4 << " "
              << "k5: "     <<ci.k5 << " "
              << "k6: "     <<ci.k6 << " "
              << "codx: "   <<ci.codx << " "
              << "cody: "   <<ci.cody << " "
              << "p1: "     <<ci.p1 << " "
              << "p2: "     <<ci.p2 << " "
              << "metric_radius: " <<ci.metric_radius << "\n";

    std::cout << "###### EXTRINSICS:\n";
    const auto &cd = color.extrinsics;
    std::cout
        << "t0: "     <<cd.translation[0] << " "
        << "t1: "     <<cd.translation[1] << " "
        << "t2: "     <<cd.translation[2] << " "
        << "r0: "     <<cd.rotation[0] << " "
        << "r1: "     <<cd.rotation[1] << " "
        << "r2: "     <<cd.rotation[2] << " "
        << "r3: "     <<cd.rotation[3] << " "
        << "r4: "     <<cd.rotation[4] << " "
        << "r5: "     <<cd.rotation[5] << " "
        << "r6: "     <<cd.rotation[6] << " "
        << "r7: "     <<cd.rotation[7] << " "
        << "r8: "     <<cd.rotation[8] << "\n";

    std::cout << "### DEPTH:\n";
    std::cout << "resolution_height: " << depth.resolution_height << "\n";
    std::cout << "resolution_width: " << depth.resolution_width << "\n";
    std::cout << "metric_radius: " << depth.metric_radius << "\n";
    std::cout << "###### INTRINSIC:\n";
    const auto &di = depth.intrinsics.parameters.param;
    std::cout
        << "cx: "     <<di.cx << " "
        << "cy: "     <<di.cy << " "
        << "fx: "     <<di.fx << " "
        << "fy: "     <<di.fy << " "
        << "k1: "     <<di.k1 << " "
        << "k2: "     <<di.k2 << " "
        << "k3: "     <<di.k3 << " "
        << "k4: "     <<di.k4 << " "
        << "k5: "     <<di.k5 << " "
        << "k6: "     <<di.k6 << " "
        << "codx: "   <<di.codx << " "
        << "cody: "   <<di.cody << " "
        << "p1: "     <<di.p1 << " "
        << "p2: "     <<di.p2 << " "
        << "metric_radius: " <<di.metric_radius << "\n";

    std::cout << "###### EXTRINSICS:\n";
    const auto &dd = depth.extrinsics;
    std::cout
        << "t0: "     <<dd.translation[0] << " "
        << "t1: "     <<dd.translation[1] << " "
        << "t2: "     <<dd.translation[2] << " "
        << "r0: "     <<dd.rotation[0] << " "
        << "r1: "     <<dd.rotation[1] << " "
        << "r2: "     <<dd.rotation[2] << " "
        << "r3: "     <<dd.rotation[3] << " "
        << "r4: "     <<dd.rotation[4] << " "
        << "r5: "     <<dd.rotation[5] << " "
        << "r6: "     <<dd.rotation[6] << " "
        << "r7: "     <<dd.rotation[7] << " "
        << "r8: "     <<dd.rotation[8] << "\n";

    const auto &e = c.extrinsics;
    const auto &ce = e[K4A_CALIBRATION_TYPE_COLOR];
    const auto &ce2d = ce[K4A_CALIBRATION_TYPE_DEPTH];
    std::cout << "EX COL-DEP TR:" << ce2d.translation[0] << " " << ce2d.translation[1] << " " << ce2d.translation[2] << " ROT: "
        << ce2d.rotation[0] << " " << ce2d.rotation[1] << " " << ce2d.rotation[2] << " "
        << ce2d.rotation[3] << " " << ce2d.rotation[4] << " " << ce2d.rotation[5] << " "
        << ce2d.rotation[6] << " " << ce2d.rotation[7] << " " << ce2d.rotation[8] << "\n";

    const auto &ce2c = ce[K4A_CALIBRATION_TYPE_COLOR];
    std::cout << "EX COL-COL TR:" << ce2c.translation[0] << " " << ce2c.translation[1] << " " << ce2c.translation[2] << " ROT: "
              << ce2c.rotation[0] << " " << ce2c.rotation[1] << " " << ce2c.rotation[2] << " "
              << ce2c.rotation[3] << " " << ce2c.rotation[4] << " " << ce2c.rotation[5] << " "
              << ce2c.rotation[6] << " " << ce2c.rotation[7] << " " << ce2c.rotation[8] << "\n";

    const auto &de = e[K4A_CALIBRATION_TYPE_DEPTH];
    const auto &de2c = de[K4A_CALIBRATION_TYPE_COLOR];
    std::cout << "EX DEP-COL TR:" << de2c.translation[0] << " " << de2c.translation[1] << " " << de2c.translation[2] << " ROT: "
              << de2c.rotation[0] << " " << de2c.rotation[1] << " " << de2c.rotation[2] << " "
              << de2c.rotation[3] << " " << de2c.rotation[4] << " " << de2c.rotation[5] << " "
              << de2c.rotation[6] << " " << de2c.rotation[7] << " " << de2c.rotation[8] << "\n";

    const auto &de2d = de[K4A_CALIBRATION_TYPE_DEPTH];
    std::cout << "EX DEP-DEP TR:" << de2d.translation[0] << " " << de2d.translation[1] << " " << de2d.translation[2] << " ROT: "
              << de2d.rotation[0] << " " << de2d.rotation[1] << " " << de2d.rotation[2] << " "
              << de2d.rotation[3] << " " << de2d.rotation[4] << " " << de2d.rotation[5] << " "
              << de2d.rotation[6] << " " << de2d.rotation[7] << " " << de2d.rotation[8] << "\n";



    // K4A_CALIBRATION_TYPE_DEPTH,        /**< Depth sensor */
    //     K4A_CALIBRATION_TYPE_COLOR,        /**< Color sensor */
    //     K4A_CALIBRATION_TYPE_GYRO,         /**< Gyroscope sensor */
    //     K4A_CALIBRATION_TYPE_ACCEL,        /**< Accelerometer sensor */

    // c.extrinsics;
    // k4a_calibration_extrinsics_t extrinsics[K4A_CALIBRATION_TYPE_NUM][K4A_CALIBRATION_TYPE_NUM];
}

}
