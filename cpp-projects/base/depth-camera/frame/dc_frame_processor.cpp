
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

#include "dc_frame_processor.hpp"

// std
#include <thread>
#include <mutex>

// local
#include "depth-camera/frame/dc_frame_generator.hpp"
#include "utility/time.hpp"
#include "utility/monitoring.hpp"

using namespace tool::cam;
using namespace std::chrono;

struct DCFrameProcessor::Impl{

    std::atomic_bool processingThreadStarted    = false;
    std::unique_ptr<std::thread> thread         = nullptr;
    std::unique_ptr<std::mutex> locker          = nullptr;

    std::shared_ptr<DCDataFrame> lastDF = nullptr;
    std::shared_ptr<DCFrame>lastF       = nullptr;

    std::shared_ptr<DCDataFrame> dFrame = nullptr;
    std::shared_ptr<DCFrame> frame      = nullptr;

    std::unique_ptr<DCFrameGenerator> frameGenerator;
    DCFrameGenerationSettings generationS;

    SumBuffer processB;
    SumBuffer sleepB;
    AverageBuffer averageProcessB;
};

DCFrameProcessor::DCFrameProcessor() : i(std::make_unique<Impl>()){
    i->locker = std::make_unique<std::mutex>();
    i->frameGenerator = std::make_unique<DCFrameGenerator>();
}

DCFrameProcessor::~DCFrameProcessor(){
    clean_processing_thread();
}

auto DCFrameProcessor::start_processing_thread() -> void {

    if(i->processingThreadStarted){
        return;
    }
    i->thread = std::make_unique<std::thread>(&DCFrameProcessor::process_thread, this);
}

auto DCFrameProcessor::stop_processing_thread() -> void {
    i->processingThreadStarted = false;
}

auto DCFrameProcessor::clean_processing_thread() -> void {

    i->processingThreadStarted = false;
    if(i->thread != nullptr){
        if(i->thread->joinable()){
            i->thread->join();
        }
        i->thread = nullptr;
    }
}

auto DCFrameProcessor::new_data_frame(std::shared_ptr<DCDataFrame> frame) -> void {
    std::lock_guard<std::mutex> guard(*i->locker);
    i->lastDF = frame;
}

auto DCFrameProcessor::new_frame(std::shared_ptr<DCFrame> frame) -> void {
    std::lock_guard<std::mutex> guard(*i->locker);
    i->lastF = frame;
}

auto DCFrameProcessor::get_frame() -> std::shared_ptr<DCFrame> {
    std::lock_guard<std::mutex> guard(*i->locker);
    return i->frame;
}

auto DCFrameProcessor::get_data_frame() -> std::shared_ptr<DCDataFrame> {
    std::lock_guard<std::mutex> guard(*i->locker);
    return i->dFrame;
}

auto DCFrameProcessor::invalid_frame() -> void {
    std::lock_guard<std::mutex> guard(*i->locker);
    i->frame = nullptr;
}

auto DCFrameProcessor::invalid_data_frame() -> void {
    std::lock_guard<std::mutex> guard(*i->locker);
    i->dFrame = nullptr;
}

auto DCFrameProcessor::update_generation_settings(const DCFrameGenerationSettings &generationS) -> void {
    std::lock_guard<std::mutex> guard(*i->locker);
    i->generationS = generationS;
}

auto DCFrameProcessor::process() -> bool{

    std::shared_ptr<DCDataFrame> dataFrameToBeProccessed = nullptr;
    {
        std::lock_guard<std::mutex> guard(*i->locker);

        if(i->lastDF){  // check for new compressed frame
            // store last
            i->dFrame = i->lastDF;
            // invalid it
            i->lastDF = nullptr;
            // ask for uncompression
            dataFrameToBeProccessed = i->dFrame;
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
        if(dataFrameToBeProccessed != nullptr){

            auto frame = std::make_shared<DCFrame>();
            if(i->frameGenerator->generate(i->generationS, dataFrameToBeProccessed.get(), *frame)){
                std::lock_guard<std::mutex> guard(*i->locker);
                i->frame = frame;
                return true;
            }
        }
    }

    return false;
}

auto DCFrameProcessor::generate(std::shared_ptr<DCDataFrame> dFrame) -> std::shared_ptr<DCFrame>{

    std::lock_guard<std::mutex> guard(*i->locker);
    if(!dFrame){
        return nullptr;
    }

    auto frame = std::make_shared<DCFrame>();
    if(i->frameGenerator->generate(i->generationS, dFrame.get(), *frame)){
        return frame;
    }
    return nullptr;
}


auto DCFrameProcessor::process_thread() -> void {

    i->processingThreadStarted = true;

    size_t idP = 0;
    while(i->processingThreadStarted){

        auto t1 = Time::nanoseconds_since_epoch();
        bool newFrame = process();
        auto t2 = Time::nanoseconds_since_epoch();        

        // sleep        
        using namespace std::chrono_literals;
        std::this_thread::sleep_for(1ms);
        auto t3 = Time::nanoseconds_since_epoch();

        i->processB.add_value(Time::difference_ms(t1,t2).count());
        i->sleepB.add_value(Time::difference_ms(t2,t3).count());

        if(newFrame){
            i->averageProcessB.add_value(1.0*duration_cast<microseconds>(t2-t1).count());
            avegageProcessMs = i->averageProcessB.get();
        }

        ++idP;
        if(idP % 100 == 0){
            idP = 0;
            auto aP = i->processB.get();
            ucUsage = aP / (aP + i->sleepB.get());            
        }
    }
}
