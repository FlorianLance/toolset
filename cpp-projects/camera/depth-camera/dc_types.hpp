
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
#include <variant>

// local
#include "dc_enums.hpp"
#include "geometry/quaternion.hpp"
#include "graphics/color/rgb.hpp"
#include "utility/image_buffer.hpp"
#include "geometry/color_cloud.hpp"

namespace tool::cam {

struct DCJoint{
    geo::Pt3f position = {};
    geo::Quaternion<float> orientation = {};
    DCJointConfidenceLevel confidence = DCJointConfidenceLevel::None;
    constexpr auto good_confidence() const noexcept -> bool {
        return confidence == DCJointConfidenceLevel::Hight || confidence == DCJointConfidenceLevel::Medium;
    }
};

// Define the bone list based on the documentation
[[maybe_unused]][[nodiscard]] static constexpr auto bones_list(DCType device) -> std::array<std::pair<DCJointType, DCJointType>, 31>{
    if(device == DCType::AzureKinect){
        return{
            std::make_pair(DCJointType::spine_chest,    DCJointType::spine_navel),
            std::make_pair(DCJointType::spine_navel,    DCJointType::pelvis),
            std::make_pair(DCJointType::spine_chest,    DCJointType::neck),
            std::make_pair(DCJointType::neck,           DCJointType::head),
            std::make_pair(DCJointType::head,           DCJointType::nose),
            std::make_pair(DCJointType::spine_chest,    DCJointType::clavicle_left),
            std::make_pair(DCJointType::clavicle_left,  DCJointType::shoulder_left),
            std::make_pair(DCJointType::shoulder_left,  DCJointType::elbow_left),
            std::make_pair(DCJointType::elbow_left,     DCJointType::wrist_left),
            std::make_pair(DCJointType::wrist_left,     DCJointType::hand_left),
            std::make_pair(DCJointType::hand_left,      DCJointType::handtip_left),
            std::make_pair(DCJointType::wrist_left,     DCJointType::thumb_left),
            std::make_pair(DCJointType::pelvis,         DCJointType::hip_left),
            std::make_pair(DCJointType::hip_left,       DCJointType::knee_left),
            std::make_pair(DCJointType::knee_left,      DCJointType::ankle_left),
            std::make_pair(DCJointType::ankle_left,     DCJointType::foot_left),
            std::make_pair(DCJointType::nose,           DCJointType::eye_left),
            std::make_pair(DCJointType::eye_left,       DCJointType::ear_left),
            std::make_pair(DCJointType::spine_chest,    DCJointType::clavicle_right),
            std::make_pair(DCJointType::clavicle_right, DCJointType::shoulder_right),
            std::make_pair(DCJointType::shoulder_right, DCJointType::elbow_right),
            std::make_pair(DCJointType::elbow_right,    DCJointType::wrist_right),
            std::make_pair(DCJointType::wrist_right,    DCJointType::hand_right),
            std::make_pair(DCJointType::hand_right,     DCJointType::handtip_right),
            std::make_pair(DCJointType::wrist_right,    DCJointType::thumb_right),
            std::make_pair(DCJointType::pelvis,         DCJointType::hand_right),
            std::make_pair(DCJointType::hip_right,      DCJointType::knee_right),
            std::make_pair(DCJointType::knee_right,     DCJointType::ankle_right),
            std::make_pair(DCJointType::ankle_right,    DCJointType::foot_right),
            std::make_pair(DCJointType::nose,           DCJointType::eye_right),
            std::make_pair(DCJointType::eye_right,      DCJointType::ear_right)
        };
    }
    return {};
}

struct DCSkeleton{

    std::array<DCJoint, static_cast<std::int8_t>(DCJointType::SizeEnum)> joints;

    constexpr auto joint(DCJointType type) const noexcept -> DCJoint { return joints[static_cast<std::int8_t>(type)];}
    constexpr auto pelvis() const noexcept -> DCJoint        { return joints[static_cast<std::int8_t>(DCJointType::pelvis)];}
    constexpr auto spine_navel() const noexcept -> DCJoint   { return joints[static_cast<std::int8_t>(DCJointType::spine_navel)];}
    constexpr auto spine_chest() const noexcept -> DCJoint   { return joints[static_cast<std::int8_t>(DCJointType::spine_chest)];}
    constexpr auto neck() const noexcept -> DCJoint          { return joints[static_cast<std::int8_t>(DCJointType::neck)];}
    constexpr auto clavicle_left() const noexcept -> DCJoint { return joints[static_cast<std::int8_t>(DCJointType::clavicle_left)];}
    constexpr auto shoulder_left() const noexcept -> DCJoint { return joints[static_cast<std::int8_t>(DCJointType::shoulder_left)];}
    constexpr auto elbow_left() const noexcept -> DCJoint    { return joints[static_cast<std::int8_t>(DCJointType::elbow_left)];}
    constexpr auto wrist_left() const noexcept -> DCJoint    { return joints[static_cast<std::int8_t>(DCJointType::wrist_left)];}
    constexpr auto hand_left() const noexcept -> DCJoint     { return joints[static_cast<std::int8_t>(DCJointType::hand_left)];}
    constexpr auto handtip_left() const noexcept -> DCJoint  { return joints[static_cast<std::int8_t>(DCJointType::handtip_left)];}
    constexpr auto thumb_left() const noexcept -> DCJoint    { return joints[static_cast<std::int8_t>(DCJointType::thumb_left)];}
    constexpr auto clavicle_right() const noexcept -> DCJoint{ return joints[static_cast<std::int8_t>(DCJointType::clavicle_right)];}
    constexpr auto shoulder_right() const noexcept -> DCJoint{ return joints[static_cast<std::int8_t>(DCJointType::shoulder_right)];}
    constexpr auto elbow_right() const noexcept -> DCJoint   { return joints[static_cast<std::int8_t>(DCJointType::elbow_right)];}
    constexpr auto wrist_right() const noexcept -> DCJoint   { return joints[static_cast<std::int8_t>(DCJointType::wrist_right)];}
    constexpr auto hand_right() const noexcept -> DCJoint    { return joints[static_cast<std::int8_t>(DCJointType::hand_right)];}
    constexpr auto handtip_right() const noexcept -> DCJoint { return joints[static_cast<std::int8_t>(DCJointType::handtip_right)];}
    constexpr auto thumb_right() const noexcept -> DCJoint   { return joints[static_cast<std::int8_t>(DCJointType::thumb_right)];}
    constexpr auto hip_left() const noexcept -> DCJoint      { return joints[static_cast<std::int8_t>(DCJointType::hip_left)];}
    constexpr auto knee_left() const noexcept -> DCJoint     { return joints[static_cast<std::int8_t>(DCJointType::knee_left)];}
    constexpr auto ankle_left() const noexcept -> DCJoint    { return joints[static_cast<std::int8_t>(DCJointType::ankle_left)];}
    constexpr auto foot_left() const noexcept -> DCJoint     { return joints[static_cast<std::int8_t>(DCJointType::foot_left)];}
    constexpr auto hip_right() const noexcept -> DCJoint     { return joints[static_cast<std::int8_t>(DCJointType::hip_right)];}
    constexpr auto knee_right() const noexcept -> DCJoint    { return joints[static_cast<std::int8_t>(DCJointType::knee_right)];}
    constexpr auto ankle_right() const noexcept -> DCJoint   { return joints[static_cast<std::int8_t>(DCJointType::ankle_right)];}
    constexpr auto foot_right() const noexcept -> DCJoint    { return joints[static_cast<std::int8_t>(DCJointType::foot_right)];}
    constexpr auto head() const noexcept -> DCJoint          { return joints[static_cast<std::int8_t>(DCJointType::head)];}
    constexpr auto nose() const noexcept -> DCJoint          { return joints[static_cast<std::int8_t>(DCJointType::nose)];}
    constexpr auto eye_left() const noexcept -> DCJoint      { return joints[static_cast<std::int8_t>(DCJointType::eye_left)];}
    constexpr auto ear_left() const noexcept -> DCJoint      { return joints[static_cast<std::int8_t>(DCJointType::ear_left)];}
    constexpr auto eye_right() const noexcept -> DCJoint     { return joints[static_cast<std::int8_t>(DCJointType::eye_right)];}
    constexpr auto ear_right() const noexcept -> DCJoint     { return joints[static_cast<std::int8_t>(DCJointType::ear_right)];}
};

struct DCBody{
    std::int8_t id = 0;
    DCSkeleton skeleton;
    virtual ~DCBody(){}
};

struct DCImuSample{
    float temperature;     /**< Temperature reading of this sample (Celsius). */
    geo::Pt3f acc;         /**< Accelerometer sample in meters per second squared. */
    std::int64_t accTsMs;  /**< Timestamp of the accelerometer in microseconds. */
    geo::Pt3f gyr;         /**< Gyro sample in radians per second. */
    std::int64_t gyrTsMs;  /**< Timestamp of the gyroscope in microseconds */
};

struct DCVertexMeshData{
    geo::Pt3f pos;
    geo::Pt4<std::uint8_t> col;
};


struct DCModeInfos{

    auto initialize(DCMode mode) -> void;

    constexpr auto increment_capture_id()                 noexcept -> void                  {++m_idCapture;}
    auto force_image_format(DCImageFormat imageFormat)    noexcept -> void                  {m_imageFormat = imageFormat;}

    [[nodiscard]] constexpr auto mode()             const noexcept -> DCMode                {return m_mode;}
    [[nodiscard]] constexpr auto device()           const noexcept -> DCType                {return dc_get_device(mode());}
    [[nodiscard]] constexpr auto id_capture()       const noexcept -> size_t                {return m_idCapture;}

    [[nodiscard]] constexpr auto image_format()     const noexcept -> DCImageFormat         {return m_imageFormat;}
    [[nodiscard]] constexpr auto color_resolution() const noexcept -> DCColorResolution     {return m_colorResolution;}
    [[nodiscard]] constexpr auto depth_resolution() const noexcept -> DCDepthResolution     {return m_depthResolution;}
    [[nodiscard]] constexpr auto framerate()        const noexcept -> DCFramerate           {return m_fps;}
    [[nodiscard]] constexpr auto framerate_value()  const noexcept -> int                   {return dc_framerate_value(framerate());}
    [[nodiscard]] constexpr auto timeout_ms()       const noexcept -> std::int32_t          {return m_timeoutMs;}

    [[nodiscard]] constexpr auto has_color()        const noexcept -> bool                  {return m_hasColor;}
    [[nodiscard]] constexpr auto has_depth()        const noexcept -> bool                  {return m_hasDepth;}
    [[nodiscard]] constexpr auto has_infra()        const noexcept -> bool                  {return m_hasInfra;}
    [[nodiscard]] constexpr auto has_audio()        const noexcept -> bool                  {return dc_has_audio(device());}

    [[nodiscard]] constexpr auto color_width()      const noexcept -> size_t                {return m_colorWidth;}
    [[nodiscard]] constexpr auto color_height()     const noexcept -> size_t                {return m_colorHeight;}
    [[nodiscard]] constexpr auto color_size()       const noexcept -> size_t                {return m_colorSize;}

    [[nodiscard]] constexpr auto depth_width()      const noexcept -> size_t                {return m_depthWidth;}
    [[nodiscard]] constexpr auto depth_height()     const noexcept -> size_t                {return m_depthHeight;}
    [[nodiscard]] constexpr auto depth_size()       const noexcept -> size_t                {return m_depthSize;}
    [[nodiscard]] constexpr auto depth_range_m()    const noexcept -> Range                 {return m_depthRange;}
    [[nodiscard]] constexpr auto depth_range_mm()   const noexcept -> Range                 {return m_depthRange*1000.f;}

    [[nodiscard]] constexpr auto infra_width()      const noexcept -> size_t                {return m_infraWidth;}
    [[nodiscard]] constexpr auto infra_height()     const noexcept -> size_t                {return m_infraHeight;}
    [[nodiscard]] constexpr auto infra_size()       const noexcept -> size_t                {return m_infraSize;}

    [[nodiscard]] constexpr auto rgba_color_size_bytes() const noexcept -> size_t           {return m_colorSize*4;}

private:

    DCMode            m_mode            = DCMode::Invalid;
    DCImageFormat     m_imageFormat     = DCImageFormat::NV12;
    DCColorResolution m_colorResolution = DCColorResolution::OFF;
    DCDepthResolution m_depthResolution = DCDepthResolution::OFF;

    size_t m_idCapture       = 0;
    DCFramerate m_fps        = DCFramerate::F30;
    std::int32_t m_timeoutMs = 500;
    bool m_hasColor          = false;
    bool m_hasDepth          = false;
    bool m_hasInfra          = false;
    size_t m_colorWidth      = 0;
    size_t m_colorHeight     = 0;
    size_t m_colorSize       = 0;
    size_t m_depthWidth      = 0;
    size_t m_depthHeight     = 0;
    size_t m_depthSize       = 0;
    Range  m_depthRange      = {0.f,0.f};
    size_t m_infraWidth      = 0;
    size_t m_infraHeight     = 0;
    size_t m_infraSize       = 0;
};

enum class SAction : int {
    Reset, Load, Save, Add_server, Remove_server
};
enum class STarget : int {
    Irrelevant, All,Individual
};
enum class SType : int {
    Global, Device, Filters, CalibrationFilters, Color, Model
};
enum class SFile : int {
    Irrelevant, Default, Normal, Specific
};

struct SettingsAction{
    SAction action;
    STarget target;
    SType type;
    SFile file;
    size_t id;
    bool valid = false;
    bool hasPath = false;
    std::string path = "";

    auto update(SAction action, STarget target, SType type, SFile file, size_t id = 0) -> void{
        this->action = action;
        this->target = target;
        this->type = type;
        this->file = file;
        this->id = id;
        path = "";
        valid = true;
    }
};

using InfoV = std::variant<
    std::int64_t>;

using ImageBufferV = std::variant<
    ImageBuffer<ColorRGBA8>,
    ImageBuffer<ColorRGB8>,
    ImageBuffer<ColorGray8>,
    ImageBuffer<ColorGray16>>;

using VolumeBufferV = std::variant<
    geo::ColorCloud>;
}
