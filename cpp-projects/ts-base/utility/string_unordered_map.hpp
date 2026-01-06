
/*******************************************************************************
** Toolset-ts-base                                                            **
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
#include <string>

// ankerl
#include <ankerl/unordered_dense.h>

namespace tool {

    // for heterogenous lookup
    struct string_hash {
        using is_transparent = void;
        [[nodiscard]] auto operator()(const char *txt) const -> size_t{
            return std::hash<std::string_view>{}(txt);
        }
        [[nodiscard]] auto operator()(std::string_view txt) const -> size_t{
            return std::hash<std::string_view>{}(txt);
        }
        [[nodiscard]] auto operator()(const std::string &txt) const -> size_t{
            return std::hash<std::string>{}(txt);
        }
    };

    template<typename ... Bases>
    struct overload : Bases ...
    {
        using is_transparent = void;
        using Bases::operator() ... ;
    };

    struct char_pointer_hash
    {
        auto operator()( const char* ptr ) const noexcept
        {
            return std::hash<std::string_view>{}( ptr );
        }
    };

    using transparent_string_hash = overload<
        std::hash<std::string>,
        std::hash<std::string_view>,
        char_pointer_hash
    >;



    template <typename Type>
    concept StringLike = std::is_convertible_v<Type, std::string_view>;

    template <typename StringLike, typename Value>
    using s_umap = ankerl::unordered_dense::map<StringLike, Value,  transparent_string_hash, std::equal_to<>>;
}
