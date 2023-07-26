
/*******************************************************************************
** Toolset-3d-engine                                                          **
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
#include "imgui_convert.hpp"

namespace tool{

/**
 * @brief get current window position in screen space
 */
[[maybe_unused]] static inline auto window_screen_pos() -> tool::geo::Pt2f{
    return to_pt2(ImGui::GetWindowPos());
}

[[maybe_unused]] static inline auto window_size() -> geo::Pt2f{
    return to_pt2(ImGui::GetWindowSize());
}

[[maybe_unused]] static inline auto item_size() -> geo::Pt2f{
    return to_pt2(ImGui::GetItemRectSize());
}

[[maybe_unused]] static inline auto last_item_top_left_screen_pos() -> geo::Pt2f{
    return to_pt2(ImGui::GetItemRectMin());
}

[[maybe_unused]] static inline auto last_item_bottom_right_screen_pos() -> geo::Pt2f{
    return to_pt2(ImGui::GetItemRectMax());
}

[[maybe_unused]] static inline auto last_item_bottom_left_screen_pos() -> geo::Pt2f{
    return last_item_top_left_screen_pos() + geo::Pt2f{0, item_size().y()};
}

[[maybe_unused]] static inline auto last_item_top_screen_value() -> float{
    return last_item_top_left_screen_pos().y();
}

[[maybe_unused]] static inline auto last_item_bottom_screen_value() -> float{
    return last_item_bottom_right_screen_pos().y();
}

[[maybe_unused]] static inline auto last_item_left_screen_value() -> float{
    return last_item_top_left_screen_pos().x();
}

[[maybe_unused]] static inline auto last_item_right_screen_value() -> float{
    return last_item_bottom_right_screen_pos().x();
}

[[maybe_unused]] static inline auto content_region_size_available() -> geo::Pt2f{
    return to_pt2(ImGui::GetContentRegionAvail());
}

[[maybe_unused]] static inline auto cursor_window_position() -> geo::Pt2f{
    return to_pt2(ImGui::GetCursorPos());
}

[[maybe_unused]] static inline auto cursor_screen_position() -> geo::Pt2f{
    return to_pt2(ImGui::GetCursorScreenPos());
}

}
