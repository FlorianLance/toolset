
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

#include "k4_server_data.hpp"

// std
#include <format>

// local
#include "utility/logger.hpp"

using namespace tool::camera;

K4ServerData::~K4ServerData(){
    clean();
}

auto K4ServerData::initialize(size_t nbGrabbers) -> void {

    m_grabbersDataProcessing.resize(nbGrabbers);
    for(auto &grabber : m_grabbersDataProcessing){
        grabber = std::make_unique<K4GrabberDataProcessing>();
    }

    for(auto &grabber : m_grabbersDataProcessing){
        grabber->start();
    }
}

auto K4ServerData::clean() -> void{

    for(auto &grabber : m_grabbersDataProcessing){
        grabber->stop();
    }
    for(auto &grabber : m_grabbersDataProcessing){
        grabber->clean();
    }
    m_grabbersDataProcessing.clear();
}

auto K4ServerData::new_compressed_frame(size_t idC, std::shared_ptr<K4CompressedFrame> frame) -> void {

    if(idC >= m_grabbersDataProcessing.size()){
        Logger::error(std::format("[K4ServerData::new_compressed_frame] Invalid frame id {}, only {} grabbers.\n", idC, m_grabbersDataProcessing.size()));
        return;
    }

    m_grabbersDataProcessing[idC]->new_compressed_frame(frame);
}


auto K4ServerData::new_frame(size_t idC, std::shared_ptr<camera::K4Frame> frame) -> void {

    if(idC >= m_grabbersDataProcessing.size()){
        Logger::error(std::format("[K4ServerData::new_frame] Invalid frame id {}, only {} grabbers.\n", idC, m_grabbersDataProcessing.size()));
        return;
    }
    m_grabbersDataProcessing[idC]->new_frame(frame);
}

auto K4ServerData::get_frame(size_t idC) -> std::shared_ptr<K4Frame>{

    if(idC >= m_grabbersDataProcessing.size()){
        Logger::error(std::format("[K4ServerData::get_frame] Invalid frame id {}, only {} grabbers.\n", idC, m_grabbersDataProcessing.size()));
        return nullptr;
    }
    return m_grabbersDataProcessing[idC]->get_frame();
}


auto K4ServerData::get_compressed_frame(size_t idC) -> std::shared_ptr<K4CompressedFrame>{

    if(idC >= m_grabbersDataProcessing.size()){
        Logger::error(std::format("[K4ServerData::get_compressed_frame] Invalid frame id {}, only {} grabbers.\n", idC, m_grabbersDataProcessing.size()));
        return nullptr;
    }
    return m_grabbersDataProcessing[idC]->get_compressed_frame();
}


auto K4ServerData::invalid_last_frame(size_t idC) -> void {

    if(idC >= m_grabbersDataProcessing.size()){
        Logger::error(std::format("[K4ServerData::invalid_last_frame] Invalid frame id {}, only {} grabbers.\n", idC, m_grabbersDataProcessing.size()));
        return;
    }
    m_grabbersDataProcessing[idC]->invalid_frame();
}

auto K4ServerData::invalid_last_compressed_frame(size_t idC) -> void {

    if(idC >= m_grabbersDataProcessing.size()){
        Logger::error(std::format("[K4ServerData::invalid_last_compressed_frame] Invalid frame id {}, only {} grabbers.\n", idC, m_grabbersDataProcessing.size()));
        return;
    }
    m_grabbersDataProcessing[idC]->invalid_compressed_frame();
}
