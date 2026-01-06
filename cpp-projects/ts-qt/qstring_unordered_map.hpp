

/*******************************************************************************
** Toolset-ts-qt                                                              **
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

// Qt
#include <QString>
#include <QHash>

// ankerl
#include <ankerl/unordered_dense.h>

namespace tool {

// for heterogenous lookup
struct qstring_hash {
    using is_transparent = void;

    [[nodiscard]] auto operator()(const QString &txt) const -> size_t{
        return qHash(txt);
    }
    [[nodiscard]] auto operator()(QStringView txt) const -> size_t{
        return qHash(txt);
    }
};

template <typename Type>
concept QStringLike = std::is_convertible_v<Type, QStringView>;

template <typename QStringLike, typename Value>
using qs_umap = ankerl::unordered_dense::map<QStringLike, Value,  qstring_hash, std::equal_to<>>;
}
