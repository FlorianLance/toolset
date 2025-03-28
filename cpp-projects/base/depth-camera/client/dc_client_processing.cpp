
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
    // cam::DCVideo videoResource;
    Buffer<std::unique_ptr<cam::DCFrameProcessor>> frameProcessors;    
};

DCClientProcessing::DCClientProcessing() : i(std::make_unique<Impl>()){
}

DCClientProcessing::~DCClientProcessing(){
    clean();
}

auto DCClientProcessing::initialize(size_t nbDevices, bool startProcessingThread) -> void {

    if(nb_devices() != 0){
        clean();
    }

    i->frameProcessors.reserve(nbDevices);
    for(size_t id = 0; id < nbDevices; ++id){
        add_device_processor(startProcessingThread);
    }
}

auto DCClientProcessing::add_device_processor(bool startProcessingThread) -> void{
    auto frameProcessor = std::make_unique<DCFrameProcessor>();
    if(startProcessingThread){
        frameProcessor->start_processing_thread();
    }
    i->frameProcessors.push_back(std::move(frameProcessor));
}

auto DCClientProcessing::remove_last_device_processor() -> void{
    if(i->frameProcessors.empty()){
        return;
    }
    i->frameProcessors.back()->stop_processing_thread();
    i->frameProcessors.back()->clean_processing_thread();
    i->frameProcessors.remove_last();
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

auto DCClientProcessing::new_data_frame(size_t idD, std::shared_ptr<DCDataFrame> frame) -> void {

    if(idD >= i->frameProcessors.size()){
        Log::error(std::format("[DCClientProcessing::new_data_frame] Invalid camera id [{}], only [{}] devices.\n", idD, i->frameProcessors.size()));
        return;
    }

    i->frameProcessors[idD]->new_data_frame(frame);
}


auto DCClientProcessing::new_frame(size_t idD, std::shared_ptr<cam::DCFrame> frame) -> void {

    if(idD >= i->frameProcessors.size()){
        Log::error(std::format("[DCClientProcessing::new_frame] Invalid camera id [{}], only [{}] devices.\n", idD, i->frameProcessors.size()));
        return;
    }
    i->frameProcessors[idD]->new_frame(frame);
}

auto DCClientProcessing::nb_devices() const noexcept -> size_t{
    return i->frameProcessors.size();
}

auto DCClientProcessing::get_frame(size_t idD) -> std::shared_ptr<DCFrame>{

    if(idD >= i->frameProcessors.size()){
        Log::error(std::format("[DCClientProcessing::get_frame] Invalid camera id [{}], only [{}] devices.\n", idD, i->frameProcessors.size()));
        return nullptr;
    }
    return i->frameProcessors[idD]->get_frame();
}


auto DCClientProcessing::get_data_frame(size_t idD) -> std::shared_ptr<DCDataFrame>{

    if(idD >= i->frameProcessors.size()){
        Log::error(std::format("[DCClientProcessing::get_data_frame] Invalid camera id [{}], only [{}] devices.\n", idD, i->frameProcessors.size()));
        return nullptr;
    }
    return i->frameProcessors[idD]->get_data_frame();
}

auto DCClientProcessing::get_uc_usage(size_t idD) const noexcept -> double{
    if(idD < i->frameProcessors.size()){
        return i->frameProcessors[idD]->ucUsage;
    }
    return 0.0;
}

auto DCClientProcessing::get_average_process_duration_micro_s(size_t idD) const noexcept -> double{
    if(idD < i->frameProcessors.size()){
        return i->frameProcessors[idD]->avegageProcessMs;
    }
    return 0.0;
}


auto DCClientProcessing::invalid_last_frame(size_t idD) -> void {

    if(idD >= i->frameProcessors.size()){
        Log::error(std::format("[DCClientProcessing::invalid_last_frame] Invalid camera id [{}], only [{}] devices.\n", idD, i->frameProcessors.size()));
        return;
    }
    i->frameProcessors[idD]->invalid_frame();
}

auto DCClientProcessing::invalid_last_data_frame(size_t idD) -> void {

    if(idD >= i->frameProcessors.size()){
        Log::error(std::format("[DCClientProcessing::invalid_last_data_frame] Invalid camera id [{}], only [{}] devices.\n", idD, i->frameProcessors.size()));
        return;
    }
    i->frameProcessors[idD]->invalid_data_frame();
}

auto DCClientProcessing::update_device_settings(size_t idD, const DCDeviceSettings &deviceS) -> void {

    if(idD >= i->frameProcessors.size()){
        Log::error(std::format("[DCClientProcessing::update_device_settings]Invalid camera id [{}], only [{}] devices.\n", idD, i->frameProcessors.size()));
        return;
    }
    i->frameProcessors[idD]->update_generation_settings(deviceS.dataS.clientGeneration);
}

auto DCClientProcessing::process_from_external_thread(size_t idD) -> std::tuple<std::shared_ptr<DCFrame>, std::shared_ptr<DCDataFrame>>{

    if(idD >= i->frameProcessors.size()){
        Log::error(std::format("[DCClientProcessing::process] Invalid camera id [{}], only [{}] devices.\n", idD, i->frameProcessors.size()));
        return {nullptr,nullptr};
    }
    return i->frameProcessors[idD]->process();
}

// auto DCClientProcessing::generate_frame(size_t idD, std::shared_ptr<DCDataFrame> frame) -> std::shared_ptr<DCFrame>{

//     if(idD >= i->frameProcessors.size()){
//         Log::error(std::format("[DCClientProcessing::uncompress_frame] Invalid camera id [{}], only [{}] devices.\n", idD, i->frameProcessors.size()));
//         return nullptr;
//     }
//     return i->frameProcessors[idD]->generate(frame);
// }

auto DCClientProcessing::save_current_cloud(size_t idD, const std::string &path) -> bool{

    if(idD >= i->frameProcessors.size()){
        Log::error(std::format("[DCClientProcessing::save_current_cloud] Invalid camera id [{}], only [{}] devices.\n", idD, i->frameProcessors.size()));
        return false;
    }
    
    Log::message(std::format("save_current_cloud {} {} {}\n", idD, path, i->frameProcessors[idD]->get_frame() != nullptr));
    if(auto frame = i->frameProcessors[idD]->get_frame(); frame != nullptr){        
        if(frame->volumesB.contains(DCVolumeBufferType::ColoredCloud)){
            auto &cloud = std::get<geo::ColorCloud>(frame->volumesB[DCVolumeBufferType::ColoredCloud]);
            Log::message(std::format("cloud {}\n", cloud.size()));
            return io::CloudIO::save_cloud(path, cloud);
        }
    }
    return false;
}

