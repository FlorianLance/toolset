
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

#include "dc_data_frame.hpp"

// kinect4
#include <k4a/k4atypes.h>

// orbbec
// #include <libobsensor/hpp/Types.hpp>

// local
#include "utility/io_data.hpp"
#include "utility/io_fstream.hpp"
#include "utility/logger.hpp"

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

auto read_buffer(BinaryBuffer &buffer, const std::span<const std::byte> data, size_t &offset) -> void{

    using namespace tool;
    size_t encodedDataSize = 0;
    read(encodedDataSize, data, offset);
    if(encodedDataSize > 0){
        buffer.resize(encodedDataSize);
        read_array(buffer.get_data(), data, encodedDataSize, offset);
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


auto read_buffer(BinaryImageBuffer &iBuffer, const std::span<const std::byte> data, size_t &offset) -> void{

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

auto write_buffer(const BinaryBuffer &buffer, std::span<std::byte> data, size_t &offset) -> void{

    using namespace tool;
    write(buffer.size(), data, offset);
    if(!buffer.empty()){
        write_array(buffer.get_data(), data, buffer.size(), offset);
    }
}

auto write_buffer(const BinaryBuffer &buffer, std::ofstream &file) -> void{

    using namespace tool;
    write(buffer.size(), file);
    if(!buffer.empty()){
        write_array(buffer.get_data(), file, buffer.size());
    }
}

auto write_buffer(const BinaryImageBuffer &iBuffer, std::span<std::byte> data, size_t &offset) -> void{

    using namespace tool;
    write(iBuffer.width,  data, offset);
    write(iBuffer.height, data, offset);
    write(iBuffer.size(), data, offset);
    if(!iBuffer.empty()){
        write_array(iBuffer.get_data(), data, iBuffer.size(), offset);
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

auto DCDataFrame::init_from_data(std::span<const std::byte> data, size_t &offset) -> void{

    // frame
    Frame::init_from_data( data, offset);

    // infos
    read(mode, data,  offset);
    read(validVerticesCount, data, offset);

    // read info buffers
    size_t nbInfosB = 0;
    read(nbInfosB, data, offset);
    infosB.reserve(nbInfosB);
    for(size_t idI = 0; idI < nbInfosB; ++idI){
        auto type = read_and_return<DCInfoType>(data, offset);
        infosB.insert({type, read_and_return<std::int64_t>(data, offset)});
    }

    // read data buffers
    size_t nbDataB = 0;    
    read(nbDataB, data, offset);
    datasB.reserve(nbDataB);
    for(size_t idD = 0; idD < nbDataB; ++idD){
        auto type       = read_and_return<DCDataBufferType>(data, offset);
        auto encoding   = read_and_return<DCCompressionMode>(data, offset);
        auto iInfo      = datasB.insert({type, {encoding, BinaryBuffer()}});
        read_buffer(std::get<1>(iInfo.first->second), data, offset);
    }

    // read image data buffers    
    size_t nbImageB = 0;
    read(nbImageB, data, offset);
    imagesB.reserve(nbImageB);
    for(size_t idID = 0; idID < nbImageB; ++idID){
        auto type       = read_and_return<DCImageBufferType>(data, offset);
        auto encoding   = read_and_return<DCCompressionMode>(data, offset);
        auto iInfo      = imagesB.insert({type, {encoding, BinaryImageBuffer()}});
        read_buffer(std::get<1>(iInfo.first->second), data, offset);
    }

    // read volume buffers
    size_t nbVolumeB = 0;
    read(nbVolumeB, data, offset);
    volumesB.reserve(nbVolumeB);
    for(size_t idID = 0; idID < nbVolumeB; ++idID){
        auto type       = read_and_return<DCVolumeBufferType>(data, offset);
        auto encoding   = read_and_return<DCCompressionMode>(data, offset);
        // Log::message(std::format("Read data volume [{}] [{}] [{}]", idID, (int)type, (int)encoding));
        auto iInfo      = volumesB.insert({type, {encoding, BinaryImageBuffer()}});
        read_buffer(std::get<1>(iInfo.first->second), data, offset);
    }
}

auto DCDataFrame::init_from_file_stream(std::ifstream &file) -> void{

    Frame::init_from_file_stream(file);

    // infos
    read(mode, file);
    read(validVerticesCount, file);

    // read info buffers
    size_t nbInfosB = 0;
    read(nbInfosB, file);
    infosB.reserve(nbInfosB);
    for(size_t idI = 0; idI < nbInfosB; ++idI){
        auto type = read_and_return<DCInfoType>(file);
        infosB.insert({type, read_and_return<std::int64_t>(file)});
    }

    // read data buffers
    size_t nbDataB = 0;
    read(nbDataB, file);
    datasB.reserve(nbDataB);
    for(size_t idD = 0; idD < nbDataB; ++idD){
        auto type       = read_and_return<DCDataBufferType>(file);
        auto encoding   = read_and_return<DCCompressionMode>(file);
        auto iInfo      = datasB.insert({type, {encoding, BinaryBuffer()}});
        read_buffer(std::get<1>(iInfo.first->second), file);
    }

    // read image buffers
    size_t nbImageB = 0;
    read(nbImageB, file);
    imagesB.reserve(nbImageB);
    for(size_t idID = 0; idID < nbImageB; ++idID){
        auto type       = read_and_return<DCImageBufferType>(file);
        auto encoding   = read_and_return<DCCompressionMode>(file);
        auto iInfo      = imagesB.insert({type, {encoding, BinaryImageBuffer()}});
        read_buffer(std::get<1>(iInfo.first->second), file);
    }

    // read volume buffers
    size_t nbVolumeB = 0;
    read(nbVolumeB, file);
    volumesB.reserve(nbVolumeB);
    for(size_t idID = 0; idID < nbVolumeB; ++idID){        
        auto type       = read_and_return<DCVolumeBufferType>(file);
        auto encoding   = read_and_return<DCCompressionMode>(file);
        // Log::message(std::format("Read file volume [{}] [{}] [{}]", idID, (int)type, (int)encoding));
        auto iInfo      = volumesB.insert({type, {encoding, BinaryImageBuffer()}});
        read_buffer(std::get<1>(iInfo.first->second), file);
    }
}

auto DCDataFrame::init_from_file_stream_legacy3(std::ifstream &file) -> void{

    // Log::message("LEGACY3\n");

    Frame::init_from_file_stream(file);

    // infos
    read(mode, file);
    read(validVerticesCount, file);

    {
        BinaryImageBuffer buffer;
        read_buffer(buffer, file);
        if(!buffer.empty()){
            imagesB.insert({DCImageBufferType::OriginalColorRGBA8, {DCCompressionMode::JPEG, std::move(buffer)}});
        }
    }

    {
        BinaryImageBuffer buffer;
        read_buffer(buffer, file);
        if(!buffer.empty()){
            imagesB.insert({DCImageBufferType::DepthSizedColorRGBA8, {DCCompressionMode::JPEG, std::move(buffer)}});
        }
    }

    {
        BinaryImageBuffer buffer;
        read_buffer(buffer, file);
        if(!buffer.empty()){
            imagesB.insert({DCImageBufferType::BodiesIdMap8, {DCCompressionMode::JPEG, std::move(buffer)}});
        }
    }

    {
        BinaryImageBuffer buffer;
        read_buffer(buffer, file);
        if(!buffer.empty()){
            imagesB.insert({DCImageBufferType::Depth16, {DCCompressionMode::FastPFor, std::move(buffer)}});
        }
    }

    {
        BinaryImageBuffer buffer;
        read_buffer(buffer, file);
        if(!buffer.empty()){
            imagesB.insert({DCImageBufferType::Infrared16, {DCCompressionMode::FastPFor, std::move(buffer)}});
        }
    }

    {
        BinaryBuffer buffer;
        read_buffer(buffer, file);
        if(!buffer.empty()){
            volumesB.insert({DCVolumeBufferType::CloudXYZ16RGB8, {DCCompressionMode::FastPFor, std::move(buffer)}});
        }
    }

    {
        BinaryBuffer buffer;
        read_buffer(buffer, file);
        if(!buffer.empty()){
            datasB.insert({DCDataBufferType::Calibration, {DCCompressionMode::None, std::move(buffer)}});
        }
    }

    {
        BinaryBuffer buffer;
        read_buffer(buffer, file);
        if(!buffer.empty()){
            datasB.insert({DCDataBufferType::IMU, {DCCompressionMode::None, std::move(buffer)}});
        }
    }


    // Log::message(std::format("{} : {} {} {} {} {} {} {} {}\n",
    //     validVerticesCount,
    //     image_buffer_size(DCImageBufferType::OriginalColorRGBA8),
    //     image_buffer_size(DCImageBufferType::DepthSizedColorRGBA8),
    //     image_buffer_size(DCImageBufferType::BodiesIdMap8),
    //     image_buffer_size(DCImageBufferType::Depth16),
    //     image_buffer_size(DCImageBufferType::Infrared16),
    //     volume_buffer_size(DCVolumeBufferType::CloudXYZ16RGB8),
    //     data_buffer_size(DCDataBufferType::Calibration),
    //     data_buffer_size(DCDataBufferType::IMU)
    // ));
    // Log::message("END\n");
}

auto DCDataFrame::init_from_file_stream_legacy2(std::ifstream &file) -> void{

    // Log::message("LEGACY2\n");

    read(afterCaptureTS, file);
    read(mode, file);

    {
        k4a_calibration_t rCalibration;
        read(rCalibration, file);

        BinaryBuffer buffer;
        buffer.resize(sizeof(k4a_calibration_t));
        auto ptr = reinterpret_cast<std::byte*>(&rCalibration);
        std::copy(ptr, ptr +  buffer.size(), buffer.begin());
        datasB.insert({DCDataBufferType::IMU, {DCCompressionMode::None, std::move(buffer)}});
    }

    std::int32_t validVerticesC;
    read(validVerticesC, file);
    validVerticesCount = validVerticesC;

    {
        std::int16_t colorW, colorH;
        std::int32_t colorBufferSize;
        read(colorW, file);
        read(colorH, file);
        read(colorBufferSize, file);

        BinaryImageBuffer buffer;
        buffer.resize_image(colorW, colorH, 4);

        read_array(buffer.get_data(), file, colorBufferSize);
    }
    // ...

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


auto DCDataFrame::write_to_file_stream(std::ofstream &file) -> void{

    // frame
    Frame::write_to_file_stream(file);

    // infos
    write(mode, file);
    write(validVerticesCount, file);

    // info buffers
    write(infosB.size(), file);
    for(const auto &infoB : infosB){
        write(infoB.first,  file);
        write(infoB.second, file);
    }

    // data buffers
    write(datasB.size(), file);
    for(const auto &dataB : datasB){
        write(dataB.first,  file);
        write(std::get<0>(dataB.second),        file);
        write_buffer(std::get<1>(dataB.second), file);
    }

    // image buffers
    write(imagesB.size(), file);
    for(const auto &imageB : imagesB){
        write(imageB.first,  file);
        write(std::get<0>(imageB.second),        file);
        write_buffer(std::get<1>(imageB.second), file);
    }

    // volume buffers
    write(volumesB.size(), file);
    for(const auto &volumeB : volumesB){
        write(volumeB.first,  file);
        write(std::get<0>(volumeB.second),        file);
        write_buffer(std::get<1>(volumeB.second), file);
    }
}

auto DCDataFrame::write_to_data(std::span<std::byte> data, size_t &offset) -> void{

    // frame
    Frame::write_to_data(data, offset);

    // infos
    write(mode, data, offset);
    write(validVerticesCount, data, offset);

    // info buffers
    write(infosB.size(), data, offset);
    for(const auto &infoB : infosB){
        write(infoB.first,  data, offset);
        write(infoB.second, data, offset);
    }

    // data buffers
    write(datasB.size(), data, offset);
    for(const auto &dataB : datasB){
        write(dataB.first,  data, offset);
        write(std::get<0>(dataB.second),        data, offset);
        write_buffer(std::get<1>(dataB.second), data, offset);
    }

    // image buffers
    write(imagesB.size(), data, offset);
    for(const auto &imageB : imagesB){
        write(imageB.first,  data, offset);
        write(std::get<0>(imageB.second),        data, offset);
        write_buffer(std::get<1>(imageB.second), data, offset);
    }

    // volume buffers
    write(volumesB.size(), data, offset);
    for(const auto &volumeB : volumesB){
        write(volumeB.first,  data, offset);
        write(std::get<0>(volumeB.second),        data, offset);
        write_buffer(std::get<1>(volumeB.second), data, offset);
    }
}

auto DCDataFrame::data_size() const noexcept -> size_t{

    size_t sizeData = Frame::data_size();
    sizeData += sizeof(mode);
    sizeData += sizeof(validVerticesCount);

    sizeData += sizeof(size_t); // nb infosB
    sizeData += infosB.size() * (sizeof(DCInfoType) + sizeof(std::int64_t));

    sizeData += sizeof(size_t); // nb dataB
    for(const auto &dataB : datasB){
        sizeData += sizeof(DCDataBufferType) + sizeof(DCCompressionMode) + sizeof(size_t) + std::get<1>(dataB.second).size();
    }

    sizeData += sizeof(size_t); // nb imageB
    for(const auto &imageB : imagesB){
        sizeData += sizeof(size_t) + sizeof(DCImageBufferType) + sizeof(DCCompressionMode) + (3*sizeof(size_t)) + std::get<1>(imageB.second).size();
    }

    sizeData += sizeof(size_t); // nb volumeB
    for(const auto &volumeB : volumesB){
        sizeData += sizeof(size_t) + sizeof(DCVolumeBufferType) + sizeof(DCCompressionMode) + sizeof(size_t) + std::get<1>(volumeB.second).size();
    }

    return sizeData;
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
        buffer_bytes_size(jpegG8BodiesIdMap) +
        // lossless std::uint16_t encoding
        buffer_bytes_size(fpfDepth) +
        buffer_bytes_size(fpfInfra) +
        buffer_bytes_size(fpfColoredCloud) +
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
    read_buffer(jpegG8BodiesIdMap, file);
    // lossless std::uint16_t encoding
    read_buffer(fpfDepth, file);
    read_buffer(fpfInfra, file);
    read_buffer(fpfColoredCloud, file);
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



auto DCCompressedFrame::init_from_data(std::span<const std::byte> data, size_t &offset) -> void{

    // frame
    Frame::init_from_data( data, offset);
    // infos
    read(mode, data,  offset);
    read(validVerticesCount, data, offset);
    // jpeg encoding
    read_buffer(jpegRGBA8Color, data, offset);
    read_buffer(jpegRGBA8DepthSizedColor, data, offset);
    read_buffer(jpegG8BodiesIdMap, data, offset);
    // lossless std::uint16_t encoding
    read_buffer(fpfDepth, data, offset);
    read_buffer(fpfInfra, data, offset);
    read_buffer(fpfColoredCloud, data, offset);
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
    write_buffer(jpegG8BodiesIdMap, file);
    // lossless std::uint16_t encoding
    write_buffer(fpfDepth, file);
    write_buffer(fpfInfra, file);
    write_buffer(fpfColoredCloud, file);
    // binary encoding
    write_buffer(calibration, file);
    write_buffer(imu, file);

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

auto DCCompressedFrame::write_to_data(std::span<std::byte> data, size_t &offset) -> void{

    // frame
    Frame::write_to_data(data, offset);
    // infos
    write(mode, data, offset);
    write(validVerticesCount, data, offset);
    // jpeg encoding
    write_buffer(jpegRGBA8Color, data, offset);
    write_buffer(jpegRGBA8DepthSizedColor, data, offset);
    write_buffer(jpegG8BodiesIdMap, data, offset);
    // lossless std::uint16_t encoding
    write_buffer(fpfDepth, data, offset);
    write_buffer(fpfInfra, data, offset);
    write_buffer(fpfColoredCloud, data, offset);
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
    static_cast<void>(file);
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
    static_cast<void>(file);
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



// auto DCUFrame::init_from_file_stream(std::ifstream &file) -> void{

// }


// auto DCUFrame::write_to_file_stream(std::ofstream &file) -> void{

// }

// auto DCUFrame::write_to_data(std::span<std::byte> data, size_t &offset) -> void{

// }


// auto DCUFrame::init_from_data(std::span<const std::byte> data, size_t &offset) -> void{

//     // frame
//     Frame::init_from_data( data, offset);

//     // read info buffers
//     size_t nbInfosB = 0;
//     read(nbInfosB, data, offset);
//     infosB.reserve(nbInfosB);
//     for(size_t idI = 0; idI < nbInfosB; ++idI){
//         auto type = read_and_return<DCInfoType>(data, offset);
//         infosB.insert({type, read_and_return<std::int64_t>(data, offset)});
//     }

//     // read data buffers
//     size_t nbDataB = 0;
//     read(nbDataB, data, offset);
//     datasB.reserve(nbDataB);
//     for(size_t idD = 0; idD < nbDataB; ++idD){
//         auto type       = read_and_return<DCDataBufferType>(data, offset);
//         auto encoding   = read_and_return<DCCompressionMode>(data, offset);
//         // auto iInfo      = datasB.insert({type, {encoding, BinaryBuffer()}});
//         // read_buffer(std::get<1>(iInfo.first->second), data, offset);
//     }
// }
