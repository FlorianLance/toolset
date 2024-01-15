
#pragma once


auto test_compress_frame() -> void{

    // DCDeviceManager m;

    // DCDeviceSettings ds;
    // ds.actionsS.startReading  = true;
    // ds.actionsS.openDevice = true;

    // ds.configS.idDevice = 2;
    // ds.configS.mode = DCMode::K4_CLOUD_C1280x720_DI640x576_NV12_F30;
    // ds.configS.subordinateDelayUsec = 0;

    // ds.dataS.sendCloud = true;
    // ds.dataS.sendColor = true;

    // m.update_device_settings(ds);

    // DCFrameUncompressor uc;


    // m.new_compressed_frame_signal.connect([&](std::shared_ptr<DCCompressedFrame> cFrame){
    //     std::cout << "cframe " << cFrame->idCapture << "\n";

    //     size_t totalDataSizeBytes = cFrame->total_data_size();

    //     std::vector<std::int8_t> bufferToSend;
    //     bufferToSend.resize(totalDataSizeBytes);

    //     size_t offset = 0;
    //     cFrame->write_to_data(bufferToSend.data(), offset, bufferToSend.size());

    //     auto cFrame2 = std::make_shared<DCCompressedFrame>();
    //     offset = 0;
    //     cFrame2->init_from_data(bufferToSend.data(), offset, totalDataSizeBytes);

    //     DCFrame frame;
    //     uc.uncompress(cFrame2.get(), frame);

    //     std::cout << "frame " << frame.idCapture << "\n" << std::flush;
    // });


    // for(size_t ii = 0; ii < 100; ++ii){
    //     std::this_thread::sleep_for(std::chrono::milliseconds(33));
    // }

}



// // frames processing thread
// void processFrames() {
//     std::vector<cv::Mat>                    mats;
//     cv::Mat                                 imuMat;
//     cv::Mat                                 rstMat;
//     std::vector<std::shared_ptr<ob::Frame>> frames;
//     while(!windowClose_) {
//         {
//             std::unique_lock<std::mutex> lk(srcFramesMtx_);
//             srcFramesCv_.wait(lk, [this] { return !srcFrames_.empty() || windowClose_; });
//             if(windowClose_) {
//                 break;
//             }
//             frames = srcFrames_;
//             srcFrames_.clear();
//         }
//         for(auto frame: frames) {
//             rstMat.release();
//             if(frame == nullptr) {
//                 mats.clear();
//                 imuMat.release();
//                 break;
//             }
//             if(frame->type() == OB_FRAME_COLOR) {
//                 auto videoFrame = frame->as<ob::VideoFrame>();
//                 switch(videoFrame->format()) {
//                 case OB_FORMAT_MJPG: {
//                     cv::Mat rawMat(1, videoFrame->dataSize(), CV_8UC1, videoFrame->data());
//                     rstMat = cv::imdecode(rawMat, 1);
//                 } break;
//                 case OB_FORMAT_NV21: {
//                     cv::Mat rawMat(videoFrame->height() * 3 / 2, videoFrame->width(), CV_8UC1, videoFrame->data());
//                     cv::cvtColor(rawMat, rstMat, cv::COLOR_YUV2BGR_NV21);
//                 } break;
//                 case OB_FORMAT_YUYV:
//                 case OB_FORMAT_YUY2: {
//                     cv::Mat rawMat(videoFrame->height(), videoFrame->width(), CV_8UC2, videoFrame->data());
//                     cv::cvtColor(rawMat, rstMat, cv::COLOR_YUV2BGR_YUY2);
//                 } break;
//                 case OB_FORMAT_RGB: {
//                     cv::Mat rawMat(videoFrame->height(), videoFrame->width(), CV_8UC3, videoFrame->data());
//                     cv::cvtColor(rawMat, rstMat, cv::COLOR_RGB2BGR);
//                 } break;
//                 case OB_FORMAT_UYVY: {
//                     cv::Mat rawMat(videoFrame->height(), videoFrame->width(), CV_8UC2, videoFrame->data());
//                     cv::cvtColor(rawMat, rstMat, cv::COLOR_YUV2BGR_UYVY);
//                 } break;
//                 default:
//                     break;
//                 }
//                 if(showInfo_ && !rstMat.empty()) {
//                     drawInfo(rstMat, videoFrame);
//                 }
//             }
//             else if(frame->type() == OB_FRAME_DEPTH) {
//                 auto videoFrame = frame->as<ob::VideoFrame>();
//                 if(videoFrame->format() == OB_FORMAT_Y16) {
//                     cv::Mat cvtMat;
//                     cv::Mat rawMat = cv::Mat(videoFrame->height(), videoFrame->width(), CV_16UC1, videoFrame->data());
//                     // depth frame pixel value multiply scale to get distance in millimeter
//                     float scale = videoFrame->as<ob::DepthFrame>()->getValueScale();

//                     // threshold to 5.12m
//                     cv::threshold(rawMat, cvtMat, 5120.0f / scale, 0, cv::THRESH_TRUNC);
//                     cvtMat.convertTo(cvtMat, CV_8UC1, scale * 0.05);
//                     cv::applyColorMap(cvtMat, rstMat, cv::COLORMAP_JET);
//                 }
//                 if(showInfo_ && !rstMat.empty()) {
//                     drawInfo(rstMat, videoFrame);
//                 }
//             }
//             else if(frame->type() == OB_FRAME_IR || frame->type() == OB_FRAME_IR_LEFT || frame->type() == OB_FRAME_IR_RIGHT) {
//                 auto videoFrame = frame->as<ob::VideoFrame>();
//                 if(videoFrame->format() == OB_FORMAT_Y16) {
//                     cv::Mat cvtMat;
//                     cv::Mat rawMat = cv::Mat(videoFrame->height(), videoFrame->width(), CV_16UC1, videoFrame->data());
//                     rawMat.convertTo(cvtMat, CV_8UC1, 1.0 / 16.0f);
//                     cv::cvtColor(cvtMat, rstMat, cv::COLOR_GRAY2RGB);
//                 }
//                 else if(videoFrame->format() == OB_FORMAT_Y8) {
//                     cv::Mat rawMat = cv::Mat(videoFrame->height(), videoFrame->width(), CV_8UC1, videoFrame->data());
//                     cv::cvtColor(rawMat * 2, rstMat, cv::COLOR_GRAY2RGB);
//                 }
//                 else if(videoFrame->format() == OB_FORMAT_MJPG) {
//                     cv::Mat rawMat(1, videoFrame->dataSize(), CV_8UC1, videoFrame->data());
//                     rstMat = cv::imdecode(rawMat, 1);
//                     cv::cvtColor(rstMat * 2, rstMat, cv::COLOR_GRAY2RGB);
//                 }
//                 if(showInfo_ && !rstMat.empty()) {
//                     drawInfo(rstMat, videoFrame);
//                 }
//             }
//             else if(frame->type() == OB_FRAME_ACCEL) {
//                 if(imuMat.empty()) {
//                     imuMat = cv::Mat::zeros(640, 360, CV_8UC3);
//                 }
//                 auto        accelFrame = frame->as<ob::AccelFrame>();
//                 auto        value      = accelFrame->value();
//                 std::string str        = "Accel:";
//                 cv::putText(imuMat, str.c_str(), cv::Point(8, 60), cv::FONT_HERSHEY_DUPLEX, 0.6, cv::Scalar(255, 255, 255), 0.5, cv::LINE_AA);
//                 str = std::string(" x=") + std::to_string(value.x) + "m/s^2";
//                 cv::putText(imuMat, str.c_str(), cv::Point(8, 120), cv::FONT_HERSHEY_DUPLEX, 0.6, cv::Scalar(255, 255, 255), 0.5, cv::LINE_AA);
//                 str = std::string(" y=") + std::to_string(value.y) + "m/s^2";
//                 cv::putText(imuMat, str.c_str(), cv::Point(8, 180), cv::FONT_HERSHEY_DUPLEX, 0.6, cv::Scalar(255, 255, 255), 0.5, cv::LINE_AA);
//                 str = std::string(" z=") + std::to_string(value.z) + "m/s^2";
//                 cv::putText(imuMat, str.c_str(), cv::Point(8, 240), cv::FONT_HERSHEY_DUPLEX, 0.6, cv::Scalar(255, 255, 255), 0.5, cv::LINE_AA);
//                 continue;
//             }
//             else if(frame->type() == OB_FRAME_GYRO) {
//                 if(imuMat.empty()) {
//                     imuMat = cv::Mat(640, 360, CV_8UC3);
//                 }
//                 auto        gyroFrame = frame->as<ob::GyroFrame>();
//                 auto        value     = gyroFrame->value();
//                 std::string str       = "Gyro:";
//                 cv::putText(imuMat, str.c_str(), cv::Point(8, 300), cv::FONT_HERSHEY_DUPLEX, 0.6, cv::Scalar(255, 255, 255), 0.5, cv::LINE_AA);
//                 str = std::string(" x=") + std::to_string(value.x) + "rad/s";
//                 cv::putText(imuMat, str.c_str(), cv::Point(8, 360), cv::FONT_HERSHEY_DUPLEX, 0.6, cv::Scalar(255, 255, 255), 0.5, cv::LINE_AA);
//                 str = std::string(" y=") + std::to_string(value.y) + "rad/s";
//                 cv::putText(imuMat, str.c_str(), cv::Point(8, 420), cv::FONT_HERSHEY_DUPLEX, 0.6, cv::Scalar(255, 255, 255), 0.5, cv::LINE_AA);
//                 str = std::string(" z=") + std::to_string(value.z) + "rad/s";
//                 cv::putText(imuMat, str.c_str(), cv::Point(8, 480), cv::FONT_HERSHEY_DUPLEX, 0.6, cv::Scalar(255, 255, 255), 0.5, cv::LINE_AA);
//                 continue;
//             }
//             if(!rstMat.empty()) {
//                 mats.push_back(rstMat);
//             }
//         }

//         if(!imuMat.empty()) {
//             mats.push_back(imuMat);
//             imuMat.release();
//         }

//         if(!mats.empty()) {
//             std::lock_guard<std::mutex> lock(renderMatsMtx_);
//             renderMats_ = mats;
//             mats.clear();
//         }
//     }
// }
