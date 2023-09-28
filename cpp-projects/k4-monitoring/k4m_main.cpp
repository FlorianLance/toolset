

// std
#include <iostream>
#include <format>

// opencv
#include <opencv2/core.hpp> // Basic OpenCV structures (cv::Mat)
#include <opencv2/videoio.hpp> // Video write
// base
#include "camera/kinect4/k4_device.hpp"

#include "camera/kinect4/k4a/k4aaudiomanager.h"


#include <QVideoFrame>
#include <QVideoFrameFormat>
#include <QCoreApplication>
#include <QMediaRecorder>

#include "vwutils.h"

auto test_k4device() -> void{

    std::cout << "test_k4device\n";

////    cv::VideoWriter outputVideo("D:/DEV/test-mpeg.mp4", cv::VideoWriter::fourcc('M', 'P', 'E', 'G'), 30, cv::Size2i(1000,1000));
//    cv::VideoWriter outputVideo("D:/DEV/test-hevc.mp4", cv::VideoWriter::fourcc('H', 'E', 'V', 'C'), 30, cv::Size2i(1000,1000));
////     cv::VideoWriter outputVideo("D:/DEV/test-h264.mp4", cv::VideoWriter::fourcc('H', '2', '6', '4'), 30, cv::Size2i(1000,1000));
////     cv::VideoWriter outputVideo("D:/DEV/test-.mp4", -1, 30, cv::Size2i(1000,1000));


//    std::cout << "video opened: " << outputVideo.isOpened() << "\n";
////    outputVideo.open
//    if (!outputVideo.isOpened())
//    {
//        std::cout << "Could not open the output video for write:\n";
//        return ;
//    }

//    for(int ii = 0; ii < 255; ++ii){
//        cv::Mat m(cv::Size2i(1000,1000), CV_8UC3);
//        std::fill(m.data, m.data + 1000*1000*3, ii);
//        outputVideo << m;
//    }

//    std::cout << "release\n";
//    outputVideo.release();
//    return;

    using namespace tool::camera;
//    std::vector<std::unique_ptr<K4Device>> devices;

////    std::cout << "audio " << k4a::K4AAudioManager::in().Initialize() << "\n";
////    std::cout << "nb devices " << k4a::K4AAudioManager::Instance().get_devices_count() << "\n";


//    for(size_t idD = 0; idD < 1; ++idD){
//        std::cout << std::format("### Open device {}.\n", std::to_string(idD));
//        devices.push_back(std::make_unique<K4Device>());

//        auto device = devices.back().get();

//        device->open(idD);

//        K4ConfigSettings cs;
//        cs.idDevice = idD;
//        cs.synchronizeColorAndDepth = true;

////        if(!device->is_sync_in_connected() && device->is_sync_out_connected()){
////            cs.synchMode = K4SynchronisationMode::Master;
////        }else if(device->is_sync_in_connected()){
////            cs.synchMode = K4SynchronisationMode::Subordinate;
////            cs.subordinateDelayUsec = 100;
////        }else{
////            cs.synchMode = K4SynchronisationMode::Standalone;
////        }
//        cs.synchMode = K4SynchronisationMode::Standalone;

//        cs.mode = K4Mode::Only_color_2048x1536;
//        device->start_cameras(cs);

//        K4DataSettings ds;
//        ds.captureAudio             = true;
//        ds.captureIMU               = false;
//        ds.captureBodies            = false;
//        ds.sendColor                = false;
//        ds.sendDepth                = false;
//        ds.sendInfra                = false;
//        ds.sendCloud                = false;
//        ds.sendIMU                  = false;
//        ds.sendAudio                = false;
//        ds.sendBodies               = false;
//        ds.generateRGBLocalFrame    = true;
//        ds.generateDepthLocalFrame  = true;
//        ds.generateInfraLocalFrame  = true;
//        ds.generateCloudLocal       = false;
//        device->set_data_settings(ds);
//    }



//    outputVideo = std::make_unique<cv::VideoWriter>("D:/DEV/test-h264.mp4", cv::CAP_FFMPEG, cv::VideoWriter::fourcc('a', 'v', 'c', '1'), 30, cv::Size2i(frame->colorWidth,frame->colorHeight));

    int mjpg = cv::VideoWriter::fourcc('M', 'J', 'P', 'G');
    int avc1 = cv::VideoWriter::fourcc('a', 'v', 'c', '1');

    std::vector<std::tuple<int,std::string, K4Mode>> vInfos={
        {mjpg, "mjpg_1280x720.avi",  K4Mode::Only_color_1280x720},
        {mjpg, "mjpg_1920x1080.avi", K4Mode::Only_color_1920x1080},
        {mjpg, "mjpg_2560x1440.avi", K4Mode::Only_color_2560x1440},
        {mjpg, "mjpg_2048x1536.avi", K4Mode::Only_color_2048x1536},
        {mjpg, "mjpg_3840x2160.avi", K4Mode::Only_color_3840x2160},
        {mjpg, "mjpg_4096x3072.avi", K4Mode::Only_color_4096x3072},
        {avc1, "avc1_1280x720.mp4",  K4Mode::Only_color_1280x720},
        {avc1, "avc1_1920x1080.mp4", K4Mode::Only_color_1920x1080},
        {avc1, "avc1_2560x1440.mp4", K4Mode::Only_color_2560x1440},
        {avc1, "avc1_2048x1536.mp4", K4Mode::Only_color_2048x1536},
        {avc1, "avc1_3840x2160.mp4", K4Mode::Only_color_3840x2160},
        {avc1, "avc1_4096x3072.mp4", K4Mode::Only_color_4096x3072},
    };


//    std::unique_ptr<VideoWriter> writer = nullptr; //("filename.mp4", fps, width, height, encoder_name, bitrate);
//    writer.write(cvFrame); //cvFrame is a cv::Mat object


//    VideoWriter writer;
//    writer.open("appsrc ! videoconvert  !  avenc_mpeg4 bitrate=100000 ! mp4mux ! filesink location=video.mp4",CAP_GSTREAMER, 0, (double) 10, cv::Size(640, 480), true);


    // init device
    std::unique_ptr<K4Device> device = std::make_unique<K4Device>();
    device->open(0);

    K4DataSettings ds;
    ds.captureAudio             = true;
    ds.captureIMU               = false;
    ds.captureBodies            = false;
    ds.sendColor                = false;
    ds.sendDepth                = false;
    ds.sendInfra                = false;
    ds.sendCloud                = false;
    ds.sendIMU                  = false;
    ds.sendAudio                = false;
    ds.sendBodies               = false;
    ds.generateRGBLocalFrame    = true;
    ds.generateDepthLocalFrame  = true;
    ds.generateInfraLocalFrame  = true;
    ds.generateCloudLocal       = false;
    device->set_data_settings(ds);

    // signals
    std::vector<cv::Mat> frames;
    std::vector<std::shared_ptr<K4Frame>> fFrames;
    device->new_frame_signal.connect([&](std::shared_ptr<K4Frame> frame){

//        cv::Mat m(cv::Size2i(frame->colorWidth,frame->colorHeight), CV_8UC3);
//        for(size_t ii = 0; ii < frame->imageColorData.size(); ++ii){
//            auto &p = frame->imageColorData[ii];
//            auto d = m.data;
//            d[3*ii]   = p(0);
//            d[3*ii+1] = p(1);
//            d[3*ii+2] = p(2);
//        }
//        frames.push_back(m);
        fFrames.push_back(frame);
        std::cout << "f";
    });

    std::cout << "start\n";
    for(const auto &info : vInfos){

        K4ConfigSettings cs;
        cs.idDevice = 0;
        cs.synchronizeColorAndDepth = true;
        cs.synchMode = K4SynchronisationMode::Standalone;
        cs.mode  = std::get<2>(info);
        device->start_cameras(cs);

        frames.clear();

        // init video writer
        auto mode = std::get<2>(info);
        auto colRes = color_resolution(mode);
        auto w = color_width(colRes);
        auto h = color_height(colRes);
        auto f = framerate_value(framerate(mode));

        auto outputVideo = std::make_unique<cv::VideoWriter>(
            std::format("D:/DEV/of{}_{}", f, std::get<1>(info)).c_str(),
            cv::CAP_FFMPEG,
            std::get<0>(info),
            f,
            cv::Size2i(w,h)
        );

        auto writer = std::make_unique<VideoWriter>(
            std::format("D:/DEV/f{}_{}", f, std::get<1>(info)),
            f,
            w,
            h,
            true,
            "libopenh264"
        );

//        int countLoops = 100;
        while(fFrames.size() < 100){
            std::this_thread::sleep_for(std::chrono::milliseconds(5));
        }

        std::cout << "w:" << w << " h: " << h << " f: " << f << " m: " << (int)mode << "\n";
        for(const auto &f : fFrames){


            std::vector<std::uint8_t> data(f->colorWidth*f->colorHeight*3);
            for(size_t id = 0; id < f->imageColorData.size(); ++id){
                data[id*3+0] = f->imageColorData[id].x();
                data[id*3+1] = f->imageColorData[id].y();
                data[id*3+2] = f->imageColorData[id].z();
            }
            std::cout << "write frame: " << data.size() << "\n";
//            if(!writer->write(data.data())){

//            }

            cv::Mat m(cv::Size2i(f->colorWidth,f->colorHeight), CV_8UC3);
            for(size_t ii = 0; ii < f->imageColorData.size(); ++ii){
                auto &p = f->imageColorData[ii];
                auto d = m.data;
                d[3*ii]   = p(0);
                d[3*ii+1] = p(1);
                d[3*ii+2] = p(2);
            }

            (*outputVideo) << m;
        }

        device->stop_cameras();
        outputVideo->release();
    }


//    std::cout << cv::getBuildInformation() << "\n";


//    std::unique_ptr<cv::VideoWriter> outputVideo = nullptr;

////    ("D:/DEV/test-hevc.mp4", cv::VideoWriter::fourcc('H', 'E', 'V', 'C'), 30, cv::Size2i(1000,1000));

//    for(auto &device : devices){
//        device->new_frame_signal.connect([&](std::shared_ptr<K4Frame> frame){

////            std::cout << device->device_id() << " " << frame->idCapture << " " << frame->imageColorData.size() << " " << frame->colorHeight << " " << frame->colorWidth <<  " " << frame->depthWidth << " " << frame->depthHeight<< " " << frame->depthSizedColorWidth << " " << frame->depthSizedColorHeight << "\n";

////            cv::Mat m(cv::Size2i(frame->colorWidth,frame->colorHeight), CV_8UC4);
////            std::copy(reinterpret_cast<std::uint8_t*>(frame->imageColorData.data()), reinterpret_cast<std::uint8_t*>(frame->imageColorData.data()) + frame->colorWidth*frame->colorHeight*4, m.data);


//            cv::Mat m(cv::Size2i(frame->colorWidth,frame->colorHeight), CV_8UC3);
//            for(size_t ii = 0; ii < frame->imageColorData.size(); ++ii){
//                auto &p = frame->imageColorData[ii];
//                auto d = m.data;
//                d[3*ii]   = p(0);
//                d[3*ii+1] = p(1);
//                d[3*ii+2] = p(2);
//            }
////            std::copy(reinterpret_cast<std::uint8_t*>(frame->imageColorData.data()), reinterpret_cast<std::uint8_t*>(frame->imageColorData.data()) + frame->colorWidth*frame->colorHeight*4, m.data);



////            fourcc tag 0x7634706d/'mp4v' codec_id 000C
////            fourcc tag 0x31637661/'avc1' codec_id 001B
////            fourcc tag 0x33637661/'avc3' codec_id 001B
////            fourcc tag 0x31766568/'hev1' codec_id 00AD
////            fourcc tag 0x31637668/'hvc1' codec_id 00AD
////            fourcc tag 0x7634706d/'mp4v' codec_id 0002
////            fourcc tag 0x7634706d/'mp4v' codec_id 0001
////            fourcc tag 0x7634706d/'mp4v' codec_id 0007
////            fourcc tag 0x7634706d/'mp4v' codec_id 003D

//            if(outputVideo == nullptr){

//                // OK rgb8 1280x720 (5mo) 1920x1080 (10mo)
////                outputVideo = std::make_unique<cv::VideoWriter>("D:/DEV/test-h264.mp4", cv::CAP_FFMPEG, cv::VideoWriter::fourcc('a', 'v', 'c', '1'), 30, cv::Size2i(frame->colorWidth,frame->colorHeight));

//                // OK rgb8 1280x720 (9mo) 1920x1080 (18mo) 2048x1536 (13mo)
//                outputVideo = std::make_unique<cv::VideoWriter>("D:/DEV/test2.avi", cv::CAP_FFMPEG, cv::VideoWriter::fourcc('M', 'J', 'P', 'G'), 30, cv::Size2i(frame->colorWidth,frame->colorHeight)); // OK 1280x720 rgb8 (18mo


////                outputVideo = std::make_unique<cv::VideoWriter>("D:/DEV/test-h264.mkv", cv::CAP_FFMPEG, cv::VideoWriter::fourcc('H', '2', '6', '4'), 30, cv::Size2i(frame->colorWidth,frame->colorHeight)); // OK 1280x720 rgb8
////                outputVideo = std::make_unique<cv::VideoWriter>("D:/DEV/test-h264.mpeg", cv::CAP_FFMPEG,cv::VideoWriter::fourcc('H', '2', '6', '4'), 30, cv::Size2i(frame->colorWidth,frame->colorHeight)); // OK 1280x720 rgb8
////                outputVideo = std::make_unique<cv::VideoWriter>("D:/DEV/test-h264.mp4", cv::CAP_FFMPEG, cv::VideoWriter::fourcc('H', '2', '6', '4'), 30, cv::Size2i(frame->colorWidth,frame->colorHeight)); // OK 1280x720 rgb8

////                  outputVideo = std::make_unique<cv::VideoWriter>("D:/DEV/test-hevc.mkv", cv::VideoWriter::fourcc('H', 'E', 'V', 'C'), 30, cv::Size2i(frame->colorWidth,frame->colorHeight)); //
////                  outputVideo = std::make_unique<cv::VideoWriter>("D:/DEV/test-hevc.mpeg", cv::VideoWriter::fourcc('H', 'E', 'V', 'C'), 30, cv::Size2i(frame->colorWidth,frame->colorHeight)); //
////                  outputVideo = std::make_unique<cv::VideoWriter>("D:/DEV/test-hevc.mp4", cv::VideoWriter::fourcc('H', 'E', 'V', 'C'), 30, cv::Size2i(frame->colorWidth,frame->colorHeight)); //
////                    outputVideo = std::make_unique<cv::VideoWriter>("D:/DEV/test-hevc.avc1", cv::VideoWriter::fourcc('H', 'E', 'V', 'C'), 30, cv::Size2i(frame->colorWidth,frame->colorHeight)); //


////                  outputVideo = std::make_unique<cv::VideoWriter>("D:/DEV/test-h265.mp4", cv::VideoWriter::fourcc('H', '2', '6', '5'), 30, cv::Size2i(frame->colorWidth,frame->colorHeight)); //

////            outputVideo = std::make_unique<cv::VideoWriter>("D:/DEV/test-hvc1.mp4", cv::VideoWriter::fourcc('h', 'v', 'c', '1'), 30, cv::Size2i(frame->colorWidth,frame->colorHeight)); //

////             outputVideo = std::make_unique<cv::VideoWriter>("D:/DEV/test-h265.mkv", cv::CAP_GSTREAMER, cv::VideoWriter::fourcc('M','J','P','G'), 30, cv::Size2i(frame->colorWidth,frame->colorHeight)); //


////            outputVideo = std::make_unique<cv::VideoWriter>("D:/DEV/test-h264.mp4", cv::CAP_MSMF, cv::VideoWriter::fourcc('H', '2', '6', '4'), 30, cv::Size2i(frame->colorWidth,frame->colorHeight)); //

////            std::cout << cv::NTDDI_WIN10 << "\n";
////#if defined(NTDDI_WIN10)
////        case CV_FOURCC_MACRO('H', '2', '6', '5'):
////                return MFVideoFormat_H265;  break;
////        case CV_FOURCC_MACRO('H', 'E', 'V', 'C'):
////                return MFVideoFormat_HEVC;  break;




////                outputVideo = std::make_unique<cv::VideoWriter>("D:/DEV/test-hevc.mp4", cv::VideoWriter::fourcc('H', 'E', 'V', 'C'), 15, cv::Size2i(frame->colorWidth,frame->colorHeight));

////                outputVideo = std::make_unique<cv::VideoWriter>("D:/DEV/test-mpeg.mp4", cv::VideoWriter::fourcc('m', 'p', '4', 'v'), 15, cv::Size2i(frame->colorWidth,frame->colorHeight));
////                outputVideo = std::make_unique<cv::VideoWriter>("D:/DEV/test-h264.avc1", cv::VideoWriter::fourcc('H', '2', '6', '4'), 15, cv::Size2i(frame->colorWidth,frame->colorHeight));
////                outputVideo = std::make_unique<cv::VideoWriter>("D:/DEV/test-h264.mp4", cv::VideoWriter::fourcc('h', 'e', 'v', '1'), 15, cv::Size2i(frame->colorWidth,frame->colorHeight));
//            }
////            cv::Mat m(frame->c);
//            std::cout << "a";
//            (*outputVideo) << m;
//        });
//    }




//    int countLoops = 100;
//    for(int ii = 0; ii < countLoops; ++ii){
////        std::cout << "frame " << ii << "\n";


//        std::this_thread::sleep_for(std::chrono::milliseconds(30));
//    }
//    outputVideo->release();

//    std::cout << "Close devices.\n";
//    for(auto &device : devices){
//        device->stop_cameras();
//        device->close();
//    }
//    tool::camera::K4Device device;
//    std::puts("### Open device.\n");
//    if(device.open(0)){
//        tool::camera::K4ConfigSettings config;
//        config.idDevice = 0;
//        config.mode = tool::camera::K4Mode::Cloud_640x576_NV12;
//        config.synchronizeColorAndDepth = true;
//        config.delayBetweenColorAndDepthUsec = 0;
//        config.synchMode = tool::camera::K4SynchronisationMode::Standalone;
//        config.subordinateDelayUsec = 0;
//        config.disableLED = false;

//        std::puts("### Start cameras.\n");
//        device.start_cameras(config);
//        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
//        std::puts("### Stop cameras.\n");
//        device.stop_cameras();

//        std::puts("### Close device.\n");
//        device.close();
//    }else{
//        std::puts("### Device opening failure.\n");
//    }

//    std::puts("End tests.\n");
//    tool::Bench::display();
    std::puts("End tests.\n");
}

#include "av.h"
#include "ffmpeg.h"
#include "codec.h"
#include "packet.h"
#include "videorescaler.h"
#include "audioresampler.h"
#include "avutils.h"

// API2
#include "format.h"
#include "formatcontext.h"
#include "codec.h"
#include "codeccontext.h"

int main(int argc, char *argv[]){

    // https://github.com/h4tr3d/avcpp/blob/master/example/api2-samples/api2-decode-encode-video.cpp
    av::Codec ocodec  = av::findEncodingCodec("hevc_amf");
    av::VideoEncoderContext encoder {ocodec};
    encoder.setWidth(1920);
    encoder.setHeight(1080);
    encoder.setTimeBase(av::Rational{1, 1000});
    encoder.setBitRate(5000000);

    // Settings
    auto name = av_get_pix_fmt_name(AV_PIX_FMT_YUV420P);  
    av::PixelFormat px(name);
    encoder.setPixelFormat(px);

    std::cout << "codec: " << ocodec.longName() << " "  <<  "\n";

    std::error_code ec;
    encoder.open(ocodec, ec);
    if (ec) {
       std::cerr << "Can't open encodec\n";
       return 1;
    }

    av::OutputFormat  ofrmt;
    av::FormatContext octx;
    ofrmt.setFormat(std::string(), "D:/DEV/test.mp4");
    octx.setFormat(ofrmt);

    av::Stream ost = octx.addStream(encoder);
    ost.setFrameRate(30);

    octx.openOutput("D:/DEV/test.mp4", ec);
    if (ec) {
        std::cerr << "Can't open output\n";
        return 1;
    }

    octx.dump();
    octx.writeHeader();
    octx.flush();


    // init device

    using namespace tool::camera;

    int mjpg = cv::VideoWriter::fourcc('M', 'J', 'P', 'G');
    int avc1 = cv::VideoWriter::fourcc('a', 'v', 'c', '1');

    std::unique_ptr<K4Device> device = std::make_unique<K4Device>();
    device->open(0);

    K4ConfigSettings cs;
    cs.idDevice                 = 0;
    cs.synchronizeColorAndDepth = true;
    cs.synchMode                = K4SynchronisationMode::Standalone;
    cs.mode                     = K4Mode::Only_color_1920x1080;
    device->start_cameras(cs);



    return 0;


//    av::VideoFrame vf(...);
//    encoder.encode(vf);



//    std::vector<std::tuple<int,std::string, K4Mode>> vInfos={
//        {mjpg, "mjpg_1280x720.avi",  K4Mode::Only_color_1280x720},
//        {mjpg, "mjpg_1920x1080.avi", K4Mode::Only_color_1920x1080},
//        {mjpg, "mjpg_2560x1440.avi", K4Mode::Only_color_2560x1440},
//        {mjpg, "mjpg_2048x1536.avi", K4Mode::Only_color_2048x1536},
//        {mjpg, "mjpg_3840x2160.avi", K4Mode::Only_color_3840x2160},
//        {mjpg, "mjpg_4096x3072.avi", K4Mode::Only_color_4096x3072},
//        {avc1, "avc1_1280x720.mp4",  K4Mode::Only_color_1280x720},
//        {avc1, "avc1_1920x1080.mp4", K4Mode::Only_color_1920x1080},
//        {avc1, "avc1_2560x1440.mp4", K4Mode::Only_color_2560x1440},
//        {avc1, "avc1_2048x1536.mp4", K4Mode::Only_color_2048x1536},
//        {avc1, "avc1_3840x2160.mp4", K4Mode::Only_color_3840x2160},
//        {avc1, "avc1_4096x3072.mp4", K4Mode::Only_color_4096x3072},
//    };

//    // init device
//    std::unique_ptr<K4Device> device = std::make_unique<K4Device>();
//    device->open(0);


//    K4DataSettings ds;
//    ds.captureAudio             = true;
//    ds.captureIMU               = false;
//    ds.captureBodies            = false;
//    ds.sendColor                = false;
//    ds.sendDepth                = false;
//    ds.sendInfra                = false;
//    ds.sendCloud                = false;
//    ds.sendIMU                  = false;
//    ds.sendAudio                = false;
//    ds.sendBodies               = false;
//    ds.generateRGBLocalFrame    = true;
//    ds.generateDepthLocalFrame  = true;
//    ds.generateInfraLocalFrame  = true;
//    ds.generateCloudLocal       = false;
//    device->set_data_settings(ds);

//    // signals
//    std::vector<cv::Mat> frames;
//    std::vector<std::shared_ptr<K4Frame>> fFrames;
//    device->new_frame_signal.connect([&](std::shared_ptr<K4Frame> frame){
//        fFrames.push_back(frame);
//    });

//    // main loop
//    for(const auto &info : vInfos){

//        K4ConfigSettings cs;
//        cs.idDevice = 0;
//        cs.synchronizeColorAndDepth = true;
//        cs.synchMode = K4SynchronisationMode::Standalone;
//        cs.mode  = std::get<2>(info);
//        device->start_cameras(cs);

//        frames.clear();

//        // init video writer
//        auto mode = std::get<2>(info);
//        auto colRes = color_resolution(mode);
//        auto w = color_width(colRes);
//        auto h = color_height(colRes);
//        auto f = framerate_value(framerate(mode));


////        encoder.pixelFormat();
////        return 0;
////        AV_PIX_FMT_ARGB

////        auto outputVideo = std::make_unique<cv::VideoWriter>(
////            std::format("D:/DEV/of{}_{}", f, std::get<1>(info)).c_str(),
////            cv::CAP_FFMPEG,
////            std::get<0>(info),
////            f,
////            cv::Size2i(w,h)
////        );

//        // wait for frames
//        int countLoops = 100;
//        while(fFrames.size() < countLoops){
//            std::this_thread::sleep_for(std::chrono::milliseconds(5));
//        }

//        std::cout << "w:" << w << " h: " << h << " f: " << f << " m: " << (int)mode << "\n";

//        // save frames to video
//        for(const auto &f : fFrames){

//        }
//    }





//    return 0;




//    QCoreApplication app(argc, argv);



//    QVideoFrame vf;

//    auto format = QVideoFrameFormat(QSize(640,576), QVideoFrameFormat::PixelFormat::Format_ABGR8888);
//    auto frame = QVideoFrame(format);
//    frame.map(QVideoFrame::ReadWrite);

//    QMediaRecorder mr;

//    void QVideoSink::setVideoFrame(const QVideoFrame &frame)

//    QImage img;
//    memcpy(frame.bits(), img.bits(), img.sizeInBytes());


//    QMediaRecorder mr;



//    QVideoFrameFormat f;
//    f.setFrameRate(30);


//    return app.exec();

    test_k4device();

    return 0;
}
