


#include "tests.hpp"

// opencv
#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>

// k4a
#include "depth-camera/impl/azure_utility.hpp"

// base
#include "depth-camera/dc_types.hpp"
#include "depth-camera/dc_device.hpp"

#include "utility/logger.hpp"

#include "io/image_io.hpp"
#include "io/cloud_io.hpp"

using namespace tool;



auto create_device(int deviceId ) -> void{

    std::unique_ptr<k4a::device> device = nullptr;

    // create device
    Log::fmessage("open device {}\n", deviceId);
    device = std::make_unique<k4a::device>(k4a::device::open(static_cast<uint32_t>(deviceId)));

    if(!device->is_valid()){
        Log::error("Invalid device.\n");
        return;
    }

    Log::fmessage("create config {}\n", deviceId);
    // set config
    k4a_device_configuration_t k4aConfig        = K4A_DEVICE_CONFIG_INIT_DISABLE_ALL;
    k4aConfig.color_format                      = K4A_IMAGE_FORMAT_COLOR_NV12;
    k4aConfig.color_resolution                  = K4A_COLOR_RESOLUTION_720P;
    k4aConfig.depth_mode                        = K4A_DEPTH_MODE_NFOV_UNBINNED;
    k4aConfig.camera_fps                        = K4A_FRAMES_PER_SECOND_30;
    k4aConfig.synchronized_images_only          = true;
    k4aConfig.depth_delay_off_color_usec        = 0;
    k4aConfig.disable_streaming_indicator       = false;
    k4aConfig.wired_sync_mode                   = K4A_WIRED_SYNC_MODE_STANDALONE;
    k4aConfig.subordinate_delay_off_master_usec = 0;

    // start camera
    Log::fmessage("start camera {}\n", deviceId);
    device->start_cameras(&k4aConfig);

    // retrieve calibration
    auto calibration        = device->get_calibration(k4aConfig.depth_mode, k4aConfig.color_resolution);

    // retrieve transformation
    auto transformation     = k4a::transformation(calibration);

    // create capture
    k4a::capture capture;
    k4a::image depthSizedColorImage;
    k4a::image colorSizedDepthImage;
    k4a::image pointCloudImage;

    ImageBuffer<ColorRGBA8> convertedColorData;

    Log::message("Start reading frames.\n");
    for(int ii = 0; ii < 500; ++ii){

        bool success = false;
        std::int32_t timeoutMs = 500;
        try{
            success = device->get_capture(&capture, std::chrono::milliseconds(timeoutMs));
        }catch(const std::runtime_error &e){
            Log::error(std::format("Get capture runtime error: {}\n", e.what()));
        }

        if(!success){
            continue;
        }

        k4a::image color;
        if(k4aConfig.color_resolution != K4A_COLOR_RESOLUTION_OFF){
            color = capture.get_color_image();
        }
        k4a::image depth;
        if(k4aConfig.depth_mode != K4A_DEPTH_MODE_OFF){
            depth = capture.get_depth_image();
        }
        k4a::image infra;
        if(k4aConfig.depth_mode != K4A_DEPTH_MODE_OFF){
            infra = capture.get_ir_image();
        }

        Log::fmessage("temp {:6} color {:6} depth {:6} infra {:6}\n",
                      capture.get_temperature_c(),
                      color.is_valid() ? color.get_width_pixels() : 0,
                      depth.is_valid() ? depth.get_width_pixels() : 0,
                      infra.is_valid() ? infra.get_width_pixels() : 0
                      );


        if(color.is_valid()){

            // convert color
            cv::Mat yuvImage(static_cast<int>(color.get_height_pixels() * 3 / 2), static_cast<int>(color.get_width_pixels()), CV_8UC1);
            std::copy(color.get_buffer(), color.get_buffer() + color.get_size(),  yuvImage.data); // copy Y plane

            convertedColorData.resize_image(color.get_width_pixels(), color.get_height_pixels());
            cv::Mat convMat(static_cast<int>(color.get_height_pixels()), static_cast<int>(color.get_width_pixels()), CV_8UC4, convertedColorData.get_byte_data());
            cv::cvtColor(yuvImage, convMat, cv::COLOR_YUV2RGBA_NV12);
            Log::fmessage("converted image {} {} | {} {}\n", color.get_size(), convertedColorData.bytes_size(), convertedColorData.width, convertedColorData.height);

            cv::imwrite(std::format("D:/temps files/color__{}.png", ii), convMat);
            // io::ImageIO::save_image(std::format("D:/temps files/color_{}.png", ii), convertedColorData);

            // resize color
            auto colorStride     = color.get_width_pixels() * 4 * sizeof(std::uint8_t);
            auto colorSizeBytes  = colorStride * color.get_height_pixels();
            auto colorDataBuffer = reinterpret_cast<std::uint8_t*>(convertedColorData.get_data());

            if(depth.is_valid()){

                auto k4aColorImage = k4a::image::create_from_buffer(
                    K4A_IMAGE_FORMAT_COLOR_BGRA32,
                    static_cast<int>(color.get_width_pixels()),
                    static_cast<int>(color.get_height_pixels()),
                    static_cast<int>(colorStride),
                    colorDataBuffer,
                    colorSizeBytes,
                    nullptr, nullptr
                    );

                if(!depthSizedColorImage.is_valid()){
                    depthSizedColorImage = k4a::image::create(
                        K4A_IMAGE_FORMAT_COLOR_BGRA32,
                        static_cast<int>(depth.get_width_pixels()),
                        static_cast<int>(depth.get_height_pixels()),
                        static_cast<int>(depth.get_width_pixels() * 4)
                        );
                }

                if(!colorSizedDepthImage){
                    colorSizedDepthImage = k4a::image::create(
                        K4A_IMAGE_FORMAT_DEPTH16,
                        static_cast<int>(color.get_width_pixels()),
                        static_cast<int>(color.get_height_pixels()),
                        static_cast<int>(color.get_width_pixels() * 2)
                        );
                }

                try{
                    transformation.color_image_to_depth_camera(
                        depth,
                        k4aColorImage,
                        &depthSizedColorImage
                        );

                    transformation.depth_image_to_color_camera(
                        depth,
                        &colorSizedDepthImage
                        );

                    Log::fmessage("colorSizedDepthImage {} {}\n", colorSizedDepthImage.get_width_pixels(), colorSizedDepthImage.get_height_pixels());
                    cv::Mat dMat(static_cast<int>(colorSizedDepthImage.get_height_pixels()), static_cast<int>(colorSizedDepthImage.get_width_pixels()), CV_16UC1, colorSizedDepthImage.get_buffer());
                    cv::imwrite(std::format("D:/temps files/rdepth__{}.png", ii), dMat);


                }catch(const std::runtime_error &error){
                    // Log::error(std::format("resize_color_image_to_depth_size Error: {}\n", error.what()));
                    Log::error(std::format("depth_image_to_color_camera Error: {}\n", error.what()));
                    continue;
                }
            }
        }

        // generate cloud
        if(depth.is_valid()){

            if(!pointCloudImage.is_valid()){
                // pointCloudImage = k4a::image::create(
                // K4A_IMAGE_FORMAT_CUSTOM,
                // static_cast<int>(depth.get_width_pixels()),
                // static_cast<int>(depth.get_height_pixels()),
                // static_cast<int>(depth.get_width_pixels() * 3 * sizeof(std::int16_t))
                // );
                pointCloudImage = k4a::image::create(
                    K4A_IMAGE_FORMAT_CUSTOM,
                    static_cast<int>(colorSizedDepthImage.get_width_pixels()),
                    static_cast<int>(colorSizedDepthImage.get_height_pixels()),
                    static_cast<int>(colorSizedDepthImage.get_width_pixels() * 3 * sizeof(std::int16_t))
                    );
            }

            try{

                // Log::fmessage("colorSizedDepthImage {} {}\n", colorSizedDepthImage.get_width_pixels(), colorSizedDepthImage.get_height_pixels());
                // Log::fmessage("pointCloudImage {} {}\n", pointCloudImage.get_width_pixels(), pointCloudImage.get_height_pixels());
                // transformation.depth_image_to_point_cloud(
                //     depth,
                //     K4A_CALIBRATION_TYPE_DEPTH,
                //     &pointCloudImage
                // );

                transformation.depth_image_to_point_cloud(
                    colorSizedDepthImage,
                    K4A_CALIBRATION_TYPE_COLOR,
                    &pointCloudImage
                    );

                auto buffer = reinterpret_cast<geo::Pt3<std::int16_t>*>(pointCloudImage.get_buffer());
                // Log::fmessage("buffer {} {} {}\n", buffer[100].x(), buffer[100].y(), buffer[100].z());
                // auto cBuffer = reinterpret_cast<ColorRGBA8*>(depthSizedColorImage.get_buffer());
                auto cBuffer = reinterpret_cast<ColorRGBA8*>(convertedColorData.get_data());
                geo::ColorCloud cc;
                for(size_t jj = 0; jj < pointCloudImage.get_width_pixels()*pointCloudImage.get_height_pixels(); ++jj){
                    if(buffer[jj].z() > 0){
                        geo::Pt3f currentP = buffer[jj].template conv<float>();
                        geo::Pt3f col = (cBuffer[jj].template conv<float>()).xyz();
                        cc.vertices.push_back({currentP * 0.001f, });
                        cc.colors.push_back(col);
                    }
                }


                Log::fmessage("cloud size {}\n",cc.size());
                io::CloudIO::save_cloud(std::format("D:/temps files/cloud_{}.obj", ii), cc);




            }catch(const std::runtime_error &error){
                Log::error(std::format("generate_cloud Error: {}\n", error.what()));
                continue;
            }
        }




        // std::copy(fData.rawColor.data() + mInfos.color_size(), fData.rawColor.data() + mInfos.color_size() + mInfos.color_size()/2, reinterpret_cast<std::byte*>(yuvImage.data) + mInfos.color_size());

        // cv::Mat convMat(static_cast<int>(mInfos.color_height()), static_cast<int>(mInfos.color_width()), CV_8UC4, fData.convertedColorData.get_byte_data());
        // cv::cvtColor(yuvImage, convMat, cv::COLOR_YUV2RGBA_NV12);
    }
}

auto multi_devices_benchmark() -> void{


    int nbDevices = 8;
    std::vector<std::unique_ptr<std::thread>> threads;
    threads.resize(nbDevices);

    for(int idD = 0; idD < nbDevices; ++idD){
        Log::fmessage("start thread {}\n", idD);
        threads[idD] =
            std::make_unique<std::thread>([=](){
                Log::fmessage("create device {}\n", idD);
                create_device(idD);
            }
        );
    }

    for(int idD = 0; idD < nbDevices; ++idD){
        threads[idD]->join();
    }
}
