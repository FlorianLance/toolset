

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
#include <cstdint>

// local
#include "utility/tuple_array.hpp"

namespace tool::cam{

using namespace std::literals::string_view_literals;

enum class K2Detection : std::uint8_t{
    unknow= 0,
    no,
    maybe,
    yes,
};

enum class K2HandStateT : std::uint8_t{
    unknow= 0,
    not_tracked,
    open,
    closed,
    lasso
};

enum class K2TrackingStateT : std::uint8_t{
    not_tracked=0, // body tracked
    inferred,
    tracked
};

enum class K2BodyJointType : std::uint8_t{
    spine_base	= 0,
    spine_mid,
    neck,
    head,
    shoulder_left,
    elbow_left,
    wrist_left,
    hand_left,
    shoulder_right,
    elbow_right,
    wrist_right,
    hand_right,
    hip_left,
    knee_left,
    ankle_left,
    foot_left,
    hip_right,
    knee_right,
    ankle_right,
    foot_right,
    spine_shoulder,
    hand_tip_left,
    thumb_left,
    hand_tip_right,
    thumb_right,
    SizeEnum
};

using SV            = std::string_view;
using K2JointName   = SV;
using K2TJoint      = std::tuple<
    K2BodyJointType,                    K2JointName>;
[[maybe_unused]] static constexpr TupleArray<K2BodyJointType::SizeEnum, K2TJoint> k2JointsM{{
    K2TJoint
    {K2BodyJointType::spine_base,       "spine_base"sv},
    {K2BodyJointType::spine_mid,        "spine_mid"sv},
    {K2BodyJointType::neck,             "neck"sv},
    {K2BodyJointType::head,             "head"sv},
    {K2BodyJointType::shoulder_left,    "shoulder_left"sv},
    {K2BodyJointType::elbow_left,       "elbow_left"sv},
    {K2BodyJointType::wrist_left,       "wrist_left"sv},
    {K2BodyJointType::hand_left,        "hand_left"sv},
    {K2BodyJointType::shoulder_right,   "shoulder_right"sv},
    {K2BodyJointType::elbow_right,      "elbow_right"sv},
    {K2BodyJointType::wrist_right,      "wrist_right"sv},
    {K2BodyJointType::hand_right,       "hand_right"sv},
    {K2BodyJointType::hip_left,         "hip_left"sv},
    {K2BodyJointType::knee_left,        "knee_left"sv},
    {K2BodyJointType::ankle_left,       "ankle_left"sv},
    {K2BodyJointType::foot_left,        "foot_left"sv},
    {K2BodyJointType::hip_right,        "hip_right"sv},
    {K2BodyJointType::knee_right,       "knee_right"sv},
    {K2BodyJointType::ankle_right,      "ankle_right"sv},
    {K2BodyJointType::foot_right,       "foot_right"sv},
    {K2BodyJointType::spine_shoulder,   "spine_shoulder"sv},
    {K2BodyJointType::hand_tip_left,    "hand_tip_left"sv},
    {K2BodyJointType::thumb_left,       "thumb_left"sv},
    {K2BodyJointType::hand_tip_right,   "hand_tip_right"sv},
    {K2BodyJointType::thumb_right,      "thumb_right"sv},
}};


enum class K2FrameRequest : uint8_t {
    compressed_color_cloud=0,
    compressed_color_mesh,
    compressed_color_512x424,
    compressed_color_1920x1080,
    depth_512x424,                  // 512 * 424 * (sizeof(float)) -> d
    infra_512x424,                  // 512 * 424 * (sizeof(ushort)) -> v
    long_exposure_infra_512x424,    // 512 * 424 * (sizeof(ushort)) -> v
    undefined,
    SizeEnum
};

enum K2MorphShapes : unsigned char{
    MORPH_RECT    = 0,
    MORPH_CROSS   = 1,
    MORPH_ELLIPSE = 2
};


}
