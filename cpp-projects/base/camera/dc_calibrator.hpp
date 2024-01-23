
/*******************************************************************************
** Toolset-k4-scaner-manager                                                  **
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
#include "thirdparty/sigslot/signal.hpp"
#include "camera/settings/dc_calibrator_settings.hpp"
#include "camera/settings/dc_model_settings.hpp"
#include "camera/states/dc_calibrator_states.hpp"

namespace tool::cam {

class DCCalibrator{
public:

    DCCalibrator();
    ~DCCalibrator();

    auto initialize(size_t nbGrabbers) -> void;

    // getters
    auto nb_frames_registered(size_t idGrabber) const noexcept -> size_t;
    auto size_all_calibration_cloud() const noexcept -> size_t;
    auto calibration_grabber_data(size_t idGrabber) const  -> const DCCalibratorGrabberData*;

    // registration
    auto is_registering() const -> bool {return m_states.isRegistering;}
    auto update_time() -> void;
    auto start_registering() -> void;
    auto stop_registering() -> void;
    auto reset_registering() -> void;    

    // frames
    auto add_frame(size_t idCloud, std::shared_ptr<cam::DCFrame> frame) -> void;
    auto process_all_frames() -> void;

    // calibrate
    auto calibrate(const std::vector<DCModelSettings> &models) -> bool;
    auto validate_calibration() -> void;

    // settings
    auto update_settings(const DCCalibratorSettings &calibrationS) -> void;

    // signals
    auto send_data_updated_signal() -> void;
    sigslot::signal<DCCalibratorStates> states_updated_signal;
    sigslot::signal<size_t, DCModelSettings> new_calibration_signal;
    sigslot::signal<size_t, DCModelSettings> validated_calibration_signal;
    sigslot::signal<int, int, std::vector<DCCalibratorGrabberData>*> data_updated_signal;

private :

    auto add_to_calibration_cloud(size_t idCloud, const geo::ColoredCloudData &cloud) -> void;
    auto add_to_proccessed_cloud(size_t idCloud, const geo::ColoredCloudData &cloud) -> void;

    DCCalibratorStates m_states;
    std::vector<std::optional<DCModelSettings>> m_calibrations;

    struct Impl;
    std::unique_ptr<Impl> m_p;
};

}
