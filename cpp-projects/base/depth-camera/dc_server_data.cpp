
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

#include "dc_server_data.hpp"

// std
#include <format>
#include <mutex>
#include <execution>

// local
#include "utility/logger.hpp"

#include "dc_grabber_data_processing.hpp"
#include "dc_video.hpp"

using namespace tool::cam;

struct DCServerData::Impl{
    std::mutex recordLocker;
    std::atomic_bool record = false;
    std::vector<std::unique_ptr<cam::DCGrabberDataProcessing>>grabbersDataProcessing;
    cam::DCVideo videoResource;
};

DCServerData::DCServerData() : i(std::make_unique<Impl>()){
}

DCServerData::~DCServerData(){
    clean();
}

auto DCServerData::initialize(size_t nbDevices, bool startProcessingThread) -> void {

    if(nb_grabbers() != 0){
        clean();
    }

    i->grabbersDataProcessing.reserve(nbDevices);
    for(size_t id = 0; id < nbDevices; ++id){
        i->grabbersDataProcessing.push_back(std::make_unique<DCGrabberDataProcessing>());
    }

    if(startProcessingThread){
        for(auto &grabber : i->grabbersDataProcessing){
            grabber->start_processing_thread();
        }
    }
}

auto DCServerData::add_device() -> void{
    auto grabber = std::make_unique<DCGrabberDataProcessing>();
    grabber->start_processing_thread();
    i->grabbersDataProcessing.push_back(std::move(grabber));
}

auto DCServerData::remove_device(size_t idDevice) -> void{
    if(idDevice < nb_grabbers()){
        i->grabbersDataProcessing[idDevice]->stop_processing_thread();
        i->grabbersDataProcessing[idDevice]->clean_processing_thread();
        i->grabbersDataProcessing.erase(i->grabbersDataProcessing.begin() + idDevice);
    }
}

auto DCServerData::clean() -> void{

    for(auto &grabber : i->grabbersDataProcessing){
        grabber->stop_processing_thread();
    }
    for(auto &grabber : i->grabbersDataProcessing){
        grabber->clean_processing_thread();
    }
    i->grabbersDataProcessing.clear();
}

auto DCServerData::new_compressed_frame(size_t idC, std::shared_ptr<DCCompressedFrame> frame) -> void {

    if(idC >= i->grabbersDataProcessing.size()){
        Logger::error(std::format("[DCServerData::new_compressed_frame] Invalid frame id {}, only {} grabbers.\n", idC, i->grabbersDataProcessing.size()));
        return;
    }

    i->grabbersDataProcessing[idC]->new_compressed_frame(frame);
}


auto DCServerData::new_frame(size_t idC, std::shared_ptr<cam::DCFrame> frame) -> void {

    if(idC >= i->grabbersDataProcessing.size()){
        Logger::error(std::format("[DCServerData::new_frame] Invalid frame id {}, only {} grabbers.\n", idC, i->grabbersDataProcessing.size()));
        return;
    }
    i->grabbersDataProcessing[idC]->new_frame(frame);
}

auto DCServerData::nb_grabbers() const noexcept -> size_t{
    return i->grabbersDataProcessing.size();
}

auto DCServerData::get_frame(size_t idC) -> std::shared_ptr<DCFrame>{

    if(idC >= i->grabbersDataProcessing.size()){
        Logger::error(std::format("[DCServerData::get_frame] Invalid frame id {}, only {} grabbers.\n", idC, i->grabbersDataProcessing.size()));
        return nullptr;
    }
    return i->grabbersDataProcessing[idC]->get_frame();
}


auto DCServerData::get_compressed_frame(size_t idC) -> std::shared_ptr<DCCompressedFrame>{

    if(idC >= i->grabbersDataProcessing.size()){
        Logger::error(std::format("[DCServerData::get_compressed_frame] Invalid frame id {}, only {} grabbers.\n", idC, i->grabbersDataProcessing.size()));
        return nullptr;
    }
    return i->grabbersDataProcessing[idC]->get_compressed_frame();
}


auto DCServerData::invalid_last_frame(size_t idC) -> void {

    if(idC >= i->grabbersDataProcessing.size()){
        Logger::error(std::format("[DCServerData::invalid_last_frame] Invalid frame id {}, only {} grabbers.\n", idC, i->grabbersDataProcessing.size()));
        return;
    }
    i->grabbersDataProcessing[idC]->invalid_frame();
}

auto DCServerData::invalid_last_compressed_frame(size_t idC) -> void {


    if(idC >= i->grabbersDataProcessing.size()){
        Logger::error(std::format("[DCServerData::invalid_last_compressed_frame] Invalid frame id {}, only {} grabbers.\n", idC, i->grabbersDataProcessing.size()));
        return;
    }
    i->grabbersDataProcessing[idC]->invalid_compressed_frame();
}

auto DCServerData::update_device_settings(size_t idC, const DCDeviceSettings &deviceS) -> void {

    if(idC >= i->grabbersDataProcessing.size()){
        Logger::error(std::format("[DCServerData::update_device_settings] Invalid frame id {}, only {} grabbers.\n", idC, i->grabbersDataProcessing.size()));
        return;
    }
    i->grabbersDataProcessing[idC]->update_device_data_settings(deviceS.dataS);
}

auto DCServerData::uncompress_frame(size_t idC, std::shared_ptr<DCCompressedFrame> frame) -> std::shared_ptr<DCFrame>{
    if(idC >= i->grabbersDataProcessing.size()){
        Logger::error(std::format("[DCServerData::uncompress_frame] Invalid frame id {}, only {} grabbers.\n", idC, i->grabbersDataProcessing.size()));
        return nullptr;
    }
    return i->grabbersDataProcessing[idC]->uncompress(frame);
}

// auto DCServerData::process_data() -> size_t{

//     std::atomic<size_t> nbFramesAvailable = 0;
//     std::for_each(std::execution::seq, std::begin(i->grabbersDataProcessing), std::end(i->grabbersDataProcessing), [&](std::unique_ptr<cam::DCGrabberDataProcessing> &grabber){
//         if(grabber->process()){
//             nbFramesAvailable++;
//         }
//     });
//     return nbFramesAvailable;
// }