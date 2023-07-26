/*******************************************************************************
** Toolbox-base                                                               **
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

// std
#include <iostream>
#include <filesystem>
#include <ostream>

// kinect4
#include <k4a/k4a.hpp>

// turbojpg
#include <turbojpeg.h>

// local
#include "utility/logger.hpp"
#include "utility/benchmark.hpp"
#include "utility/io_file.hpp"
#include "camera/kinect2/k2_device.hpp"
#include "camera/kinect4/k4_device.hpp"
#include "data/integers_encoder.hpp"
#include "graphics/texture.hpp"
#include "files/cloud_io.hpp"
#include "camera/kinect4/k4_player.hpp"
#include "camera/kinect4/k4_frame_compressor.hpp"
#include "geometry/voxel_grid.hpp"

#include "exvr/ex_experiment.hpp"

using namespace tool;
using namespace tool::camera;

void kinect2_test(){

//    using namespace std::chrono_literals;
//    camera::K2Device kinect;
//    if(!kinect.open(camera::K2FrameRequest::compressed_color_mesh)){
//        std::cerr << "Cannot init kinect 2\n";
//        return;
//    }

//    std::cout << "init\n";
//    std::this_thread::sleep_for(1000ms);
//    std::cout << "try to grab\n";

//    for(int ii = 0; ii < 10000; ++ii){
//        if(auto newFrame = kinect.get_kinect_data(); newFrame.has_value()){
//            std::cout << "-";
//        }else{
//            std::cout << "E";
//        }
//        std::this_thread::sleep_for((1000/33)*1ms);
//    }

//    std::cout << "close\n";
//    kinect.close();
}

void kinect4_test(){

//    using namespace camera;
//    using namespace camera::K4;

//    bool saveDisplayFrames = true;
//    bool saveCompressedFrames = false;


//    Logger::message("Create kinect4\n");
//    Kinect4 kinect;

//    Logger::message("Open kinect4\n");
//    if(!kinect.open(0)){
//        return;
//    }

//    k4a_calibration_t test1;
//    std::cout << "sizeof " << sizeof(test1) << "\n";
//    std::cout << "sizeof " << sizeof(k4a_calibration_t) << "\n";


//    std::cout << "Main thread id id " << std::this_thread::get_id() << "\n";

//    Config config;
////    config.mode = Mode::Only_color_1280x720; // works
////    config.mode = Mode::Only_color_1920x1080; // works
////    config.mode = Mode::Only_color_2048x1536; // works
//    config.mode = Mode::Cloud_640x576;
////    config.mode = Mode::Cloud_512x512;
//    kinect.start_cameras(config);

//    // stored frames
//    std::vector<std::shared_ptr<DisplayDataFrame>> displayFrames;
//    std::vector<std::shared_ptr<CompressedDataFrame>> compressedFrames;

//    // connections
//    kinect.new_compressed_data_frame_signal.connect([&](std::shared_ptr<CompressedDataFrame> frame){
//        std::cout << "receive compressed frame from thread id " << std::this_thread::get_id() << "\n";
////        Logger::message(std::format("receive compressed frame from thread id {}\n", std::this_thread::get_id()));
//        compressedFrames.emplace_back(frame);
//    });
//    kinect.new_display_frame_signal.connect([&](std::shared_ptr<DisplayDataFrame> frame){
//        std::cout << "receive display frame from thread id " << std::this_thread::get_id() << "\n";
////        Logger::message(std::format("receive display frame from thread id {}\n", std::this_thread::get_id()));
//        displayFrames.emplace_back(frame);
//    });

//    // parameters
//    Parameters p;
//    p.sendCompressedDataFrame   = true;
//    p.sendDisplayCloud          = true;
//    p.sendDisplayColorFrame     = true;
//    p.sendDisplayDepthFrame     = true;
//    p.sendDisplayInfraredFrame  = true;
//    p.filterDepthWithColor      = false;
//    p.jpegCompressionRate       = 80;
//    kinect.set_parameters(p);

//    Logger::message("Start reading.\n");
//    kinect.start_reading();

//    Logger::message("Sleep...\n");
//    std::this_thread::sleep_for(std::chrono::milliseconds(1000));

//    Logger::message("Stop reading.\n");
//    kinect.stop_reading();

//    Logger::message("Close.\n");
//    kinect.close();

//    if(saveDisplayFrames){

//        Logger::message("Save display frames.\n");
//        BenchGuard g("Save display frames");

//        size_t idFrame = 0;
//        for(const auto &frame : displayFrames){

//            std::string pathColor = "./display_color_" + std::to_string(idFrame) + ".png";
//            std::string pathDepth = "./display_depth_" + std::to_string(idFrame) + ".png";
//            std::string pathInfra = "./display_infra_" + std::to_string(idFrame) + ".png";
//            std::string pathCloud = "./display_cloud_" + std::to_string(idFrame) + ".obj";



//            auto &cf = frame->colorFrame;
//            std::cout << "col: " << cf.width << " "<< cf.height << " " << cf.pixels.size() << "\n";
//            if(cf.width > 0){
//                tool::graphics::Texture texColor;
//                texColor.copy_2d_data(
//                    cf.width,
//                    cf.height,
//                    cf.pixels
//                );
//                if(!texColor.write_2d_image_file_data(pathColor)){
//                    Logger::error("Failed color.\n");
//                }
//            }

//            auto &df = frame->depthFrame;
//            std::cout << "d: " << df.width << " "<< df.height << " " << df.pixels.size() << "\n";
//            if(df.width > 0){
//                tool::graphics::Texture texDepth;
//                texDepth.copy_2d_data(
//                    df.width,
//                    df.height,
//                    df.pixels
//                );
//                if(!texDepth.write_2d_image_file_data(pathDepth)){
//                    Logger::error("Failed depth.\n");
//                }
//            }

//            auto &irf = frame->infraredFrame;
//            std::cout << "r: " << irf.width << " "<< irf.height << " " << irf.pixels.size() << "\n";
//            if(irf.width > 0){
//                tool::graphics::Texture texInfra;
//                texInfra.copy_2d_data(
//                    irf.width,
//                    irf.height,
//                    irf.pixels
//                );
//                if(!texInfra.write_2d_image_file_data(pathInfra)){
//                    Logger::error("Failed infra.\n");
//                }
//            }

//            auto &cloudF = frame->cloud;
//            std::cout << "c: " << cloudF.validVerticesCount << "\n";
//            if(cloudF.validVerticesCount > 0){

//                if(!tool::files::CloudIO::save_cloud<float>(pathCloud, cloudF.vertices.data(), cloudF.colors.data(), cloudF.validVerticesCount)){
//                    Logger::error("Failed cloud.\n");
//                }
//            }

//            ++idFrame;
//        }
//    }

//    if(saveCompressedFrames){

//        Logger::message("Save uncompressed frames.\n");
//        BenchGuard g("save uncompressed frame");

//        size_t idFrame = 0;
//        tjhandle jpegUncompressor = tjInitDecompress();
//        tool::data::IntegersEncoder depthCompressor;
//        for(const auto &cFrame : compressedFrames){


//            std::string pathColor = "./uncompressed_color_" + std::to_string(idFrame) + ".png";
//            std::string pathDepth = "./uncompressed_depth_" + std::to_string(idFrame) + ".png";
//            std::string pathCloud = "./uncompressed_cloud_" + std::to_string(idFrame) + ".obj";

//            std::vector<std::uint8_t> uncompressedColorData;
//            uncompressedColorData.resize(cFrame->colorWidth * cFrame->colorHeight*4);

//            const int decompressStatus = tjDecompress2(
//                jpegUncompressor,
//                cFrame->colorBuffer.data(),
//                static_cast<unsigned long>(cFrame->colorBuffer.size()),
//                uncompressedColorData.data(),
//                cFrame->colorWidth,
//                0, // pitch
//                cFrame->colorHeight,
//                TJPF_RGBA,
//                TJFLAG_FASTDCT// | TJFLAG_FASTUPSAMPLE
//            );
//            if(decompressStatus == -1){
//                Logger::error("Error uncompress color.\n");
//                break;
//            }

//            tool::graphics::Texture texColor;
//            texColor.copy_2d_data(
//                cFrame->colorWidth,
//                cFrame->colorHeight,
//                4,
//                uncompressedColorData.data()
//            );
//            texColor.write_2d_image_file_data(pathColor);

//            // depth
//            std::vector<std::uint16_t> uncompressedDepthData;
//            uncompressedDepthData.resize(cFrame->depthWidth*cFrame->depthHeight);

//            size_t originalSize;

//            try{
//                originalSize= depthCompressor.decode(
//                    cFrame->depthBuffer.data(),
//                    cFrame->depthBuffer.size(),
//                    reinterpret_cast<std::uint32_t*>(uncompressedDepthData.data()),
//                    (cFrame->depthWidth*cFrame->depthHeight)/2
//                );
//            }catch(std::exception e){
//                Logger::error(std::format("Error uncompress depth {}.\n", e.what()));
//            }


//            float min=0.f,max =0.f,diff = 0.f;
//            const std::vector<geo::Pt3f> depthGradient ={
//                {0.f,0.f,1.f},
//                {0.f,1.f,1.f},
//                {0.f,1.f,0.f},
//                {1.f,1.f,0.f},
//                {1.f,0.f,0.f},
//            };

//            // find min/max
//            const auto [pmin, pmax] = std::minmax_element(uncompressedDepthData.begin(), uncompressedDepthData.end());
//            min = static_cast<float>(*pmin);
//            max = static_cast<float>(*pmax);
//            diff = max-min;

//            std::vector<std::uint8_t> uncompressedDepthImageData;
//            uncompressedDepthImageData.resize(cFrame->depthWidth * cFrame->depthHeight*4);

//            for(size_t ii = 0; ii < uncompressedDepthData.size(); ++ii){

//                float vF = (static_cast<float>(uncompressedDepthData[ii]) - min)/diff;
//                float intPart;
//                float decPart = std::modf((vF*(depthGradient.size()-1)), &intPart);
//                size_t idG = static_cast<size_t>(intPart);

//                auto col = depthGradient[idG]*(1.f-decPart) + depthGradient[idG+1]*decPart;
//                uncompressedDepthImageData[ii*4+0] = static_cast<std::uint8_t>(255*col.x());
//                uncompressedDepthImageData[ii*4+1] = static_cast<std::uint8_t>(255*col.y());
//                uncompressedDepthImageData[ii*4+2] = static_cast<std::uint8_t>(255*col.z());
//                uncompressedDepthImageData[ii*4+3] = 255;
//            }

//            texColor.copy_2d_data(
//                cFrame->depthWidth,
//                cFrame->depthWidth,
//                4,
//                uncompressedDepthImageData.data()
//            );
//            texColor.write_2d_image_file_data(pathDepth);


//            Logger::message("uncompress depth\n");
//            k4a::image depthImage = k4a::image::create(
//                k4a_image_format_t::K4A_IMAGE_FORMAT_DEPTH16,
//                cFrame->depthWidth, cFrame->depthWidth,
//                static_cast<int32_t>(cFrame->depthWidth * 1 * sizeof(uint16_t)));

//            try{
//                originalSize= depthCompressor.decode(
//                    cFrame->depthBuffer.data(),
//                    cFrame->depthBuffer.size(),
//                    reinterpret_cast<std::uint32_t*>(depthImage.get_buffer()),
//                    (cFrame->depthWidth*cFrame->depthHeight)/2
//                );
//            }catch(std::exception e){
//                Logger::error(std::format("Error uncompress depth {}.\n", e.what()));
//            }

//            k4a_transformation_t tr = k4a_transformation_create(&cFrame->calibration);
//            k4a::image pointCloudImage = k4a::image::create(K4A_IMAGE_FORMAT_CUSTOM,
//                cFrame->depthWidth,
//                cFrame->depthHeight,
//                static_cast<int32_t>(cFrame->depthWidth * 3 * sizeof(int16_t))
//            );

//            k4a_result_t result = k4a_transformation_depth_image_to_point_cloud(
//                tr,
//                depthImage.handle(),
//                K4A_CALIBRATION_TYPE_DEPTH,
//                pointCloudImage.handle()
//            );

//            if(result == K4A_RESULT_SUCCEEDED){

//                ColoredCloudFrame cloud;

//                auto cloudBuffer = reinterpret_cast<geo::Pt3<int16_t>*>(pointCloudImage.get_buffer());

//                const size_t width  = pointCloudImage.get_width_pixels();
//                const size_t height = pointCloudImage.get_height_pixels();
//                const size_t size = width * height;
//                cloud.vertices.resize(size);
//                cloud.colors.resize(size);
//                cloud.validVerticesCount = size;

//                for(size_t ii = 0; ii < size; ++ii){
//                    cloud.vertices[ii] = geo::Pt3f{
//                        static_cast<float>(-cloudBuffer[ii].x()),
//                        static_cast<float>(-cloudBuffer[ii].y()),
//                        static_cast<float>(-cloudBuffer[ii].z())
//                    }*0.01f;

//                    cloud.colors[ii] = geo::Pt3f{
//                        static_cast<float>(uncompressedColorData[ii*4+0]),
//                        static_cast<float>(uncompressedColorData[ii*4+1]),
//                        static_cast<float>(uncompressedColorData[ii*4+2])
//                    }/255.f;
//                }

//                if(!tool::files::CloudIO::save_cloud(pathCloud, cloud.vertices.data(), cloud.colors.data(), cloud.validVerticesCount)){
//                    Logger::error("Faild uncompressed cloud.\n");
//                }
//            }

//            idFrame++;
//        }
//        tjDestroy(jpegUncompressor);
//    }

}

void bench_test(){

//    {
//        const auto t1 = "t1"sv;
//        tool::Bench::start(t1, true);
//        std::this_thread::sleep_for(std::chrono::milliseconds(100));
//        tool::Bench::stop();

//        {
//            tool::BenchGuard g("g1", true);
//            std::this_thread::sleep_for(std::chrono::milliseconds(100));
//        }

//        tool::Bench::start("t2", true);
//        std::this_thread::sleep_for(std::chrono::milliseconds(100));
//        tool::Bench::stop();

//        tool::BenchGuard g0("g0", true);
//        {
//            tool::BenchGuard g1("g1", true);
//            std::this_thread::sleep_for(std::chrono::milliseconds(100));

//            {
//                tool::BenchGuard g2("g2", true);
//                std::this_thread::sleep_for(std::chrono::milliseconds(100));

//            }

//            tool::BenchGuard g3("g3", true);
//            std::this_thread::sleep_for(std::chrono::milliseconds(100));
//        }

//        tool::Bench::start("t3"sv, true);
//        std::this_thread::sleep_for(std::chrono::milliseconds(100));
//        tool::Bench::stop();

//        tool::Bench::start("t4", true);
//        std::this_thread::sleep_for(std::chrono::milliseconds(100));
//        tool::Bench::stop();
//    }

//    tool::BenchGuard g3("g4", true);

//    tool::Bench::start("base-lib start", true);
//    Logger::init((std::filesystem::current_path() / "base-lib-logs").string(), "logs.html");

//    auto logger = Logger::get();
//    logger->message_signal.connect([&](std::string messsage){
//        std::cout << "Message from logger: " << messsage << "\n";
//    });

//    tool::Bench::stop();
}



int main(){

    K4Player player;
    player.load_from_file("D:/a6.kvid");

    std::cout << "player " << player.nb_cameras() << " " << player.duration_ms() <<  " " << player.nb_frames(0) << "\n";


    std::cout << "set time\n";
//    player.set_current_time(1000.0);
//    player.update();
//    std::cout << "remove " << player.current_frame_id(0) << "\n";
//    player.remove_after_current_frame();

    std::cout << "uncompress\n";    
    std::cout << "time1 : " << player.current_time_ms() << " " << player.duration_ms() <<  " " << player.nb_frames(0) << "\n";
    player.set_current_time(1000);
    player.update();
    player.remove_until_current_frame();

    std::cout << "time2 : " << player.current_time_ms() << " " << player.duration_ms() <<  " " << player.nb_frames(0) << "\n";
    player.set_current_time(500);
    player.update();
//    player.remove_after_current_frame();

//    std::cout << "time3 : " << player.current_time_ms() << " " << player.duration_ms() <<  " " << player.nb_frames(0) << "\n";
//    player.set_current_time(0);
//    player.update();

//    std::cout << "time4 : " << player.current_time_ms() << " " << player.duration_ms() <<  " " << player.nb_frames(0) << "\n";
//    player.merge_cameras();


//    K4Frame ff;
//    player.uncompress_frame(0, 10, ff);
//    files::CloudIO::save_cloud("D:/after.obj", ff.cloud);

//    std::cout << "time5 : " << player.current_time_ms() << " " << player.duration_ms() <<  " " << player.nb_frames(0) << "\n";

//    std::cout << "save\n";
//    player.save_to_file("D:/after.kvid");

//    std::cout << "end\n";

//    geo::VoxelGrid grid(0.01f, {-10.f,-10.f,-10.f}, {10.f,10.f,10.f});
//    K4Frame frame;
//    std::cout << "u0 " << player.video()->get_transform(0).conv<float>() << "\n";
//    std::cout << "u1 " << player.video()->get_transform(1).conv<float>() << "\n";
//    std::cout << "u2 " << player.video()->get_transform(2).conv<float>() << "\n";
//    std::cout << "u3 " << player.video()->get_transform(3).conv<float>() << "\n";

//    for(size_t ii = 0; ii < player.min_nb_frames(); ++ii){

//        player.uncompress_frame(0, ii, frame);
//        grid.add_cloud(frame.cloud, player.video()->get_transform(0).conv<float>());

//        player.uncompress_frame(1, ii, frame);
//        grid.add_cloud(frame.cloud, player.video()->get_transform(1).conv<float>());

//        player.uncompress_frame(2, ii, frame);
//        grid.add_cloud(frame.cloud, player.video()->get_transform(2).conv<float>());

//        player.uncompress_frame(3, ii, frame);
//        grid.add_cloud(frame.cloud, player.video()->get_transform(3).conv<float>());

////    }
///
///
///     std::cout << "test\n";
    K4FrameCompressor fComp;
//    fComp.test();
///
    K4Frame frame;

//    frame.cloud.resize(30);
//    for(size_t ii = 0; ii < frame.cloud.size(); ++ii){
//        frame.cloud.resize[ii]
//    }
//    std::cout << "uncompress1\n";
    player.uncompress_frame(0, 0, frame);
//    frame.mode = K4Mode::Merged;
    std::cout << "uncompresses frame has " << frame.cloud.size() << " vertices.\n";
    files::CloudIO::save_cloud("D:/f2.obj", frame.cloud);

    auto cf = fComp.compress(frame, 90);
    std::cout << "compressed frame has " << cf->validVerticesCount << " vertices and " << cf->encodedCloudVerticesData.size() << " " << cf->encodedCloudColorData.size() << "\n";

    frame = K4Frame();
    K4FrameUncompressor fUcomp;
    fUcomp.uncompress(cf.get(), frame);
    std::cout << "uncompresses frame has " << frame.cloud.size() << " vertices.\n";

    files::CloudIO::save_cloud("D:/f3.obj", frame.cloud);


    auto mergedCloud = player.video()->merge_cameras_clouds(10, 0.02f, {-20.f,-20.f,-20.f}, {20.f,20.f,20.f});
    std::cout << "mergedCloud " << mergedCloud.size() << " " << mergedCloud.vertices.size() << " " << mergedCloud.colors.size() << "\n";
    files::CloudIO::save_cloud("D:/after.obj", mergedCloud);

    K4Frame mergedFrame;
    player.video()->merge_cameras_clouds(10, 0.02f, {-20.f,-20.f,-20.f}, {20.f,20.f,20.f}, mergedFrame);

    std::cout << "mergedFrame " << mergedFrame.cloud.size() << " " << mergedFrame.idCapture  << "\n";
    files::CloudIO::save_cloud("D:/after2.obj", mergedFrame.cloud);

    camera::K4FrameCompressor compressor;
    camera::K4FrameUncompressor uncompressor;
    if(auto cFrame = compressor.compress(mergedFrame, 90)){

        K4Frame uFrame;
        uncompressor.uncompress(cFrame.get(), uFrame);

        files::CloudIO::save_cloud("D:/after3.obj", uFrame.cloud);
    }

    std::cout << "boo " << player.nb_frames(0) << "\n";
    player.merge_cameras();

    std::cout << "merge\n";
    player.save_to_file("D:/after3.kvid");

    std::cout << "end save\n";


//    std::cout << "merge " << player.video()->nb_cameras() << " " << player.video()->get_camera_data(0)->frames.size() << "\n";
//    player.merge_cameras();

//    files::CloudIO::save_cloud("D:/before.obj", frame.cloud);
//    player.uncompress_frame(0, 0, frame);

//    std::cout << "color " << frame.colorWidth << " " << frame.colorHeight << " " << frame.imageColorData.size() << "\n";
//    std::cout << "depth " << frame.depthWidth << " " << frame.depthHeight << " " << frame.depthData.size() << " " << frame.imageDepthData.size() << "\n";
//    std::cout << "infra " << frame.infraWidth << " " << frame.infraHeight << " " << frame.infraData.size() << " " << frame.imageInfraData.size() << "\n";
//    std::cout << "cloud " << frame.cloud.size() << " " << frame.cloud.vertices.size() <<  " " << frame.cloud.colors.size() << "\n";

//    files::CloudIO::save_cloud("D:/after.obj", frame.cloud);

//    Logger::message("base-lib start\n");

//    tool::Bench::reset();
//    tool::Bench::start("kinect4_test");
//    Logger::message("kinect4_test\n");
//    kinect4_test();
//    tool::Bench::stop();
//    tool::Bench::display();

////    tool::Bench::reset();
////    tool::Bench::start("bench_test");
////    Logger::message("bench_test\n");
////    bench_test();
////    tool::Bench::stop();
////    tool::Bench::display();

//    Logger::message("base-lib end\n");

    return 0;
}



