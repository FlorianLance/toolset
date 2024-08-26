
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

#include "dc_client_processing.hpp"

// std
#include <format>

// local
#include "utility/logger.hpp"
#include "io/cloud_io.hpp"
#include "depth-camera/frame/dc_frame_processor.hpp"
#include "depth-camera/dc_video.hpp"

using namespace tool::cam;

struct DCClientProcessing::Impl{
    // std::mutex recordLocker;
    // std::atomic_bool record = false;
    std::vector<std::unique_ptr<cam::DCFrameProcessor>> frameProcessors;
    // cam::DCVideo videoResource;
};

DCClientProcessing::DCClientProcessing() : i(std::make_unique<Impl>()){
}

DCClientProcessing::~DCClientProcessing(){
    clean();
}

auto DCClientProcessing::initialize(size_t nbDevices, bool startProcessingThread) -> void {

    if(nb_frame_processors() != 0){
        clean();
    }

    i->frameProcessors.reserve(nbDevices);
    for(size_t id = 0; id < nbDevices; ++id){
        i->frameProcessors.push_back(std::make_unique<DCFrameProcessor>());
    }

    if(startProcessingThread){
        for(auto &frameProcessor : i->frameProcessors){
            frameProcessor->start_processing_thread();
        }
    }
}

auto DCClientProcessing::add_frame_processor() -> void{
    auto frameProcessor = std::make_unique<DCFrameProcessor>();
    frameProcessor->start_processing_thread();
    i->frameProcessors.push_back(std::move(frameProcessor));
}

auto DCClientProcessing::remove_frame_processor(size_t idDevice) -> void{
    if(idDevice < nb_frame_processors()){
        i->frameProcessors[idDevice]->stop_processing_thread();
        i->frameProcessors[idDevice]->clean_processing_thread();
        i->frameProcessors.erase(i->frameProcessors.begin() + idDevice);
    }
}

auto DCClientProcessing::clean() -> void{

    for(auto &frameProcessor : i->frameProcessors){
        frameProcessor->stop_processing_thread();
    }
    for(auto &frameProcessor : i->frameProcessors){
        frameProcessor->clean_processing_thread();
    }
    i->frameProcessors.clear();
}

auto DCClientProcessing::new_compressed_frame(size_t idC, std::shared_ptr<DCCompressedFrame> frame) -> void {

    if(idC >= i->frameProcessors.size()){
        Logger::error(std::format("[DCClientProcessing::new_compressed_frame] Invalid camera id {}, only {} grabbers.\n", idC, i->frameProcessors.size()));
        return;
    }

    i->frameProcessors[idC]->new_compressed_frame(frame);
}


auto DCClientProcessing::new_frame(size_t idC, std::shared_ptr<cam::DCFrame> frame) -> void {

    if(idC >= i->frameProcessors.size()){
        Logger::error(std::format("[DCClientProcessing::new_frame] Invalid camera id {}, only {} grabbers.\n", idC, i->frameProcessors.size()));
        return;
    }
    i->frameProcessors[idC]->new_frame(frame);
}

auto DCClientProcessing::nb_frame_processors() const noexcept -> size_t{
    return i->frameProcessors.size();
}

auto DCClientProcessing::get_frame(size_t idC) -> std::shared_ptr<DCFrame>{

    if(idC >= i->frameProcessors.size()){
        Logger::error(std::format("[DCClientProcessing::get_frame] Invalid camera id {}, only {} grabbers.\n", idC, i->frameProcessors.size()));
        return nullptr;
    }
    return i->frameProcessors[idC]->get_frame();
}


auto DCClientProcessing::get_compressed_frame(size_t idC) -> std::shared_ptr<DCCompressedFrame>{

    if(idC >= i->frameProcessors.size()){
        Logger::error(std::format("[DCClientProcessing::get_compressed_frame] Invalid camera id {}, only {} grabbers.\n", idC, i->frameProcessors.size()));
        return nullptr;
    }
    return i->frameProcessors[idC]->get_compressed_frame();
}


auto DCClientProcessing::invalid_last_frame(size_t idC) -> void {

    if(idC >= i->frameProcessors.size()){
        Logger::error(std::format("[DCClientProcessing::invalid_last_frame] Invalid camera id {}, only {} grabbers.\n", idC, i->frameProcessors.size()));
        return;
    }
    i->frameProcessors[idC]->invalid_frame();
}

auto DCClientProcessing::invalid_last_compressed_frame(size_t idC) -> void {

    if(idC >= i->frameProcessors.size()){
        Logger::error(std::format("[DCClientProcessing::invalid_last_compressed_frame] Invalid camera id {}, only {} grabbers.\n", idC, i->frameProcessors.size()));
        return;
    }
    i->frameProcessors[idC]->invalid_compressed_frame();
}

auto DCClientProcessing::update_device_settings(size_t idC, const DCDeviceSettings &deviceS) -> void {

    if(idC >= i->frameProcessors.size()){
        Logger::error(std::format("[DCClientProcessing::update_device_settings] Invalid camera id {}, only {} grabbers.\n", idC, i->frameProcessors.size()));
        return;
    }
    i->frameProcessors[idC]->update_generation_settings(deviceS.dataS.clientGeneration);
}

auto DCClientProcessing::process() -> void{
    for(auto &frameProcessor : i->frameProcessors){
        frameProcessor->process();
    }
}

auto DCClientProcessing::uncompress_frame(size_t idC, std::shared_ptr<DCCompressedFrame> frame) -> std::shared_ptr<DCFrame>{
    if(idC >= i->frameProcessors.size()){
        Logger::error(std::format("[DCClientProcessing::uncompress_frame] Invalid camera id {}, only {} grabbers.\n", idC, i->frameProcessors.size()));
        return nullptr;
    }
    return i->frameProcessors[idC]->uncompress(frame);
}


auto DCClientProcessing::save_current_cloud(size_t idC, const std::string &path) -> bool{
    if(idC >= i->frameProcessors.size()){
        Logger::error(std::format("[DCClientProcessing::save_current_cloud] Invalid camera id {}, only {} grabbers.\n", idC, i->frameProcessors.size()));
        return false;
    }

    Logger::message(std::format("save_current_cloud {} {} {}\n", idC, path, i->frameProcessors[idC]->get_frame() != nullptr));
    if(auto frame = i->frameProcessors[idC]->get_frame(); frame != nullptr){
        Logger::message(std::format("cloud {}\n", frame->cloud.size()));
        return io::CloudIO::save_cloud(path, frame->cloud);
    }
    return false;
}

