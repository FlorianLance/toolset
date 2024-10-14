
#pragma once

// std
#include <iostream>
#include <format>

// // opencv
// #include <opencv2/core.hpp>
// #include <opencv2/imgproc.hpp>
// #include <opencv2/imgcodecs.hpp>

// // base
// #include "depth-camera/dc_device_manager.hpp"

// // local
// #include "test_ob_utility.hpp"
// #include "io/cloud_io.hpp"

// #include "k4a/k4a.hpp"
// #include "depth-camera/dc_enums.hpp"
// #include "depth-camera/dc_frame_uncompressor.hpp"

// #include "geometry/point3.hpp"
// #include "io/cloud_io.hpp"

#include "depth-camera/dc_device.hpp"

using namespace tool::geo;
using namespace tool::cam;

// auto convert_to_k4_calibration(DCMode mode, const OBCalibrationParam &obCalibration) -> k4a::calibration{

//     const auto &obDepthIntrinsics = obCalibration.intrinsics[OB_SENSOR_DEPTH];
//     const auto &obDepthDistorsion = obCalibration.distortion[OB_SENSOR_DEPTH];
//     const auto &obDepthExtrinsics = obCalibration.extrinsics[OB_SENSOR_DEPTH];

//     const auto &obColorIntrinsics = obCalibration.intrinsics[OB_SENSOR_COLOR];
//     const auto &obColorDistorsion = obCalibration.distortion[OB_SENSOR_COLOR];
//     const auto &obColorExtrinsics = obCalibration.extrinsics[OB_SENSOR_COLOR];

//     k4a::calibration k4Calibration;
//     switch (mode) {
//     case DCMode::FB_CLOUD_C1280x720_DI640x576_NV12_F30:
//         k4Calibration.depth_mode          = K4A_DEPTH_MODE_NFOV_UNBINNED;
//         k4Calibration.color_resolution    = K4A_COLOR_RESOLUTION_720P;
//         break;
//     case DCMode::FB_CLOUD_C1280x720_DI640x576_MJPG_F30:
//         k4Calibration.depth_mode          = K4A_DEPTH_MODE_NFOV_UNBINNED;
//         k4Calibration.color_resolution    = K4A_COLOR_RESOLUTION_720P;
//         break;
//     case DCMode::FB_CLOUD_C1280x720_DI640x576_BGRA_F30:
//         k4Calibration.depth_mode          = K4A_DEPTH_MODE_NFOV_UNBINNED;
//         k4Calibration.color_resolution    = K4A_COLOR_RESOLUTION_720P;
//         break;
//     case DCMode::FB_CLOUD_C1280x720_DI512x512_MJPG_F30:
//         k4Calibration.depth_mode          = K4A_DEPTH_MODE_WFOV_2X2BINNED;
//         k4Calibration.color_resolution    = K4A_COLOR_RESOLUTION_720P;
//         break;
//     case DCMode::FB_CLOUD_DI512x512_MJPG_F30:
//         k4Calibration.depth_mode          = K4A_DEPTH_MODE_WFOV_2X2BINNED;
//         k4Calibration.color_resolution    = K4A_COLOR_RESOLUTION_OFF;
//         break;
//     case DCMode::FB_IMG_C1280x720_MJPG_F30:
//         k4Calibration.depth_mode          = K4A_DEPTH_MODE_OFF;
//         k4Calibration.color_resolution    = K4A_COLOR_RESOLUTION_720P;
//         break;
//     default:
//         break;
//     }

//     auto depthRes = dc_depth_resolution(mode);

//     // depth calibration
//     auto &cdepthCamCal              = k4Calibration.depth_camera_calibration;
//     // # resolutions
//     cdepthCamCal.resolution_width   = dc_depth_width(depthRes);
//     cdepthCamCal.resolution_height  = dc_depth_height(depthRes);
//     // # metric_radius
//     cdepthCamCal.metric_radius = 1.7399998;
//     // # intrinsics
//     // ## type
//     cdepthCamCal.intrinsics.type = K4A_CALIBRATION_LENS_DISTORTION_MODEL_BROWN_CONRADY;
//     // ## parameters
//     auto &dIntrincisP           = cdepthCamCal.intrinsics.parameters.param;
//     dIntrincisP.cx              = obDepthIntrinsics.cx; /**< Principal point in image, x */
//     dIntrincisP.cy              = obDepthIntrinsics.cy; /**< Principal point in image, y */
//     dIntrincisP.fx              = obDepthIntrinsics.fx; /**< Focal length x */
//     dIntrincisP.fy              = obDepthIntrinsics.fy; /**< Focal length y */
//     dIntrincisP.k1              = obDepthDistorsion.k1; /**< k1 radial distortion coefficient */
//     dIntrincisP.k2              = obDepthDistorsion.k2; /**< k2 radial distortion coefficient */
//     dIntrincisP.k3              = obDepthDistorsion.k3; /**< k3 radial distortion coefficient */
//     dIntrincisP.k4              = obDepthDistorsion.k4; /**< k4 radial distortion coefficient */
//     dIntrincisP.k5              = obDepthDistorsion.k5; /**< k5 radial distortion coefficient */
//     dIntrincisP.k6              = obDepthDistorsion.k6; /**< k6 radial distortion coefficient */
//     // dIntrincisP.codx; // UNUSED                      /**< Center of distortion in Z=1 plane, x (only used for Rational6KT) */
//     // dIntrincisP.cody; // UNUSED                      /**< Center of distortion in Z=1 plane, y (only used for Rational6KT) */
//     dIntrincisP.p1              = obDepthDistorsion.p1; /**< Tangential distortion coefficient 2 */
//     dIntrincisP.p2              = obDepthDistorsion.p2; /**< Tangential distortion coefficient 1 */
//     dIntrincisP.metric_radius   = 0; /**< Metric radius */
//     cdepthCamCal.intrinsics.parameter_count = 15;
//     // # extrinsics
//     // ## rotation
//     auto &dExtRot = cdepthCamCal.extrinsics.rotation;
//     dExtRot[0] = 1.f;
//     dExtRot[1] = 0.f;
//     dExtRot[2] = 0.f;
//     dExtRot[3] = 0.f;
//     dExtRot[4] = 1.f;
//     dExtRot[5] = 0.f;
//     dExtRot[6] = 0.f;
//     dExtRot[7] = 0.f;
//     dExtRot[8] = 1.f;
//     // ## translation
//     auto &dExtTr = cdepthCamCal.extrinsics.translation;
//     dExtTr[0] = 0.f;
//     dExtTr[1] = 0.f;
//     dExtTr[2] = 0.f;
    
//     auto colorRes = dc_color_resolution(mode);

//     // color calibration
//     auto &cColorCamCal              = k4Calibration.color_camera_calibration;
//     // # resolutions
//     cColorCamCal.resolution_width   = dc_color_width(colorRes);
//     cColorCamCal.resolution_height  = dc_color_height(colorRes);
//     // # metric_radius
//     cColorCamCal.metric_radius      = 1.7;
//     // # intrinsics
//     // ## type
//     cColorCamCal.intrinsics.type    = K4A_CALIBRATION_LENS_DISTORTION_MODEL_BROWN_CONRADY;
//     // ## parameters
//     auto &cIntrincisP               = cColorCamCal.intrinsics.parameters.param;
//     cIntrincisP.cx                  = obColorIntrinsics.cx; /**< Principal point in image, x */
//     cIntrincisP.cy                  = obColorIntrinsics.cy; /**< Principal point in image, y */
//     cIntrincisP.fx                  = obColorIntrinsics.fx; /**< Focal length x */
//     cIntrincisP.fy                  = obColorIntrinsics.fy; /**< Focal length y */
//     cIntrincisP.k1                  = obColorDistorsion.k1; /**< k1 radial distortion coefficient */
//     cIntrincisP.k2                  = obColorDistorsion.k2; /**< k2 radial distortion coefficient */
//     cIntrincisP.k3                  = obColorDistorsion.k3; /**< k3 radial distortion coefficient */
//     cIntrincisP.k4                  = obColorDistorsion.k4; /**< k4 radial distortion coefficient */
//     cIntrincisP.k5                  = obColorDistorsion.k5; /**< k5 radial distortion coefficient */
//     cIntrincisP.k6                  = obColorDistorsion.k6; /**< k6 radial distortion coefficient */
//     // cIntrincisP.codx; // UNUSED                          /**< Center of distortion in Z=1 plane, x (only used for Rational6KT) */
//     // cIntrincisP.cody; // UNUSED                          /**< Center of distortion in Z=1 plane, y (only used for Rational6KT) */
//     cIntrincisP.p1                  = obColorDistorsion.p1; /**< Tangential distortion coefficient 2 */
//     cIntrincisP.p2                  = obColorDistorsion.p2; /**< Tangential distortion coefficient 1 */
//     cIntrincisP.metric_radius       = 0; /**< Metric radius */
//     cColorCamCal.intrinsics.parameter_count = 15;
//     // # extrinsics
//     // ## rotation
//     auto &cExtRot = cColorCamCal.extrinsics.rotation;
//     cExtRot[0] = 1.f;
//     cExtRot[1] = 0.f;
//     cExtRot[2] = 0.f;
//     cExtRot[3] = 0.f;
//     cExtRot[4] = 1.f;
//     cExtRot[5] = 0.f;
//     cExtRot[6] = 0.f;
//     cExtRot[7] = 0.f;
//     cExtRot[8] = 1.f;
//     // ## translation
//     auto &cExtTr = cColorCamCal.extrinsics.translation;
//     cExtTr[0] = 0.f;
//     cExtTr[1] = 0.f;
//     cExtTr[2] = 0.f;

//     // extrinsics
//     std::copy(obDepthExtrinsics[OB_SENSOR_COLOR].rot,   obDepthExtrinsics[OB_SENSOR_COLOR].rot + 9,     k4Calibration.extrinsics[K4A_CALIBRATION_TYPE_DEPTH][K4A_CALIBRATION_TYPE_COLOR].rotation);
//     std::copy(obDepthExtrinsics[OB_SENSOR_COLOR].trans, obDepthExtrinsics[OB_SENSOR_COLOR].trans + 3,   k4Calibration.extrinsics[K4A_CALIBRATION_TYPE_DEPTH][K4A_CALIBRATION_TYPE_COLOR].translation);
//     std::copy(obColorExtrinsics[OB_SENSOR_DEPTH].rot,   obDepthExtrinsics[OB_SENSOR_DEPTH].rot + 9,     k4Calibration.extrinsics[K4A_CALIBRATION_TYPE_COLOR][K4A_CALIBRATION_TYPE_DEPTH].rotation);
//     std::copy(obColorExtrinsics[OB_SENSOR_DEPTH].trans, obDepthExtrinsics[OB_SENSOR_DEPTH].trans + 3,   k4Calibration.extrinsics[K4A_CALIBRATION_TYPE_COLOR][K4A_CALIBRATION_TYPE_DEPTH].translation);

//     return k4Calibration;
// }


auto test_raw_orbbec_femto() -> void{

    // DCMode mode = DCMode::FB_C2048x1536_DI512x512_MJPG_F30;
    // // DCMode mode = DCMode::FO_CLOUD_C1280x720_DI640x576_MJPG_F30;
    // auto dRes = dc_depth_resolution(mode);
    // auto cRes = dc_color_resolution(mode);
    // size_t depthWidth  = dc_depth_width(dRes);
    // size_t depthHeight = dc_depth_height(dRes);
    // size_t colorWidth  = dc_color_width(cRes);
    // size_t colorHeight = dc_color_height(cRes);

    // std::cout << "test_raw_orbbec: " << std::endl;
    // auto context = std::make_unique<ob::Context>();
    // context->setLoggerSeverity(OB_LOG_SEVERITY_WARN);
    // context->enableNetDeviceEnumeration(true);
    // auto deviceList  = context->queryDeviceList();
    // if(deviceList->deviceCount() == 0){
    //     std::cerr << "no device found: " << std::endl;
    //     return;
    // }
    // auto device     = deviceList->getDevice(0);
    // auto sensorList = device->getSensorList();

    // for(uint32_t idS = 0; idS < sensorList->count(); idS++) {
    //     auto sensor = sensorList->getSensor(idS);
    //     switch(sensor->type()) {
    //     case OB_SENSOR_COLOR:
    //         std::cout << "[test_raw_orbbec_femto] Color sensor found.\n";
    //         break;
    //     case OB_SENSOR_DEPTH:
    //         std::cout << "[test_raw_orbbec_femto] Depth sensor found.\n";
    //         break;
    //     case OB_SENSOR_IR:
    //         std::cout << "[test_raw_orbbec_femto] Infrared sensor found.\n";
    //         break;
    //     case OB_SENSOR_IR_LEFT:
    //         std::cout << "[test_raw_orbbec_femto] Infrared left sensor found.\n";
    //         break;
    //     case OB_SENSOR_IR_RIGHT:
    //         std::cout << "[test_raw_orbbec_femto] Infrared right sensor found.\n";
    //         break;
    //     case OB_SENSOR_GYRO:
    //         std::cout << "[test_raw_orbbec_femto] Gyro sensor found.\n";
    //         break;
    //     case OB_SENSOR_ACCEL:
    //         std::cout << "[test_raw_orbbec_femto] Accel sensor found.\n";
    //         break;
    //     default:
    //         break;
    //     }
    // }

    // ob::Pipeline pipe(device);
    // std::shared_ptr<ob::Config> config = std::make_shared<ob::Config>();

    // std::cout << "Load color profiles: " << std::endl;
    // auto colorStreamProfileList = pipe.getStreamProfileList(OB_SENSOR_COLOR);
    // std::cout << "Nb profiles count: " << colorStreamProfileList->count() << std::endl;
    // // for(size_t idP = 0; idP < colorStreamProfileList->count(); ++idP){
    // //     auto pr = colorStreamProfileList->getProfile(idP);
    // //     std::cout << "COLOR ---> " << idP << " ";
    // //     if(pr != nullptr){
    // //         auto vp = dynamic_cast<ob::VideoStreamProfile*>(pr.get());
    // //         if(vp != nullptr){
    // //             std::cout << vp->fps() << " " << vp->width() << " " << vp->height() << (int)vp->format() << "\n";
    // //         }else{
    // //             std::cout << "null\n";
    // //         }
    // //         // std::cout <<  " --> " << (int)pr->type() << " " << (int)pr->format() << "\n";
    // //     }

    // //     // auto p  = dynamic_cast<ob::VideoStreamProfile*>(colorStreamProfileList->getProfile(idP).get());

    // // }

    // std::cout << "Get color profile: " << std::endl;
    // // auto profile = colorStreamProfileList->getProfile(
    // //     OB_PROFILE_DEFAULT
    // // );
    // auto colorProfile = colorStreamProfileList->getVideoStreamProfile(
    //     colorWidth,
    //     colorHeight,
    //     OBFormat::OB_FORMAT_MJPG,
    //     30
    // );
    // if(colorProfile){
    //     std::cout << "color profile found\n";
    // }

    // std::cout << "Enable color stream: " << std::endl;
    // config->enableStream(colorProfile);

    // std::cout << "Load depth profiles: " << std::endl;

    // // auto depthProfileList = pipe.getStreamProfileList(OB_SENSOR_DEPTH);
    // auto depthProfileList =  pipe.getD2CDepthProfileList(colorProfile, ALIGN_DISABLE);
    // std::cout << "Nb profiles count: " << depthProfileList->count() << std::endl;
    // std::cout << "Get depth profile: " << std::endl;

    // auto depthProfile = depthProfileList->getVideoStreamProfile(
    //     depthWidth, // OB_WIDTH_ANY,  // 640,
    //     depthHeight, // OB_HEIGHT_ANY, // 576,
    //     OBFormat::OB_FORMAT_Y16, //OBFormat::OB_FORMAT_Y16,
    //     30
    // );
    // std::cout << "enable depth stream: " << std::endl;
    // config->enableStream(depthProfile);


    // std::cout << "Load infra profiles: " << std::endl;
    // auto infraProfileList = pipe.getStreamProfileList(OB_SENSOR_IR);
    // std::cout << "Nb profiles count: " << infraProfileList->count() << std::endl;

    // std::cout << "Get infra profile: " << std::endl;
    // auto infraProfile = infraProfileList->getVideoStreamProfile(
    //     depthWidth,
    //     depthHeight,
    //     OBFormat::OB_FORMAT_Y16,
    //     30
    // );
    // std::cout << "Enable infra stream: " << std::endl;
    // config->enableStream(infraProfile);


    // // set config options
    // std::cout << "Set config settings: " << std::endl;
    // config->setAlignMode(ALIGN_DISABLE);

    // // Start the pipeline with config
    // std::cout << "Start pipe with config: " << std::endl;
    // pipe.enableFrameSync();
    // pipe.start(config);


    // // typedef struct {
    // //     OBCameraIntrinsic  depthIntrinsic;   ///< Depth camera internal parameters
    // //     OBCameraIntrinsic  rgbIntrinsic;     ///< Color camera internal parameters
    // //     OBCameraDistortion depthDistortion;  ///< Depth camera distortion parameters
    // //     OBCameraDistortion rgbDistortion;    ///< Color camera distortion parameters
    // //     OBD2CTransform     transform;        ///< Rotation/transformation matrix
    // //     bool               isMirrored;       ///< Whether the image frame corresponding to this group of parameters is mirrored
    // // } OBCameraParam, ob_camera_param;

    // // typedef struct {
    // //     OBCameraIntrinsic  intrinsics[OB_SENSOR_COUNT];            ///< Sensor internal parameters
    // //     OBCameraDistortion distortion[OB_SENSOR_COUNT];            ///< Sensor distortion
    // //     OBTransform extrinsics[OB_SENSOR_COUNT][OB_SENSOR_COUNT];  ///< The extrinsic parameters allow 3D coordinate conversions between sensor.To transform from a
    // //         ///< source to a target 3D coordinate system,under extrinsics[source][target].
    // // } OBCalibrationParam, ob_calibration_param;


    // // auto cameraParam      = pipe.getCameraParam();
    // // auto calibrationParam = pipe.getCalibrationParam(config);

    // // auto k4Cali = convert_to_k4_calibration(mode, calibrationParam);
    // // k4a::transformation k4Transformation(k4Cali);

    // auto colorImage = k4a::image::create(
    //     K4A_IMAGE_FORMAT_COLOR_BGRA32,
    //     colorWidth,
    //     colorHeight,
    //     colorWidth * 4 * sizeof(uint8_t)
    // );

    // auto depthSizedColorImage = k4a::image::create(
    //     K4A_IMAGE_FORMAT_COLOR_BGRA32,
    //     depthWidth,
    //     depthHeight,
    //     depthWidth * 4 * sizeof(uint8_t)
    // );
    // auto pci = k4a::image::create(
    //     K4A_IMAGE_FORMAT_CUSTOM,
    //     depthWidth,
    //     depthHeight,
    //     depthWidth * 3 * sizeof(int16_t)
    // );

    // DCFrameUncompressor frameUncompressor;

    // int idFr = 0;
    // int maxNbFrames = 10;
    // while(idFr < maxNbFrames) {

    //     auto frameSet = pipe.waitForFrames(100);
    //     if(frameSet == nullptr) {
    //         continue;
    //     }

    //     std::cout << "get col\n";
    //     auto colorFrame = frameSet->colorFrame();
    //     std::cout << "get depth\n";
    //     auto depthFrame = frameSet->depthFrame();

    //     if(colorFrame && depthFrame){

    //         // std::cout << "uncompress color\n";
    //         // frameUncompressor.uncompress_jpeg_data(
    //         //     colorWidth,
    //         //     colorHeight,
    //         //     DCFrameUncompressor::ColorFormat::RGBA,
    //         //     colorFrame->dataSize(),
    //         //     reinterpret_cast<uint8_t*>(colorFrame->data()),
    //         //     reinterpret_cast<uint8_t*>(colorImage.get_buffer())
    //         // );

    //         std::cout << "create depth from buffer\n";
    //         auto k4aDepthImage = k4a::image::create_from_buffer(
    //             K4A_IMAGE_FORMAT_DEPTH16,
    //             depthWidth,
    //             depthHeight,
    //             depthWidth * 1 * sizeof(std::uint16_t),
    //             reinterpret_cast<std::uint8_t*>(depthFrame->data()),
    //             depthFrame->dataSize(),
    //             nullptr,
    //             nullptr
    //         );

    //         // write depth map
    //         {
    //             cv::Mat cvtMat;
    //             cv::Mat rawMat = cv::Mat(depthFrame->height(), depthFrame->width(), CV_16UC1, depthFrame->data());
    //             // depth frame pixel value multiply scale to get distance in millimeter
    //             float scale = depthFrame->getValueScale();
    //             // threshold to 5.12m
    //             cv::threshold(rawMat, cvtMat, 5120.0f / scale, 0, cv::THRESH_TRUNC);
    //             cvtMat.convertTo(cvtMat, CV_8UC1, scale * 0.05);
    //             cv::Mat rstMat;
    //             cv::applyColorMap(cvtMat, rstMat, cv::COLORMAP_JET);
    //             cv::imwrite(std::format("D:/ob_{}_img_depth.png", idFr).c_str(),rstMat);
    //         }

    //         // write color map
    //         {
    //             cv::Mat cvtMat;
    //             cv::Mat rawMat = cv::Mat(colorImage.get_height_pixels(), colorImage.get_width_pixels(), CV_8UC4, colorImage.get_buffer());
    //             cv::imwrite(std::format("D:/ob_{}_img_col.png", idFr).c_str(),rawMat);
    //         }

    //         std::cout << "resize color to depth\n";
    //         // k4Transformation.color_image_to_depth_camera(
    //         //     k4aDepthImage,
    //         //     colorImage,
    //         //     &depthSizedColorImage
    //         // );


    //         // write resized color map
    //         {
    //             cv::Mat cvtMat;
    //             cv::Mat rawMat = cv::Mat(depthSizedColorImage.get_height_pixels(), depthSizedColorImage.get_width_pixels(), CV_8UC4, depthSizedColorImage.get_buffer());
    //             cv::imwrite(std::format("D:/ob_{}_img_resized_col.png", idFr).c_str(),rawMat);
    //         }

    //         std::cout << "gen point cloud from depth\n";
    //         // k4Transformation.depth_image_to_point_cloud(
    //         //     k4aDepthImage,
    //         //     K4A_CALIBRATION_TYPE_DEPTH,
    //         //     &pci
    //         // );
    //         // std::cout << "end transform\n";

    //         auto cloudBuffer = reinterpret_cast<Pt3<int16_t>*>(pci.get_buffer());
    //         auto colorBuffer = reinterpret_cast<const Pt4<uint8_t>*>(depthSizedColorImage.get_buffer());
    //         auto depthBuffer = reinterpret_cast<int16_t*>(k4aDepthImage.get_buffer());

    //         size_t validDepth = 0;
    //         for(size_t id = 0; id < k4aDepthImage.get_width_pixels()*k4aDepthImage.get_height_pixels(); ++id){
    //             if(depthBuffer[id] != 0){
    //                 ++validDepth;
    //             }
    //         }

    //         std::cout << "--> validDepth " << validDepth << " "<< k4aDepthImage.get_size() << "\n";

    //         ColoredCloudData cloud;
    //         cloud.vertices.resize(validDepth);
    //         cloud.colors.resize(validDepth);
    //         size_t id = 0;
    //         for(size_t idV = 0; idV < pci.get_width_pixels()*pci.get_height_pixels(); ++idV){

    //             if(depthBuffer[idV] == 0){
    //                 continue;
    //             }

    //             cloud.vertices[id] = Pt3f{
    //                 static_cast<float>(-cloudBuffer[idV].x()),
    //                 static_cast<float>(-cloudBuffer[idV].y()),
    //                 static_cast<float>( cloudBuffer[idV].z())
    //             }*0.001f;
    //             cloud.colors[id] = Pt3f{
    //                 static_cast<float>(colorBuffer[idV].x()),
    //                 static_cast<float>(colorBuffer[idV].y()),
    //                 static_cast<float>(colorBuffer[idV].z())
    //             }/255.f;

    //             if(rand()%10000 == 0){
    //                 std::cout << "v " << id << " "<< cloud.vertices[id] << "  " << cloud.colors[id] << "\n";
    //             }

    //             ++id;

    //             // cloud.colors[idV] = Pt3f{
    //             //     static_cast<float>(colorBuffer[idV].z()),
    //             //     static_cast<float>(colorBuffer[idV].y()),
    //             //     static_cast<float>(colorBuffer[idV].x())
    //             // }/255.f;
    //         }
    //         std::cout << "cloud " << cloud.has_colors() << " " << cloud.is_valid() << " " << cloud.size() << "\n";

    //         std::cout << "CLOUD res " << tool::io::CloudIO::save_cloud(
    //             std::format("D:/ob_{}_cloud.obj", idFr),
    //             cloud
    //             ) << "\n";
    //     }

    //     ++idFr;
    // }

    // pipe.stop();


    // std::cout << "point cloud:\n";
    // ob::PointCloudFilter pointCloud;
    // auto cameraParam = pipe.getCameraParam();
    // pointCloud.setCameraParam(cameraParam);

    // // Create a window for rendering, and set the resolution of the window
    // // Window app("DepthViewer", depthProfile->width(), depthProfile->height());

    // // 1024 x 1024 : 0.25m – 2.21m
    // // 512  x 512  : 0.25m –2.88m
    // // 640  x 576  : 0.5m – 3.86m
    // // 320  x 288  : 0.5m – 5.46m
    // int idFr = 0;
    // int maxNbFrames = 3;

    // std::cout << "loop:\n";
    // cv::Mat rstMat;
    // while(idFr < maxNbFrames) {

    //     // Wait for up to 100ms for a frameset in blocking mode.
    //     auto frameSet = pipe.waitForFrames(100);
    //     if(frameSet == nullptr) {
    //         continue;
    //     }

    //     auto colorFrame = frameSet->colorFrame();
    //     auto depthFrame = frameSet->depthFrame();
    //     auto infraFrame = frameSet->irFrame();


    //     uint32_t  cwidth  = 0;
    //     uint32_t  cheight = 0;
    //     if(colorFrame){
    //         cwidth  = colorFrame->width();
    //         cheight = colorFrame->height();
    //     }

    //     uint32_t  iwidth  = 0;
    //     uint32_t  iheight = 0;
    //     if(infraFrame){
    //         iwidth  = infraFrame->width();
    //         iheight = infraFrame->height();
    //     }

    //     uint32_t  dwidth  = 0;
    //     uint32_t  dheight = 0;
    //     if(infraFrame){
    //         dwidth  = depthFrame->width();
    //         dheight = depthFrame->height();
    //     }

    //     std::cout << "width d: " << dwidth  << " height d: " << dheight << " ";
    //     std::cout << "width c: " << cwidth  << " height c: " << cheight << " ";
    //     std::cout << "width i: " << iwidth  << " height i: " << iheight << " ";

    //     // depth
    //     {

    //         uint16_t *data   = (uint16_t *)depthFrame->data();
    //         int valid = 0;
    //         int invalid = 0;

    //         int min = 50000000;
    //         int max = 0;
    //         for(int ii = 0; ii < dwidth*dheight; ++ii){
    //             if(min > data[ii]){
    //                 min = data[ii];
    //             }
    //             if(max < data[ii]){
    //                 max = data[ii];
    //             }

    //             if(data[ii] == 0){
    //                 invalid++;
    //             }else{
    //                 valid++;
    //             }
    //         }
    //         std::cout << "min " << min  << " max " << max << " ";
    //         std::cout << "valid: " << valid << " invalid " << invalid << "\n";

    //         // write depth map
    //         {
    //             cv::Mat cvtMat;
    //             cv::Mat rawMat = cv::Mat(depthFrame->height(), depthFrame->width(), CV_16UC1, depthFrame->data());
    //             // depth frame pixel value multiply scale to get distance in millimeter
    //             float scale = depthFrame->getValueScale();
    //             // threshold to 5.12m
    //             cv::threshold(rawMat, cvtMat, 5120.0f / scale, 0, cv::THRESH_TRUNC);
    //             cvtMat.convertTo(cvtMat, CV_8UC1, scale * 0.05);
    //             cv::applyColorMap(cvtMat, rstMat, cv::COLORMAP_JET);
    //             cv::imwrite(std::format("D:/ob_img_{}.png", idFr).c_str(),rstMat);
    //         }
    //     }

    //     if(depthFrame != nullptr && colorFrame != nullptr){

    //         auto depthValueScale = depthFrame->getValueScale();
    //         std::cout << "depthValueScale " << depthValueScale << "\n";
    //         pointCloud.setPositionDataScaled(depthValueScale);
    //         pointCloud.setCreatePointFormat(OB_FORMAT_RGB_POINT);

    //         try {
    //             // Generate a colored point cloud and save it
    //             std::cout << "Save RGBD PointCloud ply file..." << std::endl;
    //             std::shared_ptr<ob::Frame> frame = pointCloud.process(frameSet);

    //             int pointsSize = frame->dataSize() / sizeof(OBPoint);
    //             std::cout << "points size: " << pointsSize << "\n";
    //             std::cout << "depth map size: " << dwidth * dheight << "\n";


    //             saveRGBPointsToPly(frame, std::format("D:/rgb_cloud_{}.ply", idFr));
    //             // savePointsToPly(frame, std::format("D:/rgb_cloud_{}.ply", idFr));
    //             std::cout << "RGBPoints.ply Saved" << std::endl;
    //         }
    //         catch(std::exception &e) {
    //             std::cout << "Get point cloud failed" << std::endl;
    //         }
    //     }
    //     ++idFr;
    // }

    // // Stop the pipeline
    // pipe.stop();

    // return;
}

auto test_orbbec_femto() -> void {

    // using namespace tool;
    // using namespace tool::cam;
    // std::puts("### Test femto orbbec.\n");

    // DCDeviceManager dManager;

    // tool::cam::DCDeviceSettings ds;

    // // config
    // ds.configS.openDevice                      = true;
    // ds.configS.startReading                    = true;
    // ds.configS.typeDevice                       = DCType::FemtoBolt;
    // ds.configS.idDevice                         = 0;
    // ds.configS.disableLED                       = false;
    // ds.configS.synchMode                        = DCSynchronisationMode::Standalone;
    // ds.configS.mode                             = DCMode::FB_C2048x1536_DI640x576_MJPG_F30;
    // ds.configS.delayBetweenColorAndDepthUsec    = 0;
    // ds.configS.synchronizeColorAndDepth         = true;
    // // data
    // // ds.dataS.generateRGBLocalFrame   = true;
    // // ds.dataS.generateDepthLocalFrame = true;
    // // ds.dataS.generateInfraLocalFrame = true;
    // // ds.dataS.generateCloudLocal      = true;

    // std::puts("### Open device.\n");
    // dManager.update_device_settings(ds);

    // int idC = 0;
    // dManager.new_frame_signal.connect([&](std::shared_ptr<DCFrame> frame){
    //     std::cout << "save cloud: " << frame->cloud.vertices.size() << " " << frame->cloud.has_vertices() << " " << frame->cloud.has_colors() << " " << frame->cloud.has_normals() << "\n";
    //     tool::io::CloudIO::save_cloud(std::format("D:/cloud_{}.obj", idC++), frame->cloud);
    // });
    // std::this_thread::sleep_for(std::chrono::milliseconds(2000));

    // ds.configS.startReading  = false;
    // ds.configS.openDevice = false;

    // std::puts("### Close device.\n");
    // dManager.update_device_settings(ds);

    // std::puts("End tests.\n");
}


auto test_femto_mega() -> void{

    using namespace tool;
    using namespace tool::cam;
    std::puts("### Test femto mega.\n");

    DCDevice device;

    tool::cam::DCDeviceSettings ds;

    // config
    ds.configS.openDevice                      = true;
    ds.configS.startReading                    = true;
    ds.configS.typeDevice                       = DCType::FemtoMegaEthernet;
    ds.configS.idDevice                         = 0;
    ds.configS.disableLED                       = false;
    ds.configS.synchMode                        = DCSynchronisationMode::Standalone;
    ds.configS.mode                             = DCMode::FME_C1280x720_DI640x576_MJPG_F30;
    ds.configS.delayBetweenColorAndDepthUsec    = 0;
    ds.configS.synchronizeColorAndDepth         = true;
    // data
    // ds.dataS.generateRGBLocalFrame   = true;
    // ds.dataS.generateDepthLocalFrame = true;
    // ds.dataS.generateInfraLocalFrame = true;
    // ds.dataS.generateCloudLocal      = true;

    // ds.dataS.sendCloud = false;
    // ds.dataS.sendBodies= false;
    // ds.dataS.sendAudio= false;
    // ds.dataS.sendIMU= false;
    // ds.dataS.sendInfra= false;
    // ds.dataS.sendColor= false;
    // ds.dataS.sendDepth= false;

    std::puts("### Open device.\n");
    device.update_device_settings(ds);

    int idC = 0;
    device.new_frame_signal.connect([&](std::shared_ptr<DCFrame2> frame){
        // std::cout << "save cloud: " << frame->cloud.vertices.size() << " " << frame->cloud.has_vertices() << " " << frame->cloud.has_colors() << " " << frame->cloud.has_normals() << "\n";
        // tool::io::CloudIO::save_cloud(std::format("D:/fm_cloud_{}.obj", idC++), frame->cloud);
    });
    std::this_thread::sleep_for(std::chrono::milliseconds(4000));

    ds.configS.startReading  = false;
    ds.configS.openDevice = false;

    std::puts("### Close device.\n");
    device.update_device_settings(ds);

    std::this_thread::sleep_for(std::chrono::milliseconds(2000));

    std::puts("End tests.\n");
}
