
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

#include "dc_grabber_data_processing.hpp"

// std
#include <thread>
#include <mutex>

// local
#include "depth-camera/dc_frame_uncompressor.hpp"

using namespace tool::cam;

struct DCGrabberDataProcessing::Impl{

    std::atomic_bool processingThreadStarted       = false;
    std::unique_ptr<std::thread>thread = nullptr;
    std::unique_ptr<std::mutex>locker  = nullptr;

    std::shared_ptr<DCCompressedFrame>lastCF = nullptr;
    std::shared_ptr<DCFrame>lastF = nullptr;

    std::shared_ptr<DCCompressedFrame>cFrame = nullptr;
    std::shared_ptr<DCFrame> frame = nullptr;

    std::unique_ptr<DCFrameUncompressor> frameUncompressor;

    DCServerDataActions sActions;
};

DCGrabberDataProcessing::DCGrabberDataProcessing() : i(std::make_unique<Impl>()){
    i->locker = std::make_unique<std::mutex>();
    i->frameUncompressor = std::make_unique<DCFrameUncompressor>();
}

DCGrabberDataProcessing::~DCGrabberDataProcessing(){
    clean_processing_thread();
}

auto DCGrabberDataProcessing::start_processing_thread() -> void {

    if(i->processingThreadStarted){
        return;
    }
    i->thread = std::make_unique<std::thread>(&DCGrabberDataProcessing::process_thread, this);
}

auto DCGrabberDataProcessing::stop_processing_thread() -> void {
    i->processingThreadStarted = false;
}

auto DCGrabberDataProcessing::clean_processing_thread() -> void {

    i->processingThreadStarted = false;
    if(i->thread != nullptr){
        if(i->thread->joinable()){
            i->thread->join();
        }
        i->thread = nullptr;
    }
}


auto DCGrabberDataProcessing::new_compressed_frame(std::shared_ptr<DCCompressedFrame> frame) -> void {
    std::lock_guard<std::mutex> guard(*i->locker);
    i->lastCF = frame;
}

auto DCGrabberDataProcessing::new_frame(std::shared_ptr<DCFrame> frame) -> void {
    std::lock_guard<std::mutex> guard(*i->locker);
    i->lastF = frame;
}

auto DCGrabberDataProcessing::get_frame() -> std::shared_ptr<DCFrame> {
    std::lock_guard<std::mutex> guard(*i->locker);
    return i->frame;
}

auto DCGrabberDataProcessing::get_compressed_frame() -> std::shared_ptr<DCCompressedFrame> {
    std::lock_guard<std::mutex> guard(*i->locker);
    return i->cFrame;
}

auto DCGrabberDataProcessing::invalid_frame() -> void {
    std::lock_guard<std::mutex> guard(*i->locker);
    i->frame = nullptr;
}

auto DCGrabberDataProcessing::invalid_compressed_frame() -> void {
    std::lock_guard<std::mutex> guard(*i->locker);
    i->cFrame = nullptr;
}

auto DCGrabberDataProcessing::update_device_data_settings(const DCDataSettings &dataS) -> void {
    std::lock_guard<std::mutex> guard(*i->locker);
    i->sActions = dataS.server;
}

auto DCGrabberDataProcessing::process() -> bool{

    std::shared_ptr<DCCompressedFrame> frameToBeUncompresed = nullptr;
    {
        std::lock_guard<std::mutex> guard(*i->locker);

        if(i->lastCF){  // check for new compressed frame
            // store last
            i->cFrame = i->lastCF;
            // invalid it
            i->lastCF = nullptr;
            // ask for uncompression
            frameToBeUncompresed = i->cFrame;
        }else if(i->lastF){ // check for new frame
            // store last
            i->frame  = i->lastF;
            // invalid it
            i->lastF  = nullptr;
        }else{
            return false;
        }
    }

    // uncompress
    {
        if(frameToBeUncompresed != nullptr){
            auto uncompressedFrame = std::make_shared<DCFrame>();

            if(i->frameUncompressor->uncompress(i->sActions.generation, frameToBeUncompresed.get(), *uncompressedFrame)){
                std::lock_guard<std::mutex> guard(*i->locker);
                i->frame = uncompressedFrame;
                return true;
            }
        }
    }

    return false;
}

auto DCGrabberDataProcessing::uncompress(std::shared_ptr<DCCompressedFrame> cFrame) -> std::shared_ptr<DCFrame>{

    std::lock_guard<std::mutex> guard(*i->locker);
    if(!cFrame){
        return nullptr;
    }

    auto frame = std::make_shared<DCFrame>();
    if(i->frameUncompressor->uncompress(i->sActions.generation, cFrame.get(), *frame)){
        return frame;
    }
    return nullptr;
}


auto DCGrabberDataProcessing::process_thread() -> void {

    i->processingThreadStarted = true;

    while(i->processingThreadStarted){

        process();

        // sleep
        using namespace std::chrono_literals;
        std::this_thread::sleep_for(1ms);
    }
}
