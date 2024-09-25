

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

// std
#include <vector>
#include <optional>
#include <optional>

// imgui
#include <imgui/imgui.h>

// local
#include "imgui_types.hpp"
#include "geometry/point4.hpp"

namespace tool{

struct ImGuiStyles{
    struct Colors{

    };
};

struct ImGuiUiDrawer{


[[maybe_unused]] static auto combo(const char* label, int* currIndex, std::vector<std::string>& values) -> bool;
[[maybe_unused]] static auto list_box(const char* label, int* currIndex, std::vector<std::string>& values) -> bool;
[[maybe_unused]] static auto text(std::string_view text, std::optional<tool::geo::Pt4f> color = std::nullopt) -> void;
[[maybe_unused]] static auto text(const std::string &text, std::optional<tool::geo::Pt4f> color = std::nullopt) -> void;
[[maybe_unused]] static auto text_wrapped(std::string_view text, std::optional<tool::geo::Pt4f> color = std::nullopt) -> void;
[[maybe_unused]] static auto text_wrapped(const std::string &text, std::optional<tool::geo::Pt4f> color = std::nullopt) -> void;
[[maybe_unused]] static auto text_colored(const ImVec4 &col, std::string_view text) -> void;
[[maybe_unused]] static auto text_colored(const ImVec4& col, const std::string &text) -> void;

[[maybe_unused]] static auto text_centered(const char* text, ...) -> void;

[[maybe_unused]] static auto draw_drag_int_with_buttons(std::string_view displayName, std::string_view itemName, int *value, ImGuiIntS iSettings, ImGuiDragS dSettings) -> bool;
[[maybe_unused]] static auto draw_drag_float_with_buttons(std::string_view displayName, std::string_view itemName, float *value, ImGuiFloatS fSettings, ImGuiDragS dSettings) -> bool;

[[maybe_unused]] static auto title(std::string_view text, tool::geo::Pt4f color = {0.f, 1.f, 0.f, 1.f}) -> void;

[[maybe_unused]] static auto begin_tab_bar(unsigned int *tabId, const char* label, ImGuiTabBarFlags flags = 0) -> bool;
//[[maybe_unused]] static auto begin_tab_item(const char* label, ImGuiID &activeTab, const ImVec4 &activeColor = ImVec4(0, 1, 0, 1), const ImVec4 &inactiveColor = ImVec4(1, 1, 1, 1), bool* pOpen = nullptr, ImGuiTabItemFlags flags = 0) -> bool;
[[maybe_unused]] static auto begin_tab_item(const char* label, const ImVec4 &activeColor = ImVec4(0, 1, 0, 1), const ImVec4 &inactiveColor = ImVec4(1, 1, 1, 1), bool* pOpen = nullptr, ImGuiTabItemFlags flags = 0) -> bool;


private:
    static inline unsigned int *m_tabId = nullptr;
};

}
