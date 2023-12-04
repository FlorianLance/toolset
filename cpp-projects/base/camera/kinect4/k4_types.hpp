
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
#include "camera/dc_types.hpp"

// kinect4
#include <k4a/k4atypes.h>
#include <kabt/k4abttypes.h>

namespace tool::camera {

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

    struct K4Body : public DCBody {

        auto update(const k4abt_body_t &body) -> void{
            id = static_cast<std::int8_t>(body.id);
            for(const auto &jointD : dcJoints.data){
                const auto &kaKoint = body.skeleton.joints[static_cast<int>(std::get<0>(jointD))];
                auto &joint = skeleton.joints[static_cast<int>(std::get<0>(jointD))];
                joint.confidence = static_cast<DCJointConfidenceLevel>(kaKoint.confidence_level);
                const auto &p = kaKoint.position;
                joint.position = {-p.v[0],-p.v[1],p.v[2]};
                const auto &o = kaKoint.orientation;
                joint.orientation = {o.wxyz.x,o.wxyz.y,o.wxyz.z,o.wxyz.w};
            }
        }

        // Define the bone list based on the documentation
        static constexpr auto bones_list() -> std::array<std::pair<DCJointType, DCJointType>, 31>{
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
    };
}
