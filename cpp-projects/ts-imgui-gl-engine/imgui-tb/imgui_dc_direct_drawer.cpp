
/*******************************************************************************
** Toolset-ts-imgui-gl-engine                                                 **
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

#include "imgui_dc_direct_drawer.hpp"

// std
#include <format>

// base
#include "geometry/cloud_io.hpp"
#include "utility/logger.hpp"
#include "utility/time.hpp"

// local
#include "imgui_ui_drawer.hpp"

using namespace tool::graphics;

auto DCDirectDrawer::initialize(size_t nbGrabbers) -> void{
    
    auto lg = LogG("DCDirectDrawer::initialize");
    DCCloudsSceneDrawer::initialize(nbGrabbers);

    // if(m_lockers.empty()){
    //     m_lockers.resize(20);
    //     for(auto &locker : m_lockers){
    //         locker = std::make_unique<std::mutex>();
    //     }
    // }

    m_locker.lock();

    if(nbGrabbers > m_lastFrames.size()){
        while(m_lastFrames.size() != nbGrabbers){
            m_lastFrames.push_back(nullptr);
            m_swapLastFrames.push_back(nullptr);
        }
    }else if(nbGrabbers < m_lastFrames.size()){
        m_lastFrames.resize(nbGrabbers);
        m_swapLastFrames.resize(nbGrabbers);
    }

    m_locker.unlock();
}

auto DCDirectDrawer::update() -> void{

    auto t1 = Time::nanoseconds_since_epoch();

    m_locker.lock();
    for(size_t idG = 0; idG < m_lastFrames.size(); ++idG){
        m_swapLastFrames[idG] = m_lastFrames[idG];
        // std::swap(m_swapLastFrames[idG], m_lastFrames[idG]);
    }
    m_locker.unlock();

    auto t2 = Time::nanoseconds_since_epoch();

    for(size_t idG = 0; idG < m_swapLastFrames.size(); ++idG){
        if(m_swapLastFrames[idG] != nullptr){
            update_from_frame(idG, m_swapLastFrames[idG]);
            m_redrawClouds = true;
            // m_swapLastFrames[idG] = nullptr;
        }
    }

    auto t3 = Time::nanoseconds_since_epoch();

    bool redrwFBO = m_redrawClouds || has_to_redraw_clouds();
    if(redrwFBO){

        // store visibility
        std::vector<int> cloudsVisibility;
        for(size_t ii = 0; ii < cloudsD.size(); ++ii){
            cloudsVisibility.push_back(cloudsD[ii]->display.showCapture ? 1 : 0);
        }

        // display only current client id cloud if per tab
        if(!m_allTabOpened){
            for(size_t ii = 0; ii < cloudsD.size(); ++ii){
                if(cloudsD[ii]->display.showCapture){
                    cloudsD[ii]->display.showCapture = ii == m_currentTabOpened;
                }
            }
        }

        // set_frustum_display_state(m_allTabOpened);
        set_frustum_display_state(true);
        set_gizmo_display_state(!m_allTabOpened);

        draw_clouds_to_fbo();

        // restore visibility
        for(size_t ii = 0; ii < cloudsD.size(); ++ii){
            cloudsD[ii]->display.showCapture = cloudsVisibility[ii] == 1;
        }
    }
    m_redrawClouds = false;

    auto t4 = Time::nanoseconds_since_epoch();
    // Log::fmessage("direct {} {} {} {}\n", redrwFBO, Time::difference_micro_s(t1,t2),Time::difference_micro_s(t2,t3),Time::difference_micro_s(t3,t4));
}

auto DCDirectDrawer::update_frame(size_t idGrabber, std::shared_ptr<cam::DCFrame> frame) -> void{
    m_locker.lock();
    m_lastFrames[idGrabber] = std::move(frame);
    m_locker.unlock();
}

auto DCDirectDrawer::draw(bool focus) -> void{


    static ImGuiID tabId = 0;
    if (ImGuiUiDrawer::begin_tab_bar(&tabId, "###display_direct_tabbar")){

        // all
        if ((m_allTabOpened = ImGuiUiDrawer::begin_tab_item("All###display_direct_all_tabitem"))){
            display.drawOnlyCloudId  = -1;
            draw_all_clouds_drawers_in_one_tab();
            ImGui::EndTabItem();
        }

        // per client
        int previousCloudIdDisplayed = display.drawOnlyCloudId;
        for(size_t ii = 0; ii < cloudsD.size(); ++ii){
            if (ImGuiUiDrawer::begin_tab_item(std::format("[{}]###display_direct_per_client_tabitem_{}", ii, ii).c_str())){
                m_currentTabOpened = ii;
                display.drawOnlyCloudId  = ii;
                draw_cloud_drawer_tab(ii, focus, "display_direct"sv);
                ImGui::EndTabItem();
            }
        }

        ImGui::EndTabBar();
    }
}

auto DCDirectDrawer::draw_only_clouds() -> void{

    static ImGuiID tabId = 0;
    if (ImGuiUiDrawer::begin_tab_bar(&tabId, "###display_direct_tabbar")){

        // all
        if (ImGuiUiDrawer::begin_tab_item("All###display_direct_all_tabitem")){
            draw_all_clouds_drawers_in_one_tab();
            ImGui::EndTabItem();
        }

        ImGui::EndTabBar();
    }
}

auto DCDirectDrawer::save_current_cloud(size_t idC, const std::string &path) -> void{
    
    Log::log("DCDirectDrawer::save_current_cloud\n");

    m_locker.lock();
    auto frame = m_swapLastFrames[idC];// cloudsD[idC]->lastFrame;
    m_locker.unlock();

    if(frame != nullptr){
        if(auto cloud = frame->volume_buffer<geo::ColorCloud>(cam::DCVolumeBufferType::ColoredCloud)){
            geo::CloudIO::save_cloud(path, *cloud);
        }
    }
}

