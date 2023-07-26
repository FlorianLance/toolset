
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


#include "imgui_ui_drawer.hpp"

// std
#include <format>

// local
#include "imgui_convert.hpp"

using namespace tool::geo;

static auto vector_getter = [](void* vec, int idx, const char** out_text){
    auto& vector = *static_cast<std::vector<std::string>*>(vec);
    if (idx < 0 || idx >= static_cast<int>(vector.size())) {
        return false;
    }
    *out_text = vector.at(idx).c_str();
    return true;
};

auto tool::ImGuiUiDrawer::text_centered(const char* text, ...) -> void{

    va_list vaList = nullptr;
    va_start(vaList, text);

    float font_size = ImGui::GetFontSize() * strlen(text) / 2;
    ImGui::SameLine(
        ImGui::GetWindowSize().x / 2 -
        font_size + (font_size / 2)
    );

    ImGui::TextV(text, vaList);

    va_end(vaList);
}


auto tool::ImGuiUiDrawer::text_colored(const ImVec4 &col, const std::string &text) -> void{
    auto d = text.c_str();
    ImGui::TextColored(col, d, d + text.size());
}

auto tool::ImGuiUiDrawer::text_wrapped(const std::string &text, std::optional<Pt4f> color) -> void{
    auto d = text.c_str();
    if(!color.has_value()){
        ImGui::TextWrapped(d, d + text.size());
    }else{
        ImGui::PushStyleColor(ImGuiCol_Text, to_iv4(color.value()));
        ImGui::TextWrapped(d, d + text.size());
        ImGui::PopStyleColor();
    }
}

auto tool::ImGuiUiDrawer::text_wrapped(std::string_view text, std::optional<Pt4f> color) -> void{
    auto d = text.data();
    if(!color.has_value()){
        ImGui::TextWrapped(d, d + text.size());
    }else{
        ImGui::PushStyleColor(ImGuiCol_Text, to_iv4(color.value()));
        ImGui::TextWrapped(d, d + text.size());
        ImGui::PopStyleColor();
    }
}

auto tool::ImGuiUiDrawer::text(const std::string &text, std::optional<Pt4f> color) -> void{
    auto d = text.c_str();
    if(!color.has_value()){
        ImGui::Text(d, d + text.size());
    }else{
        ImGui::TextColored(to_iv4(color.value()), d, d + text.size());
    }
}

auto tool::ImGuiUiDrawer::text(std::string_view text, std::optional<Pt4f> color) -> void{
    auto d = text.data();
    if(!color.has_value()){
        ImGui::Text(d, d + text.size());
    }else{
        ImGui::TextColored(to_iv4(color.value()), d, d + text.size());
    }
}

auto tool::ImGuiUiDrawer::list_box(const char *label, int *currIndex, std::vector<std::string> &values) -> bool{
    if (values.empty()) {
        return false;
    }
    return ImGui::ListBox(label, currIndex, vector_getter,static_cast<void*>(&values), static_cast<int>(values.size()));
}

auto tool::ImGuiUiDrawer::combo(const char *label, int *currIndex, std::vector<std::string> &values) -> bool{
    if (values.empty()) {
        return false;
    }
    return ImGui::Combo(label, currIndex, vector_getter,static_cast<void*>(&values), static_cast<int>(values.size()));
}


auto tool::ImGuiUiDrawer::draw_drag_float_with_buttons(
        std::string_view displayName, std::string_view itemName, float *value, ImGuiFloatS fSettings, ImGuiDragS dSettings) -> bool{

    bool update = false;
    if(dSettings.defaultButton){
        if(ImGui::Button(std::format("D###default_{}",itemName).c_str())){
            *value = fSettings.defaultValue;
            update = true;
        }
        ImGui::SameLine();
    }

    if(dSettings.displayTextLeft && dSettings.displayText){
        text(displayName);
        ImGui::SameLine();
    }

    if(dSettings.decButton){
        if(ImGui::Button(std::format("-###minus_{}",itemName).c_str())){
            if((*value) > fSettings.min+fSettings.speedInc){
                (*value) -= fSettings.speedInc;
                update = true;
            }else{
                (*value) = fSettings.min;
                update = true;
            }
        }
        ImGui::SameLine();
    }

    ImGui::SetNextItemWidth(dSettings.widthDrag);
    auto currentV = *value;

    if(ImGui::DragFloat(std::format("###drag_{}", itemName).c_str(), value, fSettings.speedDrag, fSettings.min, fSettings.max, fSettings.format.c_str(), ImGuiSliderFlags_AlwaysClamp)){

        if((*value) < fSettings.min){
            *value = fSettings.min;
        }
        if((*value) > fSettings.max){
            *value = fSettings.max;
        }

        if(((*value) >= fSettings.min || (*value) <= fSettings.max) && currentV != (*value)){
            update = true;
        }
    }

    if(dSettings.incButton){
        ImGui::SameLine();
        if(ImGui::Button(std::format("+###plus_{}",itemName).c_str())){
            if((*value) < fSettings.max-fSettings.speedInc){
                (*value) +=fSettings.speedInc;
                update = true;
            }else{
                (*value) = fSettings.max;
                update = true;
            }
        }
    }

    if(!dSettings.displayTextLeft && dSettings.displayText){
        ImGui::SameLine();
        text(displayName);
    }

    return update;
}

auto tool::ImGuiUiDrawer::title2(std::string_view text) -> void{
    ImGui::Spacing();
    ImGui::Spacing();
    ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(0, 255, 0, 255));
    auto d = text.data();
    ImGuiUiDrawer::text_centered(d, d + text.size());
    ImGui::PopStyleColor();
    ImGui::Separator();
}

auto tool::ImGuiUiDrawer::begin_tab_bar(unsigned int *tabId, const char *label, ImGuiTabBarFlags flags) -> bool{
    m_tabId = tabId;
    return ImGui::BeginTabBar(label, flags);
}

auto tool::ImGuiUiDrawer::begin_tab_item(const char *label, const ImVec4 &activeColor, const ImVec4 &inactiveColor, bool *pOpen, ImGuiTabItemFlags flags) -> bool{

    if(!m_tabId){
        return ImGui::BeginTabItem(label, pOpen, flags);
    }

    ImGuiID id = ImGui::GetID(label);

    bool wasActive = (*m_tabId == id);

    if (wasActive){
        ImGui::PushStyleColor(ImGuiCol_Text, activeColor);
    }else{
        ImGui::PushStyleColor(ImGuiCol_Text, inactiveColor);
    }

    bool isActive = ImGui::BeginTabItem(label, pOpen, flags);
    ImGui::PopStyleColor();

    if (isActive){
        *m_tabId = id;
    }

    return isActive;
}

//auto tool::ImGuiUiDrawer::begin_tab_item(const char *label, ImGuiID &activeTab, const ImVec4 &activeColor, const ImVec4 &inactiveColor, bool *pOpen, ImGuiTabItemFlags flags) -> bool{

//    ImGuiID id = ImGui::GetID(label);
//    bool wasActive = (activeTab == id);

//    if (wasActive){
//        ImGui::PushStyleColor(ImGuiCol_Text, activeColor);
//    }else{
//        ImGui::PushStyleColor(ImGuiCol_Text, inactiveColor);
//    }

//    bool isActive = ImGui::BeginTabItem(label, pOpen, flags);
//    ImGui::PopStyleColor();

//    if (isActive){
//        activeTab = id;
//    }

//    return isActive;
//}

auto tool::ImGuiUiDrawer::draw_drag_int_with_buttons(std::string_view displayName, std::string_view itemName, int *value, ImGuiIntS iSettings, ImGuiDragS dSettings) -> bool{

    bool update = false;
    if(dSettings.defaultButton){
        if(ImGui::Button(std::format("D###default_{}",itemName).c_str())){
            *value = iSettings.defaultValue;
            update = true;
        }
        ImGui::SameLine();
    }

    if(dSettings.displayTextLeft && dSettings.displayText){
        text(displayName);
        ImGui::SameLine();
    }

    if(dSettings.decButton){
        if(ImGui::Button(std::format("-###minus_{}",itemName).c_str())){
            if((*value) > iSettings.min+iSettings.speedInc){
                (*value) -= iSettings.speedInc;
                update = true;
            }else{
                (*value) = iSettings.min;
                update = true;
            }
        }
        ImGui::SameLine();
    }

    ImGui::SetNextItemWidth(dSettings.widthDrag);
    auto currentV = *value;

    if(ImGui::DragInt(std::format("###drag_{}", itemName).c_str(), value, iSettings.speedDrag, iSettings.min, iSettings.max, "%d", ImGuiSliderFlags_AlwaysClamp)){

        if((*value) < iSettings.min){
            *value = iSettings.min;
        }
        if((*value) > iSettings.max){
            *value = iSettings.max;
        }

        if(((*value) >= iSettings.min || (*value) <= iSettings.max) && currentV != (*value)){
            update = true;
        }
    }

    if(dSettings.incButton){
        ImGui::SameLine();
        if(ImGui::Button(std::format("+###plus_{}",itemName).c_str())){
            if((*value) < iSettings.max-iSettings.speedInc){
                (*value) +=iSettings.speedInc;
                update = true;
            }else{
                (*value) = iSettings.max;
                update = true;
            }
        }
    }

    if(!dSettings.displayTextLeft && dSettings.displayText){
        ImGui::SameLine();
        text(displayName);
    }

    return update;
}


//PushStyleColor(ImGuiCol_Text, col); Text(fmt, ...); PopStyleColor();
// shortcut for PushTextWrapPos(0.0f); Text(fmt, ...); PopTextWrapPos();.
//Note that this won't work on an auto-resizing window if there's no other widgets to extend the window width, yoy may need to set a size using SetNextWindowSize().
