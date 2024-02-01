


/*******************************************************************************
** Toolset-base                                                               **
** MIT License                                                                **
** Copyright (c) [2024] [Florian Lance]                                       **
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

#include "femto_bolt_device_impl.hpp"

// local
#include "utility/benchmark.hpp"

using namespace tool;
using namespace tool::geo;
using namespace tool::cam;


FemtoBoltDeviceImpl::FemtoBoltDeviceImpl(){
    orbbecD  = std::make_unique<OrbbecBaseDevice>();
    orbbecD->query_devices("Femto Bolt"sv, false);
}

auto FemtoBoltDeviceImpl::initialize_device_specific() -> void{

    orbbecD->initialize(mInfos);

    // k4aBtConfig.gpu_device_id       = settings.config.btGPUId;
    // k4aBtConfig.processing_mode     = static_cast<k4abt_tracker_processing_mode_t>(settings.config.btProcessingMode);
    // k4aBtConfig.sensor_orientation  = static_cast<k4abt_sensor_orientation_t>(settings.config.btOrientation);
    // k4aBtConfig.model_path          = nullptr;
}

auto FemtoBoltDeviceImpl::update_camera_from_colors_settings() -> void{
    if(!is_opened()){
        return;
    }
    orbbecD->update_camera_from_colors_settings(readFramesFromCameras, settings.color);
}

auto FemtoBoltDeviceImpl::open(uint32_t deviceId) -> bool{
    return orbbecD->open_device(settings.config.idDevice = deviceId);
}

auto FemtoBoltDeviceImpl::start_reading(const DCConfigSettings &newConfigS) -> bool{

    settings.config = newConfigS;
    initialize();

    if(orbbecD->start_pipeline(mInfos, settings.config)){
        start_reading_thread();
        return true;
    }

    return false;
}

auto FemtoBoltDeviceImpl::stop_reading() -> void{
    stop_reading_thread();
}

auto FemtoBoltDeviceImpl::close() -> void{
    orbbecD->close_device();
}

auto FemtoBoltDeviceImpl::is_opened() const noexcept -> bool{
    return orbbecD->is_opened();
}

auto FemtoBoltDeviceImpl::nb_devices() const noexcept -> uint32_t{
    return static_cast<std::uint32_t>(orbbecD->nb_devices());
}

auto FemtoBoltDeviceImpl::device_name() const noexcept -> std::string{
    return orbbecD->device_name();
}

auto FemtoBoltDeviceImpl::capture_frame(int32_t timeoutMs) -> bool{
    return orbbecD->capture_frame(timeoutMs);
}

auto FemtoBoltDeviceImpl::read_color_image() -> bool{
    
    if(mInfos.has_color()){
        fData.rawColor = orbbecD->read_color_image();
    }else{
        fData.rawColor = {};
    }
    return !fData.rawColor.empty();
}

auto FemtoBoltDeviceImpl::read_depth_image() -> bool{
    
    if(mInfos.has_depth()){
        fData.depth = orbbecD->read_depth_image();
    }else{
        fData.depth = {};
    }
    return !fData.depth.empty();
}

auto FemtoBoltDeviceImpl::read_infra_image() -> bool{

    if(dc_has_infrared(settings.config.mode)){
        fData.infra = orbbecD->read_infra_image();
    }else{
        fData.infra = {};
    }
    return !fData.infra.empty();
}

auto FemtoBoltDeviceImpl::read_from_imu() -> void {
    // ...
}


auto FemtoBoltDeviceImpl::read_bodies() -> void{

    // if(!depthImage || ! infraredImage){
    //     return;
    // }

    // if((depth_resolution(settings.config.mode) != DCDepthResolution::K4_OFF) && settings.config.enableBodyTracking && (bodyTracker != nullptr)){


    //     try{

    //         k4a_capture_t captureH = nullptr;
    //         k4a_capture_create(&captureH);
    //         k4a::capture capture(captureH);

    //         // std::cout << "create d image\n";
    //         // std::cout << depthImage->width() << " " << depthImage->height() << " "<< depthImage->width()*depthImage->height() << " " << depthImage->dataSize() << "\n";
    //         k4a::image k4DepthImage = k4a::image::create_from_buffer(
    //             K4A_IMAGE_FORMAT_DEPTH16,
    //             depthImage->width(),
    //             depthImage->height(),
    //             depthImage->width()*sizeof(std::uint16_t),
    //             reinterpret_cast<std::uint8_t*>(depthImage.get()->data()),
    //             depthImage->dataSize(),
    //             nullptr,
    //             nullptr
    //         );

    //         // std::cout << "create ir image\n";
    //         // std::cout << infraredImage->width() << " " << infraredImage->height() << " "<< infraredImage->width()*infraredImage->height() << " " << infraredImage->dataSize() << "\n";
    //         k4a::image k4IRImage = k4a::image::create_from_buffer(
    //             K4A_IMAGE_FORMAT_DEPTH16,
    //             infraredImage->width(),
    //             infraredImage->height(),
    //             infraredImage->width()*sizeof(std::uint16_t),
    //             reinterpret_cast<std::uint8_t*>(infraredImage.get()->data()),
    //             infraredImage->dataSize(),
    //             nullptr,
    //             nullptr
    //         );

    //         // std::cout << "set depth image\n";
    //         capture.set_depth_image(k4DepthImage);

    //         // std::cout << "set ir image\n";
    //         capture.set_ir_image(k4IRImage);


    //         // std::cout << "enqueue capture\n";
    //         if(bodyTracker->enqueue_capture(capture, std::chrono::milliseconds(1))){
    //             if(k4abt::frame bodyFrame = bodyTracker->pop_result(std::chrono::milliseconds(1)); bodyFrame != nullptr){
    //                 auto bodiesCount = bodyFrame.get_num_bodies();
    //                 if(data.bodies.size() < bodiesCount){
    //                     data.bodies.resize(bodiesCount);
    //                 }
    //                 // std::cout << "bodiesCount " << bodiesCount << "\n";
    //                 for(size_t ii = 0; ii < bodiesCount; ++ii){
    //                     update_body(data.bodies[ii], bodyFrame.get_body(static_cast<int>(ii)));
    //                 }
    //                 timing.bodiesTS = bodyFrame.get_system_timestamp();
    //                 bodiesIndexImage = bodyFrame.get_body_index_map();
    //             }
    //         }

    //     }  catch (k4a::error error) {
    //         Logger::error("[FemtoOrbbecDeviceImpl::read_bodies] error: {}\n", error.what());
    //     }  catch (std::runtime_error error) {
    //         Logger::error("[FemtoOrbbecDeviceImpl::read_bodies] error: {}\n", error.what());
    //     }

    // }
}


auto FemtoBoltDeviceImpl::resize_color_image_to_depth_size() -> void{
    
    if(!fData.color.empty() && !fData.depth.empty()){
        fData.dephtSizedColor = orbbecD->k4a_resize_color_image_to_depth_size(mInfos, fData.color, fData.depth);
    }else{
        fData.dephtSizedColor = {};
    }
}

auto FemtoBoltDeviceImpl::generate_cloud() -> void{
    
    if(mInfos.has_cloud() && !fData.depth.empty() && fData.validDepthValues > 0){
        fData.depthCloud = orbbecD->k4a_generate_cloud(mInfos, fData.depth);
    }else{
        fData.depthCloud = {};
    }
}

auto FemtoBoltDeviceImpl::filter_cloud_image(const DCFiltersSettings &filtersS) -> void{

    // if((pointCloudImage == nullptr) || (depthImage == nullptr)){
    //     return;
    // }

    // if(filtersS.p1FMode != DCFiltersSettings::PlaneFilteringMode::None){

    //     auto p1     = filtersS.p1A;
    //     auto p2     = filtersS.p1B;
    //     auto p3     = filtersS.p1C;
    //     auto meanPt = (p1+p2+p3)/3.f;
    //     auto AB = vec(p2,p1);
    //     auto AC = vec(p3,p1);
    //     auto normalV = cross(AB,AC);
    //     normalV = normalize(normalV);

    //     auto depthBuffer  = reinterpret_cast<const uint16_t*>(depthImage->data());
    //     bool rgbCloud = colorImage != nullptr;
    //     auto cloudData = reinterpret_cast<float*>(pointCloudImage->data());
    //     int dataSize = rgbCloud ? 6 : 3;

    //     std::for_each(std::execution::par_unseq, std::begin(indices.depthVertexCorrrespondance), std::end(indices.depthVertexCorrrespondance), [&](auto idC){

    //         auto idD = std::get<0>(idC);
    //         if(depthBuffer[idD] == dc_invalid_depth_value){
    //             return;
    //         }

    //         float *cloudPoint = &cloudData[idD*dataSize];
    //         if(dot(normalV,vec(meanPt,geo::Pt3f{-cloudPoint[0], -cloudPoint[1], cloudPoint[2]})) < 0){
    //             if(filtersS.p1FMode == DCFiltersSettings::PlaneFilteringMode::Above){
    //                 depthMask[idD] = 0;
    //                 return;
    //             }
    //         }else{
    //             if(filtersS.p1FMode == DCFiltersSettings::PlaneFilteringMode::Below){
    //                 depthMask[idD] = 0;
    //                 return;
    //             }
    //         }
    //     });
    // }

    // update_valid_depth_values();
}

auto FemtoBoltDeviceImpl::create_local_frame(const DCDataSettings &dataS) -> std::unique_ptr<DCFrame>{

    tool::Bench::start("[FemtoMegaDeviceImpl::create_local_frame]");

    auto dFrame  = std::make_unique<DCFrame>();
    update_infos(dFrame.get());
    update_color(dataS, dFrame.get());
    update_depth_sized_color(dataS, dFrame.get());
    update_depth(dataS, dFrame.get());
    update_infra(dataS, dFrame.get());
    update_cloud(dataS, dFrame.get());
    update_imu(dataS, dFrame.get());
    update_bodies(dataS, dFrame.get());

    tool::Bench::stop();

    return dFrame;
}

auto FemtoBoltDeviceImpl::compress_frame(const DCFiltersSettings &filtersS, const DCDataSettings &dataS) -> std::unique_ptr<DCCompressedFrame>{

    tool::Bench::start("[FemtoBoltDeviceImpl::compress_frame]");

    auto cFrame = std::make_unique<DCCompressedFrame>();
    update_compressed_frame_infos(cFrame.get());

    auto calibrationData = orbbecD->k4a_calibration_data();    
    cFrame->calibrationData.resize(calibrationData.size());
    std::copy(calibrationData.begin(), calibrationData.end(), cFrame->calibrationData.begin());

    update_compressed_frame_color(dataS, filtersS, cFrame.get());
    update_compressed_frame_depth_sized_color(dataS, filtersS, cFrame.get());
    update_compressed_frame_depth(dataS, cFrame.get());
    update_compressed_frame_infra(dataS, cFrame.get());
    update_compressed_frame_cloud(dataS, cFrame.get());
    update_compressed_frame_imu(dataS, cFrame.get());
    update_compressed_frame_bodies(dataS, cFrame.get());

    tool::Bench::stop();

    return cFrame;
}






// auto update_body(DCBody &body, const k4abt_body_t &k4aBody) -> void{
//     body.id = static_cast<std::int8_t>(k4aBody.id);
//     for(const auto &jointD : dcJoints.data){
//         const auto &kaKoint = k4aBody.skeleton.joints[static_cast<int>(std::get<0>(jointD))];
//         auto &joint = body.skeleton.joints[static_cast<int>(std::get<0>(jointD))];
//         joint.confidence = static_cast<DCJointConfidenceLevel>(kaKoint.confidence_level);
//         const auto &p = kaKoint.position;
//         joint.position = {-p.v[0],-p.v[1],p.v[2]};
//         const auto &o = kaKoint.orientation;
//         joint.orientation = {o.wxyz.x,o.wxyz.y,o.wxyz.z,o.wxyz.w};
//     }
// }


// try {

//     Logger::message("[FemtoOrbbecDeviceImpl] Start reading:\n");

//     // configure which streams to enable or disable for the Pipeline by creating a Config
//     std::shared_ptr<ob::Config> config = std::make_shared<ob::Config>();

//     // color
//     std::shared_ptr<ob::StreamProfile> colorProfile = nullptr;
//     if(sensorList->getSensor(OBSensorType::OB_SENSOR_COLOR) && (infos.colorResolution != DCColorResolution::OFF)){
//         if(auto colorStreamProfileList = pipe->getStreamProfileList(OB_SENSOR_COLOR); colorStreamProfileList != nullptr){
//             try {
//                 colorProfile = colorStreamProfileList->getVideoStreamProfile(
//                     static_cast<int>(color_width(infos.colorResolution)),
//                     static_cast<int>(color_height(infos.colorResolution)),
//                     static_cast<OBFormat>(get_ob_image_format(infos.imageFormat)),
//                     framerate_value(infos.fps)
//                 );
//             }catch(...) {
//                 colorProfile = colorStreamProfileList->getProfile(OB_PROFILE_DEFAULT);
//             }
//             if(colorProfile){
//                 config->enableStream(colorProfile);
//             }
//         }
//     }

//     // depth
//     std::shared_ptr<ob::StreamProfile> depthProfile = nullptr;
//     OBAlignMode  alignMode = ALIGN_DISABLE;
//     std::shared_ptr<ob::StreamProfileList> depthStreamProfileList = nullptr;
//     if(sensorList->getSensor(OBSensorType::OB_SENSOR_DEPTH) && (infos.depthResolution != DCDepthResolution::K4_OFF)){
//         if(colorProfile){
//             depthStreamProfileList = pipe->getD2CDepthProfileList(colorProfile, ALIGN_D2C_HW_MODE);
//             if(depthStreamProfileList->count() > 0) {
//                 alignMode = ALIGN_D2C_HW_MODE;
//             }else{
//                 depthStreamProfileList = pipe->getD2CDepthProfileList(colorProfile, ALIGN_D2C_SW_MODE);
//                 if(depthStreamProfileList->count() > 0){
//                     alignMode = ALIGN_D2C_SW_MODE;
//                 }
//             }
//         }else{
//             depthStreamProfileList = pipe->getStreamProfileList(OB_SENSOR_DEPTH);
//         }

//         if(depthStreamProfileList->count() > 0) {
//             try {
//                 depthProfile = depthStreamProfileList->getVideoStreamProfile(
//                     static_cast<int>(depth_width(infos.depthResolution)),
//                     static_cast<int>(depth_height(infos.depthResolution)),
//                     OB_FORMAT_Y16,
//                     framerate_value(infos.fps)
//                 );
//             }catch(...) {
//                 depthProfile = depthStreamProfileList->getProfile(OB_PROFILE_DEFAULT);
//             }
//             config->enableStream(depthProfile);
//         }
//     }

//     // infrared
//     if(sensorList->getSensor(OBSensorType::OB_SENSOR_IR) && has_infrared(settings.config.mode)){
//         if(auto infraStreamProfileList = pipe->getStreamProfileList(OB_SENSOR_IR); infraStreamProfileList != nullptr){
//             std::shared_ptr<ob::StreamProfile> infraProfile = nullptr;
//             try {
//                 infraProfile = infraStreamProfileList->getVideoStreamProfile(
//                     static_cast<int>(depth_width(infos.depthResolution)),
//                     static_cast<int>(depth_height(infos.depthResolution)),
//                     OB_FORMAT_Y16,
//                     framerate_value(infos.fps)
//                 );
//             }catch(...) {
//                 infraProfile = infraStreamProfileList->getProfile(OB_PROFILE_DEFAULT);
//             }
//             config->enableStream(infraProfile);
//         }
//     }

//     // accel
//     if(sensorList->getSensor(OBSensorType::OB_SENSOR_ACCEL)){
//         if(auto accelStreamProfileList = pipe->getStreamProfileList(OB_SENSOR_ACCEL); accelStreamProfileList != nullptr){
//             OBAccelFullScaleRange fullScaleRange    = OB_ACCEL_FS_2g;
//             OBAccelSampleRate sampleRate            = OB_SAMPLE_RATE_50_HZ;
//             auto accelProfile = accelStreamProfileList->getAccelStreamProfile(fullScaleRange, sampleRate);
//             config->enableStream(accelProfile);
//         }
//     }

//     // gyro
//     if(sensorList->getSensor(OBSensorType::OB_SENSOR_GYRO)){
//         if(auto gyroStreamProfileList = pipe->getStreamProfileList(OB_SENSOR_GYRO); gyroStreamProfileList != nullptr){
//             OBGyroFullScaleRange fullScaleRange = OB_GYRO_FS_250dps;
//             OBGyroSampleRate sampleRate         = OB_SAMPLE_RATE_50_HZ;
//             auto gyraProfile = gyroStreamProfileList->getGyroStreamProfile(fullScaleRange, sampleRate);
//             config->enableStream(gyraProfile);
//         }
//     }

//     // Update the configuration items of the configuration file, and keep the original configuration for other items
//     auto currSynchConfig = device->getMultiDeviceSyncConfig();

//     switch (settings.config.synchMode) {
//     case DCSynchronisationMode::Standalone:
//         // The device does not synchronize with other devices.
//         // The Color and Depth should be set to same frame rates, the Color and Depth will be synchronized.
//         currSynchConfig.syncMode                = OB_MULTI_DEVICE_SYNC_MODE_STANDALONE;
//         currSynchConfig.triggerOutEnable        = false;
//         currSynchConfig.triggerOutDelayUs       = 0;
//         break;
//     case DCSynchronisationMode::Main:
//         // The device is the primary device in the multi-device system, it will output the trigger signal via VSYNC_OUT pin on synchronization port by default.
//         // The Color and Depth should be set to same frame rates, the Color and Depth will be synchronized and can be adjusted by @ref colorDelayUs, @ref depthDelayUs or @ref trigger2ImageDelayUs.
//         currSynchConfig.syncMode                = OB_MULTI_DEVICE_SYNC_MODE_PRIMARY;
//         currSynchConfig.triggerOutEnable        = true;
//         currSynchConfig.triggerOutDelayUs       = settings.config.subordinateDelayUsec;
//         break;
//     case DCSynchronisationMode::Subordinate:
//         // The device is the secondary device in the multi-device system, it will receive the trigger signal via VSYNC_IN pin on synchronization port. It
//         // will out the trigger signal via VSYNC_OUT pin on synchronization port by default.
//         // The Color and Depth should be set to same frame rates, the Color and Depth will be synchronized and can be adjusted by @ref colorDelayUs, @ref
//         // depthDelayUs or @ref trigger2ImageDelayUs.
//         // After starting the stream, the device will wait for the trigger signal to start capturing images, and will stop capturing images when the trigger
//         // signal is stopped.
//         // The frequency of the trigger signal should be same as the frame rate of the stream profile which is set when starting the stream.
//         currSynchConfig.syncMode                = OB_MULTI_DEVICE_SYNC_MODE_SECONDARY;
//         currSynchConfig.triggerOutEnable        = true;
//         currSynchConfig.triggerOutDelayUs       = settings.config.subordinateDelayUsec;
//         break;
//     default:
//         // The device does not synchronize with other devices,
//         // The Color and Depth can be set to different frame rates.
//         currSynchConfig.syncMode                = OB_MULTI_DEVICE_SYNC_MODE_FREE_RUN;
//         currSynchConfig.triggerOutEnable        = false;
//         currSynchConfig.triggerOutDelayUs       = 0;
//         break;
//     }

//     currSynchConfig.colorDelayUs = 0;
//     currSynchConfig.depthDelayUs = 0;

//     if(color_resolution(settings.config.mode) == DCColorResolution::OFF){
//         std::cout << "DISABLE\n";
//         alignMode = ALIGN_DISABLE;
//     }
//     config->setAlignMode(alignMode);

//     // curConfig.trigger2ImageDelayUs = c.trigger2ImageDelayUs;
//     // curConfig.framesPerTrigger     = c.framesPerTrigger;
//     // device->setMultiDeviceSyncConfig(currSynchConfig);
//     // Configure the alignment mode as hardware D2C alignment
//     // ALIGN_DISABLE,     /**< Turn off alignment */
//     // ALIGN_D2C_HW_MODE, /**< Hardware D2C alignment mode */
//     // ALIGN_D2C_SW_MODE, /**< Software D2C alignment mode */
//     // config->setDepthScaleRequire(true);
//     // config->setD2CTargetResolution(512,512);
//     // config->setD2CTargetResolution(uint32_t d2cTargetWidth, uint32_t d2cTargetHeight);

//     // frame synch
//     if(settings.config.synchronizeColorAndDepth){
//         pipe->enableFrameSync();
//     }else{
//         pipe->disableFrameSync();
//     }

//     // other
//     set_value(device.get(), OB_PROP_INDICATOR_LIGHT_BOOL, !settings.config.disableLED);

//     // start pipe with current config
//     pipe->start(config);

//     // get camera intrinsic and extrinsic parameters form pipeline and set to point cloud filter
//     cameraParam         = pipe->getCameraParam();
//     // get calibration parameters
//     calibrationParam    = pipe->getCalibrationParam(config);


//     // Create a point cloud Filter object (the device parameters will be obtained inside the Pipeline when the point cloud filter is created, so try to
//     // configure the device before creating the filter)
//     pointCloud = std::make_unique<ob::PointCloudFilter>();
//     pointCloud->setCameraParam(cameraParam);


//     if(infos.depthResolution != DCDepthResolution::K4_OFF){ // depth enabled

//         if(infos.colorResolution != DCColorResolution::OFF){ // color enabled

//             auto colorWidth  = color_width(infos.colorResolution);
//             auto colorHeight = color_height(infos.colorResolution);

//             pointCloudData.resize(colorWidth * colorHeight * sizeof(OBColorPoint));
//             std::uint32_t tableSize     = colorWidth * colorHeight * 2 * sizeof(float);
//             tableData.resize(tableSize);

//             xyTables = OBXYTables{};
//             if(!ob::CoordinateTransformHelper::transformationInitXYTables(
//                     calibrationParam,
//                     OB_SENSOR_COLOR, tableData.data(), &tableSize, &xyTables)) {
//                 // ...
//                 return false;
//             }

//         }else{

//             auto depthWidth  = depth_width(infos.depthResolution);
//             auto depthHeight = depth_height(infos.depthResolution);

//             pointCloudData.resize(depthWidth * depthHeight * sizeof(OBPoint3f));
//             uint32_t tableSize = depthWidth * depthHeight * 2 * sizeof(float);
//             tableData.resize(tableSize);

//             xyTables = OBXYTables{};
//             if(!ob::CoordinateTransformHelper::transformationInitXYTables(
//                 calibrationParam,
//                 OB_SENSOR_DEPTH, tableData.data(), &tableSize, &xyTables)) {
//                 // ...
//                 return false;
//             }
//         }
//     }

//     k4Calibration     = convert_to_k4_calibration(settings.config.mode, calibrationParam);
//     k4Transformation  = k4a::transformation(k4Calibration);

//     if((depth_resolution(settings.config.mode) != DCDepthResolution::K4_OFF) && settings.config.enableBodyTracking){

//         try{
//             Logger::message("[FemtoOrbbecDeviceImpl::start_cameras] Start body tracker\n");
//             bodyTracker = std::make_unique<k4abt::tracker>(k4abt::tracker::create(k4Calibration, k4aBtConfig));
//         }catch (k4a::error error) {
//             Logger::error("[FemtoOrbbecDeviceImpl::start_reading] body tracker error: {}\n", error.what());
//         }
//     }

// }catch(ob::Error &e) {
//     Logger::error(std::format("[OrbbecDevice::start_reading] Start reading error: {}\n", e.getMessage()));
//     device = nullptr;
//     return false;
// }

// start_reading_thread();

// return true;




// // if(has_cloud(settings.config.mode) && (depthImage != nullptr)){
// //     std::cout << "gen cloud 1\n";
// //     auto depthRes = depth_resolution(settings.config.mode);

// //     // auto k4aDepthImage = k4a::image::create(
// //     //     K4A_IMAGE_FORMAT_DEPTH16,
// //     //     static_cast<int>(depth_width(depthRes)),
// //     //     static_cast<int>(depth_height(depthRes)),
// //     //     static_cast<int32_t>(depth_width(depthRes) * 1 * sizeof(std::uint16_t))
// //     // );

// //     auto db = depth_data();

// //     auto k4aDepthImage = k4a::image::create_from_buffer(
// //         K4A_IMAGE_FORMAT_DEPTH16,
// //         static_cast<int>(depth_width(depthRes)),
// //         static_cast<int>(depth_height(depthRes)),
// //         static_cast<int32_t>(depth_width(depthRes) * 1 * sizeof(std::uint16_t)),
// //         reinterpret_cast<std::uint8_t*>(db.data()),
// //         db.size_bytes(),
// //         nullptr,
// //         nullptr
// //     );

// //     auto pci = k4a::image::create(
// //         K4A_IMAGE_FORMAT_CUSTOM,
// //         static_cast<int>(depth_width(depthRes)),
// //         static_cast<int>(depth_height(depthRes)),
// //         static_cast<int32_t>(infos.depthWidth * 3 * sizeof(int16_t))
// //     );

// //     std::cout << "gen cloud 2\n";
// //     k4Transformation.depth_image_to_point_cloud(k4aDepthImage, K4A_CALIBRATION_TYPE_DEPTH, &pci);
// //     std::cout << "gen cloud 3\n";
// // }

// if(has_cloud(settings.config.mode) && (depthImage != nullptr)){
//     if(validDepthValues > 0){
//         auto depthValueScale = depthImage->getValueScale()*0.001f;
//         if(colorImage != nullptr){
//             pointCloud->setCreatePointFormat(OB_FORMAT_RGB_POINT);
//         }else{
//             pointCloud->setCreatePointFormat(OB_FORMAT_POINT);
//         }
//         pointCloud->setPositionDataScaled(depthValueScale);
//         pointCloudImage = pointCloud->process(frameSet);

//         // reset data
//         std::fill(std::begin(pointCloudData), std::end(pointCloudData), 0);

//         size_t id = 0;
//         if(colorImage == nullptr){

//             OBPoint *pointPixel = reinterpret_cast<OBPoint *>(pointCloudData.data());
//             ob::CoordinateTransformHelper::transformationDepthToPointCloud(&xyTables, data.depth.data(), pointPixel);

//             auto depthWidth  = depth_width(infos.depthResolution);
//             auto depthHeight = depth_width(infos.depthResolution);

//             for(size_t ii = 0; ii < depthHeight; ++ii){
//                 for(size_t jj = 0; jj < depthWidth; ++jj){
//                     auto dValue = data.depth[ii * depthWidth + jj];
//                     if(dValue == dc_invalid_depth_value){
//                         pointPixel++;
//                         continue;
//                     }
//                     pointPixel->x *= depthValueScale;
//                     pointPixel->y *= depthValueScale;
//                     pointPixel->z *= depthValueScale;
//                     pointPixel++;
//                     ++id;
//                 }
//             }
//             // for(size_t ii = 0; ii < infos.depthHeight; ++ii){
//             //     for(size_t jj = 0; jj < infos.depthWidth; ++jj){

//             //         OBPoint3f targetPixel = {};
//             //         auto dValue = dData[ii * infos.depthWidth + jj];
//             //         if(dValue == dc_invalid_depth_value){
//             //             pointPixel++;
//             //             continue;
//             //         }

//             //         ob::CoordinateTransformHelper::calibration2dTo3dUndistortion(
//             //             calibrationParam,
//             //             {static_cast<float>(jj), static_cast<float>(ii)},
//             //             1.*dValue,
//             //             OB_SENSOR_DEPTH, OB_SENSOR_DEPTH,
//             //             &targetPixel
//             //         );

//             //         pointPixel->x = targetPixel.x*depthValueScale;
//             //         pointPixel->y = targetPixel.y*depthValueScale;
//             //         pointPixel->z = targetPixel.z*depthValueScale;
//             //         pointPixel++;
//             //         ++id;
//             //     }
//             // }
//         }else{
//             OBColorPoint *pointPixel = reinterpret_cast<OBColorPoint *>(pointCloudData.data());
//             ob::CoordinateTransformHelper::transformationDepthToRGBDPointCloud(
//                 &xyTables,
//                 data.depth.data(),
//                 color_rgba_data().data(),
//                 pointPixel
//             );

//             auto depthWidth  = depth_width(infos.depthResolution);
//             auto depthHeight = depth_width(infos.depthResolution);

//             for(size_t ii = 0; ii < depthHeight; ++ii){
//                 for(size_t jj = 0; jj < depthWidth; ++jj){
//                     auto dValue = data.depth[ii * depthWidth + jj];
//                     if(dValue == dc_invalid_depth_value){
//                         pointPixel++;
//                         continue;
//                     }
//                     pointPixel->x *= depthValueScale;
//                     pointPixel->y *= depthValueScale;
//                     pointPixel->z *= depthValueScale;
//                     pointPixel++;
//                     ++id;
//                 }
//             }

//             // std::cout << "transformationDepthToRGBDPointCloud\n";
//         }
//         // std::cout << id << " ";

//         // uint32_t  height     = depthFrame->height();
//         // uint32_t  width      = depthFrame->width();


//         // if(frameset != nullptr && frameset->depthFrame() != nullptr) {
//         //     memset(pointcloudData, 0, pointcloudSize);
//         //     OBPoint * pointPixel = (OBPoint *)pointcloudData;
//         //     auto      depthFrame = frameset->depthFrame();
//         //     uint32_t  height     = depthFrame->height();
//         //     uint32_t  width      = depthFrame->width();
//         //     uint16_t *pDepthData = (uint16_t *)depthFrame->data();

//         //     for(uint32_t i = 0; i < height; i++) {
//         //         for(uint32_t j = 0; j < width; j++) {
//         //             OBPoint2f sourcePixel = { static_cast<float >(j), static_cast<float>(i) };
//         //             OBPoint3f targetPixel = {};
//         //             float     depthValue  = (float)pDepthData[i * width + j];

//         //             ob::CoordinateTransformHelper::calibration2dTo3dUndistortion(param, sourcePixel, depthValue, OB_SENSOR_DEPTH, OB_SENSOR_DEPTH,
//         //                                                                          &targetPixel);

//         //             (*pointPixel).z = targetPixel.z;
//         //             (*pointPixel).x = targetPixel.x;
//         //             (*pointPixel).y = targetPixel.y;
//         //             pointPixel++;
//         //         }
//         //     }

//         //     savePointsDataToPly((uint8_t *)pointcloudData, pointcloudSize, "DepthPointsUndistortion.ply");
//         //     std::cout << "DepthPointsUndistortion.ply Saved" << std::endl;
//         //     break;
//     }
// }


// // cloud
// if(dataS.generateCloudLocal && (pointCloudImage != nullptr) && (depthImage != nullptr)){

//     tool::Bench::start("[OrbbecDevice::create_local_frame] cloud");

//     dFrame->cloud.vertices.resize(validDepthValues);
//     dFrame->cloud.colors.resize(validDepthValues);
//     // dFrame->cloud.normals.resize(validDepthValues);


//     auto depthBuffer  = reinterpret_cast<const uint16_t*>(depthImage->data());
//     bool rgbCloud = colorImage != nullptr;

//     if(!rgbCloud && !pointCloudData.empty()){
//         auto pData  = reinterpret_cast<OBPoint3f*>(pointCloudData.data());

//         std::for_each(std::execution::par_unseq, std::begin(indices.depthVertexCorrrespondance), std::end(indices.depthVertexCorrrespondance), [&](auto idC){

//             auto idD = std::get<0>(idC);
//             if(depthBuffer[idD] == dc_invalid_depth_value){
//                 return;
//             }

//             auto idV = std::get<1>(idC);
//             auto pt = pData[idD];

//             // float *cloudPoint = &cloudData[idD*dataSize];

//             // set vertex
//             dFrame->cloud.vertices[idV]= geo::Pt3f{
//                 -pt.x, -pt.y, pt.z
//             };

//             float vF = (static_cast<float>(depthBuffer[idD]) - dRange.x())/diff;
//             float intPart;
//             float decPart = std::modf((vF*(depthGradient.size()-1)), &intPart);
//             size_t idG = static_cast<size_t>(intPart);
//             dFrame->cloud.colors[idV] = depthGradient[idG]*(1.f-decPart) + depthGradient[idG+1]*decPart;

//         });

//     }else{

//         auto pData  = reinterpret_cast<OBColorPoint*>(pointCloudData.data());
//         auto cData  = color_rgba_data();

//         std::for_each(std::execution::seq, std::begin(indices.depthVertexCorrrespondance), std::end(indices.depthVertexCorrrespondance), [&](auto idC){

//             auto idD = std::get<0>(idC);
//             if(depthBuffer[idD] == dc_invalid_depth_value){
//                 return;
//             }

//             auto idV = std::get<1>(idC);
//             OBColorPoint pt = pData[idD];

//             // set vertex
//             dFrame->cloud.vertices[idV]= geo::Pt3f{
//                 -pt.x, -pt.y, pt.z
//             };

//             dFrame->cloud.colors[idV] = geo::Pt3f{
//                 static_cast<float>(cData[idD].r()),
//                 static_cast<float>(cData[idD].g()),
//                 static_cast<float>(cData[idD].b()),
//             }/255.f;

//             // dFrame->cloud.colors[idV] = geo::Pt3f{
//             //     pt.r,
//             //     pt.g,
//             //     pt.b
//             // }/255.f;

//             // if(rand() % 1000 == 0){
//                 // std::cout << "col " << idD << " " << idV << " " << pt.r << " " << pt.g << " " << pt.b << "\n";
//             // }
//         });

//         // auto cloudData = reinterpret_cast<float*>(pointCloudImage->data());
//         // int dataSize = rgbCloud ? 6 : 3;

//         // std::for_each(std::execution::par_unseq, std::begin(indices.depthVertexCorrrespondance), std::end(indices.depthVertexCorrrespondance), [&](auto idC){

//         //     auto idD = std::get<0>(idC);
//         //     if(depthBuffer[idD] == dc_invalid_depth_value){
//         //         return;
//         //     }

//         //     auto idV = std::get<1>(idC);
//         //     float *cloudPoint = &cloudData[idD*dataSize];

//         //     // set vertex
//         //     dFrame->cloud.vertices[idV]= geo::Pt3f{
//         //         -cloudPoint[0], -cloudPoint[1], cloudPoint[2]
//         //     };

//         //     // set color
//         //     if(rgbCloud){

//         //         // if(zonesId[idD] != 0){
//         //         //     if(zonesId[idD] == 1){
//         //         //         dFrame->cloud.colors[idV] = geo::Pt3f{1,0,0};
//         //         //     }else if(zonesId[idD] == 2){
//         //         //         dFrame->cloud.colors[idV] = geo::Pt3f{0,1,0};
//         //         //     }else if(zonesId[idD] == 3){
//         //         //         dFrame->cloud.colors[idV] = geo::Pt3f{0,0,1};
//         //         //     }else{
//         //         //         dFrame->cloud.colors[idV] = geo::Pt3f{1,0,1};
//         //         //     }

//         //         // }else{
//         //         //     dFrame->cloud.colors[idV] = geo::Pt3f{
//         //         //         0,
//         //         //         0,
//         //         //         0
//         //         //     };
//         //         // }

//         //         dFrame->cloud.colors[idV] = geo::Pt3f{
//         //                                         cloudPoint[3],
//         //                                         cloudPoint[4],
//         //                                         cloudPoint[5]
//         //                                     }/255.f;

//         //     }else{
//         //         float vF = (static_cast<float>(depthBuffer[idD]) - dRange.x())/diff;
//         //         float intPart;
//         //         float decPart = std::modf((vF*(depthGradient.size()-1)), &intPart);
//         //         size_t idG = static_cast<size_t>(intPart);
//         //         dFrame->cloud.colors[idV] = depthGradient[idG]*(1.f-decPart) + depthGradient[idG+1]*decPart;
//         //     }
//         // });

//     }


//     tool::Bench::stop();
// }
