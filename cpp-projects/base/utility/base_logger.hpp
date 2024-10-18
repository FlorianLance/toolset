

/*******************************************************************************
** Toolset-base                                                               **
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

// std
#include <memory>
#include <string>

// local
#include "thirdparty/sigslot/signal.hpp"

namespace tool {

class BaseLogger{
public:

    enum class MessageType : int{
        normal, warning, error, log, unknow,
    };

    virtual ~BaseLogger(){}

    virtual auto nofile_init() -> void{}
    virtual auto clean() -> void{}

    static auto set_logger_instance(std::unique_ptr<BaseLogger> logger) noexcept -> void;
    static auto get_instance() noexcept -> BaseLogger*;

    virtual auto message(std::string_view message)  -> void{static_cast<void>(message);}
    virtual auto warning(std::string_view warning)  -> void{static_cast<void>(warning);}
    virtual auto error(std::string_view error)      -> void{static_cast<void>(error);}
    virtual auto log(std::string_view error)        -> void{static_cast<void>(error);}
};
}
