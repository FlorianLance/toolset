

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

#include "imgui_texture_ui_drawer.hpp"

// std
#include <format>

// local
#include "imgui-tb/imgui_ui_drawer.hpp"
#include "imgui-tb/imgui_helper.hpp"

using namespace tool;
using namespace tool::graphics;

void ImGuiTextureUiDrawer::init(gl::TBO *texture, bool invert){
    m_invert  = invert;
    m_texture = texture;
}

void ImGuiTextureUiDrawer::draw_child(const std::string &windowName, geo::Pt2f sizeWindow, std::optional<std::string> text){

    if(!m_texture){
        return;
    }

    if(ImGui::BeginChild(std::format("{}Window",windowName).c_str(), to_iv2(sizeWindow),false,ImGuiWindowFlags_NoScrollWithMouse)){

        if(ImGui::BeginTabBar(std::format("{}Tab",windowName).c_str(), ImGuiTabBarFlags_None)){
            if(ImGui::BeginTabItem(windowName.c_str())){

                auto size    = content_region_size_available();
                float scale  = std::min(1.f*size.y() / m_texture->height(),  1.f*size.x() / m_texture->width());
                auto sizeI   = ImVec2(m_texture->width() * scale, m_texture->height() * scale);

                auto cursorScreenPos = ImGui::GetCursorScreenPos();

                if(m_texture->id() == 0){
                    ImGui::Text("Texture not initialized.");
                }else{

                    if(m_invert){
                        ImGui::Image(m_texture->id(), sizeI,  ImVec2(0,1), ImVec2(1,0));
                    }else{
                        ImGui::Image(m_texture->id(), sizeI,  ImVec2(0,0), ImVec2(1,1));
                    }

                    auto io       = ImGui::GetIO();
                    auto min      = ImGui::GetItemRectMin();
                    auto size     = ImGui::GetItemRectSize();
                    auto mousePos = io.MousePos;
                    auto diff     = geo::Pt2f{mousePos.x-min.x, mousePos.y-min.y};

                    if(diff.x() > 0 && diff.x() < size.x && diff.y() > 0 && diff.y() < size.y){

                        hoveringPixel = (diff / scale).conv<int>();
                        for(int ii = 0; ii < 5; ++ii){
                            if(io.MouseDown[ii]){
                                mouseButtonsPressed[ii] = true;
                            }else if(io.MouseReleased[ii]){
                                mouseButtonsPressed[ii] = false;
                            }
                        }

                    }else{
                        hoveringPixel = {-1,-1};
                        for(int ii = 0; ii < 5; ++ii){
                            mouseButtonsPressed[ii] = false;
                        }
                    }

                    if(text.has_value()){
                        auto newCursorPos = ImGui::GetCursorScreenPos();
                        ImGui::SetCursorScreenPos(cursorScreenPos);
                        ImGuiUiDrawer::text_colored(ImVec4(1,0,0,1), text.value());
                        ImGui::SetCursorScreenPos(newCursorPos);
                    }
                }
                ImGui::EndTabItem();
            }
            if(ImGui::BeginTabItem("Infos")){
                auto size    = content_region_size_available();
                ImGui::Text("pointer = %d", m_texture->id());
                ImGui::Text("original size = %d x %d", m_texture->width(), m_texture->height());
                ImGui::Text("window size = %d x %d", static_cast<int>(size.x()), static_cast<int>(size.y()));
                ImGui::EndTabItem();
            }

            ImGui::EndTabBar();
        }
    }
    ImGui::EndChild();

}

void ImGuiTextureUiDrawer::draw_at_position(const geo::Pt2f &screenPos, const geo::Pt2f &sizeTexture, std::optional<std::string> text){

    if(!m_texture){
        return;
    }

    ImGui::SetCursorScreenPos(to_iv2(screenPos));

    if(m_texture->id() == 0){
        ImGui::Text("Texture not initialized.");
    }else{

        auto cursorScreenPos = ImGui::GetCursorScreenPos();

        if(m_invert){
            ImGui::Image(m_texture->id(), to_iv2(sizeTexture),  ImVec2(0,1), ImVec2(1,0));
        }else{
            ImGui::Image(m_texture->id(), to_iv2(sizeTexture),  ImVec2(0,0), ImVec2(1,1));
        }

        auto io       = ImGui::GetIO();
        auto min      = ImGui::GetItemRectMin();
        auto size     = ImGui::GetItemRectSize();
        auto mousePos = io.MousePos;
        auto diff     = geo::Pt2f{mousePos.x-min.x, mousePos.y-min.y};

        if(diff.x() > 0 && diff.x() < size.x && diff.y() > 0 && diff.y() < size.y){

            hoveringPixel = (diff).conv<int>();
            for(int ii = 0; ii < 5; ++ii){
                if(io.MouseDown[ii]){
                    mouseButtonsPressed[ii] = true;
                }else if(io.MouseReleased[ii]){
                    mouseButtonsPressed[ii] = false;
                }
            }

        }else{
            hoveringPixel = {-1,-1};
            for(int ii = 0; ii < 5; ++ii){
                mouseButtonsPressed[ii] = false;
            }
        }

        if(text.has_value()){
            auto newCursorPos = ImGui::GetCursorScreenPos();
            ImGui::SetCursorScreenPos(cursorScreenPos);
            ImGuiUiDrawer::text_colored(ImVec4(1,0,0,1), text.value());
            ImGui::SetCursorScreenPos(newCursorPos);
        }
    }

    ImGui::SetCursorScreenPos(to_iv2(screenPos));
}



//std::optional<geo::Pt2<int>> ImguiTextureDrawer::draw_texture(const std::string &name, geo::Pt2f screenPos, geo::Pt2f sizeTexture, const gl::TBO *texture, bool invert){

//    std::optional<geo::Pt2<int>> hoveringPixel = std::nullopt;

////    auto pos = ImGui::GetCursorScreenPos();
//    ImGui::SetCursorScreenPos(to_iv2(screenPos));

//    if(texture->id() == 0){
//        ImGui::Text(std::format("{}: texture not initialized.", name));
//    }else{

//        if(invert){
//            ImGui::Image(texture->id(), to_iv2(sizeTexture),  ImVec2(0,1), ImVec2(1,0));
//        }else{
//            ImGui::Image(texture->id(), to_iv2(sizeTexture),  ImVec2(0,0), ImVec2(1,1));
//        }

//        auto min      = ImGui::GetItemRectMin();
//        auto mousePos = ImGui::GetIO().MousePos;
//        hoveringPixel = geo::Pt2<int>{static_cast<int>(mousePos.x-min.x), static_cast<int>(mousePos.y-min.y)};

//        ImGui::SetCursorScreenPos(to_iv2(screenPos));
//        ImGui::TextColored(ImVec4(1,0,0,1),name);
//    }

////    ImGui::SetCursorScreenPos(pos);
//    return hoveringPixel;
//}


//std::optional<geo::Pt2<int>> ImguiTextureDrawer::draw_texture_tab_child(const std::string &windowName, geo::Pt2f sizeWindow, const gl::TBO *texture, bool invert, const std::string &optionalText){

//    std::optional<geo::Pt2<int>> hoveringPixel = std::nullopt;
//    if(ImGui::BeginChild(std::format("{}Window",windowName).c_str(), to_iv2(sizeWindow),false,ImGuiWindowFlags_NoScrollWithMouse)){

//        if(ImGui::BeginTabBar(std::format("{}Tab",windowName).c_str(), ImGuiTabBarFlags_None)){
//            if(ImGui::BeginTabItem(windowName.c_str())){

//                auto size    = ImGui::content_region_size_available();
//                float scale  = std::min(1.f*size.y() / texture->height(),  1.f*size.x() / texture->width());
//                auto sizeI   = ImVec2(texture->width() * scale, texture->height() * scale);

//                auto cursorScreenPos = ImGui::GetCursorScreenPos();

//                if(texture->id() == 0){
//                    ImGui::Text("Texture not initialized.");
//                }else{

//                    if(invert){
//                        ImGui::Image(texture->id(), sizeI,  ImVec2(0,1), ImVec2(1,0));
//                    }else{
//                        ImGui::Image(texture->id(), sizeI,  ImVec2(0,0), ImVec2(1,1));
//                    }

//                    auto min      = ImGui::GetItemRectMin();
//                    auto mousePos = ImGui::GetIO().MousePos;
//                    hoveringPixel = geo::Pt2<int>{static_cast<int>((mousePos.x-min.x)/ scale), static_cast<int>((mousePos.y-min.y)/ scale)} ;

//                    if(optionalText.length() > 0){
//                        auto newCursorPos = ImGui::GetCursorScreenPos();
//                        ImGui::SetCursorScreenPos(cursorScreenPos);
//                        ImGui::TextColored(ImVec4(1,0,0,1), optionalText);
//                        ImGui::SetCursorScreenPos(newCursorPos);
//                    }
//                }
//                ImGui::EndTabItem();
//            }
//            if(ImGui::BeginTabItem("Infos")){
//                auto size    = ImGui::content_region_size_available();
//                ImGui::Text("pointer = %d", texture->id());
//                ImGui::Text("original size = %d x %d", texture->width(), texture->height());
//                ImGui::Text("window size = %d x %d", static_cast<int>(size.x()), static_cast<int>(size.y()));
//                ImGui::EndTabItem();
//            }

//            ImGui::EndTabBar();
//        }
//    }
//    ImGui::EndChild();
//    return hoveringPixel;
//}
