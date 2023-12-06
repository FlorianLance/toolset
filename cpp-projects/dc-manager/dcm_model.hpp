
/*******************************************************************************
** Toolset-dc-manager                                                         **
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

// base
#include "camera/network/dc_server_network.hpp"
#include "camera/dc_server_data.hpp"
#include "camera/dc_recorder.hpp"
#include "camera/dc_player.hpp"
#include "camera/dc_calibrator.hpp"

// local
#include "data/dcm_settings.hpp"

namespace tool {

struct DCMModel{

    auto initialize() -> bool;
    auto clean() -> void;
    auto update() -> void;
    auto trigger_settings() -> void;


    // network
    auto add_feedback(size_t id, network::Feedback feedback) -> void;
    auto update_synchro(size_t id, std::int64_t averageDiffNs) -> void;

    // action
    auto ask_calibration() -> void;

    // # filtering
    auto update_filtering_mode(bool useNormalMode) -> void;
    auto update_filters(size_t id, const camera::DCFiltersSettings& filters) -> void;
    auto update_calibration_filters(size_t id, const camera::DCFiltersSettings& filters) -> void;


    DCMSettings settings;
    network::DCServerNetwork network;
    camera::DCServerData sData;
    camera::DCRecorder recorder;
    camera::DCPlayer player;
    camera::DCCalibrator calibration;

private:

    auto read_feedbacks() -> void;

    std::mutex readMessagesL;
    std::deque<std::pair<size_t, network::Feedback>> messages;
    std::vector<std::pair<size_t, network::Feedback>> messagesR;

};
}
