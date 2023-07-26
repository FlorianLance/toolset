
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

#include "k4_grabber_data_processing.hpp"

using namespace tool::camera;

K4GrabberDataProcessing::K4GrabberDataProcessing(){
    m_locker            = std::make_unique<std::mutex>();
    m_locker            = std::make_unique<std::mutex>();
    m_frameUncompressor = std::make_unique<K4FrameUncompressor>();
}

K4GrabberDataProcessing::~K4GrabberDataProcessing(){
    clean();
}

auto K4GrabberDataProcessing::start() -> void {

    if(m_isProcessing){
        return;
    }
    m_thread = std::make_unique<std::thread>(&K4GrabberDataProcessing::process, this);
}

auto K4GrabberDataProcessing::stop() -> void {
    m_isProcessing = false;
}

auto K4GrabberDataProcessing::clean() -> void {

    m_isProcessing = false;

    if(m_thread != nullptr){
        if(m_thread->joinable()){
            m_thread->join();
        }
        m_thread = nullptr;
    }
}

auto K4GrabberDataProcessing::new_compressed_frame(std::shared_ptr<K4CompressedFrame> frame) -> void {
    std::lock_guard<std::mutex> guard(*m_locker);
    m_lastCF = frame;
}

auto K4GrabberDataProcessing::new_frame(std::shared_ptr<K4Frame> frame) -> void {
    std::lock_guard<std::mutex> guard(*m_locker);
    m_lastF = frame;
}

auto K4GrabberDataProcessing::get_frame() -> std::shared_ptr<K4Frame> {
    std::lock_guard<std::mutex> guard(*m_locker);
    return m_frame;
}

auto K4GrabberDataProcessing::get_compressed_frame() -> std::shared_ptr<K4CompressedFrame> {
    std::lock_guard<std::mutex> guard(*m_locker);
    return m_cFrame;
}

auto K4GrabberDataProcessing::invalid_frame() -> void {
    std::lock_guard<std::mutex> guard(*m_locker);
    m_frame = nullptr;
}

auto K4GrabberDataProcessing::invalid_compressed_frame() -> void {
    std::lock_guard<std::mutex> guard(*m_locker);
    m_cFrame = nullptr;
}


auto K4GrabberDataProcessing::process() -> void {

    m_isProcessing = true;

    while(m_isProcessing){

        std::shared_ptr<K4CompressedFrame> frameToBeUncompresed = nullptr;
        {
            std::lock_guard<std::mutex> guard(*m_locker);

            // check for new compressed frame
            if(m_lastCF){
                // store last
                m_cFrame = m_lastCF;
                // invalid it
                m_lastCF = nullptr;
                // ask for uncompression
                frameToBeUncompresed = m_cFrame;
            }
            // check for new frame
            if(m_lastF){
                // store last
                m_frame  = m_lastF;
                // invalid it
                m_lastF  = nullptr;
            }
        }

        // uncompress
        {
            if(frameToBeUncompresed != nullptr){
                auto uncompressedFrame = std::make_shared<K4Frame>();
                if(m_frameUncompressor->uncompress(frameToBeUncompresed.get(), *uncompressedFrame)){
                    std::lock_guard<std::mutex> guard(*m_locker);
                    m_frame = uncompressedFrame;
                }                
            }
        }

        // sleep
        using namespace std::chrono_literals;
        std::this_thread::sleep_for(1ms);
    }
}
