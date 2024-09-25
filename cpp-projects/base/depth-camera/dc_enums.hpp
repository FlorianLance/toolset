
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

namespace tool::cam {

using namespace std::literals::string_view_literals;

// Camera types
enum class DCType : std::int8_t{
    AzureKinect = 0,
    FemtoBolt,
    FemtoMegaEthernet,
    FemtoMegaUSB,
    Kinect2,
    Recording,
    Undefined,
    SizeEnum,
    AK  = AzureKinect,
    K2  = Kinect2,
    FB  = FemtoBolt,
    FME  = FemtoMegaEthernet,
    FMU  = FemtoMegaUSB,
    REC = Recording,
    UNDEF = Undefined
};

// Camera parameters
// # powerline frequency
enum class DCPowerlineFrequency : std::int8_t{
    Undefined = 0,
    F50,
    F60
};


// ## all
enum class DCImageFormat : std::int8_t {
    NV12        = 0,
    YUY2        = 1,
    MJPG        = 2,
    BGRA        = 3,
    DEPTH16     = 4,
    INFRA16     = 5,
    NA          = 6,
    SizeEnum
};

// # color resolution
enum class DCColorResolution : std::int8_t {
    OFF = 0, // Color camera will be turned off with this setting
    R720P,   // 1280 * 720  16:9
    R960P,   // 1280 * 960  4:3
    R1080P,  // 1920 * 1080 16:9
    R1440P,  // 2560 * 1440 16:9
    R1536P,  // 2048 * 1536 4:3
    R2160P,  // 3840 * 2160 16:9
    R3072P,  // 4096 * 3072 4:3
    SizeEnum,
};

enum class DCCompressionMode : std::int8_t{
    None = 0,
    JPEG,
    FastPFor
};

using TDCColRes = std::tuple<
    DCColorResolution,          int, int>;
static constexpr TupleArray<DCColorResolution::SizeEnum, TDCColRes> dcColRes = {{
    TDCColRes
    {DCColorResolution::OFF,    0,    0},
    {DCColorResolution::R720P,  1280, 720},
    {DCColorResolution::R960P,  1280, 960},
    {DCColorResolution::R1080P, 1920, 1080},
    {DCColorResolution::R1440P, 2560, 1440},
    {DCColorResolution::R1536P, 2048, 1536},
    {DCColorResolution::R2160P, 3840, 2160},
    {DCColorResolution::R3072P, 4096, 3072}
}};

[[maybe_unused]] static constexpr auto dc_color_width(DCColorResolution r) -> int{
    return dcColRes.at<0,1>(r);
}

[[maybe_unused]] static constexpr auto dc_color_height(DCColorResolution r) -> int{
    return dcColRes.at<0,2>(r);
}

[[maybe_unused]] static constexpr auto dc_color_size(DCColorResolution r) -> int{
    return dc_color_width(r)*dc_color_height(r);
}

// enum class K4AExposureTimesMicroS : std::int8_t{
//     t500,
//     t1250,
//     t2500,
//     t8330,
//     t16670,
//     t33330
// };

enum class DCFramerate : std::int8_t{
    F5 = 0,
    F15,
    F30,
    Undefined,
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
    {DCFramerate::Undefined, 0},
}};

[[maybe_unused]] static constexpr auto dc_framerate_value(DCFramerate f) -> int{
    return dcFramerates.at<0,1>(f);
}

enum class DCDepthResolution : std::int8_t {
    OFF = 0,
    K4A_320x288,
    K4A_640x576,
    K4A_512x512,
    K4A_1024x1024,
    SizeEnum,
};

using Range = geo::Pt2f;
using Res   = geo::Pt2<int>;
using MaxFPS= DCFramerate;
using FPS   = DCFramerate;
using DRes  = DCDepthResolution;
using FovH        = int;
using FovV        = int;

using TDCDepthRes = std::tuple<
    DRes,                   Res,         Range,          MaxFPS,        FovH,  FovV>;
static constexpr TupleArray<DCDepthResolution::SizeEnum, TDCDepthRes> dcDepthRes = {{
    // cloud
    TDCDepthRes
    {DRes::OFF,             {0,0},       {0.f,0.f},      FPS::Undefined, 0,     0},
    {DRes::K4A_320x288,     {320,288},   {0.5f,5.46f},   FPS::F30,       75,    65},
    {DRes::K4A_640x576,     {640,576},   {0.5f,3.86f},   FPS::F30,       75,    65},
    {DRes::K4A_512x512,     {512,512},   {0.25f,2.88f},  FPS::F30,       120,   120},
    {DRes::K4A_1024x1024,   {1024,1024}, {0.25f,2.21f},  FPS::F15,       120,   120},
}};

[[nodiscard]] [[maybe_unused]] static constexpr auto dc_depth_width(DCDepthResolution r) -> int{
    return dcDepthRes.at<0,1>(r).x();
}

[[nodiscard]] [[maybe_unused]] static constexpr auto dc_depth_height(DCDepthResolution r) -> int{
    return dcDepthRes.at<0,1>(r).y();
}

[[nodiscard]] [[maybe_unused]] static constexpr auto dc_depth_size(DCDepthResolution r) -> int{
    return dc_depth_width(r)*dc_depth_height(r);
}

[[nodiscard]] [[maybe_unused]] static constexpr auto dc_depth_range(DCDepthResolution r) -> Range{
    return dcDepthRes.at<0,2>(r);
}

[[nodiscard]] [[maybe_unused]] static constexpr auto dc_max_fps(DCDepthResolution r) -> FPS{
    return dcDepthRes.at<0,3>(r);
}

[[nodiscard]] [[maybe_unused]] static constexpr auto dc_depth_h_fov(DCDepthResolution r) -> int{
    return dcDepthRes.at<0,4>(r);
}

[[nodiscard]] [[maybe_unused]] static constexpr auto dc_depth_v_fov(DCDepthResolution r) -> int{
    return dcDepthRes.at<0,5>(r);
}

// camera mode
enum class DCMode : std::int8_t {

    // azure kinect
    // # narrow
    // ## 4:3 color
    AK_C2048x1536_DI640x576_MJPG_F30,
    AK_C4096x3072_DI640x576_MJPG_F15,
    // ## 16:9 color
    AK_C1280x720_DI320x288_NV12_F30,
    AK_C1280x720_DI640x576_NV12_F30,
    AK_C1280x720_DI640x576_YUY2_F30,
    AK_C1280x720_DI640x576_MJPG_F30,
    // # wide
    // ## 4:3 color
    AK_C2048x1536_DI512x512_MJPG_F30,
    AK_C2048x1536_DI1024x1024_MJPG_F30,
    // ## 16:9 color
    AK_C1280x720_DI512x512_NV12_F30,
    AK_C1280x720_DI512x512_YUY2_F30,
    AK_C1280x720_DI512x512_MJPG_F30,
    AK_C1280x720_DI1024x1024_NV12_F15,
    AK_C1280x720_DI1024x1024_YUY2_F15,
    AK_C1280x720_DI1024x1024_MJPG_F15,
    // # no depth
    // ## 4:3 color
    AK_C2048x1536_MJPG_F30,
    AK_C4096x3072_MJPG_F15,
    // ## 19:9 color
    AK_C1280x720_NV12_F30,
    AK_C1920x1080_MJPG_F30,
    AK_C2560x1440_MJPG_F30,
    AK_C3840x2160_MJPG_F15,

    // femto bolt orbbec
    // # narrow
    // ## 4:3 color
    FB_C2048x1536_DI640x576_MJPG_F30,
    FB_C4096x3072_DI640x576_MJPG_F15,
    // ## 16:9 color
    FB_C1280x720_DI320x288_NV12_F30,
    FB_C1280x720_DI640x576_NV12_F30,      
    // # wide
    // ## 4:3 color
    FB_C2048x1536_DI512x512_MJPG_F30,
    // ## 16:9 color
    FB_C1280x720_DI512x512_NV12_F30,
    // ## no color
    FB_DI512x512_F30,
    // # no depth
    // ## 4:3 color
    // ## 16:9 color
    FB_C1280x720_MJPG_F30,

    // femto mega orbbec ethernet
    // # narrow
    // ## 4:3 color
    // ...
    // ## 16:9 color
    FME_C1280x720_DI640x576_MJPG_F30,
    // # wide
    // ## 4:3 color
    // ...
    // ## 16:9 color
    FME_C1280x720_DI512x512_MJPG_F30,
    // ## no color
    FME_DI512x512_F30,
    // # no depth
    // ## 4:3 color
    // ...
    // ## 16:9 color
    FME_C1280x720_MJPG_F30,
    // femto mega orbbec usb
    // # narrow
    // ## 16:9 color
    FMU_C1280x720_DI640x576_MJPG_F30,

    // other
    Invalid,
    Merged,
    SizeEnum,
};


using M     = DCMode;
using IF    = DCImageFormat;
using CR    = DCColorResolution;
using DR    = DCDepthResolution;
using Dev   = DCType;
using Name  = std::string_view;

using TDCMode = std::tuple<
    DCMode,                                 IF,       CR,         DR,                  FPS,      Dev,       Name>;
static constexpr TupleArray<DCMode::SizeEnum, TDCMode> dcModes = {{
    // azure kinect
    TDCMode
    // azure kinect
    // # narrow
    // ## 4:3 color
    {M::AK_C2048x1536_DI640x576_MJPG_F30,   IF::MJPG, CR::R1536P, DR::K4A_640x576,     FPS::F30, Dev::AK,   "C-1536p D-576p  I-MJPG F-30"sv},
    {M::AK_C4096x3072_DI640x576_MJPG_F15,   IF::MJPG, CR::R3072P, DR::K4A_640x576,     FPS::F15, Dev::AK,   "C-3072p D-576p  I-MJPG F-15"sv},
    // ## 16:9 color
    {M::AK_C1280x720_DI320x288_NV12_F30,    IF::NV12, CR::R720P,  DR::K4A_320x288,     FPS::F30, Dev::AK,   "C-720p  D-288p  I-NV12 F-30"sv},
    {M::AK_C1280x720_DI640x576_NV12_F30,    IF::NV12, CR::R720P,  DR::K4A_640x576,     FPS::F30, Dev::AK,   "C-720p  D-576p  I-NV12 F-30"sv},
    {M::AK_C1280x720_DI640x576_YUY2_F30,    IF::YUY2, CR::R720P,  DR::K4A_640x576,     FPS::F30, Dev::AK,   "C-720p  D-576p  I-YUY2 F-30"sv},
    {M::AK_C1280x720_DI640x576_MJPG_F30,    IF::MJPG, CR::R720P,  DR::K4A_640x576,     FPS::F30, Dev::AK,   "C-720p  D-576p  I-MJPG F-30"sv},
    // # wide
    // ## 4:3 color
    {M::AK_C2048x1536_DI512x512_MJPG_F30,   IF::MJPG, CR::R1536P, DR::K4A_512x512,     FPS::F30, Dev::AK,   "C-1536p D-512p  I-MJPG F-30"sv},
    {M::AK_C2048x1536_DI1024x1024_MJPG_F30, IF::MJPG, CR::R1536P, DR::K4A_1024x1024,   FPS::F15, Dev::AK,   "C-1536p D-1024p I-MJPG F-15"sv},
    // ## 16:9 color
    {M::AK_C1280x720_DI512x512_NV12_F30,    IF::NV12, CR::R720P,  DR::K4A_512x512,     FPS::F30, Dev::AK,   "C-720p  D-512p  I-NV12 F-30"sv},
    {M::AK_C1280x720_DI512x512_YUY2_F30,    IF::YUY2, CR::R720P,  DR::K4A_512x512,     FPS::F30, Dev::AK,   "C-720p  D-512p  I-YUY2 F-30"sv},
    {M::AK_C1280x720_DI512x512_MJPG_F30,    IF::MJPG, CR::R720P,  DR::K4A_512x512,     FPS::F30, Dev::AK,   "C-720p  D-512p  I-MJPG F-30"sv},
    {M::AK_C1280x720_DI1024x1024_NV12_F15,  IF::NV12, CR::R720P,  DR::K4A_1024x1024,   FPS::F15, Dev::AK,   "C-720p  D-1024p I-NV12 F-15"sv},
    {M::AK_C1280x720_DI1024x1024_YUY2_F15,  IF::YUY2, CR::R720P,  DR::K4A_1024x1024,   FPS::F15, Dev::AK,   "C-720p  D-1024p I-YUY2 F-15"sv},
    {M::AK_C1280x720_DI1024x1024_MJPG_F15,  IF::MJPG, CR::R720P,  DR::K4A_1024x1024,   FPS::F15, Dev::AK,   "C-720p  D-1024p I-MJPG F-15"sv},
    // # no depth
    // ## 4:3 color
    {M::AK_C2048x1536_MJPG_F30,             IF::MJPG, CR::R1536P, DR::OFF,             FPS::F30, Dev::AK,   "C-1536p D-OFF   I-MJPG F-30"sv},
    {M::AK_C4096x3072_MJPG_F15,             IF::MJPG, CR::R3072P, DR::OFF,             FPS::F15, Dev::AK,   "C-3072p D-OFF   I-MJPG F-15"sv},
    // ## 19:9 color
    {M::AK_C1280x720_NV12_F30,              IF::NV12, CR::R720P,  DR::OFF,             FPS::F30, Dev::AK,   "C-720p  D-OFF   I-NV12 F-30"sv},
    {M::AK_C1920x1080_MJPG_F30,             IF::MJPG, CR::R1080P, DR::OFF,             FPS::F30, Dev::AK,   "C-1080p D-OFF   I-MJPG F-30"sv},
    {M::AK_C2560x1440_MJPG_F30,             IF::MJPG, CR::R1440P, DR::OFF,             FPS::F30, Dev::AK,   "C-1440p D-OFF   I-MJPG F-30"sv},
    {M::AK_C3840x2160_MJPG_F15,             IF::MJPG, CR::R2160P, DR::OFF,             FPS::F15, Dev::AK,   "C-2160p D-OFF   I-MJPG F-15"sv},

    // femto bolt orbbec
    // # narrow
    // ## 4:3 color
    // ## 16:9 color
    {M::FB_C1280x720_DI320x288_NV12_F30,    IF::NV12, CR::R720P,  DR::K4A_320x288,     FPS::F30, Dev::FB,   "C-720p  D-288p  I-NV12 F-30"sv},
    {M::FB_C1280x720_DI640x576_NV12_F30,    IF::NV12, CR::R720P,  DR::K4A_640x576,     FPS::F30, Dev::FB,   "C-720p  D-576p  I-NV12 F-30"sv},
    // # wide
    // ## 4:3 color
    // ## 16:9 color
    {M::FB_C1280x720_DI512x512_NV12_F30,    IF::NV12, CR::R720P,  DR::K4A_512x512,     FPS::F30, Dev::FB,   "C-720p  D-512p  I-NV12 F-30"sv},
    // ## no color
    {M::FB_DI512x512_F30,                   IF::NA,   CR::OFF,    DR::K4A_512x512,     FPS::F30, Dev::FB,   "C-OFF   D-512p  I-NA   F-30"sv},
    // # no depth
    // ## 4:3 color
    // ## 16:9 color
    {M::FB_C1280x720_MJPG_F30,              IF::MJPG, CR::R720P,  DR::OFF,             FPS::F30, Dev::FB,   "C-720p  D-OFF   I-MJPG F-30"sv},

    // femto mega orbbec ethernet
    // # narrow
    // ## 4:3 color    
    // ## 16:9 color
    {M::FME_C1280x720_DI640x576_MJPG_F30,   IF::MJPG, CR::R720P,  DR::K4A_640x576,     FPS::F30, Dev::FME,  "C-720p  D-576p  I-MJPG F-30"sv},
    // # wide
    // ## 4:3 color    
    // ## 16:9 color
    {M::FME_C1280x720_DI512x512_MJPG_F30,   IF::MJPG, CR::R720P,  DR::K4A_512x512,     FPS::F30, Dev::FME,  "C-720p  D-512p  I-MJPG F-30"sv},
    // ## no color
    {M::FME_DI512x512_F30,                  IF::NA,   CR::OFF,    DR::K4A_512x512,     FPS::F30, Dev::FME,  "C-OFF   D-512p  I-NA   F-30"sv},
    // # no depth
    // ## 4:3 color
    // ## 16:9 color

    // femto mega orbbec usb
    // # narrow
    // ## 4:3 color
    // ## 16:9 color
    {M::FMU_C1280x720_DI640x576_MJPG_F30,   IF::MJPG, CR::R720P,  DR::K4A_640x576,     FPS::F30, Dev::FME,  "C-720p  D-576p  I-MJPG F-30"sv},

    // others
    {M::Invalid,                            IF::BGRA, CR::OFF,    DR::OFF,             FPS::F30, Dev::UNDEF,"Merged"sv},
    {M::Merged,                             IF::BGRA, CR::OFF,    DR::OFF,             FPS::F30, Dev::UNDEF,"Invalid"sv},
}};


[[maybe_unused]] static constexpr auto dc_image_format(DCMode m) -> DCImageFormat{
    return dcModes.at<0,1>(m);
}
[[maybe_unused]] static constexpr auto dc_color_resolution(DCMode m) -> DCColorResolution{
    return dcModes.at<0,2>(m);
}
[[maybe_unused]] static constexpr auto dc_depth_resolution(DCMode m) -> DCDepthResolution {
    return dcModes.at<0,3>(m);
}
[[maybe_unused]] static constexpr auto dc_framerate(DCMode m) -> DCFramerate{
    return dcModes.at<0,4>(m);
}
[[maybe_unused]] static constexpr auto dc_depth_range(DCMode m) -> Range{
    return dc_depth_range(dc_depth_resolution(m));
}
[[maybe_unused]] static constexpr auto dc_has_color(DCMode m) -> bool{
    return dc_color_resolution(m) != CR::OFF;
}
[[maybe_unused]] static constexpr auto dc_has_depth(DCMode m) -> bool{
    return dc_depth_resolution(m) != DR::OFF;
}
[[maybe_unused]] static constexpr auto dc_get_device(DCMode m) -> DCType{
    return dcModes.at<0,5>(m);
}
[[maybe_unused]] static auto dc_get_device_modes(Dev dev) -> std::vector<DCMode> {
    return dcModes.elements_matching_columns_values<5,0>(dev);
}
[[maybe_unused]] static constexpr auto dc_mode_name(DCMode m) -> std::string_view{
    return dcModes.at<0,6>(m);
}

using hasAudio        = bool;
using hasInfra        = bool;
using hasBodyTracking = bool;
using hasBodiesIdMap  = bool;
using hasIMU          = bool;

using TDCDevices = std::tuple<
    DCType,                     hasAudio,   hasInfra,   hasBodyTracking,    hasBodiesIdMap, hasIMU,  DCMode>;
static constexpr TupleArray<DCType::SizeEnum, TDCDevices> dcDevices = {{
    TDCDevices
    {DCType::AzureKinect,       true,       true,       true,               true,           true,    DCMode::AK_C1280x720_DI640x576_NV12_F30},
    {DCType::FemtoBolt,         false,      true,       false,              false,          true,    DCMode::FB_C1280x720_DI640x576_NV12_F30},
    {DCType::FemtoMegaEthernet, false,      true,       false,              false,          true,    DCMode::FME_C1280x720_DI640x576_MJPG_F30},
    {DCType::FemtoMegaUSB,      false,      true,       false,              false,          true,    DCMode::FMU_C1280x720_DI640x576_MJPG_F30},
    {DCType::Kinect2,           true,       true,       false,              false,          false,   DCMode::Invalid},
    {DCType::Recording,         true,       true,       false,              false,          true,    DCMode::Invalid},
    {DCType::Undefined,         false,      false,      false,              false,          false,   DCMode::Invalid},
}};

[[nodiscard]] [[maybe_unused]] static constexpr auto dc_has_audio(DCType devT) -> bool{
    return dcDevices.at<0,1>(devT);
}
[[nodiscard]] [[maybe_unused]] static constexpr auto dc_has_infra(DCType devT) -> bool{
    return dcDevices.at<0,2>(devT);
}
[[nodiscard]] [[maybe_unused]] static constexpr auto dc_has_imu(DCType devT) -> bool{
    return dcDevices.at<0,3>(devT);
}
[[nodiscard]] [[maybe_unused]] static constexpr auto dc_has_body_tracking(DCType devT) -> bool{
    return dcDevices.at<0,4>(devT);
}
[[nodiscard]] [[maybe_unused]] static constexpr auto dc_has_body_id_map(DCType devT) -> bool{
    return dcDevices.at<0,5>(devT);
}
[[nodiscard]] [[maybe_unused]] static constexpr auto dc_default_mode(DCType devT) -> DCMode{
    return dcDevices.at<0,6>(devT);
}


[[maybe_unused]] static constexpr std::int16_t dc_invalid_depth_value = 0;
[[maybe_unused]] static constexpr std::int16_t dc_invalid_infra_value = 0;
[[maybe_unused]] static constexpr geo::Pt4<std::uint8_t> dc_invalid_color_value = {0,0,0,0};



// # synch
enum class DCSynchronisationMode : std::int8_t{
    Standalone  = 0,
    Main,
    Subordinate,
};

// # body tracking

// joints types
enum class DCJointType : std::int8_t{
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

[[maybe_unused]] static constexpr auto dc_get_joint_name(DCJointType t) -> std::string_view{
    return dcJoints.at<0,1>(t);
}

enum class DCJointConfidenceLevel : std::int8_t{
    None = 0,
    Low,
    Medium,
    Hight
};

enum class DCBTProcessingMode : std::int8_t{
    GPU = 0,
    CPU,
    GPU_CUDA,
    GPU_TENSORRT,
    GPU_DIRECTML
};

enum class DCBTSensorOrientation : std::int8_t{
    default_orientation = 0,
    clockwise_90,
    counter_clockwise_90,
    flip_180
};

enum class DCConnectivity : std::int8_t{
    Connectivity_2H,
    Connectivity_2V,
    Connectivity_4,
    Connectivity_8
};

enum class CloudColorMode : std::int8_t{
    FromDepthData = 0,          // use a gradient level to generate colors corresponding to depth
    FromDepthSizedColorImage    // use colors from deph-sized color image
};

enum class CloudGenerationMode : std::int8_t{
    FromDepth = 0,
    FromDecodedCloud
};

enum class DCClientType{
    Remote, Local, Undefined
};

enum class DCApplicationType : std::int8_t {
    DCManager, DCGrabber
};

}
