
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

#include "dc_compressed_frame.hpp"

// kinect4
#include <k4a/k4atypes.h>

// orbbec
#include <libobsensor/hpp/Types.hpp>

// local
#include "utility/io_data.hpp"
#include "utility/io_fstream.hpp"

using namespace tool;
using namespace tool::cam;

auto buffer_bytes_size(const BinaryImageBuffer &iBuffer) -> size_t{
    return
        sizeof(size_t) + iBuffer.size() +
        sizeof(iBuffer.width) + sizeof(iBuffer.height) ;
}

auto buffer_bytes_size(const BinaryBuffer &iBuffer) -> size_t{
    return
        sizeof(size_t) + iBuffer.size();
}

auto read_buffer(BinaryImageBuffer &iBuffer, const std::span<const std::int8_t> data, size_t &offset) -> void{

    using namespace tool;
    read(iBuffer.width, data, offset);
    read(iBuffer.height, data, offset);
    size_t encodedDataSize = 0;
    read(encodedDataSize, data, offset);
    if(encodedDataSize > 0){
        iBuffer.resize(encodedDataSize);
        read_array(iBuffer.get_data(), data, encodedDataSize, offset);
    }
}

auto read_buffer(BinaryBuffer &buffer, const std::span<const std::int8_t> data, size_t &offset) -> void{

    using namespace tool;
    size_t encodedDataSize = 0;
    read(encodedDataSize, data, offset);
    if(encodedDataSize > 0){
        buffer.resize(encodedDataSize);
        read_array(buffer.get_data(), data, encodedDataSize, offset);
    }
}

auto read_buffer(BinaryImageBuffer &iBuffer, std::ifstream &file) -> void{

    using namespace tool;
    read(iBuffer.width, file);
    read(iBuffer.height, file);
    size_t encodedDataSize = 0;
    read(encodedDataSize, file);
    if(encodedDataSize > 0){
        iBuffer.resize(encodedDataSize);
        read_array(iBuffer.get_data(), file, encodedDataSize);
    }
}

auto read_buffer(BinaryBuffer &buffer, std::ifstream &file) -> void{

    using namespace tool;
    size_t encodedDataSize = 0;
    read(encodedDataSize, file);
    if(encodedDataSize > 0){
        buffer.resize(encodedDataSize);
        read_array(buffer.get_data(), file, encodedDataSize);
    }
}

auto write_buffer(const BinaryImageBuffer &iBuffer, std::span<int8_t> data, size_t &offset) -> void{

    using namespace tool;
    write(iBuffer.width,  data, offset);
    write(iBuffer.height, data, offset);
    write(iBuffer.size(), data, offset);
    if(!iBuffer.empty()){
        write_array(iBuffer.get_data(), data, iBuffer.size(), offset);
    }
}

auto write_buffer(const BinaryBuffer &buffer, std::span<int8_t> data, size_t &offset) -> void{

    using namespace tool;
    write(buffer.size(), data, offset);
    if(!buffer.empty()){
        write_array(buffer.get_data(), data, buffer.size(), offset);
    }
}


auto write_buffer(const BinaryImageBuffer &iBuffer, std::ofstream &file) -> void{

    using namespace tool;
    write(iBuffer.width,  file);
    write(iBuffer.height, file);
    write(iBuffer.size(), file);
    if(!iBuffer.empty()){
        write_array(iBuffer.get_data(), file, iBuffer.size());
    }
}

auto write_buffer(const BinaryBuffer &buffer, std::ofstream &file) -> void{

    using namespace tool;
    write(buffer.size(), file);
    if(!buffer.empty()){
        write_array(buffer.get_data(), file, buffer.size());
    }
}


auto DCCompressedFrame::data_size() const noexcept -> size_t {
    return
        // frame
        Frame::data_size() +
        // infos
        sizeof(mode) +
        sizeof(validVerticesCount) +
        // jpeg encoding
        buffer_bytes_size(jpegRGBA8Color) +
        buffer_bytes_size(jpegRGBA8DepthSizedColor) +
        buffer_bytes_size(jpegG8BodiesId) +
        // lossless std::uint16_t encoding
        buffer_bytes_size(ll16eDepth) +
        buffer_bytes_size(ll16eInfra) +
        buffer_bytes_size(ll16eColoredCloud) +
        // binary encoding
        buffer_bytes_size(calibration) +
        buffer_bytes_size(imu);
        // // audio
        // sizeof(size_t) + // nb audio frames
        // audioFrames.size()*7*sizeof(float) +
        // // bodies
        // sizeof(bool); // has body
        // ...
}

auto DCCompressedFrame::init_from_file_stream(std::ifstream &file) -> void{

    // frame
    Frame::init_from_file_stream(file);

    // infos
    read(mode, file);
    read(validVerticesCount, file);
    // jpeg encoding
    read_buffer(jpegRGBA8Color, file);
    read_buffer(jpegRGBA8DepthSizedColor, file);
    read_buffer(jpegG8BodiesId, file);
    // lossless std::uint16_t encoding
    read_buffer(ll16eDepth, file);
    read_buffer(ll16eInfra, file);
    read_buffer(ll16eColoredCloud, file);
    // binary encoding
    read_buffer(calibration, file);
    read_buffer(imu, file);

    // // audio
    // size_t nbAudioFrames = 0;
    // read(nbAudioFrames, file);
    // if(nbAudioFrames > 0){
    //     audioFrames.resize(nbAudioFrames);
    //     read_array(audioFrames.data()->data(), file, nbAudioFrames*7);
    // }

    // // bodies
    // bool hasBodies = false;
    // read(hasBodies, file);
    // if(hasBodies){
    //     // TODO: ...
    // }
}


auto DCCompressedFrame::init_from_data(std::span<const std::int8_t> data, size_t &offset) -> void{

    // frame
    Frame::init_from_data( data, offset);
    // infos
    read(mode, data,  offset);
    read(validVerticesCount, data, offset);
    // jpeg encoding
    read_buffer(jpegRGBA8Color, data, offset);
    read_buffer(jpegRGBA8DepthSizedColor, data, offset);
    read_buffer(jpegG8BodiesId, data, offset);
    // lossless std::uint16_t encoding
    read_buffer(ll16eDepth, data, offset);
    read_buffer(ll16eInfra, data, offset);
    read_buffer(ll16eColoredCloud, data, offset);
    // binary encoding
    read_buffer(calibration, data, offset);
    read_buffer(imu, data, offset);

    // // audio
    // size_t nbAudioFrames    = 0;
    // read(nbAudioFrames, data, offset);
    // if(nbAudioFrames > 0){
    //     audioFrames.resize(nbAudioFrames);
    //     read_array(audioFrames.data()->data(), data, nbAudioFrames*7, offset);
    // }

    // // bodies
    // bool hasBodies = false;
    // read(hasBodies, data, offset);
    // if(hasBodies){
    //     // TODO: ...
    // }

}


auto DCCompressedFrame::write_to_file_stream(std::ofstream &file) -> void{

    // frame
    Frame::write_to_file_stream(file);
    // infos
    write(mode, file);
    write(validVerticesCount, file);
    // jpeg encoding
    write_buffer(jpegRGBA8Color, file);
    write_buffer(jpegRGBA8DepthSizedColor, file);
    write_buffer(jpegG8BodiesId, file);
    // lossless std::uint16_t encoding
    write_buffer(ll16eDepth, file);
    write_buffer(ll16eInfra, file);
    write_buffer(ll16eColoredCloud, file);
    // binary encoding
    write_buffer(calibration, file);
    write_buffer(imu, file);

    // // imu
    // write(imuSample.has_value(), file);
    // if(imuSample.has_value()){
    //     write(imuSample.value(), file);
    // }

    // // audio
    // size_t nbAudioFrames    = audioFrames.size();
    // write(nbAudioFrames, file);
    // if(nbAudioFrames > 0){
    //     write_array(audioFrames.data()->data(), file, nbAudioFrames*7);
    // }

    // // bodies
    // bool hasBodies = false;
    // write(hasBodies, file);
    // if(hasBodies){
    //     // TODO: ...
    // }
}

auto DCCompressedFrame::write_to_data(std::span<int8_t> data, size_t &offset) -> void{

    // frame
    Frame::write_to_data(data, offset);
    // infos
    write(mode, data, offset);
    write(validVerticesCount, data, offset);
    // jpeg encoding
    write_buffer(jpegRGBA8Color, data, offset);
    write_buffer(jpegRGBA8DepthSizedColor, data, offset);
    write_buffer(jpegG8BodiesId, data, offset);
    // lossless std::uint16_t encoding
    write_buffer(ll16eDepth, data, offset);
    write_buffer(ll16eInfra, data, offset);
    write_buffer(ll16eColoredCloud, data, offset);
    // binary encoding
    write_buffer(calibration, data, offset);
    write_buffer(imu, data, offset);

    // no encoding
    // sizeof(DCBody);

    // // audio
    // size_t nbAudioFrames    = audioFrames.size();
    // write(nbAudioFrames, data, offset, sizeData);
    // if(nbAudioFrames > 0){
    //     write_array(audioFrames.data()->data(), data, nbAudioFrames*7, offset, sizeData);
    // }
}

auto DCCompressedFrame::init_legacy_cloud_frame_from_file_stream(std::ifstream &file) -> void{

    // // # read info
    // read(idCapture, file);
    // read(afterCaptureTS, file);
    // receivedTS = afterCaptureTS;

    // // # read cloud
    // std::int32_t validVerticesC;
    // read(validVerticesC, file);
    // validVerticesCount = validVerticesC;
    // std::int32_t cloudBufferSize;
    // read(cloudBufferSize, file);
    // encodedCloudVerticesData.resize(cloudBufferSize);
    // if(!encodedCloudVerticesData.empty()){
    //     read_array(encodedCloudVerticesData.data(), file, encodedCloudVerticesData.size());
    // }

    // // # read color
    // std::int16_t colorW, colorH;
    // std::int32_t colorBufferSize;
    // read(colorW, file);
    // read(colorH, file);
    // read(colorBufferSize, file);
    // encodedImageColor.width  = colorW;
    // encodedImageColor.height = colorH;
    // encodedDepthSizedColorData.resize(colorBufferSize);
    // if(!encodedDepthSizedColorData.empty()){
    //     read_array(encodedDepthSizedColorData.data(), file, encodedDepthSizedColorData.size());
    // }

    // // # read audio
    // std::int32_t audioBufferSize;
    // read(audioBufferSize, file);

    // audioFrames.resize(audioBufferSize);
    // if(!audioFrames.empty()){
    //     read_array(reinterpret_cast<float*>(audioFrames.data()), file, audioBufferSize*7);
    // }
    // // # read imu
    // read_array(file, reinterpret_cast<char*>(&imuSample), sizeof (DCImuSample));
}

auto DCCompressedFrame::init_legacy_full_frame_from_file_stream(std::ifstream &file) -> void{

    // // # read info
    // read(afterCaptureTS, file);
    // receivedTS = afterCaptureTS;
    // read(mode, file);

    // k4a_calibration_t rCalibration;
    // read(rCalibration, file);
    // i->k4Calibration = rCalibration;
    // std::int32_t validVerticesC;
    // read(validVerticesC, file);
    // validVerticesCount = validVerticesC;
    // // # read color
    // std::int16_t colorW, colorH;
    // std::int32_t colorBufferSize;
    // read(colorW, file);
    // read(colorH, file);
    // read(colorBufferSize, file);
    // encodedImageColor.width  = colorW;
    // encodedImageColor.height = colorH;
    // encodedImageColor.resize(colorBufferSize);
    // if(!encodedImageColor.empty()){
    //     read_array(encodedImageColor.get_data(), file, encodedImageColor.size());
    // }
    // // # read depth
    // std::int16_t depthW, depthH;
    // std::int32_t depthBufferSize;
    // read(depthW, file);
    // read(depthH, file);
    // read(depthBufferSize, file);
    // depthWidth  = depthW;
    // depthHeight = depthH;
    // encodedDepthData.resize(depthBufferSize);
    // if(!encodedDepthData.empty()){
    //     read_array(encodedDepthData.data(), file, encodedDepthData.size());
    // }
    // // # read infra
    // std::int16_t infraW, infraH;
    // std::int32_t infraBufferSize;
    // read(infraW, file);
    // read(infraH, file);
    // read(infraBufferSize, file);
    // infraWidth  = infraW;
    // infraHeight = infraH;
    // encodedInfraData.resize(infraBufferSize);
    // if(!encodedInfraData.empty()){
    //     read_array(encodedInfraData.data(), file, encodedInfraData.size());
    // }

    // // # read audio
    // std::int32_t audioBufferSize;
    // read(audioBufferSize, file);
    // audioFrames.resize(audioBufferSize);
    // if(!audioFrames.empty()){
    //     read_array(reinterpret_cast<float*>(audioFrames.data()), file, audioBufferSize*7);
    // }
    // // # read imu
    // read_array(file, reinterpret_cast<char*>(&imuSample), sizeof (DCImuSample));
}



// bool hasCalibration = false;
// read(hasCalibration, data, offset, sizeData);
// if(hasCalibration){
//     if(dc_get_device(mode) == DCType::AzureKinect){
//         k4a_calibration_t rCalibration;
//         read(rCalibration, data, offset, sizeData);
//         i->k4Calibration = rCalibration;
//     }else if(dc_get_device(mode) == DCType::FemtoBolt){
//         OBCameraParam rCalibration;
//         read(rCalibration, data, offset, sizeData);
//         i->obCalibration = rCalibration;
//     }
// }


// if(i->k4Calibration.has_value()){
//     write(true, file);
//     write(i->k4Calibration.value(), file);
// }else if(i->obCalibration.has_value()){
//     write(true, file);
//     write(i->obCalibration.value(), file);
// }else{
//     write(false, file);
// }
