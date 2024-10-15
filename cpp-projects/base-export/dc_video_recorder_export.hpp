/*******************************************************************************
** Toolset-base-export                                                        **
** MIT License                                                                **
** Copyright (c) [2024] [Florian Lance]                                       **
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
#include "depth-camera/dc_video_recorder.hpp"

// local
#include "dc_client_export.hpp"

namespace tool::cam{

struct DCVideoRecorderExport{

    DCVideoRecorder recorder;

    // callbacks
    std::unique_ptr<LogMessageCB> logMessageCBP = nullptr;

    DCVideoRecorderExport();
    ~DCVideoRecorderExport();

    auto dll_log_message(const std::string &message) -> void;
    auto dll_log_warning(const std::string &message) -> void;
    auto dll_log_error(const std::string &message) -> void;

    auto init_callbacks(LogMessageCB logCB) -> void;
    auto associate_with_client(DCClient &client) -> void;
    auto dissociate_from_client(DCClient &client) -> void;

private:
    
    auto add_frame(size_t idD, std::shared_ptr<DCFrame> frame) -> void;
};

}

extern "C"{

    // create / delete
    DECL_EXPORT tool::cam::DCVideoRecorderExport* create__dc_video_recorder_export();
    DECL_EXPORT void delete__dc_video_recorder_export(tool::cam::DCVideoRecorderExport *dcVideoRecorderExport);

    // init
    DECL_EXPORT void init_callbacks__dc_video_recorder_export(tool::cam::DCVideoRecorderExport *dcVideoRecorderExport, LogMessageCB logCB);
    DECL_EXPORT void associate_with_client__dc_video_recorder_export(tool::cam::DCVideoRecorderExport *dcVideoRecorderExport, tool::cam::DCClientExport *dcClientExport);
    DECL_EXPORT void dissociate_from_client__dc_video_recorder_export(tool::cam::DCVideoRecorderExport *dcVideoRecorderExport, tool::cam::DCClientExport *dcClientExport);

    // actions
    DECL_EXPORT void start_recording__dc_video_recorder_export(tool::cam::DCVideoRecorderExport *dcVideoRecorderExport);
    DECL_EXPORT void stop_recording__dc_video_recorder_export(tool::cam::DCVideoRecorderExport *dcVideoRecorderExport);
    DECL_EXPORT void reset_recording__dc_video_recorder_export(tool::cam::DCVideoRecorderExport *dcVideoRecorderExport);

    // data
    DECL_EXPORT int is_recording__dc_video_recorder_export(tool::cam::DCVideoRecorderExport *dcVideoRecorderExport);
    DECL_EXPORT int nb_devices__dc_video_recorder_export(tool::cam::DCVideoRecorderExport *dcVideoRecorderExport);
    DECL_EXPORT int nb_frames__dc_video_recorder_export(tool::cam::DCVideoRecorderExport *dcVideoRecorderExport, int idD);
    DECL_EXPORT float duration__dc_video_recorder_export(tool::cam::DCVideoRecorderExport *dcVideoRecorderExport);
}



