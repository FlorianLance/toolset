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

#include "TurboPFor/vp4.h"

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

                    size_t decodedBytesNb = p4nzdec128v16(
                        encodedCloudVerticesData.data(),
                        paddedUncompressedSize,
                        decodedVertices.data());
                    if(decodedBytesNb == 0){
                        Logger::error("[K4FrameUncompressor::uncompress_lossless_16_bits_128padded_data] Error decoding data.\n");
                    }

                    decodedVertices.resize(uncompressedSize);


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

int main(){

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
    test_femto_mega();
    return 0;
}



