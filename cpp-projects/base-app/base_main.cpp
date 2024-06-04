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
#include "data/jpeg_encoding.hpp"

#include "utility/io_fstream.hpp"

// #include "TurboPFor/vp4.h"

#include "thirdparty/stb/stb_image_write.h"
#include "io/cloud_io.hpp"

#include "depth-camera/dc_frame_compressor.hpp"

using namespace tool;
using namespace tool::geo;
using namespace tool::cam;

auto convert_kvid(const std::string &path, const std::string &dest) -> void{

    if(path.empty()){
        return;
    }

    Logger::message(std::format("Convert_kvid: {}\n", path));


    // open file
    std::ifstream file(path.data(), std::ios_base::binary);
    if(!file.is_open()){
        Logger::error(std::format("Cannot open binary file with path: [{}].\n", path));
        return;
    }
    file.exceptions ( std::ifstream::eofbit | std::ifstream::failbit | std::ifstream::badbit);

    data::JpegDecoder jpgDecoder;
    DCFrameCompressor frameCompressor;
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

        std::vector<std::vector<std::shared_ptr<DCFrame>>> frames;
        frames.resize(nbCameras);

        for(size_t id = 0; id < static_cast<size_t>(nbCameras); ++id){

            std::int32_t nbFrames;
            read(nbFrames, file);
            std::cout << "nbFrames: " << (int)nbFrames << "\n";
            frames[id].resize(nbFrames);

            for(auto &frame : frames[id]){
                frame = std::make_shared<DCFrame>();
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
                    //     Logger::error("[K4FrameUncompressor::uncompress_lossless_16_bits_128padded_data] Error decoding data.\n");
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

                auto cFrame = frameCompressor.compress(*frame, 80);
                DCCompressedFrame *ptr = cFrame.release();
                // cFrame.release();
                auto s_ptr = std::shared_ptr<DCCompressedFrame>(ptr);
                std::cout << "idCapture : " << s_ptr->idCapture << "\n";
                std::cout << "vertices : " << s_ptr->validVerticesCount << "\n";
                std::cout << "fpfColoredCloud : " << s_ptr->fpfColoredCloud.size() << "\n";

                video.add_compressed_frame(idC, std::move(s_ptr));
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

int main(int argc, char *argv[]){

    std::cout << "argc " << argc << "\n";
    for(int ii = 0; ii < argc; ++ii){
        std::cout << "-> " << argv[ii] << "\n";
    }

    std::cout << "start\n";
    Logger::init("./");
    Logger::get()->message_signal.connect([&](std::string message){
        std::cout << message;
    });
    Logger::get()->warning_signal.connect([&](std::string warning){
        std::cerr << warning;
    });
    Logger::get()->error_signal.connect([&](std::string error){
        std::cerr << error;
    });

    test_microphone();
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



