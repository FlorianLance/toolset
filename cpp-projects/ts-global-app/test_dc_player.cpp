
/*******************************************************************************
** Toolset-ts-global-app                                                      **
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

#include "tests.hpp"


// std
#include <iostream>

// base
#include "depth-camera/dc_video_player.hpp"
#include "utility/time.hpp"
#include "utility/logger.hpp"

using namespace tool;
using namespace tool::geo;
using namespace tool::cam;
using namespace std::chrono;

auto dc_player_test() -> void{

    // DCVideoPlayer p;
    // DCVideo v1;
    // Log::message("load\n");
    // v1.load_from_file("E:/_kvid/aaaa.kvid");
    // p.set_video(v1);

    // DCVideoPlayerSettings ps;
    // ps.doLoop = false;
    // p.update_settings(ps);
    // p.start_video();

    // int idF = -5;
    // while(p.is_playing()){

    //     p.update();

    //     std::cout << p.current_frame_id(0) << " " << p.current_compressed_frame_cloud_size(0) << "\n";

    //     if(auto frame = p.current_frame(0); frame != nullptr){

    //         // std::cout << " --- > " << frame->volumesB.size() << " " << frame->imagesB.contains(DCImageBufferType::DepthSizedColorRGBA8) << " " <<  frame->image_buffer_size(DCImageBufferType::DepthSizedColorRGBA8) << " " <<  frame->volumesB.contains(DCVolumeBufferType::ColoredCloud) <<  "\n";
    //         if(frame->volumesB.contains(DCVolumeBufferType::ColoredCloud)){
    //             auto buffer = frame->volume_buffer<ColorCloud>(DCVolumeBufferType::ColoredCloud);
    //             // std::cout << "[ -- " << buffer->vertices.size() <<  " " << frame->volume_buffer_size(DCVolumeBufferType::ColoredCloud) << "\n";
    //         }
    //         if(frame->imagesB.contains(DCImageBufferType::DepthSizedColorRGBA8)){
    //             auto buffer = frame->image_buffer<ColorRGBA8>(DCImageBufferType::DepthSizedColorRGBA8);
    //             std::cout << "#### " << buffer->width << " " << buffer->height << " " << buffer->size() << "\n";// << frame->image_buffer_size(DCImageBufferType::DepthSizedColorRGBA8) << "\n";

    //             // const auto &imageB = frame->imagesB[DCImageBufferType::DepthSizedColorRGBA8];
    //             // std::visit(Visitor{
    //             //        [](const ImageBuffer<ColorRGBA8> &sImg){
    //             //         std::cout << "A: " << sImg.size() << "\n";
    //             //            return sImg.size();
    //             //        },
    //             //        [](const ImageBuffer<ColorRGB8> &sImg){
    //             //            return sImg.size();
    //             //        },
    //             //        [](const ImageBuffer<ColorGray8> &sImg){
    //             //            return sImg.size();
    //             //        },
    //             //        [](const ImageBuffer<ColorGray16> &sImg){
    //             //            return sImg.size();
    //             //        }
    //             //    }, imageB
    //             //    );
    //         }
    //         // if(frame->volumesB.size() > 0){
    //             // std::cout << " --- > " << frame->volume_buffer_size(DCVolumeBufferType::ColoredCloud) << "\n";
    //         // }

    //     }

    //     if(p.current_time_ms() > p.end_time_ms()){
    //         break;
    //     }


    //     // if(auto cloud = frame->volume_buffer<geo::ColorCloud>(DCVolumeBufferType::ColoredCloud)){
    // }
}


auto test_dc_player_merge(std::string kvidPath) -> void{

    {
        DCVideo video;

        DCFrameGenerationSettings gs;
        gs.cloud = true;
        gs.depthSizedColorImage = false;


        std::puts("Load from file\n");
        video.load_from_file(kvidPath);


        {
            std::puts("Generate frames\n");
            DCFrame frame;
            for(size_t idF = 0; idF < video.nb_frames(0); ++idF){
                auto t1 = Time::nanoseconds_since_epoch();
                video.generate_frame(gs, 0, idF, frame);
                auto t2 = Time::nanoseconds_since_epoch();
                std::puts(std::format("duration gen: [{}]ms\n", Time::difference_micro_s(t1,t2).count()).c_str());
                frame.infos();
            }
        }


        // std::puts("Merge\n");
        // auto t1 = Time::nanoseconds_since_epoch();
        // video.merge_all_devices(gs, 0.0025f, {-2.f,-2.f,-2.f}, {+4.f,+4.f,+4.f});
        // auto t2 = Time::nanoseconds_since_epoch();
        // Log::message(std::format("Duration merge: [{}]ms [{}] cameras\n", Time::difference_ms(t1,t2).count(), video.nb_devices()));
        // video.save_to_file("E:/_kvid/merged.kvid");

        std::puts("Load merged from file\n");
        video.load_from_file("E:/_kvid/merged.kvid");

        {
            std::puts("Generate frames\n");
            DCFrame frame;
            for(size_t idF = 0; idF < video.nb_frames(0); ++idF){
                auto t1 = Time::nanoseconds_since_epoch();
                video.generate_frame(gs, 0, idF, frame);
                auto t2 = Time::nanoseconds_since_epoch();
                std::puts(std::format("duration gen: [{}]ms\n", Time::difference_micro_s(t1,t2).count()).c_str());
                frame.infos();
            }

            std::puts("Save to cloud\n");
            auto c = frame.volume_buffer<ColorCloud>(DCVolumeBufferType::ColoredCloud);
            CloudIO::save_cloud("E:/_clouds/merged1.obj", *c);
        }


        // video.get_data_frame(0, 50).lock()->volume_buffer(DCVolumeBufferType::ColoredCloud);
    }
    {
        // DCVideoPlayer playerVox;
        // playerVox.load_from_file("E:/_kvid/merged.kvid");

        // playerVox.

        // DCFrameGenerator frameGen;

    }
}



