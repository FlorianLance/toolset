
#pragma once

// std
#include <iostream>

// base
#include "depth-camera/dc_video_player.hpp"

auto test_dc_player() -> void{

    using namespace tool;
    using namespace tool::cam;
    using namespace std::chrono;
    
    DCVideoPlayer player;
    player.load_from_file("D:/kvidtest.kvid");


    auto duration = player.video()->duration_ms();
    
    
    auto ffTs = duration_cast<milliseconds>(nanoseconds(player.video()->first_frame_timestamp()));//.value()));
    for(size_t idC = 0; idC < player.video()->nb_devices(); ++idC){
        std::cout << player.video()->get_compressed_frames_ptr(idC)->frames.size() << "\n";
        //        auto ff = duration_cast<milliseconds>(nanoseconds(player.video()->get_camera_data(idC)->first_frame_capture_timestamp().value()));
        //        auto sec = duration_cast<milliseconds>(nanoseconds(player.video()->get_camera_data(idC)->frames[1]->afterCaptureTS));
        //        std::cout << "IDC " << idC << " " << ffTs << " " << ff << " " << sec << "\n";
    }

    for(size_t ii = 0; ii < 10; ++ii){
        auto time = (ii / 10.0)* duration;
        
        for(size_t idC = 0; idC < player.video()->nb_devices(); ++idC){
            auto id = player.video()->closest_frame_id_from_time(idC, time);
            std::cout << "time " << time << " idC " << idC << " " << id/**.value()*/ << "\n";
        }
    }

    player.load_from_file("D:/_debug/adriel2.kvid");

    player.set_current_time(1000);
    player.update();
    player.remove_until_current_frame();
    player.update();

    player.merge_cameras(0.005f, {-20.f,-20.f,-20.f}, {+20.f,+20.f,+20.f});
    player.save_to_file("D:/--.kvid");


    duration = player.video()->duration_ms();

    for(size_t ii = 0; ii < 1000; ++ii){
        auto time = (ii / 1000.0)* duration;
        auto id = player.video()->closest_frame_id_from_time(0, time);
        if(id != -1){//.has_value()){
            if(auto frame = player.video()->get_data_frame(0, id/**.value()*/).lock(); frame != nullptr){
                std::cout << "time " << time << " id " << id/**.value()*/ << " " << frame->cloud_size() <<  "\n";
            }

        }else{
            std::cout << "ERROR " << id/**.error()*/ << "\n";
        }
    }
}



