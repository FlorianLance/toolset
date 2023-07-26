
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

#include "k4_actions_settings.hpp"

// local
#include "utility/io_data.hpp"

using namespace tool::camera;

auto K4ActionsSettings::default_init_for_grabber() -> K4ActionsSettings{
    K4ActionsSettings actions;
    actions.startDevice   = false;
    actions.openCamera    = false;
    return actions;
}

auto K4ActionsSettings::default_init_for_manager() -> K4ActionsSettings{
    K4ActionsSettings actions;
    actions.startDevice   = false;
    actions.openCamera    = false;
    return actions;
}

auto K4ActionsSettings::init_from_data(std::int8_t *data) -> void{
    size_t offset = 0;
    read(startDevice, data, offset);
    read(openCamera, data, offset);
}

auto K4ActionsSettings::convert_to_data(std::int8_t *data) const -> void{
    size_t offset = 0;
    write(startDevice, data, offset);
    write(openCamera, data, offset);
}
