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

// local
#include "test_ak_device.hpp"
#include "test_ob_device.hpp"
#include "depth-camera/dc_video_player.hpp"
#include "depth-camera/dc_video_recorder.hpp"
#include "utility/logger.hpp"
#include "utility/time.hpp"
#include "data/jpeg_encoding.hpp"

#include "utility/io_fstream.hpp"
#include "geometry/color_voxel_grid.hpp"

// #include "TurboPFor/vp4.h"

#include "thirdparty/stb/stb_image_write.h"
#include "io/cloud_io.hpp"

#include "depth-camera/frame/dc_data_frame_generator.hpp"

using namespace tool;
using namespace tool::geo;
using namespace tool::cam;

auto convert_kvid(const std::string &path, const std::string &dest) -> void{

    if(path.empty()){
        return;
    }
    
    Log::message(std::format("Convert_kvid: {}\n", path));


    // open file
    std::ifstream file(path.data(), std::ios_base::binary);
    if(!file.is_open()){
        Log::error(std::format("Cannot open binary file with path: [{}].\n", path));
        return;
    }
    file.exceptions ( std::ifstream::eofbit | std::ifstream::failbit | std::ifstream::badbit);

    data::JpegDecoder jpgDecoder;
    DCDataFrameGenerator frameCompressor;
    // DCVideoRecorder recorder;
    DCVideo video;


    // read mode
    std::int8_t videoType;
    read(videoType, file);
    std::cout << "videoType: " << (int)videoType << "\n";

    if(videoType == 1){

        // read nb of cameras
        std::int8_t nbCameras;
        read(nbCameras, file);
        std::cout << "nbCameras: " << (int)nbCameras << "\n";

        video.initialize(nbCameras);

        std::vector<geo::Mat4d> calibrations;
        
        std::vector<std::vector<std::shared_ptr<DCDeprecatedFrame>>> frames;
        frames.resize(nbCameras);

        for(size_t id = 0; id < static_cast<size_t>(nbCameras); ++id){

            std::int32_t nbFrames;
            read(nbFrames, file);
            std::cout << "nbFrames: " << (int)nbFrames << "\n";
            frames[id].resize(nbFrames);

            for(auto &frame : frames[id]){
                frame = std::make_shared<DCDeprecatedFrame>();
            }

            geo::Mat4d mat;
            read_array(mat.array.data(), file, 16);
            std::cout << "mat:\n " << mat << "\n";
            calibrations.push_back(mat);

            video.set_transform(id, mat);
        }

        for(size_t idC = 0; idC < frames.size(); ++idC){
            for(size_t idF = 0; idF < frames[idC].size(); ++idF){

                std::cout << (1.f*idF/frames[idC].size()) << " ";
                auto frame = frames[idC][idF].get();
                frame->mode = DCMode::AK_C1280x720_DI640x576_NV12_F30;

                read(frame->idCapture, file);
                // std::cout << "idCapture: " << (int)frame->idCapture << "\n";

                read(frame->afterCaptureTS, file);
                // std::cout << "afterCaptureTS: " << (int)frame->afterCaptureTS << "\n";
                frame->receivedTS = frame->afterCaptureTS;

                std::int32_t validVerticesC;
                read(validVerticesC, file);
                frame->cloud.resize(validVerticesC);
                // std::cout << "validVerticesCount: " << (int)validVerticesC << "\n";

                std::int32_t cloudBufferSize;
                read(cloudBufferSize, file);

                // std::cout << "cloudBufferSize: " << (int)cloudBufferSize << "\n";
                std::vector<std::uint8_t> encodedCloudVerticesData;
                if(cloudBufferSize > 0){
                    encodedCloudVerticesData.resize(cloudBufferSize);
                    read_array(encodedCloudVerticesData.data(), file, encodedCloudVerticesData.size());
                }

                std::int16_t colorW, colorH;
                std::int32_t colorBufferSize;
                read(colorW, file);
                read(colorH, file);
                read(colorBufferSize, file);

                // std::cout << "colorW: " << (int)colorW << "\n";
                // std::cout << "colorH: " << (int)colorH << "\n";
                // std::cout << "colorBufferSize: " << (int)colorBufferSize << "\n";
                std::vector<std::uint8_t> encodedCloudColorData;

                ImageBuffer<ColorRGB8> image;
                if(colorBufferSize > 0){
                    encodedCloudColorData.resize(colorBufferSize);
                    read_array(encodedCloudColorData.data(), file, encodedCloudColorData.size());
                    auto bs = BinarySpan(reinterpret_cast<std::byte*>(encodedCloudColorData.data()) ,encodedCloudColorData.size());
                    jpgDecoder.decode(colorW, colorH, bs, image);

                    // stbi_write_jpg("D:/video_image.jpg", colorW, colorH, 3, image.get_data(), 90);
                    // image.
                }

                // # read audio
                std::int32_t audioBufferSize;
                read(audioBufferSize, file);
                if(audioBufferSize > 0){
                    frame->audioFrames.resize(audioBufferSize);
                    read_array(reinterpret_cast<float*>(frame->audioFrames.data()), file, audioBufferSize*7);
                }

                // # read imu
                DCImuSample imusSample;
                read_array(file, reinterpret_cast<char*>(&imusSample), sizeof (DCImuSample));

                if(encodedCloudVerticesData.size() > 0){
                    std::vector<std::uint16_t> decodedVertices;

                    // decode cloud
                    size_t uncompressedSize = frame->cloud.size()*3;
                    size_t rest = uncompressedSize % 128;
                    size_t paddedUncompressedSize = rest == 0 ? uncompressedSize : uncompressedSize + 128-rest;

                    // std::cout << "uncompressedSize: " << uncompressedSize << "\n";
                    // std::cout << "rest: " << rest << "\n";
                    // std::cout << "paddedUncompressedSize: " << paddedUncompressedSize << "\n";

                    if(decodedVertices.size() < paddedUncompressedSize){
                        decodedVertices.resize(paddedUncompressedSize);
                    }

                    // size_t decodedBytesNb = p4nzdec128v16(
                    //     encodedCloudVerticesData.data(),
                    //     paddedUncompressedSize,
                    //     decodedVertices.data());
                    // if(decodedBytesNb == 0){
                    //     Log::error("[K4FrameUncompressor::uncompress_lossless_16_bits_128padded_data] Error decoding data.\n");
                    // }

                    // decodedVertices.resize(uncompressedSize);


                    // auto vData = reinterpret_cast<geo::Pt3<std::uint16_t>*>(decodedVertices.data());

                    auto vvc = frame->cloud.size();
                    for(size_t idP = 0; idP < frame->cloud.size(); ++idP){
                        frame->cloud.vertices[idP]= Pt3f{
                            -(static_cast<float>(decodedVertices[        idP]) - 4096),
                            -(static_cast<float>(decodedVertices[vvc   + idP]) - 4096),
                            static_cast<float>(decodedVertices  [2*vvc + idP])
                        }*0.001f;

                        frame->cloud.colors[idP] = Pt3f{
                           static_cast<float>(image.values[idP].r()),
                           static_cast<float>(image.values[idP].g()),
                           static_cast<float>(image.values[idP].b())
                        }/255.f;
                    }
                }

                // auto cFrame = frameCompressor.compress(*frame, 80);
                // DCCompressedFrame *ptr = cFrame.release();
                // // cFrame.release();
                // auto s_ptr = std::shared_ptr<DCCompressedFrame>(ptr);
                // std::cout << "idCapture : " << s_ptr->idCapture << "\n";
                // std::cout << "vertices : " << s_ptr->validVerticesCount << "\n";
                // std::cout << "fpfColoredCloud : " << s_ptr->fpfColoredCloud.size() << "\n";

                // video.add_compressed_frame(idC, std::move(s_ptr));
            }
        }

        video.save_to_file(dest);

    }
}

#include "soundio.h"


static auto read_callback(struct SoundIoInStream *instream, int frame_count_min, int frame_count_max) -> void {

    // struct SoundIoChannelArea *areas;
    // int err;
    // char *write_ptr = soundio_ring_buffer_write_ptr(ring_buffer);
    // int free_bytes = soundio_ring_buffer_free_count(ring_buffer);
    // int free_count = free_bytes / instream->bytes_per_frame;

    // if (frame_count_min > free_count)
    //     panic("ring buffer overflow");

    // int write_frames = min_int(free_count, frame_count_max);
    // int frames_left = write_frames;

    // for (;;) {
    //     int frame_count = frames_left;

    //     if ((err = soundio_instream_begin_read(instream, &areas, &frame_count)))
    //         panic("begin read error: %s", soundio_strerror(err));

    //     if (!frame_count)
    //         break;

    //     if (!areas) {
    //         // Due to an overflow there is a hole. Fill the ring buffer with
    //         // silence for the size of the hole.
    //         memset(write_ptr, 0, frame_count * instream->bytes_per_frame);
    //         fprintf(stderr, "Dropped %d frames due to internal overflow\n", frame_count);
    //     } else {
    //         for (int frame = 0; frame < frame_count; frame += 1) {
    //             for (int ch = 0; ch < instream->layout.channel_count; ch += 1) {
    //                 memcpy(write_ptr, areas[ch].ptr, instream->bytes_per_sample);
    //                 areas[ch].ptr += areas[ch].step;
    //                 write_ptr += instream->bytes_per_sample;
    //             }
    //         }
    //     }

    //     if ((err = soundio_instream_end_read(instream)))
    //         panic("end read error: %s", soundio_strerror(err));

    //     frames_left -= frame_count;
    //     if (frames_left <= 0)
    //         break;
    // }

    // int advance_bytes = write_frames * instream->bytes_per_frame;
    // soundio_ring_buffer_advance_write_ptr(ring_buffer, advance_bytes);
}

auto test_microphone() -> void{

    std::cout << "test_microphone\n";

    enum SoundIoBackend backend = SoundIoBackendWasapi;
    std::cout << "soundio_create\n";
    auto soundio = soundio_create();
    if(!soundio){
        std::cerr << "out of memory\n";
        return;
    }

    std::cout << "soundio_connect\n";
    int err = 0;
    if(backend == SoundIoBackendNone){
        err = soundio_connect(soundio);
    }else{
        err = soundio_connect_backend(soundio, backend);
    }
    std::cout << "-->" << err << "\n";

    bool connected = false;
    if(err == 0){
        connected = true;
    }else{
        std::cerr << std::format("error connecting {}\n", soundio_strerror(err));
    }

    if(connected){
        soundio_flush_events(soundio);
    }

    int defaultOuputDeviceId = soundio_default_output_device_index(soundio);
    if (defaultOuputDeviceId < 0){
        std::cerr << "no output device found\n";
    }

    int defaultInputdeviceiD = soundio_default_input_device_index(soundio);
    if (defaultInputdeviceiD < 0){
        std::cerr << "no input device found\n";
    }
    std::cout << "default output device index " << defaultOuputDeviceId << " input device " << defaultInputdeviceiD << "\n";

    int inputDevicesCount = soundio_input_device_count(soundio);
    int outputDevicesCount = soundio_output_device_count(soundio);
    std::cout << "inputDevicesCount " << inputDevicesCount << " outputDevicesCount " << outputDevicesCount << "\n";


    std::string wantedDevice = "Microphone (HD Webcam C525)";
    bool useRawDevice = true;

    bool found = false;
    int correpondingInputDeviceId = 0;
    if(defaultInputdeviceiD >= 0){
        for (int id = 0; id < soundio_input_device_count(soundio); ++id) {
            auto device = soundio_get_input_device(soundio, id);

            std::cout << id << " | "  <<  device->id << " [" << device->name << "]\n";
            if(useRawDevice == device->is_raw){
                if(wantedDevice == std::string(device->name)){
                    correpondingInputDeviceId = id;
                    found = true;
                    soundio_device_unref(device);
                    break;
                }
            }

            soundio_device_unref(device);
        }
    }

    std::cout << "device found " << found << " with id " << correpondingInputDeviceId << "\n";
    if(found){
        auto inDevice = soundio_get_input_device(soundio, correpondingInputDeviceId);

        // const struct SoundIoChannelLayout *layout = soundio_best_matching_channel_layout(
        //     nullptr, 0,
        //     inDevice->layouts, inDevice->layout_count
        // );

        std::cout << "layout count " << inDevice->layout_count << "\n";
        for(int idL = 0; idL < inDevice->layout_count; ++idL){
            const auto &layout = inDevice->layouts[idL];
            std::cout << "channels count " << layout.channel_count << "\n";
            std::cout << "name" << layout.name << "\n";
        }



        std::cout << "support format SoundIoFormatInvalid " << soundio_device_supports_format(inDevice, SoundIoFormat::SoundIoFormatInvalid) << "\n";
        std::cout << "support format SoundIoFormatS8 " << soundio_device_supports_format(inDevice, SoundIoFormat::SoundIoFormatS8) << "\n";
        std::cout << "support format SoundIoFormatU8 " << soundio_device_supports_format(inDevice, SoundIoFormat::SoundIoFormatU8) << "\n";
        std::cout << "support format SoundIoFormatS16LE " << soundio_device_supports_format(inDevice, SoundIoFormat::SoundIoFormatS16LE) << "\n";
        std::cout << "support format SoundIoFormatS16BE " << soundio_device_supports_format(inDevice, SoundIoFormat::SoundIoFormatS16BE) << "\n";
        std::cout << "support format SoundIoFormatU16LE " << soundio_device_supports_format(inDevice, SoundIoFormat::SoundIoFormatU16LE) << "\n";
        std::cout << "support format SoundIoFormatU16BE " << soundio_device_supports_format(inDevice, SoundIoFormat::SoundIoFormatU16BE) << "\n";
        std::cout << "support format SoundIoFormatS24LE " << soundio_device_supports_format(inDevice, SoundIoFormat::SoundIoFormatS24LE) << "\n";
        std::cout << "support format SoundIoFormatS24BE " << soundio_device_supports_format(inDevice, SoundIoFormat::SoundIoFormatS24BE) << "\n";
        std::cout << "support format SoundIoFormatU24LE " << soundio_device_supports_format(inDevice, SoundIoFormat::SoundIoFormatU24LE) << "\n";
        std::cout << "support format SoundIoFormatU24BE " << soundio_device_supports_format(inDevice, SoundIoFormat::SoundIoFormatU24BE) << "\n";
        std::cout << "support format SoundIoFormatS32LE " << soundio_device_supports_format(inDevice, SoundIoFormat::SoundIoFormatS32LE) << "\n";
        std::cout << "support format SoundIoFormatS32BE " << soundio_device_supports_format(inDevice, SoundIoFormat::SoundIoFormatS32BE) << "\n";
        std::cout << "support format SoundIoFormatU32LE " << soundio_device_supports_format(inDevice, SoundIoFormat::SoundIoFormatU32LE) << "\n";
        std::cout << "support format SoundIoFormatU32BE " << soundio_device_supports_format(inDevice, SoundIoFormat::SoundIoFormatU32BE) << "\n";
        std::cout << "support format SoundIoFormatFloat32LE " << soundio_device_supports_format(inDevice, SoundIoFormat::SoundIoFormatFloat32LE) << "\n";
        std::cout << "support format SoundIoFormatFloat32BE " << soundio_device_supports_format(inDevice, SoundIoFormat::SoundIoFormatFloat32BE) << "\n";
        std::cout << "support format SoundIoFormatFloat64LE " << soundio_device_supports_format(inDevice, SoundIoFormat::SoundIoFormatFloat64LE) << "\n";
        std::cout << "support format SoundIoFormatFloat64BE " << soundio_device_supports_format(inDevice, SoundIoFormat::SoundIoFormatFloat64BE) << "\n";

        std::cout << "support samplerate 48000 " << soundio_device_supports_sample_rate(inDevice, 48000) << "\n";
        std::cout << "support samplerate 44100 " << soundio_device_supports_sample_rate(inDevice, 44100) << "\n";
        std::cout << "support samplerate 96000 " << soundio_device_supports_sample_rate(inDevice, 96000) << "\n";
        std::cout << "support samplerate 24000 " << soundio_device_supports_sample_rate(inDevice, 24000) << "\n";
        std::cout << "support samplerate 0 " << soundio_device_supports_sample_rate(inDevice, 0) << "\n";
        // soundio_device_nearest_sample_rate()

        auto instream = soundio_instream_create(inDevice);
        if(!instream){
            std::cerr << "out of memory\n";
        }

        double microphone_latency = 0.05;
        instream->format = SoundIoFormat::SoundIoFormatS16LE;
        instream->sample_rate = 48000;
        instream->layout = inDevice->layouts[0];
        instream->software_latency = microphone_latency;
        instream->read_callback = read_callback;

        if ((err = soundio_instream_open(instream))) {
            std::cerr << "unable to open output stream: " << soundio_strerror(err) << "\n";
        }

        int capacity = microphone_latency * 2 * instream->sample_rate * instream->bytes_per_frame;
        auto ring_buffer = soundio_ring_buffer_create(soundio, capacity);


        // soundio_wait_events(soundio);


        soundio_ring_buffer_destroy(ring_buffer);
        soundio_instream_destroy(instream);
        soundio_device_unref(inDevice);
    }
    // ...

    std::cout << "soundio_destroy\n";
    soundio_destroy(soundio);

    std::cout << "end test_microphone\n";
}

#include "depth-camera/frame/dc_depth_indices.hpp"
#include <execution>
#include "utility/string.hpp"

auto process_kvid() -> void{

    DCVideo video;
    std::string path = "E:/newtournage outro1b.kvid";
    video.load_from_file(path);

    DCFrameGenerationSettings settings;
    // settings.calibration = true;
    settings.cloud = false;


    bool initialized = false;
    tool::cam::DCDepthIndices dIndices;
    tool::cam::DCModeInfos mInfos;
    
    tool::cam::DCDataFrameGenerator compressor;

    std::cout << "START\n" << std::endl;
    
    for(size_t idC = 0; idC < video.nb_devices(); ++idC){
        std::cout << "CAMERA " << idC << "\n";
        for(size_t idF = 0; idF < video.nb_frames(idC); ++idF){
            std::cout << idF << " " << std::flush;
            DCFrame frame;
            // std::cout << "uncompress " << idC << " " << idF << "\n";
            video.generate_frame(settings, idC, idF, frame);
            
            // auto ccc = video.get_data_frame(idC, idF);
            // int v = ccc.lock().get()->validVerticesCount;

            // if(!initialized){
            //     mInfos.initialize(frame.mode);
            //     dIndices.initialize(mInfos.has_depth(), mInfos.depth_width(), mInfos.depth_height());
            //     initialized = true;

            //     std::cout << "cloud " << (int) frame.mode  << " " << frame.calibration.size() << " " << frame.depth.size() << " " << frame.rgbaDepthSizedColor.size() << " " <<  frame.cloud.empty() << std::endl;
            // }

            // std::vector<bool> filteringM(frame.depth.size(), false);

            // for(size_t idL = 0; idL < 3; ++idL){
            //     std::for_each(std::execution::par_unseq, std::begin(dIndices.depths1DNoBorders), std::end(dIndices.depths1DNoBorders), [&](size_t id){
            //         if(frame.depth[id] ==  dc_invalid_depth_value){
            //             return;
            //         }
            //         std::uint8_t count = 0;
            //         for(auto cId : dIndices.neighbours8Depth1D[id]){
            //             if(frame.depth[cId] != dc_invalid_depth_value){
            //                 ++count;
            //             }
            //         }
            //         filteringM[id] = count >= 6;
            //     });


            //     for(size_t id = 0; id < filteringM.size(); ++id){
            //         if(!filteringM[id]){
            //             frame.depth[id] = dc_invalid_depth_value;
            //         }
            //     }
            // }

            // size_t idValid = 0;
            // for(size_t id = 0; id < filteringM.size(); ++id){
            //     if(filteringM[id]){
            //         ++idValid;
            //     }
            // }

            // auto dFrame = std::make_shared<DCDataFrame>();
            // // compressor.compress(frame, 100, cFrame.get());
            // // cFrame->validVerticesCount = idValid;


            // // std::cout << "ccloud " << cFrame->calibration.size() << " " << (int)cFrame->mode << " "<< cFrame->fpfDepth.size() << " " << cFrame->validVerticesCount << " " << v <<  "\n";
            // video.replace_data_frame(idC, idF, std::move(dFrame));
            // // fIndices.neighbours8Depth1D;
            // // frame.depth.values;



            // // fIndices
            // frame.mode;
            // frame.calibration;
            // frame.depth


        }
    }

    String::replace_first(path, ".kvid", "-flo-modified.kvid");
    video.save_to_file(path);
}

auto test_device_idle() -> void{

    DCDevice device;
    device.start_thread();

    DCDeviceSettings settings;
    settings.configS.idDevice = 0;
    settings.configS.openDevice = true;
    settings.configS.startReading = true;
    settings.configS.typeDevice = DCType::AzureKinect;
    device.update_device_settings(settings);

    for(int ii = 0; ii < 20; ++ii){
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        Log::warning(std::format("idle {}\n",ii));
    }
}

#include "depth-camera/settings/dc_client_settings.hpp"
#include "depth-camera/settings/dc_server_settings.hpp"

int main(int argc, char *argv[]){

    std::cout << "argc " << argc << "\n";
    for(int ii = 0; ii < argc; ++ii){
        std::cout << "-> " << argv[ii] << "\n";
    }

    std::cout << "start\n";
    
    auto logger = std::make_unique<Logger>();
    logger->init("./");
    logger->message_signal.connect([&](std::string message){
        std::cout << message;
    });
    logger->warning_signal.connect([&](std::string warning){
        std::cerr << warning;
    });
    logger->error_signal.connect([&](std::string error){
        std::cerr << error;
    });
    Logger::set_logger_instance(std::move(logger));


    DCVideoPlayer p;
    DCVideo v1;
    Log::message("load\n");
    v1.load_from_file("E:/_kvid/aaaa.kvid");
    p.set_video(v1);

    DCVideoPlayerSettings ps;
    ps.doLoop = false;
    p.update_settings(ps);
    p.start_video();

    int idF = -5;
    while(p.is_playing()){

        p.update();

        std::cout << p.current_frame_id(0) << " " << p.current_compressed_frame_cloud_size(0) << "\n";

        if(auto frame = p.current_frame(0); frame != nullptr){

            // std::cout << " --- > " << frame->volumesB.size() << " " << frame->imagesB.contains(DCImageBufferType::DepthSizedColorRGBA8) << " " <<  frame->image_buffer_size(DCImageBufferType::DepthSizedColorRGBA8) << " " <<  frame->volumesB.contains(DCVolumeBufferType::ColoredCloud) <<  "\n";
            if(frame->volumesB.contains(DCVolumeBufferType::ColoredCloud)){
                auto buffer = frame->volume_buffer<ColorCloud>(DCVolumeBufferType::ColoredCloud);
                // std::cout << "[ -- " << buffer->vertices.size() <<  " " << frame->volume_buffer_size(DCVolumeBufferType::ColoredCloud) << "\n";
            }
            if(frame->imagesB.contains(DCImageBufferType::DepthSizedColorRGBA8)){
                auto buffer = frame->image_buffer<ColorRGBA8>(DCImageBufferType::DepthSizedColorRGBA8);
                std::cout << "#### " << buffer->width << " " << buffer->height << " " << buffer->size() << "\n";// << frame->image_buffer_size(DCImageBufferType::DepthSizedColorRGBA8) << "\n";

                // const auto &imageB = frame->imagesB[DCImageBufferType::DepthSizedColorRGBA8];
                // std::visit(Visitor{
                //        [](const ImageBuffer<ColorRGBA8> &sImg){
                //         std::cout << "A: " << sImg.size() << "\n";
                //            return sImg.size();
                //        },
                //        [](const ImageBuffer<ColorRGB8> &sImg){
                //            return sImg.size();
                //        },
                //        [](const ImageBuffer<ColorGray8> &sImg){
                //            return sImg.size();
                //        },
                //        [](const ImageBuffer<ColorGray16> &sImg){
                //            return sImg.size();
                //        }
                //    }, imageB
                //    );
            }
            // if(frame->volumesB.size() > 0){
                // std::cout << " --- > " << frame->volume_buffer_size(DCVolumeBufferType::ColoredCloud) << "\n";
            // }

        }

        if(p.current_time_ms() > p.end_time_ms()){
            break;
        }


        // if(auto cloud = frame->volume_buffer<geo::ColorCloud>(DCVolumeBufferType::ColoredCloud)){
    }

    return 0;

    std::string baseGrabberPath = "C:/Users/SDKim/Desktop/conversions/copy/dc-grabber/config/";
    std::string baseManagerPath = "C:/Users/SDKim/Desktop/conversions/copy/dc-manager/config/";
    //
    // settings/device_ALL_HERE_NUC2_G0.json
    // settings/device_ALL_HERE_NUC3_G0.json
    // settings/device_ALL_HERE_NUC4_G0.json

    // network/network_ALL_HERE_NUC1_G0.config
    // id_interface | reading_port | ipv4/ipv6
    // 0 8888 ipv4
    // 0 8890 ipv4
    // 0 8892 ipv4
    // 0 8894 ipv4

    DCServerSettings serverS;
    serverS.udpServerS.protocol = net::Protocol::ipv4;
    serverS.udpServerS.readingInterfaceId = 0;
    serverS.udpServerS.readingPort = 8888;
    serverS.udpServerS.anyReadingInterface = false;
    // serverS.deviceS.load_from_file(baseGrabberPath + "settings/device_ALL_HERE_NUC1_G0.json");
    auto &c = serverS.deviceS.configS;
    c.openDevice    = true;
    c.startReading  = true;
    c.typeDevice    = DCType::FemtoBolt;
    c.idDevice      = 0;
    c.mode          = DCMode::FB_C1280x720_DI640x576_NV12_F30;

    auto &d = serverS.deviceS.dataS;
    auto &cap = d.server.capture;
    cap.depth = true;
    cap.color = true;

    auto &send = d.server.sending;
    send.addDepth               = true;
    send.addDepthSizedColor     = true;
    send.addOriginalSizeColor   = false;
    send.addInfra               = false;
    send.addBodiesId            = false;
    send.addBodiesSkeleton      = false;
    send.depthCM                = DCCompressionMode::FastPFor;
    send.depthSizedColorCM      = DCCompressionMode::JPEG;
    d.server.generation.depthSizedColorImage = true;
    d.server.generation.cloud                = true;

    serverS.save_to_json_str_file("C:/Users/SDKim/Desktop/server_ALL_HERE_NUC1_0.json");
    serverS.udpServerS.readingPort = 8890;
    serverS.save_to_json_str_file("C:/Users/SDKim/Desktop/server_ALL_HERE_NUC2_0.json");
    serverS.udpServerS.readingPort = 8892;
    serverS.save_to_json_str_file("C:/Users/SDKim/Desktop/server_ALL_HERE_NUC3_0.json");
    serverS.udpServerS.readingPort = 8894;
    serverS.save_to_json_str_file("C:/Users/SDKim/Desktop/server_ALL_HERE_NUC4_0.json");

    // DCDeviceDataSettings server;
    // DCFrameGenerationSettings clientGeneration;

    // std::cout << serverS.deviceS.configS.


    DCClientSettings clientS;
    clientS.devicesS.resize(4);

    auto &co1 = clientS.devicesS[0].connectionS;
    co1.connectionType      = DCClientType::Remote;
    co1.protocol            = net::Protocol::ipv4;
    co1.readingPort         = 8889;
    co1.sendingPort         = 8888;
    co1.sendingAddress      = "ALL_HERE_NUC1";
    co1.idReadingInterface  = 0;
    co1.anyReadingInterface = false;

    auto &co2 = clientS.devicesS[1].connectionS;
    co2.connectionType      = DCClientType::Remote;
    co2.protocol            = net::Protocol::ipv4;
    co2.readingPort         = 8891;
    co2.sendingPort         = 8890;
    co2.sendingAddress      = "ALL_HERE_NUC2";
    co2.idReadingInterface  = 0;
    co2.anyReadingInterface = false;

    auto &co3 = clientS.devicesS[2].connectionS;
    co3.connectionType      = DCClientType::Remote;
    co3.protocol            = net::Protocol::ipv4;
    co3.readingPort         = 8893;
    co3.sendingPort         = 8892;
    co3.sendingAddress      = "ALL_HERE_NUC3";
    co3.idReadingInterface  = 0;
    co3.anyReadingInterface = false;

    auto &co4 = clientS.devicesS[3].connectionS;
    co4.connectionType      = DCClientType::Remote;
    co4.protocol            = net::Protocol::ipv4;
    co4.readingPort         = 8895;
    co4.sendingPort         = 8894;
    co4.sendingAddress      = "ALL_HERE_NUC4";
    co4.idReadingInterface  = 0;
    co4.anyReadingInterface = false;

    clientS.devicesS[0].deviceS.load_from_file(baseManagerPath + "settings/device_ALL_HERE_VRPC_G0.json");
    clientS.devicesS[1].deviceS.load_from_file(baseManagerPath + "settings/device_ALL_HERE_VRPC_G1.json");
    clientS.devicesS[2].deviceS.load_from_file(baseManagerPath + "settings/device_ALL_HERE_VRPC_G2.json");
    clientS.devicesS[3].deviceS.load_from_file(baseManagerPath + "settings/device_ALL_HERE_VRPC_G3.json");

    clientS.devicesS[0].filtersS.load_from_file(baseManagerPath + "settings/filters_ALL_HERE_VRPC_G0.json");
    clientS.devicesS[1].filtersS.load_from_file(baseManagerPath + "settings/filters_ALL_HERE_VRPC_G1.json");
    clientS.devicesS[2].filtersS.load_from_file(baseManagerPath + "settings/filters_ALL_HERE_VRPC_G2.json");
    clientS.devicesS[3].filtersS.load_from_file(baseManagerPath + "settings/filters_ALL_HERE_VRPC_G3.json");

    clientS.devicesS[0].calibrationFiltersS.load_from_file(baseManagerPath + "settings/calibration_filters_ALL_HERE_VRPC_G0.config");
    clientS.devicesS[1].calibrationFiltersS.load_from_file(baseManagerPath + "settings/calibration_filters_ALL_HERE_VRPC_G1.config");
    clientS.devicesS[2].calibrationFiltersS.load_from_file(baseManagerPath + "settings/calibration_filters_ALL_HERE_VRPC_G2.config");
    clientS.devicesS[3].calibrationFiltersS.load_from_file(baseManagerPath + "settings/calibration_filters_ALL_HERE_VRPC_G3.config");

    clientS.devicesS[0].colorS.load_from_file(baseManagerPath + "settings/color_ALL_HERE_VRPC_G0.json");
    clientS.devicesS[1].colorS.load_from_file(baseManagerPath + "settings/color_ALL_HERE_VRPC_G1.json");
    clientS.devicesS[2].colorS.load_from_file(baseManagerPath + "settings/color_ALL_HERE_VRPC_G2.json");
    clientS.devicesS[3].colorS.load_from_file(baseManagerPath + "settings/color_ALL_HERE_VRPC_G3.json");

    clientS.devicesS[0].modelS.load_from_file(baseManagerPath + "calibration/model_ALL_HERE_VRPC_G0.json");
    clientS.devicesS[1].modelS.load_from_file(baseManagerPath + "calibration/model_ALL_HERE_VRPC_G1.json");
    clientS.devicesS[2].modelS.load_from_file(baseManagerPath + "calibration/model_ALL_HERE_VRPC_G2.json");
    clientS.devicesS[3].modelS.load_from_file(baseManagerPath + "calibration/model_ALL_HERE_VRPC_G3.json");

    clientS.save_to_json_str_file("C:/Users/SDKim/Desktop/client_ALL_HERE_VRPC_0.json");

    return 0;

    DCVideoPlayer player;


    DCVideo v;
    Log::message("load\n");
    v.load_from_file("E:/07-09-fam2.kvid");
    player.set_video(v);
    // auto dfp = v.get_data_frames_ptr(0);
    // auto df = dfp->get_data_frame(dfp->nb_frames()/2);
    
    Log::message("loaded\n");
    DCFrameGenerationSettings fgS;
    DCDeprecatedFrame frame;

    player.set_current_time(25000.0);
    player.update();

    ColorVoxelGrid cGrid(0.0025f, {-2.f,-2.f,-2.f}, {+2.f,+2.f,+2.f});

    auto t1 = Time::nanoseconds_since_epoch();
    for(size_t idD = 0; idD < v.nb_devices(); ++idD){
        auto frame = player.current_frame(idD);
        // cGrid.add_cloud(frame->cloud, v.get_transform(idD).conv<float>());
    }
    Log::message(std::format("elapsed: {}\n", Time::difference_ms(t1, Time::nanoseconds_since_epoch())));
    cGrid.compute_grid();
    Log::message(std::format("elapsed: {}\n", Time::difference_ms(t1, Time::nanoseconds_since_epoch())));

    ColorCloud vCloud;
    Log::message(std::format("elapsed: {}\n", Time::difference_ms(t1, Time::nanoseconds_since_epoch())));
    cGrid.convert_to_cloud(vCloud);
    Log::message(std::format("elapsed: {}\n", Time::difference_ms(t1, Time::nanoseconds_since_epoch())));
    io::CloudIO::save_cloud("E:/agg_c1.obj", vCloud);
    
    Log::message("ended\n");
    // if(auto frame = df.lock()){
    //     Log::message(std::format("size {}\n", frame->validVerticesCount));

    //
    //     // cGrid.add_cloud(frame->)
    // }

    // for(size_t idD = 0; idD < k4a::device::get_installed_count(); ++idD){
    //     try{
    //         k4a::device dev = k4a::device::open(idD);

    //         auto sn = dev.get_serialnum();
    //         std::cout << "open device with " << sn << "\n";

    //     }catch(const std::runtime_error &er){
    //         std::cerr << std::format("Cannot open device with id [{}], error: [{}].\n", idD, er.what()) << "\n";
    //     }

    // }

    // test_device_idle();
    // test_microphone();
    // process_kvid();
    // test_femto_mega();

    return 0;

    // convert_kvid("D:/test_kvid/thirdPartR.kvid", "D:/test_kvid/thirdPartR_conv.kvid");


    // std::vector<Vertex> generateVertices(int widthSegments, int heightSegments, float a, float b, float c, float d, float e, float g) {
    //     std::vector<Vertex> vertices;
    //     for (int i =  0; i <= heightSegments; ++i) {
    //         for (int j =  0; j <= widthSegments; ++j) {
    //             float x = (float)j / (float)widthSegments;
    //             float y = (float)i / (float)heightSegments;
    //             float z = a * x * x + b * y * y + c * x * y + d * x + e * y + g;
    //             vertices.push_back({x, y, z});
    //         }
    //     }
    //     return vertices;
    // }

    // auto id = Mat4f::identity();
    // id.at(0) = 2;
    // id.at(1,1) = 3;
    // std::cout << "mat " << id << "\n";

    // auto mat3 = cut(id,3,3);
    // auto inv = inverse(mat3);

    // std::cout << mat3 << "\n";
    // std::cout << inv << "\n";
    // std::cout << inv * mat3 << "\n";

    // auto tp = transpose(inv);
    // std::cout << tp << "\n";


    // test_raw_orbbec_femto();
    // test_raw_orbbec_femto();
    // test_raw_k4device();
    // test_kinect4();

    return 0;
}



