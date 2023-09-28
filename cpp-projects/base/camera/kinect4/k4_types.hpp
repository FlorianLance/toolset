
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

#pragma once

// std
#include <format>

// kinect4
#include <k4a/k4atypes.h>
#include <kabt/k4abttypes.h>

// local
#include "geometry/point2.hpp"
#include "geometry/point4.hpp"
#include "geometry/quaternion.hpp"
#include "utility/tuple_array.hpp"

namespace tool::camera {

    using namespace std::literals::string_view_literals;

    enum class K4PowerlineFrequency : std::int8_t{
        F50=1, F60=2
    };

    enum class K4ColorControl : std::int8_t {
        Exposure_time_absolute = K4A_COLOR_CONTROL_EXPOSURE_TIME_ABSOLUTE, // Manual or Auto,
        // Exposure time is measured in microseconds.
        // The Azure Kinect supports a limited number of fixed expsore settings.
        // When setting this, expect the exposure to(in microseconds) be rounded up to the nearest setting. Exceptions are
        // 1) The last value in the table is the upper limit, so a value larger than this will be overridden to the largest entry in the table.
        // 2) The exposure time cannot be larger than the equivelent FPS. So expect 100ms exposure time to be reduced to 30ms or 33.33ms when the camera is
        //   started. The most recent copy of the table 'device_exposure_mapping' is in https://github.com/microsoft/Azure-Kinect-Sensor-SDK/blob/develop/src/color/color_priv.h
        // Default: 33330
        Exposure_priority = K4A_COLOR_CONTROL_AUTO_EXPOSURE_PRIORITY, // Only Manual DEPRECATED
        // Value of 0 means framerate priority. Value of 1 means exposure priority.
        // Using exposure priority may impact the framerate of both the color and depth cameras.
        Brightness = K4A_COLOR_CONTROL_BRIGHTNESS, // Only manual
        // The valid range is 0 to 255. The default value is 128.
        Contrast = K4A_COLOR_CONTROL_CONTRAST, // Only manual
        // 0 - 10 : 5
        Saturation = K4A_COLOR_CONTROL_SATURATION, // Only manual
        // 0 - 63 : 32
        Sharpness = K4A_COLOR_CONTROL_SHARPNESS, // Only manual
        // 0 - 4 : 2
        White_balance = K4A_COLOR_CONTROL_WHITEBALANCE, // Manual or Auto
        // The unit is degrees Kelvin.
        // The setting must be set to a value evenly divisible by 10 degrees.
        // 2500 - 12500 : 4500
        Backlight_compensation = K4A_COLOR_CONTROL_BACKLIGHT_COMPENSATION, // Only manual
        // Value of 0 means backlight compensation is disabled. Value of 1 means backlight compensation is enabled.
        // Default: 0
        Gain = K4A_COLOR_CONTROL_GAIN, // Only Manual
        // 0 - 255 : 128
        Powerline_frequency = K4A_COLOR_CONTROL_POWERLINE_FREQUENCY, // Only manual
        // Value of 1 sets the powerline compensation to 50 Hz. Value of 2 sets the powerline compensation to 60 Hz.
        // Default: 2
    };

    enum class K4ImageFormat : std::int8_t {
        MJPEG    = K4A_IMAGE_FORMAT_COLOR_MJPG,   // The buffer for each image is encoded as a JPEG and can be decoded by a JPEG decoder.
        NV12     = K4A_IMAGE_FORMAT_COLOR_NV12,   // NV12 images separate the luminance and chroma data such that all the luminance is at the
        // beginning of the buffer, and the chroma lines follow immediately after.
        YUY2     = K4A_IMAGE_FORMAT_COLOR_YUY2,   // YUY2 stores chroma and luminance data in interleaved pixels.
        BGRA32   = K4A_IMAGE_FORMAT_COLOR_BGRA32, // Each pixel of BGRA32 data is four bytes. The first three bytes represent Blue, Green,
        // and Red data. The fourth byte is the alpha channel and is unused in the Azure Kinect APIs.
        IR16     = K4A_IMAGE_FORMAT_IR16,         // Each pixel of IR16 data is two bytes of little endian unsigned depth data.
        // The value of the data represents brightness.
        DEPTH16  = K4A_IMAGE_FORMAT_DEPTH16,      // Each pixel of DEPTH16 data is two bytes of little endian unsigned depth data.
        // The unit of the data is in millimeters from the origin of the camera.
        CUSTOM   = K4A_IMAGE_FORMAT_CUSTOM,       // Used in conjunction with user created images or images packing non-standard data.
        CUSTOM8  = K4A_IMAGE_FORMAT_CUSTOM8,      // Each pixel of CUSTOM8 is a single channel one byte of unsigned data.
        CUSTOM16 = K4A_IMAGE_FORMAT_CUSTOM16      // Each pixel of CUSTOM16 is a single channel two bytes of little endian unsigned data.
    };

    enum class K4ColorResolution : std::int8_t {
        OFF     = K4A_COLOR_RESOLUTION_OFF,     // Color camera will be turned off with this setting
        R720P   = K4A_COLOR_RESOLUTION_720P,    // 1280 * 720  16:9
        R1080P  = K4A_COLOR_RESOLUTION_1080P,   // 1920 * 1080 16:9
        R1440P  = K4A_COLOR_RESOLUTION_1440P,   // 2560 * 1440 16:9
        R1536P  = K4A_COLOR_RESOLUTION_1536P,   // 2048 * 1536 4:3
        R2160P  = K4A_COLOR_RESOLUTION_2160P,   // 3840 * 2160 16:9
        R3072P  = K4A_COLOR_RESOLUTION_3072P,   // 4096 * 3072 4:3
        SizeEnum
    };

    using TColRes = std::tuple<
        K4ColorResolution, int, int>;
    static constexpr TupleArray<K4ColorResolution::SizeEnum, TColRes> k4ColRes = {{
        // cloud
        TColRes
        {K4ColorResolution::OFF,    0,    0},
        {K4ColorResolution::R720P,  1280, 720},
        {K4ColorResolution::R1080P, 1920, 1080},
        {K4ColorResolution::R1440P, 2560, 1440},
        {K4ColorResolution::R1536P, 2048, 1536},
        {K4ColorResolution::R2160P, 3840, 2160},
        {K4ColorResolution::R3072P, 4096, 3072}
    }};

    [[maybe_unused]] static constexpr auto color_width(K4ColorResolution r) -> int{
        return k4ColRes.at<0,1>(r);
    }

    [[maybe_unused]] static constexpr auto color_height(K4ColorResolution r) -> int{
        return k4ColRes.at<0,2>(r);
    }

    enum class K4DepthMode : std::int8_t {
        OFF            = K4A_DEPTH_MODE_OFF ,              // Depth sensor will be turned off with this setting.
        NFOV_2X2BINNED = K4A_DEPTH_MODE_NFOV_2X2BINNED,    // Depth captured at 320x288. Passive IR is also captured at 320x288.
        NFOV_UNBINNED  = K4A_DEPTH_MODE_NFOV_UNBINNED,     // Depth captured at 640x576. Passive IR is also captured at 640x576.
        WFOV_2X2BINNED = K4A_DEPTH_MODE_WFOV_2X2BINNED,    // Depth captured at 512x512. Passive IR is also captured at 512x512.
        WFOV_UNBINNED  = K4A_DEPTH_MODE_WFOV_UNBINNED,     // Depth captured at 1024x1024. Passive IR is also captured at 1024x1024.
        PASSIVE_IR     = K4A_DEPTH_MODE_PASSIVE_IR,        // Passive IR only, captured at 1024x1024.
    };

    enum class K4Framerate : std::int8_t{
        F5  = K4A_FRAMES_PER_SECOND_5,   // 5 FPS
        F15 = K4A_FRAMES_PER_SECOND_15,  // 15 FPS
        F30 = K4A_FRAMES_PER_SECOND_30,  // 30 FPS
    };

    using TFramerate = std::tuple<
        K4Framerate, int>;
    static constexpr TupleArray<K4ColorResolution::SizeEnum, TFramerate> k4Framerates = {{
        // cloud
        TFramerate
        {K4Framerate::F5,  5},
        {K4Framerate::F15, 15},
        {K4Framerate::F30, 30},

    }};

    [[maybe_unused]] static constexpr auto framerate_value(K4Framerate f) -> int{
        return k4Framerates.at<0,1>(f);
    }


    enum class K4SynchronisationMode : std::int8_t{
        Standalone  = K4A_WIRED_SYNC_MODE_STANDALONE, // Neither 'Sync In' or 'Sync Out' connections are used.
        Master      = K4A_WIRED_SYNC_MODE_MASTER,     // The 'Sync Out' jack is enabled and synchronization data it driven out the connected wire.
        // While in master mode the color camera must be enabled as part of the multi device sync signalling logic.
        // Even if the color image is not needed, the color camera must be running.
        Subordinate = K4A_WIRED_SYNC_MODE_SUBORDINATE // The 'Sync In' jack is used for synchronization and 'Sync Out' is driven for the
        // next device in the chain. 'Sync Out' is a mirror of 'Sync In' for this mode.
    };
    // https://docs.microsoft.com/fr-FR/azure/Kinect-dk/multi-camera-sync

    enum class K4Mode : std::int8_t {
        // clouds
        Cloud_320x288,
        Cloud_640x576_MJPEG,
        Cloud_640x576_YUY2,
        Cloud_640x576_NV12,
        Cloud_640x576_BGRA32,
        Cloud_512x512,
        Cloud_1024x1024,
        // frames
        Full_frame_320x288,
        Full_frame_640x576,
        Full_frame_512x512,
        Full_frame_1024x1024,
        // colors
        Only_color_1280x720,
        Only_color_1920x1080,
        Only_color_2560x1440,
        Only_color_2048x1536,
        Only_color_3840x2160,
        Only_color_4096x3072,
        //
        C2048x1536_D640x576,
        C2048x1536_D1024x1024,
        C4096x3072_D640x576,
        C4096x3072_D1024x1024,
        // custom
        Merged,
        SizeEnum
    };

    enum class K4ExposureTimesMicroS : std::int8_t{
        t500,
        t1250,
        t2500,
        t8330,
        t16670,
        t33330
    };


    using M   = K4Mode;
    using IF  = K4ImageFormat;
    using CR  = K4ColorResolution;
    using DM  = K4DepthMode;
    using FPS = K4Framerate;
    using CE = bool;
    using ME = bool;
    using IE = bool;
    using Range = geo::Pt2f;
    using Resolution = geo::Pt2<int>;
    using DRes = Resolution;
    using CRes = Resolution;

    using TMode = std::tuple<
        K4Mode,                   IF,         CR,         DM,                 FPS,      Range,          DRes,       CE,    ME,    IE>;
    static constexpr TupleArray<K4Mode::SizeEnum, TMode> k4Modes = {{
        // cloud
        TMode
        {M::Cloud_320x288,        IF::NV12,   CR::R720P,  DM::NFOV_2X2BINNED, FPS::F30, {0.5f,5.46f},   {320,288},  true,  false, true},
        {M::Cloud_640x576_MJPEG,  IF::MJPEG,  CR::R720P,  DM::NFOV_UNBINNED,  FPS::F30, {0.5f,3.86f},   {640,576},  true,  false, true},
        {M::Cloud_640x576_YUY2,   IF::YUY2,   CR::R720P,  DM::NFOV_UNBINNED,  FPS::F30, {0.5f,3.86f},   {640,576},  true,  false, true},
        {M::Cloud_640x576_NV12,   IF::NV12,   CR::R720P,  DM::NFOV_UNBINNED,  FPS::F30, {0.5f,3.86f},   {640,576},  true,  false, true},
        {M::Cloud_640x576_BGRA32, IF::BGRA32, CR::R720P,  DM::NFOV_UNBINNED,  FPS::F30, {0.5f,3.86f},   {640,576},  true,  false, true},
        {M::Cloud_512x512,        IF::NV12,   CR::R720P,  DM::WFOV_2X2BINNED, FPS::F30, {0.25f,2.88f},  {512,512},  true,  false, true},
        {M::Cloud_1024x1024,      IF::NV12,   CR::R720P,  DM::WFOV_UNBINNED,  FPS::F15, {0.25f,2.21f},  {1024,1024},true,  false, true},
        // frame
        {M::Full_frame_320x288,   IF::MJPEG,  CR::R720P,  DM::NFOV_2X2BINNED, FPS::F30, {0.5f,5.46f},   {320,288},  false, false, true},
        {M::Full_frame_640x576,   IF::MJPEG,  CR::R720P,  DM::NFOV_UNBINNED,  FPS::F30, {0.5f,3.86f},   {640,576},  false, false, true},
        {M::Full_frame_512x512,   IF::MJPEG,  CR::R720P,  DM::WFOV_2X2BINNED, FPS::F30, {0.25f,2.88f},  {512,512},  false, false, true},
        {M::Full_frame_1024x1024, IF::MJPEG,  CR::R720P,  DM::WFOV_UNBINNED,  FPS::F15, {0.25f,2.21f},  {1024,1024},false, false, true},
        // only color
        {M::Only_color_1280x720,  IF::BGRA32, CR::R720P,  DM::OFF,            FPS::F30, {0,0},          {0,0},      false, false, false},
        {M::Only_color_1920x1080, IF::BGRA32, CR::R1080P, DM::OFF,            FPS::F30, {0,0},          {0,0},      false, false, false},
        {M::Only_color_2560x1440, IF::BGRA32, CR::R1440P, DM::OFF,            FPS::F30, {0,0},          {0,0},      false, false, false},
        {M::Only_color_2048x1536, IF::BGRA32, CR::R1536P, DM::OFF,            FPS::F30, {0,0},          {0,0},      false, false, false},
        {M::Only_color_3840x2160, IF::BGRA32, CR::R2160P, DM::OFF,            FPS::F30, {0,0},          {0,0},      false, false, false},
        {M::Only_color_4096x3072, IF::BGRA32, CR::R3072P, DM::OFF,            FPS::F15, {0,0},          {0,0},      false, false, false},
        // other
        {M::C2048x1536_D640x576,    IF::BGRA32, CR::R1536P, DM::NFOV_UNBINNED,  FPS::F15, {0.5f,3.86f},   {640,576},  false, false, true},
        {M::C2048x1536_D1024x1024,  IF::BGRA32, CR::R1536P, DM::WFOV_UNBINNED,  FPS::F15, {0.25f,2.21f},   {1024,1024},  false, false, true},
        {M::C4096x3072_D640x576,    IF::BGRA32, CR::R3072P, DM::NFOV_UNBINNED,  FPS::F15, {0.5f,3.86f},   {640,576},  false, false, true},
        {M::C4096x3072_D1024x1024,  IF::BGRA32, CR::R3072P, DM::WFOV_UNBINNED,  FPS::F15, {0.25f,2.21f},   {1024,1024},  false, false, true}
    }};



//    3840x2160 16:9 MJPEG 0, 5, 15, 30 90°x59°
//    2560x1440 16:9 MJPEG 0, 5, 15, 30 90°x59°
//    1920x1080 16:9 MJPEG 0, 5, 15, 30 90°x59°
//    1280x720 16:9 MJPEG/YUY2/NV12 0, 5, 15, 30 90°x59°
//    4096x3072 4:3 MJPEG 0, 5, 15 90°x74.3°
//    2048x1536 4:3 MJPEG 0, 5, 15, 30 90°x74.3°
//    The RGB camera is USB Video class-compatible and can be used without the Sensor SDK. The RGB
//    camera color space: BT.601 full range [0..255]. The MJPEG chroma sub-sampling is 4:2:2.
//    Note
//    The Sensor SDK can provide color images in the BGRA pixel format. This is not a native mode
//    supported by the device and causes additional CPU load when used. The host CPU is used to
//    convert from MJPEG images received from the device.


    [[maybe_unused]] static constexpr auto image_format(K4Mode m) -> K4ImageFormat{
        return k4Modes.at<0,1>(m);
    }
    [[maybe_unused]] static constexpr auto color_resolution(K4Mode m) -> K4ColorResolution{
        return k4Modes.at<0,2>(m);
    }
    [[maybe_unused]] static constexpr auto depth_mode(K4Mode m) -> K4DepthMode {
        return k4Modes.at<0,3>(m);
    }
    [[maybe_unused]] static constexpr auto framerate(K4Mode m) -> K4Framerate{
        return k4Modes.at<0,4>(m);
    }
    [[maybe_unused]] static constexpr auto range(K4Mode m) -> Range{
        return k4Modes.at<0,5>(m);
    }
    [[maybe_unused]] static constexpr auto depth_resolution(K4Mode m) -> Resolution{
        return k4Modes.at<0,6>(m);
    }
    [[maybe_unused]] static constexpr auto has_depth(K4Mode m) -> bool{
        return depth_mode(m) != DM::OFF;
    }
    [[maybe_unused]] static constexpr auto has_cloud(K4Mode m) -> bool{
        return k4Modes.at<0,7>(m);
    }
    [[maybe_unused]] static constexpr auto has_mesh(K4Mode m) -> bool{
        return k4Modes.at<0,8>(m);
    }
    [[maybe_unused]] static constexpr auto has_infrared(K4Mode m) -> bool{
        return k4Modes.at<0,9>(m);
    }


    [[maybe_unused]] static constexpr std::int16_t k4_invalid_depth_value = 0;
    [[maybe_unused]] static constexpr std::int16_t k4_invalid_infra_value = 0;
    [[maybe_unused]] static constexpr geo::Pt4<std::uint8_t> k4_invalid_color_value = {0,0,0,0};
    [[maybe_unused]] static constexpr K4Mode k4DefaultMode = K4Mode::Cloud_640x576_MJPEG;

    struct K4ImuSample{
        float temperature;     /**< Temperature reading of this sample (Celsius). */
        geo::Pt3f acc;         /**< Accelerometer sample in meters per second squared. */
        std::int64_t accTsMs;  /**< Timestamp of the accelerometer in microseconds. */
        geo::Pt3f gyr;         /**< Gyro sample in radians per second. */
        std::int64_t gyrTsMs;  /**< Timestamp of the gyroscope in microseconds */
    };


    enum class SettingsType : std::int32_t{
        Network, Filters, Color, Device, Model,Delay,
        SizeEnum
    };

    using TSettings = std::tuple< SettingsType, std::string_view>;
    static constexpr TupleArray<SettingsType::SizeEnum, TSettings> settings = {{
        // cloud
        TSettings
        {SettingsType::Network, "network settings"},
        {SettingsType::Filters, "filters settings"},
        {SettingsType::Color,   "color settings"},
        {SettingsType::Device,  "device settings"},
        {SettingsType::Model,   "calibration"},
        {SettingsType::Delay,   "delay"},
    }};

    [[maybe_unused]] static constexpr auto settings_name(SettingsType s) -> std::string_view{
        return settings.at<0,1>(s);
    }


    struct K4VertexMeshData{
        geo::Pt3f pos;
        geo::Pt4<std::uint8_t> col;
    };

    enum class K4BTProcessingMode : std::int8_t{
        GPU             = K4ABT_TRACKER_PROCESSING_MODE_GPU,      /**< SDK will use the most appropriate GPU mode for the operating system to run the tracker */
                      /**< Currently this is ONNX DirectML EP for Windows and ONNX Cuda EP for Linux. ONNX TensorRT EP is experimental */
        CPU             = K4ABT_TRACKER_PROCESSING_MODE_CPU,          /**< SDK will use CPU only mode to run the tracker */
        GPU_CUDA        = K4ABT_TRACKER_PROCESSING_MODE_GPU_CUDA,     /**< SDK will use ONNX Cuda EP to run the tracker */
        GPU_TENSORRT    = K4ABT_TRACKER_PROCESSING_MODE_GPU_TENSORRT, /**< SDK will use ONNX TensorRT EP to run the tracker */
        GPU_DIRECTML    = K4ABT_TRACKER_PROCESSING_MODE_GPU_DIRECTML  /**< SDK will use ONNX DirectML EP to run the tracker (Windows only) */
    };

    enum class K4BTSensorOrientation : std::int8_t{
        default_orientation     = K4ABT_SENSOR_ORIENTATION_DEFAULT,            /**< Mount the sensor at its default orientation */
        clockwise_90            = K4ABT_SENSOR_ORIENTATION_CLOCKWISE90,        /**< Clockwisely rotate the sensor 90 degree */
        counter_clockwise_90    = K4ABT_SENSOR_ORIENTATION_COUNTERCLOCKWISE90, /**< Counter-clockwisely rotate the sensor 90 degrees */
        flip_180                = K4ABT_SENSOR_ORIENTATION_FLIP180,            /**< Mount the sensor upside-down */
    };

    enum class K4JointType : std::int8_t{
        pelvis          = K4ABT_JOINT_PELVIS,
        spine_navel     = K4ABT_JOINT_SPINE_NAVEL,
        spine_chest     = K4ABT_JOINT_SPINE_CHEST,
        neck            = K4ABT_JOINT_NECK,
        clavicle_left   = K4ABT_JOINT_CLAVICLE_LEFT,
        shoulder_left   = K4ABT_JOINT_SHOULDER_LEFT,
        elbow_left      = K4ABT_JOINT_ELBOW_LEFT,
        wrist_left      = K4ABT_JOINT_WRIST_LEFT,
        hand_left       = K4ABT_JOINT_HAND_LEFT,
        handtip_left    = K4ABT_JOINT_HANDTIP_LEFT,
        thumb_left      = K4ABT_JOINT_THUMB_LEFT,
        clavicle_right  = K4ABT_JOINT_CLAVICLE_RIGHT,
        shoulder_right  = K4ABT_JOINT_SHOULDER_RIGHT,
        elbow_right     = K4ABT_JOINT_ELBOW_RIGHT,
        wrist_right     = K4ABT_JOINT_WRIST_RIGHT,
        hand_right      = K4ABT_JOINT_HAND_RIGHT,
        handtip_right   = K4ABT_JOINT_HANDTIP_RIGHT,
        thumb_right     = K4ABT_JOINT_THUMB_RIGHT,
        hip_left        = K4ABT_JOINT_HIP_LEFT,
        knee_left       = K4ABT_JOINT_KNEE_LEFT,
        ankle_left      = K4ABT_JOINT_ANKLE_LEFT,
        foot_left       = K4ABT_JOINT_FOOT_LEFT,
        hip_right       = K4ABT_JOINT_HIP_RIGHT,
        knee_right      = K4ABT_JOINT_KNEE_RIGHT,
        ankle_right     = K4ABT_JOINT_ANKLE_RIGHT,
        foot_right      = K4ABT_JOINT_FOOT_RIGHT,
        head            = K4ABT_JOINT_HEAD,
        nose            = K4ABT_JOINT_NOSE,
        eye_left        = K4ABT_JOINT_EYE_LEFT,
        ear_left        = K4ABT_JOINT_EAR_LEFT,
        eye_right       = K4ABT_JOINT_EYE_RIGHT,
        ear_right       = K4ABT_JOINT_EAR_RIGHT,
        SizeEnum
    };

    [[maybe_unused]] static constexpr size_t k4JointsCount = static_cast<size_t>(K4JointType::SizeEnum);

    using TJoint = std::tuple<K4JointType, std::string_view>;
    static constexpr TupleArray<K4JointType::SizeEnum,TJoint> joints ={{
        TJoint
        {K4JointType::pelvis, "pelvis"sv},
        {K4JointType::spine_navel, "spine_navel"sv},
        {K4JointType::spine_chest, "spine_chest"sv},
        {K4JointType::neck, "neck"sv},
        {K4JointType::clavicle_left, "clavicle_left"sv},
        {K4JointType::shoulder_left, "shoulder_left"sv},
        {K4JointType::elbow_left, "elbow_left"sv},
        {K4JointType::wrist_left, "wrist_left"sv},
        {K4JointType::hand_left, "hand_left"sv},
        {K4JointType::handtip_left, "handtip_left"sv},
        {K4JointType::thumb_left, "thumb_left"sv},
        {K4JointType::clavicle_right, "clavicle_right"sv},
        {K4JointType::shoulder_right, "shoulder_right"sv},
        {K4JointType::elbow_right, "elbow_right"sv},
        {K4JointType::wrist_right, "wrist_right"sv},
        {K4JointType::hand_right, "hand_right"sv},
        {K4JointType::handtip_right, "handtip_right"sv},
        {K4JointType::thumb_right, "thumb_right"sv},
        {K4JointType::hip_left, "hip_left"sv},
        {K4JointType::knee_left, "knee_left"sv},
        {K4JointType::ankle_left, "ankle_left"sv},
        {K4JointType::foot_left, "foot_left"sv},
        {K4JointType::hip_right, "hip_right"sv},
        {K4JointType::knee_right, "knee_right"sv},
        {K4JointType::ankle_right, "ankle_right"sv},
        {K4JointType::foot_right, "foot_right"sv},
        {K4JointType::head, "head"sv},
        {K4JointType::nose, "nose"sv},
        {K4JointType::eye_left, "eye_left"sv},
        {K4JointType::ear_left, "ear_left"sv},
        {K4JointType::eye_right, "eye_right"sv},
        {K4JointType::ear_right, "ear_right"sv}
    }};

    [[maybe_unused]] static constexpr std::string_view get_joint_name(K4JointType t) {
        return joints.at<0,1>(t);
    }

    enum class K4JointConfidenceLevel : std::int8_t{
        None   = K4ABT_JOINT_CONFIDENCE_NONE,   /**< The joint is out of range (too far from depth camera) */
        Low    = K4ABT_JOINT_CONFIDENCE_LOW,    /**< The joint is not observed (likely due to occlusion), predicted joint pose */
        Medium = K4ABT_JOINT_CONFIDENCE_MEDIUM, /**< Medium confidence in joint pose. Current SDK will only provide joints up to this confidence level */
        Hight  = K4ABT_JOINT_CONFIDENCE_HIGH,   /**< High confidence in joint pose. Placeholder for future SDK */
    };

    struct K4Joint{
        geo::Pt3f position = {};
        geo::Quaternion<float> orientation = {};
        K4JointConfidenceLevel confidence = K4JointConfidenceLevel::None;

        constexpr auto good_confidence() const noexcept{
            return confidence == K4JointConfidenceLevel::Hight || confidence == K4JointConfidenceLevel::Medium;
        }
    };

    // Define the bone list based on the documentation
    [[maybe_unused]] static constexpr std::array<std::pair<K4JointType, K4JointType>, 31> bonesList = {
        std::make_pair(K4JointType::spine_chest,    K4JointType::spine_navel),
        std::make_pair(K4JointType::spine_navel,    K4JointType::pelvis),
        std::make_pair(K4JointType::spine_chest,    K4JointType::neck),
        std::make_pair(K4JointType::neck,           K4JointType::head),
        std::make_pair(K4JointType::head,           K4JointType::nose),
        std::make_pair(K4JointType::spine_chest,    K4JointType::clavicle_left),
        std::make_pair(K4JointType::clavicle_left,  K4JointType::shoulder_left),
        std::make_pair(K4JointType::shoulder_left,  K4JointType::elbow_left),
        std::make_pair(K4JointType::elbow_left,     K4JointType::wrist_left),
        std::make_pair(K4JointType::wrist_left,     K4JointType::hand_left),
        std::make_pair(K4JointType::hand_left,      K4JointType::handtip_left),
        std::make_pair(K4JointType::wrist_left,     K4JointType::thumb_left),
        std::make_pair(K4JointType::pelvis,         K4JointType::hip_left),
        std::make_pair(K4JointType::hip_left,       K4JointType::knee_left),
        std::make_pair(K4JointType::knee_left,      K4JointType::ankle_left),
        std::make_pair(K4JointType::ankle_left,     K4JointType::foot_left),
        std::make_pair(K4JointType::nose,           K4JointType::eye_left),
        std::make_pair(K4JointType::eye_left,       K4JointType::ear_left),
        std::make_pair(K4JointType::spine_chest,    K4JointType::clavicle_right),
        std::make_pair(K4JointType::clavicle_right, K4JointType::shoulder_right),
        std::make_pair(K4JointType::shoulder_right, K4JointType::elbow_right),
        std::make_pair(K4JointType::elbow_right,    K4JointType::wrist_right),
        std::make_pair(K4JointType::wrist_right,    K4JointType::hand_right),
        std::make_pair(K4JointType::hand_right,     K4JointType::handtip_right),
        std::make_pair(K4JointType::wrist_right,    K4JointType::thumb_right),
        std::make_pair(K4JointType::pelvis,         K4JointType::hand_right),
        std::make_pair(K4JointType::hip_right,      K4JointType::knee_right),
        std::make_pair(K4JointType::knee_right,     K4JointType::ankle_right),
        std::make_pair(K4JointType::ankle_right,    K4JointType::foot_right),
        std::make_pair(K4JointType::nose,           K4JointType::eye_right),
        std::make_pair(K4JointType::eye_right,      K4JointType::ear_right)
    };

    struct K4Skeleton{
        std::array<K4Joint, static_cast<std::int8_t>(K4JointType::SizeEnum)> joints;
        constexpr auto joint(K4JointType type) const noexcept -> K4Joint { return joints[static_cast<std::int8_t>(type)];}
        constexpr auto pelvis() const noexcept -> K4Joint        { return joints[static_cast<std::int8_t>(K4JointType::pelvis)];}
        constexpr auto spine_navel() const noexcept -> K4Joint   { return joints[static_cast<std::int8_t>(K4JointType::spine_navel)];}
        constexpr auto spine_chest() const noexcept -> K4Joint   { return joints[static_cast<std::int8_t>(K4JointType::spine_chest)];}
        constexpr auto neck() const noexcept -> K4Joint          { return joints[static_cast<std::int8_t>(K4JointType::neck)];}
        constexpr auto clavicle_left() const noexcept -> K4Joint { return joints[static_cast<std::int8_t>(K4JointType::clavicle_left)];}
        constexpr auto shoulder_left() const noexcept -> K4Joint { return joints[static_cast<std::int8_t>(K4JointType::shoulder_left)];}
        constexpr auto elbow_left() const noexcept -> K4Joint    { return joints[static_cast<std::int8_t>(K4JointType::elbow_left)];}
        constexpr auto wrist_left() const noexcept -> K4Joint    { return joints[static_cast<std::int8_t>(K4JointType::wrist_left)];}
        constexpr auto hand_left() const noexcept -> K4Joint     { return joints[static_cast<std::int8_t>(K4JointType::hand_left)];}
        constexpr auto handtip_left() const noexcept -> K4Joint  { return joints[static_cast<std::int8_t>(K4JointType::handtip_left)];}
        constexpr auto thumb_left() const noexcept -> K4Joint    { return joints[static_cast<std::int8_t>(K4JointType::thumb_left)];}
        constexpr auto clavicle_right() const noexcept -> K4Joint{ return joints[static_cast<std::int8_t>(K4JointType::clavicle_right)];}
        constexpr auto shoulder_right() const noexcept -> K4Joint{ return joints[static_cast<std::int8_t>(K4JointType::shoulder_right)];}
        constexpr auto elbow_right() const noexcept -> K4Joint   { return joints[static_cast<std::int8_t>(K4JointType::elbow_right)];}
        constexpr auto wrist_right() const noexcept -> K4Joint   { return joints[static_cast<std::int8_t>(K4JointType::wrist_right)];}
        constexpr auto hand_right() const noexcept -> K4Joint    { return joints[static_cast<std::int8_t>(K4JointType::hand_right)];}
        constexpr auto handtip_right() const noexcept -> K4Joint { return joints[static_cast<std::int8_t>(K4JointType::handtip_right)];}
        constexpr auto thumb_right() const noexcept -> K4Joint   { return joints[static_cast<std::int8_t>(K4JointType::thumb_right)];}
        constexpr auto hip_left() const noexcept -> K4Joint      { return joints[static_cast<std::int8_t>(K4JointType::hip_left)];}
        constexpr auto knee_left() const noexcept -> K4Joint     { return joints[static_cast<std::int8_t>(K4JointType::knee_left)];}
        constexpr auto ankle_left() const noexcept -> K4Joint    { return joints[static_cast<std::int8_t>(K4JointType::ankle_left)];}
        constexpr auto foot_left() const noexcept -> K4Joint     { return joints[static_cast<std::int8_t>(K4JointType::foot_left)];}
        constexpr auto hip_right() const noexcept -> K4Joint     { return joints[static_cast<std::int8_t>(K4JointType::hip_right)];}
        constexpr auto knee_right() const noexcept -> K4Joint    { return joints[static_cast<std::int8_t>(K4JointType::knee_right)];}
        constexpr auto ankle_right() const noexcept -> K4Joint   { return joints[static_cast<std::int8_t>(K4JointType::ankle_right)];}
        constexpr auto foot_right() const noexcept -> K4Joint    { return joints[static_cast<std::int8_t>(K4JointType::foot_right)];}
        constexpr auto head() const noexcept -> K4Joint          { return joints[static_cast<std::int8_t>(K4JointType::head)];}
        constexpr auto nose() const noexcept -> K4Joint          { return joints[static_cast<std::int8_t>(K4JointType::nose)];}
        constexpr auto eye_left() const noexcept -> K4Joint      { return joints[static_cast<std::int8_t>(K4JointType::eye_left)];}
        constexpr auto ear_left() const noexcept -> K4Joint      { return joints[static_cast<std::int8_t>(K4JointType::ear_left)];}
        constexpr auto eye_right() const noexcept -> K4Joint     { return joints[static_cast<std::int8_t>(K4JointType::eye_right)];}
        constexpr auto ear_right() const noexcept -> K4Joint     { return joints[static_cast<std::int8_t>(K4JointType::ear_right)];}
    };

    struct K4Body{
        std::int8_t id = 0;
        K4Skeleton skeleton;

        auto update(const k4abt_body_t &body) -> void{
            id = static_cast<std::int8_t>(body.id);
            for(const auto &jointD : joints.data){
                const auto &kaKoint = body.skeleton.joints[static_cast<int>(std::get<0>(jointD))];
                auto &joint = skeleton.joints[static_cast<int>(std::get<0>(jointD))];
                joint.confidence = static_cast<K4JointConfidenceLevel>(kaKoint.confidence_level);
                const auto &p = kaKoint.position;
                joint.position = {-p.v[0],-p.v[1],p.v[2]};
                const auto &o = kaKoint.orientation;
                joint.orientation = {o.wxyz.x,o.wxyz.y,o.wxyz.z,o.wxyz.w};
            }
        }
    };
}
