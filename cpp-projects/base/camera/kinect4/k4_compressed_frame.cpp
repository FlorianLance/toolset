

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

#include "k4_compressed_frame.hpp"

// local
#include "utility/io_data.hpp"
#include "utility/io_fstream.hpp"

#include "camera/kinect4/k4_types.hpp"

#include <iostream>

using namespace tool::camera;

auto K4CompressedFrame::init_from_file_stream(std::ifstream &file) -> void{

    size_t encColorDataSize = 0;
    size_t encDepthDataSize = 0;
    size_t encInfraDataSize = 0;
    size_t encCloudVertexDataSize = 0;
    size_t encCloudColorDataSize = 0;
    size_t nbAudioFrames    = 0;

    read_infos_from_file_stream(file);
    read_color_from_file_stream(file);


    // # depth
    read(depthWidth, file);
    read(depthHeight, file);
    read(encDepthDataSize, file);
    if(encDepthDataSize > 0){
        encodedDepthData.resize(encDepthDataSize);
        read_array(encodedDepthData.data(), file, encDepthDataSize);
    }
    // # infra
    read(infraWidth, file);
    read(infraHeight, file);
    read(encInfraDataSize, file);
    if(encInfraDataSize > 0){
        encodedInfraData.resize(encInfraDataSize);
        read_array(encodedInfraData.data(), file, encInfraDataSize);
    }

    // # cloud
    // ## vertex
    read(validVerticesCount, file);
    read(encCloudVertexDataSize, file);
    if(encCloudVertexDataSize > 0){
        encodedCloudVerticesData.resize(encCloudVertexDataSize);
        read_array(encodedCloudVerticesData.data(), file, encCloudVertexDataSize);
    }
    // ## color
    read(cloudColorWidth, file);
    read(cloudColorHeight, file);
    read(encCloudColorDataSize, file);
    if(encCloudColorDataSize > 0){
        encodedCloudColorData.resize(encCloudColorDataSize);
        read_array(encodedCloudColorData.data(), file, encCloudColorDataSize);
    }

    bool hasCalibration = false;
    read(hasCalibration, file);    
    if(hasCalibration){
        k4a_calibration_t rCalibration;
        read(rCalibration, file);
        calibration = rCalibration;
    }

    // # imu
    bool hasIMU = false;
    read(hasIMU, file);
    if(hasIMU){
        DCImuSample rImuSample;
        read(rImuSample, file);
        imuSample = rImuSample;
    }

    // # audio
    read(nbAudioFrames, file);
    if(nbAudioFrames > 0){
        audioFrames.resize(nbAudioFrames);
        read_array(audioFrames.data()->data(), file, nbAudioFrames*7);
    }

    // # bodies
    bool hasBodies = false;
    read(hasBodies, file);
    if(hasBodies){
        // TODO
    }
}

auto K4CompressedFrame::write_to_file_stream(std::ofstream &file) -> void{

    size_t encColorDataSize = encodedColorData.size();
    size_t encDepthDataSize = encodedDepthData.size();
    size_t encInfraDataSize = encodedInfraData.size();
    size_t encCloudVertexDataSize = encodedCloudVerticesData.size();
    size_t encCloudColorDataSize = encodedCloudColorData.size();
    size_t nbAudioFrames    = audioFrames.size();

    // write
    // # infos
    write(idCapture, file);
    write(afterCaptureTS, file);
    write(mode, file);

    // # color
    write(colorWidth, file);
    write(colorHeight, file);
    write(encColorDataSize, file);
    if(encColorDataSize > 0){
        write_array(encodedColorData.data(), file, encColorDataSize);
    }

    // # depth
    write(depthWidth, file);
    write(depthHeight, file);
    write(encDepthDataSize, file);
    if(encDepthDataSize > 0){
        write_array(encodedDepthData.data(), file, encDepthDataSize);
    }

    // # infra
    write(infraWidth, file);
    write(infraHeight, file);
    write(encInfraDataSize, file);
    if(encInfraDataSize > 0){
        write_array(encodedInfraData.data(), file, encInfraDataSize);
    }

    // # cloud
    // ## vertex
    write(validVerticesCount, file);
    write(encCloudVertexDataSize, file);
    if(encCloudVertexDataSize > 0){
        write_array(encodedCloudVerticesData.data(), file, encCloudVertexDataSize);
    }

    // ## color
    write(cloudColorWidth, file);
    write(cloudColorHeight, file);
    write(encCloudColorDataSize, file);
    if(encCloudColorDataSize > 0){
        write_array(encodedCloudColorData.data(), file, encCloudColorDataSize);
    }

    // ## calibration
    write(calibration.has_value(), file);
    if(calibration.has_value()){
        write(calibration.value(), file);
    }

    // # imu
    write(imuSample.has_value(), file);
    if(imuSample.has_value()){
        write(imuSample.value(), file);
    }

    // # audio
    write(nbAudioFrames, file);
    if(nbAudioFrames > 0){
        write_array(audioFrames.data()->data(), file, nbAudioFrames*7);
    }

    // # bodies
    bool hasBodies = false;
    write(hasBodies, file);
    // TODO
}


auto K4CompressedFrame::init_legacy_cloud_frame_file_stream(std::ifstream &file) -> void{

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
//    std::cout << colorBufferSize << " " << colorWidth << " " << colorHeight << " " << colorWidth*colorHeight << "\n";
//    encodedColorData.resize(colorBufferSize);
    encodedCloudColorData.resize(colorBufferSize);
    if(!encodedCloudColorData.empty()){
        read_array(encodedCloudColorData.data(), file, encodedCloudColorData.size());
//        read_array(encodedColorData.data(), file, encodedColorData.size());
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

auto K4CompressedFrame::init_legacy_full_frame_file_stream(std::ifstream &file) -> void{

    // # read info
    read(afterCaptureTS, file);
    read(mode, file);

    read(calibration, file);
    //read_array(reinterpret_cast<char*>(&calibration), file, sizeof (k4a_calibration_t));
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

auto K4CompressedFrame::init_from_data(int8_t *data) -> void{

    size_t offset = 0;
    size_t encColorDataSize = 0;
    size_t encDepthDataSize = 0;
    size_t encInfraDataSize = 0;
    size_t encCloudVertexDataSize = 0;
    size_t encCloudColorDataSize = 0;
    size_t nbAudioFrames    = 0;

    // init
    // # infos
    read(idCapture, data, offset);
    read(afterCaptureTS, data, offset);
    read(mode, data, offset);

    // # color
    read(colorWidth, data, offset);
    read(colorHeight, data, offset);
    read(encColorDataSize, data, offset);
    if(encColorDataSize > 0){
        encodedColorData.resize(encColorDataSize);
        read_array(encodedColorData.data(), data, encColorDataSize, offset);
    }

    // # depth
    read(depthWidth, data, offset);
    read(depthHeight, data, offset);
    read(encDepthDataSize, data, offset);
    if(encDepthDataSize > 0){
        encodedDepthData.resize(encDepthDataSize);
        read_array(encodedDepthData.data(), data, encDepthDataSize, offset);
    }
    // # infra
    read(infraWidth, data, offset);
    read(infraHeight, data, offset);
    read(encInfraDataSize, data, offset);
    if(encInfraDataSize > 0){
        encodedInfraData.resize(encInfraDataSize);
        read_array(encodedInfraData.data(), data, encInfraDataSize, offset);
    }

    // # cloud
    // ## vertex
    read(validVerticesCount, data, offset);
    read(encCloudVertexDataSize, data, offset);
    if(encCloudVertexDataSize > 0){
        encodedCloudVerticesData.resize(encCloudVertexDataSize);
        read_array(encodedCloudVerticesData.data(), data, encCloudVertexDataSize, offset);
    }
    // ## color
    read(cloudColorWidth, data, offset);
    read(cloudColorHeight, data, offset);
    read(encCloudColorDataSize, data, offset);
    if(encCloudColorDataSize > 0){
        encodedCloudColorData.resize(encCloudColorDataSize);
        read_array(encodedCloudColorData.data(), data, encCloudColorDataSize, offset);
    }

    // # calibration
    bool hasCalibration = false;
    read(hasCalibration, data, offset);
    if(hasCalibration){
        k4a_calibration_t rCalibration;
        read(rCalibration, data, offset);
        calibration = rCalibration;
    }

    // # imu
    bool hasIMU = false;
    read(hasIMU, data, offset);
    if(hasIMU){
        DCImuSample rImuSample;
        read(rImuSample, data, offset);
        imuSample = rImuSample;
    }

    // # audio
    read(nbAudioFrames, data, offset);
    if(nbAudioFrames > 0){
        audioFrames.resize(nbAudioFrames);
        read_array(audioFrames.data()->data(), data, nbAudioFrames*7, offset);
    }

    // # bodies
    bool hasBodies = false;
    read(hasBodies, data, offset);
    if(hasBodies){
        // TODO
    }
}


auto K4CompressedFrame::convert_to_data(std::vector<int8_t> &data) -> size_t{

    size_t encColorDataSize = encodedColorData.size();
    size_t encDepthDataSize = encodedDepthData.size();
    size_t encInfraDataSize = encodedInfraData.size();
    size_t encCloudVertexDataSize = encodedCloudVerticesData.size();
    size_t encCloudColorDataSize = encodedCloudColorData.size();
    size_t nbAudioFrames    = audioFrames.size();
    size_t totalDataSize    = total_data_size();

    size_t offset = 0;
    if(data.size() < totalDataSize){
        data.resize(totalDataSize);
    }

    auto dataP = data.data();

    // write
    // # infos
    write(idCapture, dataP, offset);
    write(afterCaptureTS, dataP, offset);
    write(mode, dataP, offset);

    // # color
    write(colorWidth, dataP, offset);
    write(colorHeight, dataP, offset);
    write(encColorDataSize, dataP, offset);
    if(encColorDataSize > 0){
        write_array(encodedColorData.data(), dataP, encColorDataSize, offset);
    }

    // # depth
    write(depthWidth, dataP, offset);
    write(depthHeight, dataP, offset);
    write(encDepthDataSize, dataP, offset);
    if(encDepthDataSize > 0){
        write_array(encodedDepthData.data(), dataP, encDepthDataSize, offset);
    }

    // # infra
    write(infraWidth, dataP, offset);
    write(infraHeight, dataP, offset);
    write(encInfraDataSize, dataP, offset);
    if(encInfraDataSize > 0){
        write_array(encodedInfraData.data(), dataP, encInfraDataSize, offset);
    }

    // # cloud
    // ## vertex
    write(validVerticesCount, dataP, offset);
    write(encCloudVertexDataSize, dataP, offset);
    if(encCloudVertexDataSize > 0){
        write_array(encodedCloudVerticesData.data(), dataP, encCloudVertexDataSize, offset);
    }
    // ## color
    write(cloudColorWidth, dataP, offset);
    write(cloudColorHeight, dataP, offset);
    write(encCloudColorDataSize, dataP, offset);
    if(encCloudColorDataSize > 0){
        write_array(encodedCloudColorData.data(), dataP, encCloudColorDataSize, offset);
    }

    // # calibration
    write(calibration.has_value(), dataP, offset);
    if(calibration.has_value()){
        write(calibration.value(), dataP, offset);
    }

    // # imu
    write(imuSample.has_value(), dataP, offset);
    if(imuSample.has_value()){
        write(imuSample.value(), dataP, offset);
    }

    // # audio
    write(nbAudioFrames, dataP, offset);
    if(nbAudioFrames > 0){
        write_array(audioFrames.data()->data(), dataP, nbAudioFrames*7, offset);
    }

    // # bodies
    write(false, dataP, offset);
    // TODO



    return totalDataSize;
}


