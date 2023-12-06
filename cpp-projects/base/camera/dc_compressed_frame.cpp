


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

// local
#include "utility/io_data.hpp"
#include "utility/io_fstream.hpp"

using namespace tool::camera;


struct DCCompressedFrame::Impl{
    // kinect4 types
    std::optional<k4a_calibration_t> k4Calibration = std::nullopt;
    // orbbec types
    // ...
};

DCCompressedFrame::DCCompressedFrame() : i(std::make_unique<Impl>()){
}

DCCompressedFrame::~DCCompressedFrame(){}

auto DCCompressedFrame::infos_size() const noexcept -> size_t {
    return
        sizeof(std::int32_t) + // id capture
        sizeof(std::int64_t) + // after capture TS
        sizeof(DCMode);
}
auto DCCompressedFrame::color_size()  const noexcept -> size_t{
    return encodedColorData.size() +
        sizeof(size_t) * 3; // colorWidth, colorHeight, encoded size
}

auto DCCompressedFrame::depth_size()  const noexcept -> size_t{
    return encodedDepthData.size() +
        sizeof(size_t) * 3; // depthWidth, depthHeight, encoded size
}
auto DCCompressedFrame::infra_size()  const noexcept -> size_t{
    return encodedInfraData.size() +
        sizeof(size_t) * 3; // infraWidth, infraHeight, encoded size
}

auto DCCompressedFrame::cloud_vertices_size() const noexcept -> size_t{
    return encodedCloudVerticesData.size() +
        sizeof(size_t)*2; // validVerticesCount, encoded size
}
auto DCCompressedFrame::cloud_color_size()  const noexcept -> size_t{
    return encodedCloudColorData.size() +
        sizeof(size_t) * 3; // cloudColorWidth, cloudColorHeight, encoded size
}

auto DCCompressedFrame::calibration_size() const noexcept -> size_t{
    if(i->k4Calibration.has_value()){
        return sizeof(k4a_calibration_t) + sizeof(bool);
    }
    return sizeof(bool);
}

auto DCCompressedFrame::has_calibration() const noexcept -> bool {
    if(i->k4Calibration.has_value()){
        return true;
    }
    return false;
}

auto DCCompressedFrame::init_from_file_stream(std::ifstream &file) -> void{
    read_infos_from_file_stream(file);
    read_color_from_file_stream(file);
    read_depth_from_file_stream(file);
    read_infra_from_file_stream(file);
    read_cloud_from_file_stream(file);
    read_calibration_from_file_stream(file);
    read_imu_from_file_stream(file);
    read_audio_from_file_stream(file);
    read_bodies_from_file_stream(file);
}

auto DCCompressedFrame::write_to_file_stream(std::ofstream &file) -> void{
    write_infos_to_file_stream(file);
    write_color_to_file_stream(file);
    write_depth_to_file_stream(file);
    write_infra_to_file_stream(file);
    write_cloud_to_file_stream(file);
    write_calibration_to_file_stream(file);
    write_imu_to_file_stream(file);
    write_audio_to_file_stream(file);
    write_bodies_to_file_stream(file);
}

auto DCCompressedFrame::init_from_data(int8_t *data) -> void{

    size_t offset = 0;
    init_infos_from_data(data, offset);
    init_color_from_data(data, offset);
    init_depth_from_data(data, offset);
    init_infra_from_data(data, offset);
    init_cloud_from_data(data, offset);
    init_calibration_from_data(data, offset);
    init_imu_from_data(data, offset);
    init_audio_from_data(data, offset);
    init_bodies_from_data(data, offset);
}

auto DCCompressedFrame::write_to_data(std::vector<int8_t> &data) -> size_t{

    auto totalDataSize = total_data_size();
    if(data.size() < totalDataSize){
        data.resize(totalDataSize);
    }

    size_t offset = 0;
    auto dataP = data.data();
    write_infos_to_data(dataP, offset);
    write_color_to_data(dataP, offset);
    write_depth_to_data(dataP, offset);
    write_infra_to_data(dataP, offset);
    write_cloud_to_data(dataP, offset);
    write_calibration_to_data(dataP, offset);
    write_imu_to_data(dataP, offset);
    write_audio_to_data(dataP, offset);
    write_bodies_to_data(dataP, offset);
    return totalDataSize;
}

auto DCCompressedFrame::write_calibration_to_data(int8_t *data) -> void{
    if(i->k4Calibration.has_value()){
        size_t offset = 0;
        write(i->k4Calibration.value(), data, offset);
    }
}

auto DCCompressedFrame::update_calibration_from_data(int8_t *data) -> void{
    if(get_device(mode) == DCType::Kinect4){
        k4a_calibration_t rCalibration;
        size_t offset = 0;
        read(rCalibration, data, offset);
        i->k4Calibration = rCalibration;
    }
}

auto DCCompressedFrame::imu_sample_size() const noexcept -> size_t{
    return (imuSample.has_value() ? sizeof(DCImuSample) : 0) +
        sizeof(bool); // has IMU
}

auto DCCompressedFrame::audio_size() const noexcept      -> size_t{
    return audioFrames.size()*7*sizeof(float) +
        sizeof(size_t); // nb audio frames
}

auto DCCompressedFrame::bodies_size() const noexcept -> size_t{
    return sizeof(bool); // has body // TODO: ...
}

auto DCCompressedFrame::read_infos_from_file_stream(std::ifstream &file) -> void{
    read(idCapture, file);
    read(afterCaptureTS, file);
    read(mode, file);
}

auto DCCompressedFrame::read_color_from_file_stream(std::ifstream &file) -> void{
    size_t encColorDataSize = 0;
    read(colorWidth, file);
    read(colorHeight, file);
    read(encColorDataSize, file);
    if(encColorDataSize > 0){
        encodedColorData.resize(encColorDataSize);
        read_array(encodedColorData.data(), file, encColorDataSize);
    }
}

auto DCCompressedFrame::read_depth_from_file_stream(std::ifstream &file) -> void{
    size_t encDepthDataSize = 0;
    read(depthWidth, file);
    read(depthHeight, file);
    read(encDepthDataSize, file);
    if(encDepthDataSize > 0){
        encodedDepthData.resize(encDepthDataSize);
        read_array(encodedDepthData.data(), file, encDepthDataSize);
    }
}

auto DCCompressedFrame::read_infra_from_file_stream(std::ifstream &file) -> void{
    size_t encInfraDataSize = 0;
    read(infraWidth, file);
    read(infraHeight, file);
    read(encInfraDataSize, file);
    if(encInfraDataSize > 0){
        encodedInfraData.resize(encInfraDataSize);
        read_array(encodedInfraData.data(), file, encInfraDataSize);
    }
}

auto DCCompressedFrame::read_cloud_from_file_stream(std::ifstream &file) -> void{

    read(validVerticesCount, file);

    // vertices
    size_t encCloudVertexDataSize = 0;
    size_t encCloudColorDataSize = 0;
    read(encCloudVertexDataSize, file);
    if(encCloudVertexDataSize > 0){
        encodedCloudVerticesData.resize(encCloudVertexDataSize);
        read_array(encodedCloudVerticesData.data(), file, encCloudVertexDataSize);
    }
    // colors
    read(cloudColorWidth, file);
    read(cloudColorHeight, file);
    read(encCloudColorDataSize, file);
    if(encCloudColorDataSize > 0){
        encodedCloudColorData.resize(encCloudColorDataSize);
        read_array(encodedCloudColorData.data(), file, encCloudColorDataSize);
    }
}

auto DCCompressedFrame::read_calibration_from_file_stream(std::ifstream &file) -> void{

    bool hasCalibration = false;
    read(hasCalibration, file);
    if(hasCalibration){
        if(get_device(mode) == DCType::Kinect4){
            k4a_calibration_t rCalibration;
            read(rCalibration, file);
            i->k4Calibration = rCalibration;
        }
    }
}

auto DCCompressedFrame::read_imu_from_file_stream(std::ifstream &file) -> void{
    bool hasIMU = false;
    read(hasIMU, file);
    if(hasIMU){
        DCImuSample rImuSample;
        read(rImuSample, file);
        imuSample = rImuSample;
    }
}

auto DCCompressedFrame::read_audio_from_file_stream(std::ifstream &file) -> void{
    size_t nbAudioFrames = 0;
    read(nbAudioFrames, file);
    if(nbAudioFrames > 0){
        audioFrames.resize(nbAudioFrames);
        read_array(audioFrames.data()->data(), file, nbAudioFrames*7);
    }
}

auto DCCompressedFrame::read_bodies_from_file_stream(std::ifstream &file) -> void{

    bool hasBodies = false;
    read(hasBodies, file);
    if(hasBodies){
        // TODO: ...
    }
}

auto DCCompressedFrame::write_infos_to_file_stream(std::ofstream &file) -> void{
    write(idCapture, file);
    write(afterCaptureTS, file);
    write(mode, file);
}

auto DCCompressedFrame::write_color_to_file_stream(std::ofstream &file) -> void{
    size_t encColorDataSize = encodedColorData.size();
    write(colorWidth, file);
    write(colorHeight, file);
    write(encColorDataSize, file);
    if(encColorDataSize > 0){
        write_array(encodedColorData.data(), file, encColorDataSize);
    }
}

auto DCCompressedFrame::write_depth_to_file_stream(std::ofstream &file) -> void{
    size_t encDepthDataSize = encodedDepthData.size();
    write(depthWidth, file);
    write(depthHeight, file);
    write(encDepthDataSize, file);
    if(encDepthDataSize > 0){
        write_array(encodedDepthData.data(), file, encDepthDataSize);
    }
}

auto DCCompressedFrame::write_infra_to_file_stream(std::ofstream &file) -> void{
    size_t encInfraDataSize = encodedInfraData.size();
    write(infraWidth, file);
    write(infraHeight, file);
    write(encInfraDataSize, file);
    if(encInfraDataSize > 0){
        write_array(encodedInfraData.data(), file, encInfraDataSize);
    }
}

auto DCCompressedFrame::write_cloud_to_file_stream(std::ofstream &file) -> void{

    write(validVerticesCount, file);

    size_t encCloudVertexDataSize = encodedCloudVerticesData.size();
    size_t encCloudColorDataSize = encodedCloudColorData.size();

    // vertices
    write(encCloudVertexDataSize, file);
    if(encCloudVertexDataSize > 0){
        write_array(encodedCloudVerticesData.data(), file, encCloudVertexDataSize);
    }

    // colors
    write(cloudColorWidth, file);
    write(cloudColorHeight, file);
    write(encCloudColorDataSize, file);
    if(encCloudColorDataSize > 0){
        write_array(encodedCloudColorData.data(), file, encCloudColorDataSize);
    }
}

auto DCCompressedFrame::write_calibration_to_file_stream(std::ofstream &file) -> void{
    if(i->k4Calibration.has_value()){
        write(true, file);
        write(i->k4Calibration.value(), file);
    }else{
        write(false, file);
    }
}

auto DCCompressedFrame::write_imu_to_file_stream(std::ofstream &file) -> void{
    write(imuSample.has_value(), file);
    if(imuSample.has_value()){
        write(imuSample.value(), file);
    }
}

auto DCCompressedFrame::write_audio_to_file_stream(std::ofstream &file) -> void{

    size_t nbAudioFrames    = audioFrames.size();
    write(nbAudioFrames, file);
    if(nbAudioFrames > 0){
        write_array(audioFrames.data()->data(), file, nbAudioFrames*7);
    }
}

auto DCCompressedFrame::write_bodies_to_file_stream(std::ofstream &file) -> void{
    bool hasBodies = false;
    write(hasBodies, file);
    // TODO: ...
}

auto DCCompressedFrame::init_infos_from_data(int8_t *data, size_t &offset) -> void{
    read(idCapture, data, offset);
    read(afterCaptureTS, data, offset);
    read(mode, data, offset);
}

auto DCCompressedFrame::init_color_from_data(int8_t *data, size_t &offset) -> void{
    size_t encColorDataSize = 0;
    read(colorWidth, data, offset);
    read(colorHeight, data, offset);
    read(encColorDataSize, data, offset);
    if(encColorDataSize > 0){
        encodedColorData.resize(encColorDataSize);
        read_array(encodedColorData.data(), data, encColorDataSize, offset);
    }
}

auto DCCompressedFrame::init_depth_from_data(int8_t *data, size_t &offset) -> void{
    size_t encDepthDataSize = 0;
    read(depthWidth, data, offset);
    read(depthHeight, data, offset);
    read(encDepthDataSize, data, offset);
    if(encDepthDataSize > 0){
        encodedDepthData.resize(encDepthDataSize);
        read_array(encodedDepthData.data(), data, encDepthDataSize, offset);
    }
}

auto DCCompressedFrame::init_infra_from_data(int8_t *data, size_t &offset) -> void{
    size_t encInfraDataSize = 0;
    read(infraWidth, data, offset);
    read(infraHeight, data, offset);
    read(encInfraDataSize, data, offset);
    if(encInfraDataSize > 0){
        encodedInfraData.resize(encInfraDataSize);
        read_array(encodedInfraData.data(), data, encInfraDataSize, offset);
    }
}

auto DCCompressedFrame::init_cloud_from_data(int8_t *data, size_t &offset) -> void{

    read(validVerticesCount, data, offset);

    size_t encCloudVertexDataSize = 0;
    size_t encCloudColorDataSize = 0;
    // vertices
    read(encCloudVertexDataSize, data, offset);
    if(encCloudVertexDataSize > 0){
        encodedCloudVerticesData.resize(encCloudVertexDataSize);
        read_array(encodedCloudVerticesData.data(), data, encCloudVertexDataSize, offset);
    }
    // colors
    read(cloudColorWidth, data, offset);
    read(cloudColorHeight, data, offset);
    read(encCloudColorDataSize, data, offset);
    if(encCloudColorDataSize > 0){
        encodedCloudColorData.resize(encCloudColorDataSize);
        read_array(encodedCloudColorData.data(), data, encCloudColorDataSize, offset);
    }
}

auto DCCompressedFrame::init_calibration_from_data(int8_t *data, size_t &offset) -> void{
    bool hasCalibration = false;
    read(hasCalibration, data, offset);
    if(hasCalibration){
        if(get_device(mode) == DCType::Kinect4){
            k4a_calibration_t rCalibration;
            read(rCalibration, data, offset);
            i->k4Calibration = rCalibration;
        }
    }
}

auto DCCompressedFrame::init_imu_from_data(int8_t *data, size_t &offset) -> void{

    bool hasIMU = false;
    read(hasIMU, data, offset);
    if(hasIMU){
        DCImuSample rImuSample;
        read(rImuSample, data, offset);
        imuSample = rImuSample;
    }
}

auto DCCompressedFrame::init_audio_from_data(int8_t *data, size_t &offset) -> void{
    size_t nbAudioFrames    = 0;
    read(nbAudioFrames, data, offset);
    if(nbAudioFrames > 0){
        audioFrames.resize(nbAudioFrames);
        read_array(audioFrames.data()->data(), data, nbAudioFrames*7, offset);
    }
}

auto DCCompressedFrame::init_bodies_from_data(int8_t *data, size_t &offset) -> void{
    bool hasBodies = false;
    read(hasBodies, data, offset);
    if(hasBodies){
        // TODO: ...
    }
}

auto DCCompressedFrame::write_infos_to_data(int8_t *data, size_t &offset) -> void{
    write(idCapture, data, offset);
    write(afterCaptureTS, data, offset);
    write(mode, data, offset);
}

auto DCCompressedFrame::write_color_to_data(int8_t *data, size_t &offset) -> void{
    size_t encColorDataSize = encodedColorData.size();
    write(colorWidth, data, offset);
    write(colorHeight, data, offset);
    write(encColorDataSize, data, offset);
    if(encColorDataSize > 0){
        write_array(encodedColorData.data(), data, encColorDataSize, offset);
    }
}

auto DCCompressedFrame::write_depth_to_data(int8_t *data, size_t &offset) -> void{
    size_t encDepthDataSize = encodedDepthData.size();
    write(depthWidth, data, offset);
    write(depthHeight, data, offset);
    write(encDepthDataSize, data, offset);
    if(encDepthDataSize > 0){
        write_array(encodedDepthData.data(), data, encDepthDataSize, offset);
    }
}

auto DCCompressedFrame::write_infra_to_data(int8_t *data, size_t &offset) -> void{
    size_t encInfraDataSize = encodedInfraData.size();
    write(infraWidth, data, offset);
    write(infraHeight, data, offset);
    write(encInfraDataSize, data, offset);
    if(encInfraDataSize > 0){
        write_array(encodedInfraData.data(), data, encInfraDataSize, offset);
    }
}

auto DCCompressedFrame::write_cloud_to_data(int8_t *data, size_t &offset) -> void{

    write(validVerticesCount, data, offset);

    size_t encCloudVertexDataSize = encodedCloudVerticesData.size();
    size_t encCloudColorDataSize = encodedCloudColorData.size();

    // vertices
    write(encCloudVertexDataSize, data, offset);
    if(encCloudVertexDataSize > 0){
        write_array(encodedCloudVerticesData.data(), data, encCloudVertexDataSize, offset);
    }
    // colors
    write(cloudColorWidth, data, offset);
    write(cloudColorHeight, data, offset);
    write(encCloudColorDataSize, data, offset);
    if(encCloudColorDataSize > 0){
        write_array(encodedCloudColorData.data(), data, encCloudColorDataSize, offset);
    }
}

auto DCCompressedFrame::write_calibration_to_data(int8_t *data, size_t &offset) -> void{
    if(i->k4Calibration.has_value()){
        write(true, data, offset);
        write(i->k4Calibration.value(), data, offset);
    }else{
        write(false, data, offset);
    }
}

auto DCCompressedFrame::write_imu_to_data(int8_t *data, size_t &offset) -> void{
    write(imuSample.has_value(), data, offset);
    if(imuSample.has_value()){
        write(imuSample.value(), data, offset);
    }
}

auto DCCompressedFrame::write_audio_to_data(int8_t *data, size_t &offset) -> void{
    size_t nbAudioFrames    = audioFrames.size();
    write(nbAudioFrames, data, offset);
    if(nbAudioFrames > 0){
        write_array(audioFrames.data()->data(), data, nbAudioFrames*7, offset);
    }
}

auto DCCompressedFrame::write_bodies_to_data(int8_t *data, size_t &offset) -> void{
    write(false, data, offset);
    // TODO: ...
}

auto DCCompressedFrame::init_legacy_cloud_frame_from_file_stream(std::ifstream &file) -> void{

    // # read info
    read(idCapture, file);
    read(afterCaptureTS, file);

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
    encodedCloudColorData.resize(colorBufferSize);
    if(!encodedCloudColorData.empty()){
        read_array(encodedCloudColorData.data(), file, encodedCloudColorData.size());
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
