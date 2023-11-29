
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

//// kinect4
//#include <k4a/k4atypes.h>
//#include <kabt/k4abttypes.h>

// local
//#include "geometry/point2.hpp"
//#include "geometry/point4.hpp"
//#include "geometry/quaternion.hpp"
#include "utility/tuple_array.hpp"

namespace tool::camera {

    enum class SettingsType : std::int32_t{
        Network, Filters, Color, Device, Model, Delay,
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

    enum class DCType : std::int8_t{
        Kinec2 = 0,
        Kinect4,
        Orbbec,
        SizeEnum
    };

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
//        INVALID  = 8,
        SizeEnum
    };

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
//        INVALID  = 26,
        SizeEnum
    };

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

    enum class K4ColorResolution : std::int8_t {
        OFF     = 0, // K4A_COLOR_RESOLUTION_OFF    : Color camera will be turned off with this setting
        R720P   = 1, // K4A_COLOR_RESOLUTION_720P   : 1280 * 720  16:9
        R1080P  = 2, // K4A_COLOR_RESOLUTION_1080P  : 1920 * 1080 16:9
        R1440P  = 3, // K4A_COLOR_RESOLUTION_1440P  : 2560 * 1440 16:9
        R1536P  = 4, // K4A_COLOR_RESOLUTION_1536P  : 2048 * 1536 4:3
        R2160P  = 5, // K4A_COLOR_RESOLUTION_2160P  : 3840 * 2160 16:9
        R3072P  = 6, // K4A_COLOR_RESOLUTION_3072P  : 4096 * 3072 4:3
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


//    enum class DCColorResolution : std::int8_t {
//        OFF,        // Color camera will be turned off with this setting
//        R720P,      // 1280 * 720  16:9
//        R1080P,     // 1920 * 1080 16:9
//        R1440P,     // 2560 * 1440 16:9
//        R1536P,     // 2048 * 1536 4:3
//        R2160P,     // 3840 * 2160 16:9
//        R3072P,     // 4096 * 3072 4:3
//        SizeEnum
//    };

//    using TDcColRes = std::tuple<
//        DCColorResolution, int, int>;
//    static constexpr TupleArray<DCColorResolution::SizeEnum, TDcColRes>dcColRes = {{
//        // cloud
//        TDcColRes
//        {DCColorResolution::OFF,    0,    0},
//        {DCColorResolution::R720P,  1280, 720},
//        {DCColorResolution::R1080P, 1920, 1080},
//        {DCColorResolution::R1440P, 2560, 1440},
//        {DCColorResolution::R1536P, 2048, 1536},
//        {DCColorResolution::R2160P, 3840, 2160},
//        {DCColorResolution::R3072P, 4096, 3072}
//    }};
//    [[maybe_unused]] static constexpr auto color_width(DCColorResolution r) -> int{
//        return dcColRes.at<0,1>(r);
//    }

//    [[maybe_unused]] static constexpr auto color_height(DCColorResolution r) -> int{
//        return dcColRes.at<0,2>(r);
//    }

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

    enum class K4Framerate : std::int8_t{
        F5  = 0, // K4A_FRAMES_PER_SECOND_5  : 5 FPS
        F15 = 1, // K4A_FRAMES_PER_SECOND_15 : 15 FPS
        F30 = 2, // K4A_FRAMES_PER_SECOND_30 : 30 FPS
        SizeEnum
    };
    using TK4Framerate = std::tuple<
        K4Framerate, int>;
    static constexpr TupleArray<K4Framerate::SizeEnum, TK4Framerate> k4Framerates = {{
        // cloud
        TK4Framerate
        {K4Framerate::F5,  5},
        {K4Framerate::F15, 15},
        {K4Framerate::F30, 30},
    }};

    [[maybe_unused]] static constexpr auto framerate_value(K4Framerate f) -> int{
        return k4Framerates.at<0,1>(f);
    }

//    enum class DCFramerate : std::int8_t{
//        F5,   // 5 FPS
//        F15,  // 15 FPS
//        F30,  // 30 FPS
//        SizeEnum
//    };

//    using TDCFramerate = std::tuple<
//        DCFramerate, int>;
//    static constexpr TupleArray<DCFramerate::SizeEnum, TDCFramerate> dcFramerates = {{
//        // cloud
//        TDCFramerate
//        {DCFramerate::F5,  5},
//        {DCFramerate::F15, 15},
//        {DCFramerate::F30, 30},
//    }};

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


}
