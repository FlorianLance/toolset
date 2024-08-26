
/*******************************************************************************
** Toolset-base-export
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

#include "dc_client_export.hpp"

using namespace tool::cam;

auto create__dc_client() -> tool::cam::DCClientExport*{
    return new tool::cam::DCClientExport();
}

auto delete__dc_client(tool::cam::DCClientExport *dcClientE) -> void{
    delete dcClientE;
}

auto initialize__dc_client(tool::cam::DCClientExport *dcClientE, const char* clientSettingsPath) -> int{
    return dcClientE->client.initialize(clientSettingsPath, false) ? 1 : 0;
}

auto init_callbacks__dc_client(tool::cam::DCClientExport *dcClientE, LogMessageCB logCB) -> void{
    dcClientE->init_callbacks(logCB);
}

auto update__dc_client(tool::cam::DCClientExport *dcClientE) -> void{
    dcClientE->client.update();

    // retrieve frames
}

auto process_frames__dc_client(tool::cam::DCClientExport *dcClientE) -> void{
    dcClientE->client.process_frames_from_external_thread();
}

auto read_network_data__dc_client(tool::cam::DCClientExport *dcClientE, size_t idDevice) -> void{

}
