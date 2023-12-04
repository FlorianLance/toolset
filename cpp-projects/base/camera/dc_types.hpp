
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
#include "dc_enums.hpp"
#include "geometry/quaternion.hpp"

namespace tool::camera {

    struct DCJoint{

        geo::Pt3f position = {};
        geo::Quaternion<float> orientation = {};
        DCJointConfidenceLevel confidence = DCJointConfidenceLevel::None;

        constexpr auto good_confidence() const noexcept{
            return confidence == DCJointConfidenceLevel::Hight || confidence == DCJointConfidenceLevel::Medium;
        }
    };

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
}
