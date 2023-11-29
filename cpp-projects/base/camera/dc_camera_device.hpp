

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
#include <string>

// local
//#include "thirdparty/sigslot/signal.hpp"

namespace tool::camera {


class DCDevice{
public:

    virtual ~DCDevice(){}

    virtual auto open(std::uint32_t deviceId) -> bool{return false;}
    virtual auto refresh_devices_list() -> void{}
    virtual auto close() -> void{}
    virtual auto clean() -> void{}

    virtual auto nb_devices() const noexcept -> std::uint32_t{return 0;}
    virtual auto device_id() const noexcept -> std::uint32_t{return 0;};
    virtual auto device_name() const noexcept -> std::string{return "";}
    virtual auto is_opened() const noexcept -> bool{return false;}
    virtual auto cameras_started() const noexcept -> bool{return false;}


};
}
