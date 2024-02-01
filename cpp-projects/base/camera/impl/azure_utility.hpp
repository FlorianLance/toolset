
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

// k4a
// #include "k4a/k4a.h"
#include <k4a/k4atypes.h>

// kabt
#include <kabt/k4abt.hpp>

// local
#include "camera/dc_enums.hpp"

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
}
