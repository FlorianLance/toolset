
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

// local
#include "geometry/point2.hpp"
#include "geometry/point4.hpp"
#include "utility/tuple_array.hpp"

namespace tool::camera {

using namespace std::literals::string_view_literals;

// Camera types
enum class DCType : std::int8_t{
    Kinect2 = 0,
    Kinect4,
    Orbbec,
    All,
    SizeEnum,
    K2 = Kinect2,
    K4 = Kinect4,
    OR = Orbbec
};

// Camera parameters
// # powerline frequency
enum class PowerlineFrequency : std::int8_t{
    F50=1, F60=2
};

// # image format
// ## kinect4
enum class K4ImageFormat : std::int8_t {
    MJPEG    = 0, // K4A_IMAGE_FORMAT_COLOR_MJPG   : The buffer for each image is encoded as a JPEG and can be decoded by a JPEG decoder.
    NV12     = 1, // K4A_IMAGE_FORMAT_COLOR_NV12   : NV12 images separate the luminance and chroma data such that all the luminance is at the beginning of the buffer, and the chroma lines follow immediately after.
    YUY2     = 2, // K4A_IMAGE_FORMAT_COLOR_YUY2   : YUY2 stores chroma and luminance data in interleaved pixels.
    BGRA32   = 3, // K4A_IMAGE_FORMAT_COLOR_BGRA32 : Each pixel of BGRA32 data is four bytes. The first three bytes represent Blue, Green, and Red data. The fourth byte is the alpha channel and is unused in the Azure Kinect APIs.
    IR16     = 5, // K4A_IMAGE_FORMAT_IR16         : Each pixel of IR16 data is two bytes of little endian unsigned depth data. The value of the data represents brightness.
    DEPTH16  = 4, // K4A_IMAGE_FORMAT_DEPTH16      : Each pixel of DEPTH16 data is two bytes of little endian unsigned depth data. The unit of the data is in millimeters from the origin of the camera.
    CUSTOM   = 8, // K4A_IMAGE_FORMAT_CUSTOM       : Used in conjunction with user created images or images packing non-standard data.
    CUSTOM8  = 6, // K4A_IMAGE_FORMAT_CUSTOM8      : Each pixel of CUSTOM8 is a single channel one byte of unsigned data.
    CUSTOM16 = 7, // K4A_IMAGE_FORMAT_CUSTOM16     : Each pixel of CUSTOM16 is a single channel two bytes of little endian unsigned data.
    SizeEnum
};
// ## orbbec
enum class OBImageFormat : std::int8_t {
    YUYV     = 0,    /**< YUYV format */
    YUY2     = 1,    /**< YUY2 format (the actual format is the same as YUYV) */
    UYVY     = 2,    /**< UYVY format */
    NV12     = 3,    /**< NV12 format */
    NV21     = 4,    /**< NV21 format */
    MJPG     = 5,    /**< MJPEG encoding format */
    H264     = 6,    /**< H.264 encoding format */
    H265     = 7,    /**< H.265 encoding format */
    Y16      = 8,    /**< Y16 format, single channel 16-bit depth */
    Y8       = 9,    /**< Y8 format, single channel 8-bit depth */
    Y10      = 10,   /**< Y10 format, single channel 10-bit depth (SDK will unpack into Y16 by default) */
    Y11      = 11,   /**< Y11 format, single channel 11-bit depth (SDK will unpack into Y16 by default) */
    Y12      = 12,   /**< Y12 format, single channel 12-bit depth (SDK will unpack into Y16 by default) */
    GRAY     = 13,   /**< GRAY (the actual format is the same as YUYV) */
    HEVC     = 14,   /**< HEVC encoding format (the actual format is the same as H265) */
    I420     = 15,   /**< I420 format */
    RGB      = 22,   /**< RGB format (actual RGB888)  */
    BGR      = 23,   /**< BGR format (actual BGR888) */
    Y14      = 24,   /**< Y14 format, single channel 14-bit depth (SDK will unpack into Y16 by default) */
    BGRA     = 25,   /**< BGRA format */
    SizeEnum
};
// ## all
enum class DCImageFormat : std::int8_t {
    NV12        = 0,
    YUY2        = 1,
    MJPG        = 2,
    BGRA        = 3,
    DEPTH16     = 4,
    SizeEnum
};

using TDCCorr = std::tuple<
    DCImageFormat,          K4ImageFormat,                 OBImageFormat>;
static constexpr TupleArray<DCImageFormat::SizeEnum, TDCCorr> dcDCK4Corr = {{
    // cloud
    TDCCorr
    {DCImageFormat::NV12,    K4ImageFormat::NV12,       OBImageFormat::NV12},
    {DCImageFormat::YUY2,    K4ImageFormat::YUY2,       OBImageFormat::YUY2},
    {DCImageFormat::MJPG,    K4ImageFormat::MJPEG,      OBImageFormat::MJPG},
    {DCImageFormat::BGRA,    K4ImageFormat::BGRA32,     OBImageFormat::BGRA},
    {DCImageFormat::DEPTH16, K4ImageFormat::DEPTH16,    OBImageFormat::Y16},
}};

[[maybe_unused]] static constexpr auto get_k4_image_format(DCImageFormat f) -> K4ImageFormat{
    return dcDCK4Corr.at<0,1>(f);
}
[[maybe_unused]] static constexpr auto get_ob_image_format(DCImageFormat f) -> OBImageFormat{
    return dcDCK4Corr.at<0,2>(f);
}

// # color resolution
enum class DCColorResolution : std::int8_t {
    OFF     = 0, // K4A_COLOR_RESOLUTION_OFF    : Color camera will be turned off with this setting
    R720P   = 1, // K4A_COLOR_RESOLUTION_720P   : 1280 * 720  16:9
    R1080P  = 2, // K4A_COLOR_RESOLUTION_1080P  : 1920 * 1080 16:9
    R1440P  = 3, // K4A_COLOR_RESOLUTION_1440P  : 2560 * 1440 16:9
    R1536P  = 4, // K4A_COLOR_RESOLUTION_1536P  : 2048 * 1536 4:3
    R2160P  = 5, // K4A_COLOR_RESOLUTION_2160P  : 3840 * 2160 16:9
    R3072P  = 6, // K4A_COLOR_RESOLUTION_3072P  : 4096 * 3072 4:3
    SizeEnum
};
using TDCColRes = std::tuple<
    DCColorResolution, int, int>;
static constexpr TupleArray<DCColorResolution::SizeEnum, TDCColRes> dcColRes = {{
    // cloud
    TDCColRes
    {DCColorResolution::OFF,    0,    0},
    {DCColorResolution::R720P,  1280, 720},
    {DCColorResolution::R1080P, 1920, 1080},
    {DCColorResolution::R1440P, 2560, 1440},
    {DCColorResolution::R1536P, 2048, 1536},
    {DCColorResolution::R2160P, 3840, 2160},
    {DCColorResolution::R3072P, 4096, 3072}
}};

[[maybe_unused]] static constexpr auto color_width(DCColorResolution r) -> int{
    return dcColRes.at<0,1>(r);
}

[[maybe_unused]] static constexpr auto color_height(DCColorResolution r) -> int{
    return dcColRes.at<0,2>(r);
}

// # color exposure time
enum class K4ExposureTimesMicroS : std::int8_t{
    t500,
    t1250,
    t2500,
    t8330,
    t16670,
    t33330
};
// ...

// # depth mdoe

enum class DCDepthMode : std::int8_t {
    OFF = 0,
    DI_320x288,
    DI_640x576,
    DI_512x512,
    DI_1024x1024,
    I_1024x1024,
    // KINECT4
    K4_OFF              = OFF,          // Depth sensor will be turned off with this setting.
    K4_NFOV_2X2BINNED   = DI_320x288,   // Depth captured at 320x288. Passive IR is also captured at 320x288.
    K4_NFOV_UNBINNED    = DI_640x576,   // Depth captured at 640x576. Passive IR is also captured at 640x576.
    K4_WFOV_2X2BINNED   = DI_512x512,   // Depth captured at 512x512. Passive IR is also captured at 512x512.
    K4_WFOV_UNBINNED    = DI_1024x1024, // Depth captured at 1024x1024. Passive IR is also captured at 1024x1024.
    K4_PASSIVE_IR       = I_1024x1024,  // Passive IR only, captured at 1024x1024.
};

// # framerate

enum class DCFramerate : std::int8_t{
    F5  = 0, // K4A_FRAMES_PER_SECOND_5  : 5 FPS
    F15 = 1, // K4A_FRAMES_PER_SECOND_15 : 15 FPS
    F30 = 2, // K4A_FRAMES_PER_SECOND_30 : 30 FPS
    SizeEnum
};
using TDCFramerate = std::tuple<
    DCFramerate, int>;
static constexpr TupleArray<DCFramerate::SizeEnum, TDCFramerate> dcFramerates = {{
    // cloud
    TDCFramerate
    {DCFramerate::F5,  5},
    {DCFramerate::F15, 15},
    {DCFramerate::F30, 30},
}};

[[maybe_unused]] static constexpr auto framerate_value(DCFramerate f) -> int{
    return dcFramerates.at<0,1>(f);
}

// camera mode
enum class DCMode : std::int8_t {
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

[[maybe_unused]] static constexpr DCMode dcDefaultMode = DCMode::Cloud_640x576_MJPEG;


using M   = DCMode;
using IF  = DCImageFormat;
using CR  = DCColorResolution;
using DM  = DCDepthMode;
using FPS = DCFramerate;
using CE = bool;
using ME = bool;
using IE = bool;
using Range = geo::Pt2f;
using Resolution = geo::Pt2<int>;
using DRes = Resolution;
using CRes = Resolution;
using Dev  = DCType;
static constexpr bool T = true;
static constexpr bool F = false;

using TDCMode = std::tuple<
    DCMode,                    IF,       CR,         DM,                    FPS,      Range,          DRes,        CE, ME, IE,  Dev>;
static constexpr TupleArray<DCMode::SizeEnum, TDCMode> dcModes = {{
    // cloud
    TDCMode
    {M::Cloud_320x288,         IF::NV12, CR::R720P,  DM::K4_NFOV_2X2BINNED, FPS::F30, {0.5f,5.46f},   {320,288},   T,  F,  T,   Dev::All},
    {M::Cloud_640x576_MJPEG,   IF::MJPG, CR::R720P,  DM::K4_NFOV_UNBINNED,  FPS::F30, {0.5f,3.86f},   {640,576},   T,  F,  T,   Dev::All},
    {M::Cloud_640x576_YUY2,    IF::YUY2, CR::R720P,  DM::K4_NFOV_UNBINNED,  FPS::F30, {0.5f,3.86f},   {640,576},   T,  F,  T,   Dev::All},
    {M::Cloud_640x576_NV12,    IF::NV12, CR::R720P,  DM::K4_NFOV_UNBINNED,  FPS::F30, {0.5f,3.86f},   {640,576},   T,  F,  T,   Dev::All},
    {M::Cloud_640x576_BGRA32,  IF::BGRA, CR::R720P,  DM::K4_NFOV_UNBINNED,  FPS::F30, {0.5f,3.86f},   {640,576},   T,  F,  T,   Dev::All},
    {M::Cloud_512x512,         IF::NV12, CR::R720P,  DM::K4_WFOV_2X2BINNED, FPS::F30, {0.25f,2.88f},  {512,512},   T,  F,  T,   Dev::All},
    {M::Cloud_1024x1024,       IF::NV12, CR::R720P,  DM::K4_WFOV_UNBINNED,  FPS::F15, {0.25f,2.21f},  {1024,1024}, T,  F,  T,   Dev::All},
    // frame
    {M::Full_frame_320x288,    IF::MJPG, CR::R720P,  DM::K4_NFOV_2X2BINNED, FPS::F30, {0.5f,5.46f},   {320,288},   F,  F,  T,   Dev::All},
    {M::Full_frame_640x576,    IF::MJPG, CR::R720P,  DM::K4_NFOV_UNBINNED,  FPS::F30, {0.5f,3.86f},   {640,576},   F,  F,  T,   Dev::All},
    {M::Full_frame_512x512,    IF::MJPG, CR::R720P,  DM::K4_WFOV_2X2BINNED, FPS::F30, {0.25f,2.88f},  {512,512},   F,  F,  T,   Dev::All},
    {M::Full_frame_1024x1024,  IF::MJPG, CR::R720P,  DM::K4_WFOV_UNBINNED,  FPS::F15, {0.25f,2.21f},  {1024,1024}, F,  F,  T,   Dev::All},
    // only color
    {M::Only_color_1280x720,   IF::BGRA, CR::R720P,  DM::K4_OFF,            FPS::F30, {0,0},          {0,0},       F,  F,  F,   Dev::All},
    {M::Only_color_1920x1080,  IF::BGRA, CR::R1080P, DM::K4_OFF,            FPS::F30, {0,0},          {0,0},       F,  F,  F,   Dev::All},
    {M::Only_color_2560x1440,  IF::BGRA, CR::R1440P, DM::K4_OFF,            FPS::F30, {0,0},          {0,0},       F,  F,  F,   Dev::All},
    {M::Only_color_2048x1536,  IF::BGRA, CR::R1536P, DM::K4_OFF,            FPS::F30, {0,0},          {0,0},       F,  F,  F,   Dev::All},
    {M::Only_color_3840x2160,  IF::BGRA, CR::R2160P, DM::K4_OFF,            FPS::F30, {0,0},          {0,0},       F,  F,  F,   Dev::All},
    {M::Only_color_4096x3072,  IF::BGRA, CR::R3072P, DM::K4_OFF,            FPS::F15, {0,0},          {0,0},       F,  F,  F,   Dev::All},
    // other
    {M::C2048x1536_D640x576,   IF::BGRA, CR::R1536P, DM::K4_NFOV_UNBINNED,  FPS::F15, {0.5f,3.86f},   {640,576},   F,  F,  T,   Dev::All},
    {M::C2048x1536_D1024x1024, IF::BGRA, CR::R1536P, DM::K4_WFOV_UNBINNED,  FPS::F15, {0.25f,2.21f},  {1024,1024}, F,  F,  T,   Dev::All},
    {M::C4096x3072_D640x576,   IF::BGRA, CR::R3072P, DM::K4_NFOV_UNBINNED,  FPS::F15, {0.5f,3.86f},   {640,576},   F,  F,  T,   Dev::All},
    {M::C4096x3072_D1024x1024, IF::BGRA, CR::R3072P, DM::K4_WFOV_UNBINNED,  FPS::F15, {0.25f,2.21f},  {1024,1024}, F,  F,  T,   Dev::All},
}};


[[maybe_unused]] static constexpr auto image_format(DCMode m) -> DCImageFormat{
    return dcModes.at<0,1>(m);
}
[[maybe_unused]] static constexpr auto color_resolution(DCMode m) -> DCColorResolution{
    return dcModes.at<0,2>(m);
}
[[maybe_unused]] static constexpr auto depth_mode(DCMode m) -> DCDepthMode {
    return dcModes.at<0,3>(m);
}
[[maybe_unused]] static constexpr auto framerate(DCMode m) -> DCFramerate{
    return dcModes.at<0,4>(m);
}
[[maybe_unused]] static constexpr auto range(DCMode m) -> Range{
    return dcModes.at<0,5>(m);
}
[[maybe_unused]] static constexpr auto depth_resolution(DCMode m) -> Resolution{
    return dcModes.at<0,6>(m);
}
[[maybe_unused]] static constexpr auto has_depth(DCMode m) -> bool{
    return depth_mode(m) != DM::OFF;
}
[[maybe_unused]] static constexpr auto has_cloud(DCMode m) -> bool{
    return dcModes.at<0,7>(m);
}
[[maybe_unused]] static constexpr auto has_mesh(DCMode m) -> bool{
    return dcModes.at<0,8>(m);
}
[[maybe_unused]] static constexpr auto has_infrared(DCMode m) -> bool{
    return dcModes.at<0,9>(m);
}
[[maybe_unused]] static constexpr auto get_device(DCMode m) -> DCType{
    return dcModes.at<0,10>(m);
}

[[maybe_unused]] static constexpr std::int16_t dc_invalid_depth_value = 0;
[[maybe_unused]] static constexpr std::int16_t dc_invalid_infra_value = 0;
[[maybe_unused]] static constexpr geo::Pt4<std::uint8_t> dc_invalid_color_value = {0,0,0,0};


// joints types
enum class DCJointType : std::int8_t{
    // K4 joints
    pelvis          = 0,  // K4ABT_JOINT_PELVIS,
    spine_navel     = 1,  // K4ABT_JOINT_SPINE_NAVEL,
    spine_chest     = 2,  // K4ABT_JOINT_SPINE_CHEST,
    neck            = 3,  // K4ABT_JOINT_NECK,
    clavicle_left   = 4,  // K4ABT_JOINT_CLAVICLE_LEFT,
    shoulder_left   = 5,  // K4ABT_JOINT_SHOULDER_LEFT,
    elbow_left      = 6,  // K4ABT_JOINT_ELBOW_LEFT,
    wrist_left      = 7,  // K4ABT_JOINT_WRIST_LEFT,
    hand_left       = 8,  // K4ABT_JOINT_HAND_LEFT,
    handtip_left    = 9,  // K4ABT_JOINT_HANDTIP_LEFT,
    thumb_left      = 10, // K4ABT_JOINT_THUMB_LEFT,
    clavicle_right  = 11, // K4ABT_JOINT_CLAVICLE_RIGHT,
    shoulder_right  = 12, // K4ABT_JOINT_SHOULDER_RIGHT,
    elbow_right     = 13, // K4ABT_JOINT_ELBOW_RIGHT,
    wrist_right     = 14, // K4ABT_JOINT_WRIST_RIGHT,
    hand_right      = 15, // K4ABT_JOINT_HAND_RIGHT,
    handtip_right   = 16, // K4ABT_JOINT_HANDTIP_RIGHT,
    thumb_right     = 17, // K4ABT_JOINT_THUMB_RIGHT,
    hip_left        = 18, // K4ABT_JOINT_HIP_LEFT,
    knee_left       = 19, // K4ABT_JOINT_KNEE_LEFT,
    ankle_left      = 20, // K4ABT_JOINT_ANKLE_LEFT,
    foot_left       = 21, // K4ABT_JOINT_FOOT_LEFT,
    hip_right       = 22, // K4ABT_JOINT_HIP_RIGHT,
    knee_right      = 23, // K4ABT_JOINT_KNEE_RIGHT,
    ankle_right     = 24, // K4ABT_JOINT_ANKLE_RIGHT,
    foot_right      = 25, // K4ABT_JOINT_FOOT_RIGHT,
    head            = 26, // K4ABT_JOINT_HEAD,
    nose            = 27, // K4ABT_JOINT_NOSE,
    eye_left        = 28, // K4ABT_JOINT_EYE_LEFT,
    ear_left        = 29, // K4ABT_JOINT_EAR_LEFT,
    eye_right       = 30, // K4ABT_JOINT_EYE_RIGHT,
    ear_right       = 31, // K4ABT_JOINT_EAR_RIGHT,
    SizeEnum
};
[[maybe_unused]] static constexpr size_t dcJointsCount = static_cast<size_t>(DCJointType::SizeEnum);

using TJoint = std::tuple<DCJointType, std::string_view>;
static constexpr TupleArray<DCJointType::SizeEnum,TJoint> dcJoints ={{
    TJoint
    {DCJointType::pelvis,           "pelvis"sv},
    {DCJointType::spine_navel,      "spine_navel"sv},
    {DCJointType::spine_chest,      "spine_chest"sv},
    {DCJointType::neck,             "neck"sv},
    {DCJointType::clavicle_left,    "clavicle_left"sv},
    {DCJointType::shoulder_left,    "shoulder_left"sv},
    {DCJointType::elbow_left,       "elbow_left"sv},
    {DCJointType::wrist_left,       "wrist_left"sv},
    {DCJointType::hand_left,        "hand_left"sv},
    {DCJointType::handtip_left,     "handtip_left"sv},
    {DCJointType::thumb_left,       "thumb_left"sv},
    {DCJointType::clavicle_right,   "clavicle_right"sv},
    {DCJointType::shoulder_right,   "shoulder_right"sv},
    {DCJointType::elbow_right,      "elbow_right"sv},
    {DCJointType::wrist_right,      "wrist_right"sv},
    {DCJointType::hand_right,       "hand_right"sv},
    {DCJointType::handtip_right,    "handtip_right"sv},
    {DCJointType::thumb_right,      "thumb_right"sv},
    {DCJointType::hip_left,         "hip_left"sv},
    {DCJointType::knee_left,        "knee_left"sv},
    {DCJointType::ankle_left,       "ankle_left"sv},
    {DCJointType::foot_left,        "foot_left"sv},
    {DCJointType::hip_right,        "hip_right"sv},
    {DCJointType::knee_right,       "knee_right"sv},
    {DCJointType::ankle_right,      "ankle_right"sv},
    {DCJointType::foot_right,       "foot_right"sv},
    {DCJointType::head,             "head"sv},
    {DCJointType::nose,             "nose"sv},
    {DCJointType::eye_left,         "eye_left"sv},
    {DCJointType::ear_left,         "ear_left"sv},
    {DCJointType::eye_right,        "eye_right"sv},
    {DCJointType::ear_right,        "ear_right"sv}
}};

[[maybe_unused]] static constexpr auto get_joint_name(DCJointType t) -> std::string_view{
    return dcJoints.at<0,1>(t);
}

enum class DCJointConfidenceLevel : std::int8_t{
    None   = 0, // K4ABT_JOINT_CONFIDENCE_NONE,   /**< The joint is out of range (too far from depth camera) */
    Low    = 1, // K4ABT_JOINT_CONFIDENCE_LOW,    /**< The joint is not observed (likely due to occlusion), predicted joint pose */
    Medium = 2, // K4ABT_JOINT_CONFIDENCE_MEDIUM, /**< Medium confidence in joint pose. Current SDK will only provide joints up to this confidence level */
    Hight  = 3  // K4ABT_JOINT_CONFIDENCE_HIGH,   /**< High confidence in joint pose. Placeholder for future SDK */
};

// # synch
enum class DCSynchronisationMode : std::int8_t{
    // KINECT4
    K4_Standalone  = 0, // K4A_WIRED_SYNC_MODE_STANDALONE, // Neither 'Sync In' or 'Sync Out' connections are used.
    K4_Master      = 1, // K4A_WIRED_SYNC_MODE_MASTER,     // The 'Sync Out' jack is enabled and synchronization data it driven out the connected wire. While in master mode the color camera must be enabled as part of the multi device sync signalling logic. Even if the color image is not needed, the color camera must be running.
    K4_Subordinate = 2, // K4A_WIRED_SYNC_MODE_SUBORDINATE // The 'Sync In' jack is used for synchronization and 'Sync Out' is driven for the next device in the chain. 'Sync Out' is a mirror of 'Sync In' for this mode.
}; // https://docs.microsoft.com/fr-FR/azure/Kinect-dk/multi-camera-sync

// # body tracking
enum class DCBTProcessingMode : std::int8_t{
    // KINECT 4
    K4_GPU             = 0, // K4ABT_TRACKER_PROCESSING_MODE_GPU,          /**< SDK will use the most appropriate GPU mode for the operating system to run the tracker. Currently this is ONNX DirectML EP for Windows and ONNX Cuda EP for Linux. ONNX TensorRT EP is experimental */
    K4_CPU             = 1, // K4ABT_TRACKER_PROCESSING_MODE_CPU,          /**< SDK will use CPU only mode to run the tracker */
    K4_GPU_CUDA        = 2, // K4ABT_TRACKER_PROCESSING_MODE_GPU_CUDA,     /**< SDK will use ONNX Cuda EP to run the tracker */
    K4_GPU_TENSORRT    = 3, // K4ABT_TRACKER_PROCESSING_MODE_GPU_TENSORRT, /**< SDK will use ONNX TensorRT EP to run the tracker */
    K4_GPU_DIRECTML    = 4, // K4ABT_TRACKER_PROCESSING_MODE_GPU_DIRECTML  /**< SDK will use ONNX DirectML EP to run the tracker (Windows only) */
};

enum class DCBTSensorOrientation : std::int8_t{
    // KINECT 4
    K4_default_orientation     = 0, // K4ABT_SENSOR_ORIENTATION_DEFAULT,            /**< Mount the sensor at its default orientation */
    K4_clockwise_90            = 1, // K4ABT_SENSOR_ORIENTATION_CLOCKWISE90,        /**< Clockwisely rotate the sensor 90 degree */
    K4_counter_clockwise_90    = 2, // K4ABT_SENSOR_ORIENTATION_COUNTERCLOCKWISE90, /**< Counter-clockwisely rotate the sensor 90 degrees */
    K4_flip_180                = 3, // K4ABT_SENSOR_ORIENTATION_FLIP180,            /**< Mount the sensor upside-down */
};



}
