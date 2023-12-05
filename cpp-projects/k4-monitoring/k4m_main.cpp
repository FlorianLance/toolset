

// std
#include <iostream>
#include <format>
#include <fstream>

// opencv
#include <opencv2/core.hpp> // Basic OpenCV structures (cv::Mat)
#include <opencv2/videoio.hpp> // Video write

// thirdparty
#include "thirdparty/k4a/k4aaudiomanager.h"

// base
#include "camera/kinect4/k4_device.hpp"



//#include <QVideoFrame>
//#include <QVideoFrameFormat>
//#include <QCoreApplication>
//#include <QMediaRecorder>

//#include "vwutils.h"



//#include "av.h"
//#include "ffmpeg.h"
//#include "codec.h"
//#include "packet.h"
//#include "videorescaler.h"
//#include "audioresampler.h"
//#include "avutils.h"

// API2
//#include "format.h"
//#include "formatcontext.h"
//#include "codec.h"

//#include "codeccontext.h"
//#include <stdio.h>
//#include <stdlib.h>
//#include <string.h>

// ffmepg
extern "C"
{
    #include <libavutil/avutil.h>
    #include <libavutil/parseutils.h>
    #include <libavutil/mathematics.h>
    #include <libavutil/opt.h>
    #include <libavutil/pixdesc.h>
    #include <libavdevice/avdevice.h>
    #include <libswscale/swscale.h>
    #include <libswresample/swresample.h>
    #include <libavformat/version.h>

    #include <libavformat/avformat.h>
    #include <libavcodec/avcodec.h>
}

static void encode(AVCodecContext *enc_ctx, AVFrame *frame, AVPacket *pkt, FILE *outfile){

    int ret;
    ret = avcodec_send_frame(enc_ctx, frame);
    if (ret < 0) {
        fprintf(stderr, "Error sending a frame for encoding\n");
        exit(1);
    }

    while (ret >= 0) {
        ret = avcodec_receive_packet(enc_ctx, pkt);
        if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)
            return;
        else if (ret < 0) {
            fprintf(stderr, "Error during encoding\n");
            exit(1);
        }

//        myFile.write(reinterpret_cast<char*>(&magicn), sizeof(magicn));

//        outfile->write(reinterpret_cast<char*>(pkt->data), pkt->size);
        fwrite(pkt->data, 1, pkt->size, outfile);
        av_packet_unref(pkt);
    }
}


auto test_encode_video() -> void{

//    [libx265 @ 0000021516f5ec80] Specified pixel format nv12 is not supported by the libx265 encoder.
//    [libx265 @ 0000021516f5ec80] Supported pixel formats:
//    [libx265 @ 0000021516f5ec80]   yuv420p
//    [libx265 @ 0000021516f5ec80]   yuvj420p
//    [libx265 @ 0000021516f5ec80]   yuv422p
//    [libx265 @ 0000021516f5ec80]   yuvj422p
//    [libx265 @ 0000021516f5ec80]   yuv444p
//    [libx265 @ 0000021516f5ec80]   yuvj444p
//    [libx265 @ 0000021516f5ec80]   gbrp
//    [libx265 @ 0000021516f5ec80]   yuv420p10le
//    [libx265 @ 0000021516f5ec80]   yuv422p10le
//    [libx265 @ 0000021516f5ec80]   yuv444p10le
//    [libx265 @ 0000021516f5ec80]   gbrp10le
//    [libx265 @ 0000021516f5ec80]   yuv420p12le
//    [libx265 @ 0000021516f5ec80]   yuv422p12le
//    [libx265 @ 0000021516f5ec80]   yuv444p12le
//    [libx265 @ 0000021516f5ec80]   gbrp12le
//    [libx265 @ 0000021516f5ec80]   gray
//    [libx265 @ 0000021516f5ec80]   gray10le
//    [libx265 @ 0000021516f5ec80]   gray12le

    // I420ToARGB
//    // Convert ARGB To YUY2.
//    LIBYUV_API
//    int ARGBToYUY2(const uint8_t* src_argb,
//                   int src_stride_argb,
//                   uint8_t* dst_yuy2,
//                   int dst_stride_yuy2,
//                   int width,
//                   int height);

    // Convert ARGB To NV12.
//    LIBYUV_API
//    int ARGBToNV12(const uint8_t* src_argb,
//                   int src_stride_argb,
//                   uint8_t* dst_y,
//                   int dst_stride_y,
//                   uint8_t* dst_uv,
//                   int dst_stride_uv,
//                   int width,
//                   int height);
    // ARGBToNV21
    // ABGRToNV12
    // ARGBToYUY2
    // ARGBToUYVY
    // RAWToJNV21

    const char *filename  = "D:/DEV/test2.h265";
//    *codec_name;

    const AVCodec *codec;
    AVCodecContext *c= NULL;
    int i, ret, x, y;
    std::FILE *f;
//    std::ofstream f;
//    std::ofstream myFile (std::ios::binary);

    AVFrame *frame;
    AVPacket *pkt;
    uint8_t endcode[] = { 0, 0, 1, 0xb7 };

    std::cout << "FIND CODEC\n";
    /* find the mpeg1video encoder */
    codec = avcodec_find_encoder_by_name("libx265");//avcodec_find_encoder(AV_CODEC_ID_H264);
    if (!codec) {
        fprintf(stderr, "Codec not found\n");
        exit(1);
    }


//    av_register_all();
//    AVFormatContext *ifmt_ctx = NULL;
    const AVOutputFormat *of = av_guess_format(nullptr, filename, nullptr);
    if(!of){
        exit(1);
    }
//    AVFormatContext *oc = avformat_alloc_context();
//    if (!oc) {
//        exit(1);
//    }
//    oc->oformat = of;
//    if(of){
//        std::cout << "found\n";
//        std::cout << "- > " << of->long_name << "\n";

//        /* allocate the output media context */
//        oc = avformat_alloc_context();
//    }
//    if ((ret = avformat_open_input(&ifmt_ctx, filename, 0, 0)) < 0) {
//        fprintf(stderr, "Could not open input file '%s'", filename);
//    }


    std::cout << "ALLOC CONTEXT\n";
    c = avcodec_alloc_context3(codec);
    if (!c) {
        fprintf(stderr, "Could not allocate video codec context\n");
        exit(1);
    }

    std::cout << "ALLOC PACKET\n";
    pkt = av_packet_alloc();
    if (!pkt){
        exit(1);
    }

    /* put sample parameters */
    c->bit_rate = 400000;
    /* resolution must be a multiple of two */
    c->width    = 1920;
    c->height   = 1080;
    /* frames per second */
    c->time_base = AVRational{1, 30};
    c->framerate = AVRational{30, 1};

    /* emit one intra frame every ten frames
     * check frame pict_type before passing frame
     * to encoder, if frame->pict_type is AV_PICTURE_TYPE_I
     * then gop_size is ignored and the output of encoder
     * will always be I frame irrespective to gop_size
     */
    c->gop_size = 10;
    c->max_b_frames = 1;
    c->pix_fmt = AV_PIX_FMT_YUV420P;
//    c->pix_fmt = AV_PIX_FMT_NV12;
    c->codec_type = AVMEDIA_TYPE_VIDEO; // test

    if (codec->id == AV_CODEC_ID_H264){
        if(av_opt_set(c->priv_data, "preset", "slow", 0) < 0){
            std::cout << "no preset\n";
        }
    }

//    if (ofctx->oformat->flags & AVFMT_GLOBALHEADER) {
//            cctx->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;
//    }


    /* open it */
    std::cout << "OPEN CODEC\n";
    ret = avcodec_open2(c, codec, NULL);
    if (ret < 0) {
        fprintf(stderr, "Could not open codec \n");
        exit(1);
    }

    std::cout << "OPEN FILE\n";
    f = std::fopen(filename, "wb");

//    f.open(filename, std::ios::out, std::ios::binary);
//    if(!f.is_open()){
    if (!f) {
        fprintf(stderr, "Could not open %s\n", filename);
        exit(1);
    }

    std::cout << "ALLOC FRAME\n";
    frame = av_frame_alloc();
    if (!frame) {
        fprintf(stderr, "Could not allocate video frame\n");
        exit(1);
    }
    frame->format = c->pix_fmt;
    frame->width  = c->width;
    frame->height = c->height;

    ret = av_frame_get_buffer(frame, 0);
    if (ret < 0) {
        fprintf(stderr, "Could not allocate the video frame data\n");
        exit(1);
    }



    std::cout << "ALLOC SRC FRAME\n";
    auto srcFrame = av_frame_alloc();
    srcFrame->format = AV_PIX_FMT_RGB24;
    srcFrame->width  = 1920/4;//c->width;
    srcFrame->height = 1080/4;//c->height;
    ret = av_frame_get_buffer(srcFrame, 0);
    if (ret < 0) {
        fprintf(stderr, "Could not allocate the video frame data\n");
        exit(1);
    }

    SwsContext * ctx = sws_getContext(
        srcFrame->width, srcFrame->height,AV_PIX_FMT_RGB24,
        c->width, c->height, c->pix_fmt,
        SWS_BILINEAR, 0, 0, 0);




//    std::vector<std::uint8_t> srcData;
//    srcData.resize(c->width*c->height*3);
//    std::fill(srcData.begin(), srcData.end(), 255);

//    std::vector<std::uint8_t> dstData1;
//    dstData1.resize(srcData.size());

//    std::vector<std::uint8_t> dstData2;
//    dstData2.resize(srcData.size());

//    uint8_t * inData[1] = { srcData.data() }; // RGB24 have one plane
//    int inLinesize[1] = { 3*c->width }; // RGB stride

//    uint8_t * outData[2] = {dstData1.data(), dstData2.data()};

////    AVFrame* dst = av_frame_alloc(AV_PIX_FMT_YUV420P, c->width, c->height);


//    int outLinesize[2] = {1,2};
//    sws_scale(ctx, inData, inLinesize, 0, c->height, outData, outLinesize);
//    sws_scale(ctx, inData, inLinesize, 0, c->height, dstData.data(), outLinesize);

//    * @param srcSlice  the array containing the pointers to the planes of
//    *                  the source slice
//    * @param srcStride the array containing the strides for each plane of
//    *                  the source image
//    * @param srcSliceY the position in the source image of the slice to
//    *                  process, that is the number (counted starting from
//    *                  zero) in the image of the first row of the slice
//    * @param srcSliceH the height of the source slice, that is the number
//    *                  of rows in the slice
//    * @param dst       the array containing the pointers to the planes of
//    *                  the destination image
//    * @param dstStride the array containing the strides for each plane of
//    *                  the destination image


    /* encode 1 second of video */
    std::cout << "ENCODE 10 S \n";
    for (i = 0; i < 25*10; i++) {
        fflush(stdout);


        auto d = srcFrame->data[0];
        for(int ii = 0; ii < srcFrame->width*srcFrame->height; ++ii){
            if(ii%3 == 0){
                d[ii*3+0] = 255;
                d[ii*3+1] = 0;
                d[ii*3+2] = 0;
            }else if(ii%3 == 1){
                d[ii*3+0] = 0;
                d[ii*3+1] = 255;
                d[ii*3+2] = 0;
            }else{
                d[ii*3+0] = 0;
                d[ii*3+1] = 0;
                d[ii*3+2] = 255;
            }
        }



        /* convert to destination format */
        sws_scale(
            ctx, srcFrame->data,
            srcFrame->linesize, 0,
            srcFrame->height, frame->data, frame->linesize
        );




        /* Make sure the frame data is writable.
           On the first round, the frame is fresh from av_frame_get_buffer()
           and therefore we know it is writable.
           But on the next rounds, encode() will have called
           avcodec_send_frame(), and the codec may have kept a reference to
           the frame in its internal structures, that makes the frame
           unwritable.
           av_frame_make_writable() checks that and allocates a new buffer
           for the frame only if necessary.
         */
        ret = av_frame_make_writable(frame);
        if (ret < 0)
            exit(1);

//        /* Prepare a dummy image.
//           In real code, this is where you would have your own logic for
//           filling the frame. FFmpeg does not care what you put in the
//           frame.
//         */
//        /* Y */
//        for (y = 0; y < c->height; y++) {
//            for (x = 0; x < c->width; x++) {
//                frame->data[0][y * frame->linesize[0] + x] = x + y + i * 3;
//            }
//        }

//        /* Cb and Cr */
//        for (y = 0; y < c->height/2; y++) {
//            for (x = 0; x < c->width/2; x++) {
//                frame->data[1][y * frame->linesize[1] + x] = 128 + y + i * 2;
//                frame->data[2][y * frame->linesize[2] + x] = 64 + x + i * 5;
//            }
//        }

        frame->pts = i;

        /* encode the image */
        encode(c, frame, pkt, f);
    }

    std::cout << "FLUSH ENCODER\n";
    /* flush the encoder */
    encode(c, NULL, pkt, f);

    /* Add sequence end code to have a real MPEG file.
       It makes only sense because this tiny examples writes packets
       directly. This is called "elementary stream" and only works for some
       codecs. To create a valid file, you usually need to write packets
       into a proper file format or protocol; see mux.c.
     */
//    if (codec->id == AV_CODEC_ID_MPEG1VIDEO || codec->id == AV_CODEC_ID_MPEG2VIDEO)
//        fwrite(endcode, 1, sizeof(endcode), f);
    fclose(f);
//    f.close();

    std::cout << "FREE\n";
    sws_freeContext(ctx);
    avcodec_free_context(&c);
    av_frame_free(&frame);
    av_frame_free(&srcFrame);
    av_packet_free(&pkt);

    std::cout << "END\n";
}


int main(int argc, char *argv[]){

//    auto writer = std::make_unique<VideoWriter>(
//        "D:/DEV/test3.mp4",
//        30,
//        1920,
//        1080,
//        true,
//        "libx265"
//    );
//    std::vector<std::uint8_t> d;
//    d.resize(1920*1080*3);
//    std::fill(d.begin(), d.end(), 255);
//    writer->write(d.data());

//    test_encode_2();

//    test_k4device();
    test_encode_video();


//    auto outputVideo = std::make_unique<cv::VideoWriter>(
//        "D:/DEV/test3.mp4",
//        cv::CAP_FFMPEG,
//        cv::VideoWriter::fourcc('h', '2', '6', '4'),
//        30,
//        cv::Size2i(1920,1080)
//    );
//    CV_WRAP VideoWriter(const String& filename, int apiPreference, int fourcc, double fps,
//                Size frameSize, bool isColor = true);
    //        for(const auto &f : fFrames){


    //            std::vector<std::uint8_t> data(f->colorWidth*f->colorHeight*3);
    //            for(size_t id = 0; id < f->imageColorData.size(); ++id){
    //                data[id*3+0] = f->imageColorData[id].x();
    //                data[id*3+1] = f->imageColorData[id].y();
    //                data[id*3+2] = f->imageColorData[id].z();
    //            }
    //            std::cout << "write frame: " << data.size() << "\n";
    ////            if(!writer->write(data.data())){

    ////            }

    //            cv::Mat m(cv::Size2i(f->colorWidth,f->colorHeight), CV_8UC3);
    //            for(size_t ii = 0; ii < f->imageColorData.size(); ++ii){
    //                auto &p = f->imageColorData[ii];
    //                auto d = m.data;
    //                d[3*ii]   = p(0);
    //                d[3*ii+1] = p(1);
    //                d[3*ii+2] = p(2);
    //            }

    //            (*outputVideo) << m;
    //        }

    //        device->stop_cameras();
    //        outputVideo->release();

//    outputVideo->release();

    return 0;
//    av::init();

//    // https://github.com/h4tr3d/avcpp/blob/master/example/api2-samples/api2-decode-encode-video.cpp
////    av::Codec ocodec  = av::findEncodingCodec("hevc_amf");
////    av::Codec ocodec  = av::findEncodingCodec(AV_CODEC_ID_H265);//av::findEncodingCodec("hevc_amf");
//    av::Codec ocodec  = av::findEncodingCodec(AV_CODEC_ID_H264);//av::findEncodingCodec("hevc_amf");
//    std::cout << "codec valid " << !ocodec.isNull() << "\n";

////

//    std::cout << "can decode " << ocodec.canDecode() << "\n";
//    std::cout << "can encode " << ocodec.canEncode() << "\n";
//    std::cout << "is encoder " << ocodec.isEncoder() << "\n";
//    std::cout << "name " << ocodec.name() << "\n";
//    std::cout << "long name " << ocodec.longName() << "\n";

//    std::cout << "Supported framerates:\n";
//    for(const auto &sf : ocodec.supportedFramerates()){
//        std::cout << sf.getDouble() << " - ";
//    }
//    std::cout << "\n";

//    std::cout << "Supported pixel formats:\n";
//    for(const auto &pf : ocodec.supportedPixelFormats()){
//        std::cout << pf.name() << " - ";
//    }
//    std::cout << "\n";

//    std::cout << "Supported samplerates:\n";
//    for(const auto &ss : ocodec.supportedSamplerates()){
//        std::cout << ss << " - ";
//    }
//    std::cout << "\n";

//    std::cout << "Supported sample formats:\n";
//    for(const auto &sf : ocodec.supportedSampleFormats()){
//        std::cout << sf.name() << " - ";
//    }
//    std::cout << "\n";

//    auto pixF = ocodec.supportedPixelFormats().front();


////    std::deque<SampleFormat>   supportedSampleFormats() const;
////    std::deque<uint64_t>       supportedChannelLayouts() const;


////    return 0;
//    // Settings
////    auto name = av_get_pix_fmt_name(AV_PIX_FMT_YUV420P);
////    av::PixelFormat px(av_get_pix_fmt_name(AV_PIX_FMT_YUV420P));

//    std::vector<av::PixelFormat> pFormats;
////    pFormats.push_back(pixF);
//    pFormats.push_back(av::PixelFormat(AV_PIX_FMT_GRAY8));


//    std::cout << "formats\n";
//    for(auto pFormat : pFormats){


//        av::VideoEncoderContext encoder {ocodec};
//        encoder.setWidth(1920);
//        encoder.setHeight(1080);
//        encoder.setTimeBase(av::Rational{1, 1000});
//        encoder.setBitRate(5000000);
//        std::cout << "codec: " << ocodec.longName() << " "  <<  "\n";


//        std::cout << "pFormat " << av_get_pix_fmt_name(pFormat) << " " << pFormat.bitsPerPixel() << " " << pFormat.name() << "\n";
//        encoder.setPixelFormat(pFormat);


//        std::cout << "1\n";
//        std::error_code ec;
//        encoder.open(ocodec, ec);
//        if (ec) {
//           std::cerr << "Can't open encodec\n";
//           continue;
//        }

//        std::cout << "SUCCESSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSS\n";
////        return 0;




//        std::cout << "2\n";
//        av::OutputFormat  ofrmt;
//        ofrmt.setFormat(std::string(), "D:/DEV/test.mp4");

//        std::cout << "3\n";
//        av::FormatContext octx;
//        octx.setFormat(ofrmt);

//        av::Stream ost = octx.addStream(encoder);
//        ost.setFrameRate(30);

//        octx.openOutput("D:/DEV/test.mp4", ec);
//        if (ec) {
//            std::cerr << "Can't open output\n";
//            continue;
//        }

//        std::cout << "4\n";
//        octx.writeHeader();
//        octx.dump();
//        octx.flush();


//        // ... copy


//        std::cout << "5\n";




//        for(int idF = 0; idF < 100; ++idF){


//            // planar YUV 4:2:0, 12bpp, (1 Cr & Cb sample per 2x2 Y samples)
//            av::VideoFrame frame(av::PixelFormat(AV_PIX_FMT_GRAY8),1920,1080);

//            frame.setTimeBase(encoder.timeBase());
//            frame.setStreamIndex(0);
//            frame.setPictureType();

//           std::uint8_t *d = frame.data();
//           std::cout << "6 " << frame.size() << " " << frame.bufferSize() << " " << 1920*1080 << "\n";

//            for(int ii = 0; ii < 1920*1080; ++ii){
//                d[ii] = 255;
////                d[ii*3 + 0] = 255;//rand()%255;
////                d[ii*3 + 1] = 255;
////                d[ii*3 + 2] = 255;
//            }

////            std::cout << "encode\n";
//            auto pac = encoder.encode(frame, ec);
////            std::cout << "--> " << pac.isComplete();

//            if(ec){
//                std::cerr << "Can't write frame " << ec.message() << "\n";
//                break;
//            }else if(!pac){
//                std::cerr << "empty packet \n";
//                break;
//            }

//            // Only one output stream
//            pac.setStreamIndex(0);
//            std::cout << idF << " ";

////            std::cout << "Write packet: pts=" << pac.pts() << ", dts=" << pac.dts() << " / " << pac.pts().seconds() << " / " << pac.timeBase() << " / st: " << pac.streamIndex() << "\n";


//            octx.writePacket(pac, ec);
//            if (ec) {
//                std::cerr << "Error write packet: " << ec << ", " << ec.message() << "\n";
//                return 1;
//            }

//        }

//        octx.writeTrailer();
//        std::cout << "7\n";
//        return 0;
//    }













//    std::cout << "formats\n";
//    for(auto pFormat : pFormats){



////        av::PixelFormat px(av_get_pix_fmt_name(AV_PIX_FMT_BGR24));




////        av::Stream ost = octx.addStream(encoder);
////        ost.setFrameRate(30);
////        ost.





//    }








    // init device

//    using namespace tool::camera;

//    int mjpg = cv::VideoWriter::fourcc('M', 'J', 'P', 'G');
//    int avc1 = cv::VideoWriter::fourcc('a', 'v', 'c', '1');

//    std::unique_ptr<K4Device> device = std::make_unique<K4Device>();
//    device->open(0);

//    K4ConfigSettings cs;
//    cs.idDevice                 = 0;
//    cs.synchronizeColorAndDepth = true;
//    cs.synchMode                = K4SynchronisationMode::Standalone;
//    cs.mode                     = K4Mode::Only_color_1920x1080;
//    device->start_cameras(cs);



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

//    test_k4device();

    return 0;
}



//auto test_k4device() -> void{

//    std::cout << "test_k4device\n";

//////    cv::VideoWriter outputVideo("D:/DEV/test-mpeg.mp4", cv::VideoWriter::fourcc('M', 'P', 'E', 'G'), 30, cv::Size2i(1000,1000));
////    cv::VideoWriter outputVideo("D:/DEV/test-hevc.mp4", cv::VideoWriter::fourcc('H', 'E', 'V', 'C'), 30, cv::Size2i(1000,1000));
//////     cv::VideoWriter outputVideo("D:/DEV/test-h264.mp4", cv::VideoWriter::fourcc('H', '2', '6', '4'), 30, cv::Size2i(1000,1000));
//////     cv::VideoWriter outputVideo("D:/DEV/test-.mp4", -1, 30, cv::Size2i(1000,1000));


////    std::cout << "video opened: " << outputVideo.isOpened() << "\n";
//////    outputVideo.open
////    if (!outputVideo.isOpened())
////    {
////        std::cout << "Could not open the output video for write:\n";
////        return ;
////    }

////    for(int ii = 0; ii < 255; ++ii){
////        cv::Mat m(cv::Size2i(1000,1000), CV_8UC3);
////        std::fill(m.data, m.data + 1000*1000*3, ii);
////        outputVideo << m;
////    }

////    std::cout << "release\n";
////    outputVideo.release();
////    return;

//    using namespace tool::camera;
////    std::vector<std::unique_ptr<K4Device>> devices;

//////    std::cout << "audio " << k4a::K4AAudioManager::in().Initialize() << "\n";
//////    std::cout << "nb devices " << k4a::K4AAudioManager::Instance().get_devices_count() << "\n";


////    for(size_t idD = 0; idD < 1; ++idD){
////        std::cout << std::format("### Open device {}.\n", std::to_string(idD));
////        devices.push_back(std::make_unique<K4Device>());

////        auto device = devices.back().get();

////        device->open(idD);

////        K4ConfigSettings cs;
////        cs.idDevice = idD;
////        cs.synchronizeColorAndDepth = true;

//////        if(!device->is_sync_in_connected() && device->is_sync_out_connected()){
//////            cs.synchMode = K4SynchronisationMode::Master;
//////        }else if(device->is_sync_in_connected()){
//////            cs.synchMode = K4SynchronisationMode::Subordinate;
//////            cs.subordinateDelayUsec = 100;
//////        }else{
//////            cs.synchMode = K4SynchronisationMode::Standalone;
//////        }
////        cs.synchMode = K4SynchronisationMode::Standalone;

////        cs.mode = K4Mode::Only_color_2048x1536;
////        device->start_cameras(cs);

////        K4DataSettings ds;
////        ds.captureAudio             = true;
////        ds.captureIMU               = false;
////        ds.captureBodies            = false;
////        ds.sendColor                = false;
////        ds.sendDepth                = false;
////        ds.sendInfra                = false;
////        ds.sendCloud                = false;
////        ds.sendIMU                  = false;
////        ds.sendAudio                = false;
////        ds.sendBodies               = false;
////        ds.generateRGBLocalFrame    = true;
////        ds.generateDepthLocalFrame  = true;
////        ds.generateInfraLocalFrame  = true;
////        ds.generateCloudLocal       = false;
////        device->set_data_settings(ds);
////    }


////    outputVideo = std::make_unique<cv::VideoWriter>("D:/DEV/test-h264.mp4", cv::CAP_FFMPEG, cv::VideoWriter::fourcc('a', 'v', 'c', '1'), 30, cv::Size2i(frame->colorWidth,frame->colorHeight));

//    int mjpg = cv::VideoWriter::fourcc('M', 'J', 'P', 'G');
//    int avc1 = cv::VideoWriter::fourcc('a', 'v', 'c', '1');

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


////    std::unique_ptr<VideoWriter> writer = nullptr; //("filename.mp4", fps, width, height, encoder_name, bitrate);
////    writer.write(cvFrame); //cvFrame is a cv::Mat object


////    VideoWriter writer;
////    writer.open("appsrc ! videoconvert  !  avenc_mpeg4 bitrate=100000 ! mp4mux ! filesink location=video.mp4",CAP_GSTREAMER, 0, (double) 10, cv::Size(640, 480), true);


//    // init device
//    std::unique_ptr<K4Device> device = std::make_unique<K4Device>();
//    device->open(1);

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


//    return;

//    // signals
//    std::vector<cv::Mat> frames;
//    std::vector<std::shared_ptr<K4Frame>> fFrames;
//    device->new_frame_signal.connect([&](std::shared_ptr<K4Frame> frame){

////        cv::Mat m(cv::Size2i(frame->colorWidth,frame->colorHeight), CV_8UC3);
////        for(size_t ii = 0; ii < frame->imageColorData.size(); ++ii){
////            auto &p = frame->imageColorData[ii];
////            auto d = m.data;
////            d[3*ii]   = p(0);
////            d[3*ii+1] = p(1);
////            d[3*ii+2] = p(2);
////        }
////        frames.push_back(m);
//        fFrames.push_back(frame);
//        std::cout << "f";
//    });

//    std::cout << "start\n";
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

//        auto outputVideo = std::make_unique<cv::VideoWriter>(
//            std::format("D:/DEV/of{}_{}", f, std::get<1>(info)).c_str(),
//            cv::CAP_FFMPEG,
//            std::get<0>(info),
//            f,
//            cv::Size2i(w,h)
//        );

//        auto writer = std::make_unique<VideoWriter>(
//            std::format("D:/DEV/f{}_{}", f, std::get<1>(info)),
//            f,
//            w,
//            h,
//            true,
//            "libx265"
//        );

////        int countLoops = 100;
//        while(fFrames.size() < 100){
//            std::this_thread::sleep_for(std::chrono::milliseconds(5));
//        }

//        std::cout << "w:" << w << " h: " << h << " f: " << f << " m: " << (int)mode << "\n";
//        for(const auto &f : fFrames){


//            std::vector<std::uint8_t> data(f->colorWidth*f->colorHeight*3);
//            for(size_t id = 0; id < f->imageColorData.size(); ++id){
//                data[id*3+0] = f->imageColorData[id].x();
//                data[id*3+1] = f->imageColorData[id].y();
//                data[id*3+2] = f->imageColorData[id].z();
//            }
//            std::cout << "write frame: " << data.size() << "\n";
////            if(!writer->write(data.data())){

////            }

//            cv::Mat m(cv::Size2i(f->colorWidth,f->colorHeight), CV_8UC3);
//            for(size_t ii = 0; ii < f->imageColorData.size(); ++ii){
//                auto &p = f->imageColorData[ii];
//                auto d = m.data;
//                d[3*ii]   = p(0);
//                d[3*ii+1] = p(1);
//                d[3*ii+2] = p(2);
//            }

//            (*outputVideo) << m;
//        }

//        device->stop_cameras();
//        outputVideo->release();
//    }


////    std::cout << cv::getBuildInformation() << "\n";


////    std::unique_ptr<cv::VideoWriter> outputVideo = nullptr;

//////    ("D:/DEV/test-hevc.mp4", cv::VideoWriter::fourcc('H', 'E', 'V', 'C'), 30, cv::Size2i(1000,1000));

////    for(auto &device : devices){
////        device->new_frame_signal.connect([&](std::shared_ptr<K4Frame> frame){

//////            std::cout << device->device_id() << " " << frame->idCapture << " " << frame->imageColorData.size() << " " << frame->colorHeight << " " << frame->colorWidth <<  " " << frame->depthWidth << " " << frame->depthHeight<< " " << frame->depthSizedColorWidth << " " << frame->depthSizedColorHeight << "\n";

//////            cv::Mat m(cv::Size2i(frame->colorWidth,frame->colorHeight), CV_8UC4);
//////            std::copy(reinterpret_cast<std::uint8_t*>(frame->imageColorData.data()), reinterpret_cast<std::uint8_t*>(frame->imageColorData.data()) + frame->colorWidth*frame->colorHeight*4, m.data);


////            cv::Mat m(cv::Size2i(frame->colorWidth,frame->colorHeight), CV_8UC3);
////            for(size_t ii = 0; ii < frame->imageColorData.size(); ++ii){
////                auto &p = frame->imageColorData[ii];
////                auto d = m.data;
////                d[3*ii]   = p(0);
////                d[3*ii+1] = p(1);
////                d[3*ii+2] = p(2);
////            }
//////            std::copy(reinterpret_cast<std::uint8_t*>(frame->imageColorData.data()), reinterpret_cast<std::uint8_t*>(frame->imageColorData.data()) + frame->colorWidth*frame->colorHeight*4, m.data);



//////            fourcc tag 0x7634706d/'mp4v' codec_id 000C
//////            fourcc tag 0x31637661/'avc1' codec_id 001B
//////            fourcc tag 0x33637661/'avc3' codec_id 001B
//////            fourcc tag 0x31766568/'hev1' codec_id 00AD
//////            fourcc tag 0x31637668/'hvc1' codec_id 00AD
//////            fourcc tag 0x7634706d/'mp4v' codec_id 0002
//////            fourcc tag 0x7634706d/'mp4v' codec_id 0001
//////            fourcc tag 0x7634706d/'mp4v' codec_id 0007
//////            fourcc tag 0x7634706d/'mp4v' codec_id 003D

////            if(outputVideo == nullptr){

////                // OK rgb8 1280x720 (5mo) 1920x1080 (10mo)
//////                outputVideo = std::make_unique<cv::VideoWriter>("D:/DEV/test-h264.mp4", cv::CAP_FFMPEG, cv::VideoWriter::fourcc('a', 'v', 'c', '1'), 30, cv::Size2i(frame->colorWidth,frame->colorHeight));

////                // OK rgb8 1280x720 (9mo) 1920x1080 (18mo) 2048x1536 (13mo)
////                outputVideo = std::make_unique<cv::VideoWriter>("D:/DEV/test2.avi", cv::CAP_FFMPEG, cv::VideoWriter::fourcc('M', 'J', 'P', 'G'), 30, cv::Size2i(frame->colorWidth,frame->colorHeight)); // OK 1280x720 rgb8 (18mo


//////                outputVideo = std::make_unique<cv::VideoWriter>("D:/DEV/test-h264.mkv", cv::CAP_FFMPEG, cv::VideoWriter::fourcc('H', '2', '6', '4'), 30, cv::Size2i(frame->colorWidth,frame->colorHeight)); // OK 1280x720 rgb8
//////                outputVideo = std::make_unique<cv::VideoWriter>("D:/DEV/test-h264.mpeg", cv::CAP_FFMPEG,cv::VideoWriter::fourcc('H', '2', '6', '4'), 30, cv::Size2i(frame->colorWidth,frame->colorHeight)); // OK 1280x720 rgb8
//////                outputVideo = std::make_unique<cv::VideoWriter>("D:/DEV/test-h264.mp4", cv::CAP_FFMPEG, cv::VideoWriter::fourcc('H', '2', '6', '4'), 30, cv::Size2i(frame->colorWidth,frame->colorHeight)); // OK 1280x720 rgb8

//////                  outputVideo = std::make_unique<cv::VideoWriter>("D:/DEV/test-hevc.mkv", cv::VideoWriter::fourcc('H', 'E', 'V', 'C'), 30, cv::Size2i(frame->colorWidth,frame->colorHeight)); //
//////                  outputVideo = std::make_unique<cv::VideoWriter>("D:/DEV/test-hevc.mpeg", cv::VideoWriter::fourcc('H', 'E', 'V', 'C'), 30, cv::Size2i(frame->colorWidth,frame->colorHeight)); //
//////                  outputVideo = std::make_unique<cv::VideoWriter>("D:/DEV/test-hevc.mp4", cv::VideoWriter::fourcc('H', 'E', 'V', 'C'), 30, cv::Size2i(frame->colorWidth,frame->colorHeight)); //
//////                    outputVideo = std::make_unique<cv::VideoWriter>("D:/DEV/test-hevc.avc1", cv::VideoWriter::fourcc('H', 'E', 'V', 'C'), 30, cv::Size2i(frame->colorWidth,frame->colorHeight)); //


//////                  outputVideo = std::make_unique<cv::VideoWriter>("D:/DEV/test-h265.mp4", cv::VideoWriter::fourcc('H', '2', '6', '5'), 30, cv::Size2i(frame->colorWidth,frame->colorHeight)); //

//////            outputVideo = std::make_unique<cv::VideoWriter>("D:/DEV/test-hvc1.mp4", cv::VideoWriter::fourcc('h', 'v', 'c', '1'), 30, cv::Size2i(frame->colorWidth,frame->colorHeight)); //

//////             outputVideo = std::make_unique<cv::VideoWriter>("D:/DEV/test-h265.mkv", cv::CAP_GSTREAMER, cv::VideoWriter::fourcc('M','J','P','G'), 30, cv::Size2i(frame->colorWidth,frame->colorHeight)); //


//////            outputVideo = std::make_unique<cv::VideoWriter>("D:/DEV/test-h264.mp4", cv::CAP_MSMF, cv::VideoWriter::fourcc('H', '2', '6', '4'), 30, cv::Size2i(frame->colorWidth,frame->colorHeight)); //

//////            std::cout << cv::NTDDI_WIN10 << "\n";
//////#if defined(NTDDI_WIN10)
//////        case CV_FOURCC_MACRO('H', '2', '6', '5'):
//////                return MFVideoFormat_H265;  break;
//////        case CV_FOURCC_MACRO('H', 'E', 'V', 'C'):
//////                return MFVideoFormat_HEVC;  break;




//////                outputVideo = std::make_unique<cv::VideoWriter>("D:/DEV/test-hevc.mp4", cv::VideoWriter::fourcc('H', 'E', 'V', 'C'), 15, cv::Size2i(frame->colorWidth,frame->colorHeight));

//////                outputVideo = std::make_unique<cv::VideoWriter>("D:/DEV/test-mpeg.mp4", cv::VideoWriter::fourcc('m', 'p', '4', 'v'), 15, cv::Size2i(frame->colorWidth,frame->colorHeight));
//////                outputVideo = std::make_unique<cv::VideoWriter>("D:/DEV/test-h264.avc1", cv::VideoWriter::fourcc('H', '2', '6', '4'), 15, cv::Size2i(frame->colorWidth,frame->colorHeight));
//////                outputVideo = std::make_unique<cv::VideoWriter>("D:/DEV/test-h264.mp4", cv::VideoWriter::fourcc('h', 'e', 'v', '1'), 15, cv::Size2i(frame->colorWidth,frame->colorHeight));
////            }
//////            cv::Mat m(frame->c);
////            std::cout << "a";
////            (*outputVideo) << m;
////        });
////    }




////    int countLoops = 100;
////    for(int ii = 0; ii < countLoops; ++ii){
//////        std::cout << "frame " << ii << "\n";


////        std::this_thread::sleep_for(std::chrono::milliseconds(30));
////    }
////    outputVideo->release();

////    std::cout << "Close devices.\n";
////    for(auto &device : devices){
////        device->stop_cameras();
////        device->close();
////    }
////    tool::camera::K4Device device;
////    std::puts("### Open device.\n");
////    if(device.open(0)){
////        tool::camera::K4ConfigSettings config;
////        config.idDevice = 0;
////        config.mode = tool::camera::K4Mode::Cloud_640x576_NV12;
////        config.synchronizeColorAndDepth = true;
////        config.delayBetweenColorAndDepthUsec = 0;
////        config.synchMode = tool::camera::K4SynchronisationMode::Standalone;
////        config.subordinateDelayUsec = 0;
////        config.disableLED = false;

////        std::puts("### Start cameras.\n");
////        device.start_cameras(config);
////        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
////        std::puts("### Stop cameras.\n");
////        device.stop_cameras();

////        std::puts("### Close device.\n");
////        device.close();
////    }else{
////        std::puts("### Device opening failure.\n");
////    }

////    std::puts("End tests.\n");
////    tool::Bench::display();
//    std::puts("End tests.\n");
//}



//struct Params{
//    uint32_t width;
//    uint32_t height;
//    double fps;
//    uint32_t bitrate;
//    const char *preset;
//    uint32_t crf; //0–51 https://superuser.com/questions/677576/what-is-crf-used-for-in-ffmpeg
//    AVPixelFormat src_format;
//    AVPixelFormat dst_format;
//};

//struct Context{
//    AVFormatContext *format_context = nullptr;
//    AVStream *stream = nullptr;
//    AVCodecContext *codec_context = nullptr;
//    AVFrame *frame = nullptr;
//    SwsContext *sws_context = nullptr;
//    const AVCodec *codec = nullptr;
//    uint32_t frame_index = 0;
//};

//auto test_encode_2() -> void{

//    // https://github.com/denesik/ffmpeg_video_encoder/blob/main/ffmpeg_encode.cpp
//    // presets:
//    //
//    // ultrafast
//    // superfast
//    // veryfast
//    // faster
//    // fast
//    // medium – default preset
//    // slow
//    // slower
//    // veryslow

//    const char*filename = "D:/DEV/test3.mp4";

//    Context mContext;
//    Params params;
//    params.width   = 1920;
//    params.height  = 1080;
//    params.fps     = 30.0;
//    params.bitrate = 400000;
//    params.src_format = AVPixelFormat::AV_PIX_FMT_BGR24;
//    params.dst_format = AVPixelFormat::AV_PIX_FMT_YUV420P;
//    params.crf = 23;
//    params.preset = "veryslow";

//    std::cout << "open\n";
//    // open
//    avformat_alloc_output_context2(&mContext.format_context, nullptr, nullptr, filename);
//    if (!mContext.format_context){
//        std::cout << "could not allocate output format" << std::endl;
//        return;
//    }

//    std::cout << "find encoder\n";
//    mContext.codec = avcodec_find_encoder_by_name("libx264");// avcodec_find_encoder(AV_CODEC_ID_H264);
//    if (!mContext.codec) {
//        std::cout << "could not find encoder" << std::endl;
//        return;
//    }

//    std::cout << "init stream\n";
//    mContext.stream = avformat_new_stream(mContext.format_context, nullptr);
//    if (!mContext.stream) {
//        std::cout << "could not create stream" << std::endl;
//        return;
//    }
//    mContext.stream->id = (int)(mContext.format_context->nb_streams - 1);

//    std::cout << "alloc context\n";
//    mContext.codec_context = avcodec_alloc_context3(mContext.codec);
//    if (!mContext.codec_context) {
//        std::cout << "could not allocate mContext codec context" << std::endl;
//        return;
//    }

//    mContext.codec_context->codec_id        = AV_CODEC_ID_H264;//mContext.format_context->oformat->video_codec;
//    mContext.codec_context->bit_rate        = params.bitrate;
//    mContext.codec_context->width           = static_cast<int>(params.width);
//    mContext.codec_context->height          = static_cast<int>(params.height);
//    mContext.stream->time_base              = av_d2q(1.0 / params.fps, 120);
//    mContext.codec_context->time_base       = mContext.stream->time_base;
//    mContext.codec_context->pix_fmt         = params.dst_format;
//    mContext.codec_context->gop_size        = 12;
//    mContext.codec_context->max_b_frames    = 2;


//    if (mContext.format_context->oformat->flags & AVFMT_GLOBALHEADER){
//        mContext.codec_context->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;
//    }


//    int ret = 0;
////    if (params.preset){
//////        if(mContext.codec_context->codec_id == AV_CODEC_ID_H264){
////            ret = av_opt_set(mContext.codec_context->priv_data, "preset", "slow", 0);
//////        }else{
//////            ret = av_opt_set(mContext.codec_context->priv_data, "preset", params.preset, 0);
//////        }

////        if (ret != 0)
////        {
////            std::cout << "could not set preset: " << params.preset << std::endl;
////            return;
////        }
////    }


////    if (params.preset){
////        ret = av_opt_set(mContext.codec_context->priv_data, "preset", params.preset, 0);
////        if (ret != 0)
////        {
////            std::cout << "could not set preset: " << params.preset << std::endl;
////            return;
////        }
////    }

////    {
////        ret = av_opt_set_int(mContext.codec_context->priv_data, "crf", params.crf, 0);
////        if (ret != 0)
////        {
////            std::cout << "could not set crf: " << params.crf << std::endl;
////            return;
////        }
////    }

//    ret = avcodec_open2(mContext.codec_context, mContext.codec, nullptr);
//    if (ret != 0) {
//        std::cout << "could not open codec: " << ret << std::endl;
//        return;
//    }

//    mContext.frame = av_frame_alloc();
//    if (!mContext.frame){
//        std::cout << "could not allocate mContext frame" << std::endl;
//        return;
//    }
//    mContext.frame->format = mContext.codec_context->pix_fmt;
//    mContext.frame->width = mContext.codec_context->width;
//    mContext.frame->height = mContext.codec_context->height;

//    ret = av_frame_get_buffer(mContext.frame, 32);
//    if (ret < 0) {
//        std::cout << "could not allocate the mContext frame data" << std::endl;
//        return;
//    }

//    ret = avcodec_parameters_from_context(mContext.stream->codecpar, mContext.codec_context);
//    if (ret < 0) {
//        std::cout << "could not copy the stream parameters" << std::endl;
//        return;
//    }

//    mContext.sws_context = sws_getContext(
//        mContext.codec_context->width, mContext.codec_context->height, params.src_format,   // src
//        mContext.codec_context->width, mContext.codec_context->height, params.dst_format, // dst
//        SWS_BICUBIC, nullptr, nullptr, nullptr
//    );
//    if (!mContext.sws_context) {
//        std::cout << "could not initialize the conversion context" << std::endl;
//        return;
//    }

//    av_dump_format(mContext.format_context, 0, filename, 1);
//    ret = avio_open(&mContext.format_context->pb, filename, AVIO_FLAG_WRITE);
//    if (ret != 0) {
//        std::cout << "could not open " << filename << std::endl;
//        return;
//    }

//    ret = avformat_write_header(mContext.format_context, nullptr);
//    if (ret < 0){
//        std::cout << "could not write" << std::endl;
//        ret = avio_close(mContext.format_context->pb);
//        if (ret != 0)
//            std::cout << "failed to close file" << std::endl;
//        return;
//    }

//    mContext.frame_index = 0;
//    bool mIsOpen = true;


//    // close
//    if (mIsOpen)
//    {
//        avcodec_send_frame(mContext.codec_context, nullptr);

////        FlushPackets();

//        av_write_trailer(mContext.format_context);

//        auto ret = avio_close(mContext.format_context->pb);
//        if (ret != 0)
//            std::cout << "failed to close file" << std::endl;
//    }

//    if (mContext.sws_context)
//        sws_freeContext(mContext.sws_context);

//    if (mContext.frame)
//        av_frame_free(&mContext.frame);

//    if (mContext.codec_context)
//        avcodec_free_context(&mContext.codec_context);

//    if (mContext.codec_context)
//        avcodec_close(mContext.codec_context);

//    if (mContext.format_context)
//        avformat_free_context(mContext.format_context);

//    mContext = {};
//    mIsOpen = false;

//}

