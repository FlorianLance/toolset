
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

using namespace tool::cam;


struct DCCompressedFrame::Impl{
    // kinect4 types
    std::optional<k4a_calibration_t> k4Calibration = std::nullopt;
    // orbbec types
    std::optional<OBCameraParam> obCalibration = std::nullopt;
};

DCCompressedFrame::DCCompressedFrame() : i(std::make_unique<Impl>()){
}

DCCompressedFrame::~DCCompressedFrame(){}

auto DCCompressedFrame::infos_size() const noexcept -> size_t {
    return
        sizeof(idCapture) +
        sizeof(afterCaptureTS) +
        sizeof(receivedTS) +
        sizeof(mode) +
        sizeof(validVerticesCount);
}

auto DCCompressedFrame::color_size()  const noexcept -> size_t{
    return
        encodedColorData.size() +
        sizeof(colorWidth) + sizeof(colorHeight) +
        sizeof(size_t); // encoded size value
}

auto DCCompressedFrame::depth_sized_color_size() const noexcept -> size_t{
    return
        encodedDepthSizedColorData.size() +
        sizeof(depthSizedColorWidth) + sizeof(depthSizedColorHeight) +
        sizeof(size_t); // encoded size value
}

auto DCCompressedFrame::depth_size()  const noexcept -> size_t{
    return
        encodedDepthData.size() +
        sizeof(depthWidth) + sizeof(depthHeight) +
        sizeof(size_t); // encoded size value
}

auto DCCompressedFrame::infra_size()  const noexcept -> size_t{
    return
        encodedInfraData.size() +
        sizeof(infraWidth) + sizeof(infraHeight) +
        sizeof(size_t); // encoded size value
}

auto DCCompressedFrame::cloud_size() const noexcept -> size_t{
    return
        encodedCloudVerticesData.size() +
        sizeof(size_t); // encoded size value
}

auto DCCompressedFrame::calibration_size() const noexcept -> size_t{
    if(i->k4Calibration.has_value()){
        return
            sizeof(k4a_calibration_t) +
            sizeof(bool); // has calibration
    }else if(i->obCalibration.has_value()){
        return
            sizeof(OBCameraParam) +
            sizeof(bool); // has calibration
    }
    return sizeof(bool); // has calibration
}

auto DCCompressedFrame::imu_sample_size() const noexcept -> size_t{
    return
        (imuSample.has_value() ? sizeof(DCImuSample) : 0) +
        sizeof(bool); // has IMU
}

auto DCCompressedFrame::audio_size() const noexcept -> size_t{
    return
        audioFrames.size()*7*sizeof(float) +
        sizeof(size_t); // nb audio frames
}

auto DCCompressedFrame::bodies_size() const noexcept -> size_t{
    return
        // ...
        sizeof(bool); // has body
}

auto DCCompressedFrame::total_data_size() const -> size_t{
    return
        infos_size() +
        color_size() + depth_sized_color_size() + depth_size() + infra_size() + cloud_size() +
        calibration_size() + imu_sample_size() + audio_size() + bodies_size();
}

auto DCCompressedFrame::has_calibration() const noexcept -> bool {
    return
        i->k4Calibration.has_value() ||
        i->obCalibration.has_value();
}

auto DCCompressedFrame::init_from_file_stream(std::ifstream &file) -> void{

    // infos
    read(idCapture, file);
    read(afterCaptureTS, file);
    receivedTS = afterCaptureTS;
    read(mode, file);

    // colors
    size_t encColorDataSize = 0;
    read(colorWidth, file);
    read(colorHeight, file);
    read(encColorDataSize, file);
    if(encColorDataSize > 0){
        encodedColorData.resize(encColorDataSize);
        read_array(encodedColorData.data(), file, encColorDataSize);
    }

    // depth
    size_t encDepthDataSize = 0;
    read(depthWidth, file);
    read(depthHeight, file);
    read(encDepthDataSize, file);
    if(encDepthDataSize > 0){
        encodedDepthData.resize(encDepthDataSize);
        read_array(encodedDepthData.data(), file, encDepthDataSize);
    }

    // infra
    size_t encInfraDataSize = 0;
    read(infraWidth, file);
    read(infraHeight, file);
    read(encInfraDataSize, file);
    if(encInfraDataSize > 0){
        encodedInfraData.resize(encInfraDataSize);
        read_array(encodedInfraData.data(), file, encInfraDataSize);
    }

    // cloud
    read(validVerticesCount, file);

    // # vertices
    size_t encCloudVertexDataSize = 0;
    size_t encCloudColorDataSize = 0;
    read(encCloudVertexDataSize, file);
    if(encCloudVertexDataSize > 0){
        encodedCloudVerticesData.resize(encCloudVertexDataSize);
        read_array(encodedCloudVerticesData.data(), file, encCloudVertexDataSize);
    }
    // # colors
    read(depthSizedColorWidth, file);
    read(depthSizedColorHeight, file);
    read(encCloudColorDataSize, file);
    if(encCloudColorDataSize > 0){
        encodedDepthSizedColorData.resize(encCloudColorDataSize);
        read_array(encodedDepthSizedColorData.data(), file, encCloudColorDataSize);
    }

    // calibration
    bool hasCalibration = false;
    read(hasCalibration, file);
    if(hasCalibration){
        if(dc_get_device(mode) == DCType::AzureKinect){
            k4a_calibration_t rCalibration;
            read(rCalibration, file);
            i->k4Calibration = rCalibration;
        }else if(dc_get_device(mode) == DCType::FemtoBolt){
            OBCameraParam rCalibration;
            read(rCalibration, file);
            i->obCalibration = rCalibration;
        }
    }

    // imu
    bool hasIMU = false;
    read(hasIMU, file);
    if(hasIMU){
        DCImuSample rImuSample;
        read(rImuSample, file);
        imuSample = rImuSample;
    }

    // audio
    size_t nbAudioFrames = 0;
    read(nbAudioFrames, file);
    if(nbAudioFrames > 0){
        audioFrames.resize(nbAudioFrames);
        read_array(audioFrames.data()->data(), file, nbAudioFrames*7);
    }

    // bodies
    bool hasBodies = false;
    read(hasBodies, file);
    if(hasBodies){
        // TODO: ...
    }
}

auto DCCompressedFrame::write_to_file_stream(std::ofstream &file) -> void{

    // infos
    write(idCapture, file);
    write(receivedTS, file);
    write(mode, file);

    // colors
    size_t encColorDataSize = encodedColorData.size();
    write(colorWidth, file);
    write(colorHeight, file);
    write(encColorDataSize, file);
    if(encColorDataSize > 0){
        write_array(encodedColorData.data(), file, encColorDataSize);
    }

    // depth
    size_t encDepthDataSize = encodedDepthData.size();
    write(depthWidth, file);
    write(depthHeight, file);
    write(encDepthDataSize, file);
    if(encDepthDataSize > 0){
        write_array(encodedDepthData.data(), file, encDepthDataSize);
    }

    // infra
    size_t encInfraDataSize = encodedInfraData.size();
    write(infraWidth, file);
    write(infraHeight, file);
    write(encInfraDataSize, file);
    if(encInfraDataSize > 0){
        write_array(encodedInfraData.data(), file, encInfraDataSize);
    }

    // cloud
    write(validVerticesCount, file);

    size_t encCloudVertexDataSize = encodedCloudVerticesData.size();
    size_t encCloudColorDataSize = encodedDepthSizedColorData.size();

    // # vertices
    write(encCloudVertexDataSize, file);
    if(encCloudVertexDataSize > 0){
        write_array(encodedCloudVerticesData.data(), file, encCloudVertexDataSize);
    }

    // # colors
    write(depthSizedColorWidth, file);
    write(depthSizedColorHeight, file);
    write(encCloudColorDataSize, file);
    if(encCloudColorDataSize > 0){
        write_array(encodedDepthSizedColorData.data(), file, encCloudColorDataSize);
    }

    // calibration
    if(i->k4Calibration.has_value()){
        write(true, file);
        write(i->k4Calibration.value(), file);
    }else if(i->obCalibration.has_value()){
        write(true, file);
        write(i->obCalibration.value(), file);
    }else{
        write(false, file);
    }

    // imu
    write(imuSample.has_value(), file);
    if(imuSample.has_value()){
        write(imuSample.value(), file);
    }

    // audio
    size_t nbAudioFrames    = audioFrames.size();
    write(nbAudioFrames, file);
    if(nbAudioFrames > 0){
        write_array(audioFrames.data()->data(), file, nbAudioFrames*7);
    }

    // bodies
    bool hasBodies = false;
    write(hasBodies, file);
    if(hasBodies){
        // TODO: ...
    }
}

auto DCCompressedFrame::init_from_data(std::int8_t const * const data, size_t &offset, size_t sizeData) -> void{

    // infos
    read(idCapture, data, offset, sizeData);
    read(afterCaptureTS, data, offset, sizeData);
    receivedTS = afterCaptureTS;
    read(mode, data, offset, sizeData);

    // colors
    size_t encColorDataSize = 0;
    read(colorWidth, data, offset, sizeData);
    read(colorHeight, data, offset, sizeData);
    read(encColorDataSize, data, offset, sizeData);
    if(encColorDataSize > 0){
        encodedColorData.resize(encColorDataSize);
        read_array(encodedColorData.data(), data, encColorDataSize, offset, sizeData);
    }

    // depth
    size_t encDepthDataSize = 0;
    read(depthWidth, data, offset, sizeData);
    read(depthHeight, data, offset, sizeData);
    read(encDepthDataSize, data, offset, sizeData);
    if(encDepthDataSize > 0){
        encodedDepthData.resize(encDepthDataSize);
        read_array(encodedDepthData.data(), data, encDepthDataSize, offset, sizeData);
    }

    // infra
    size_t encInfraDataSize = 0;
    read(infraWidth, data, offset, sizeData);
    read(infraHeight, data, offset, sizeData);
    read(encInfraDataSize, data, offset, sizeData);
    if(encInfraDataSize > 0){
        encodedInfraData.resize(encInfraDataSize);
        read_array(encodedInfraData.data(), data, encInfraDataSize, offset, sizeData);
    }

    // cloud
    read(validVerticesCount, data, offset, sizeData);

    size_t encCloudVertexDataSize = 0;
    size_t encCloudColorDataSize = 0;
    // # vertices
    read(encCloudVertexDataSize, data, offset, sizeData);
    if(encCloudVertexDataSize > 0){
        encodedCloudVerticesData.resize(encCloudVertexDataSize);
        read_array(encodedCloudVerticesData.data(), data, encCloudVertexDataSize, offset, sizeData);
    }
    // # colors
    read(depthSizedColorWidth, data, offset, sizeData);
    read(depthSizedColorHeight, data, offset, sizeData);
    read(encCloudColorDataSize, data, offset, sizeData);
    if(encCloudColorDataSize > 0){
        encodedDepthSizedColorData.resize(encCloudColorDataSize);
        read_array(encodedDepthSizedColorData.data(), data, encCloudColorDataSize, offset, sizeData);
    }

    // calibration
    bool hasCalibration = false;
    read(hasCalibration, data, offset, sizeData);
    if(hasCalibration){
        if(dc_get_device(mode) == DCType::AzureKinect){
            k4a_calibration_t rCalibration;
            read(rCalibration, data, offset, sizeData);
            i->k4Calibration = rCalibration;
        }else if(dc_get_device(mode) == DCType::FemtoBolt){
            OBCameraParam rCalibration;
            read(rCalibration, data, offset, sizeData);
            i->obCalibration = rCalibration;
        }
    }

    // imu
    bool hasIMU = false;
    read(hasIMU, data, offset, sizeData);
    if(hasIMU){
        DCImuSample rImuSample;
        read(rImuSample, data, offset, sizeData);
        imuSample = rImuSample;
    }

    // audio
    size_t nbAudioFrames    = 0;
    read(nbAudioFrames, data, offset, sizeData);
    if(nbAudioFrames > 0){
        audioFrames.resize(nbAudioFrames);
        read_array(audioFrames.data()->data(), data, nbAudioFrames*7, offset, sizeData);
    }

    // bodies
    bool hasBodies = false;
    read(hasBodies, data, offset, sizeData);
    if(hasBodies){
        // TODO: ...
    }

}

auto DCCompressedFrame::write_to_data(int8_t * const data, size_t &offset, size_t sizeData) -> void{

    // infos
    write(idCapture, data, offset, sizeData);
    write(receivedTS, data, offset, sizeData);
    write(mode, data, offset, sizeData);

    // color
    size_t encColorDataSize = encodedColorData.size();
    write(colorWidth, data, offset, sizeData);
    write(colorHeight, data, offset, sizeData);
    write(encColorDataSize, data, offset, sizeData);
    if(encColorDataSize > 0){
        write_array(encodedColorData.data(), data, encColorDataSize, offset, sizeData);
    }

    // depth
    size_t encDepthDataSize = encodedDepthData.size();
    write(depthWidth, data, offset, sizeData);
    write(depthHeight, data, offset, sizeData);
    write(encDepthDataSize, data, offset, sizeData);
    if(encDepthDataSize > 0){
        write_array(encodedDepthData.data(), data, encDepthDataSize, offset, sizeData);
    }

    // infra
    size_t encInfraDataSize = encodedInfraData.size();
    write(infraWidth, data, offset, sizeData);
    write(infraHeight, data, offset, sizeData);
    write(encInfraDataSize, data, offset, sizeData);
    if(encInfraDataSize > 0){
        write_array(encodedInfraData.data(), data, encInfraDataSize, offset, sizeData);
    }

    // cloud
    write(validVerticesCount, data, offset, sizeData);

    size_t encCloudVertexDataSize = encodedCloudVerticesData.size();
    size_t encCloudColorDataSize = encodedDepthSizedColorData.size();

    // # vertices
    write(encCloudVertexDataSize, data, offset, sizeData);
    if(encCloudVertexDataSize > 0){
        write_array(encodedCloudVerticesData.data(), data, encCloudVertexDataSize, offset, sizeData);
    }
    // # colors
    write(depthSizedColorWidth, data, offset, sizeData);
    write(depthSizedColorHeight, data, offset, sizeData);
    write(encCloudColorDataSize, data, offset, sizeData);
    if(encCloudColorDataSize > 0){
        write_array(encodedDepthSizedColorData.data(), data, encCloudColorDataSize, offset, sizeData);
    }

    // calibration
    if(i->k4Calibration.has_value()){
        write(true, data, offset, sizeData);
        write(i->k4Calibration.value(), data, offset, sizeData);
    }else if(i->obCalibration.has_value()){
        write(true, data, offset, sizeData);
        write(i->obCalibration.value(), data, offset, sizeData);
    }else{
        write(false, data, offset, sizeData);
    }

    // imu
    write(imuSample.has_value(), data, offset, sizeData);
    if(imuSample.has_value()){
        write(imuSample.value(), data, offset, sizeData);
    }

    // audio
    size_t nbAudioFrames    = audioFrames.size();
    write(nbAudioFrames, data, offset, sizeData);
    if(nbAudioFrames > 0){
        write_array(audioFrames.data()->data(), data, nbAudioFrames*7, offset, sizeData);
    }

    // bodies
    write(false, data, offset, sizeData);
    // TODO: ...
}

auto DCCompressedFrame::write_calibration_content_to_data(int8_t * const data, size_t &offset, size_t sizeData) -> void{
    if(i->k4Calibration.has_value()){
        write(i->k4Calibration.value(), data, offset, sizeData);
    }else if(i->obCalibration.has_value()){
        write(i->obCalibration.value(), data, offset, sizeData);
    }
}

auto DCCompressedFrame::init_calibration_from_data(DCType type, std::int8_t const * const data, size_t &offset, size_t sizeData) -> void{
    if(type == DCType::AzureKinect){
        k4a_calibration_t rCalibration;
        read(rCalibration, data, offset, sizeData);
        i->k4Calibration = rCalibration;
    }else if(type == DCType::FemtoBolt){
        k4a_calibration_t rCalibration;
        read(rCalibration, data, offset, sizeData);
        i->k4Calibration = rCalibration;
    }else if(type == DCType::FemtoBolt){
        k4a_calibration_t rCalibration;
        read(rCalibration, data, offset, sizeData);
        i->k4Calibration = rCalibration;
    }

    // OBCameraParam rCalibration;
    // read(rCalibration, data, offset, sizeData);
    // i->obCalibration = rCalibration;
}

auto DCCompressedFrame::calibration_data_size() const noexcept -> size_t{
    if(dc_get_device(mode) == DCType::AzureKinect){
        return sizeof(k4a_calibration_t);
    }else if(dc_get_device(mode) == DCType::FemtoBolt){
        return sizeof(k4a_calibration_t);
    }else if(dc_get_device(mode) == DCType::FemtoMega){
        return sizeof(k4a_calibration_t);
    }
    return 0;

    // return sizeof(OBCameraParam);
}

auto DCCompressedFrame::init_legacy_cloud_frame_from_file_stream(std::ifstream &file) -> void{

    // # read info
    read(idCapture, file);
    read(afterCaptureTS, file);
    receivedTS = afterCaptureTS;

    // # read cloud
    std::int32_t validVerticesC;
    read(validVerticesC, file);
    validVerticesCount = validVerticesC;
    std::int32_t cloudBufferSize;
    read(cloudBufferSize, file);
    encodedCloudVerticesData.resize(cloudBufferSize);
    if(!encodedCloudVerticesData.empty()){
        read_array(encodedCloudVerticesData.data(), file, encodedCloudVerticesData.size());
    }

    // # read color
    std::int16_t colorW, colorH;
    std::int32_t colorBufferSize;
    read(colorW, file);
    read(colorH, file);
    read(colorBufferSize, file);
    colorWidth  = colorW;
    colorHeight = colorH;
    encodedDepthSizedColorData.resize(colorBufferSize);
    if(!encodedDepthSizedColorData.empty()){
        read_array(encodedDepthSizedColorData.data(), file, encodedDepthSizedColorData.size());
    }

    // # read audio
    std::int32_t audioBufferSize;
    read(audioBufferSize, file);

    audioFrames.resize(audioBufferSize);
    if(!audioFrames.empty()){
        read_array(reinterpret_cast<float*>(audioFrames.data()), file, audioBufferSize*7);
    }
    // # read imu
    read_array(file, reinterpret_cast<char*>(&imuSample), sizeof (DCImuSample));
}

auto DCCompressedFrame::init_legacy_full_frame_from_file_stream(std::ifstream &file) -> void{

    // # read info
    read(afterCaptureTS, file);
    receivedTS = afterCaptureTS;
    read(mode, file);

    k4a_calibration_t rCalibration;
    read(rCalibration, file);
    i->k4Calibration = rCalibration;
    std::int32_t validVerticesC;
    read(validVerticesC, file);
    validVerticesCount = validVerticesC;
    // # read color
    std::int16_t colorW, colorH;
    std::int32_t colorBufferSize;
    read(colorW, file);
    read(colorH, file);
    read(colorBufferSize, file);
    colorWidth  = colorW;
    colorHeight = colorH;
    encodedColorData.resize(colorBufferSize);
    if(!encodedColorData.empty()){
        read_array(encodedColorData.data(), file, encodedColorData.size());
    }
    // # read depth
    std::int16_t depthW, depthH;
    std::int32_t depthBufferSize;
    read(depthW, file);
    read(depthH, file);
    read(depthBufferSize, file);
    depthWidth  = depthW;
    depthHeight = depthH;
    encodedDepthData.resize(depthBufferSize);
    if(!encodedDepthData.empty()){
        read_array(encodedDepthData.data(), file, encodedDepthData.size());
    }
    // # read infra
    std::int16_t infraW, infraH;
    std::int32_t infraBufferSize;
    read(infraW, file);
    read(infraH, file);
    read(infraBufferSize, file);
    infraWidth  = infraW;
    infraHeight = infraH;
    encodedInfraData.resize(infraBufferSize);
    if(!encodedInfraData.empty()){
        read_array(encodedInfraData.data(), file, encodedInfraData.size());
    }

    // # read audio
    std::int32_t audioBufferSize;
    read(audioBufferSize, file);
    audioFrames.resize(audioBufferSize);
    if(!audioFrames.empty()){
        read_array(reinterpret_cast<float*>(audioFrames.data()), file, audioBufferSize*7);
    }
    // # read imu
    read_array(file, reinterpret_cast<char*>(&imuSample), sizeof (DCImuSample));
}
