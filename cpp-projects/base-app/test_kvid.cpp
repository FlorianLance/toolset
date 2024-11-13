
#include "tests.hpp"

// std
#include <iostream>

// fastfor
#include "fastpfor/simdfastpfor.h"
#include "fastpfor/compositecodec.h"
#include "fastpfor/variablebyte.h"

// base
#include "depth-camera/frame/dc_data_frame_generator.hpp"
#include "data/jpeg_encoding.hpp"
#include "data/fastpfor_encoding.hpp"
#include "utility/logger.hpp"
#include "utility/io_fstream.hpp"
#include "utility/time.hpp"
#include "geometry/color_voxel_grid.hpp"
#include "depth-camera/dc_video.hpp"
#include "utility/string.hpp"


auto convert_kvid(const std::string &path, const std::string &dest) -> void{

    using namespace tool;
    using namespace tool::geo;
    using namespace tool::cam;

    if(path.empty()){
        return;
    }

    Log::message(std::format("Convert_kvid: {}\n", path));


    // open file
    std::ifstream file(path.data(), std::ios_base::binary);
    if(!file.is_open()){
        Log::error(std::format("Cannot open binary file with path: [{}].\n", path));
        return;
    }
    file.exceptions ( std::ifstream::eofbit | std::ifstream::failbit | std::ifstream::badbit);

    data::JpegDecoder jpgDecoder;
    DCDataFrameGenerator frameCompressor;
    // DCVideoRecorder recorder;
    DCVideo video;


    // read mode
    std::int8_t videoType;
    read(videoType, file);
    std::cout << "videoType: " << (int)videoType << "\n";

    if(videoType == 1){

        // read nb of cameras
        std::int8_t nbCameras;
        read(nbCameras, file);
        std::cout << "nbCameras: " << (int)nbCameras << "\n";

        video.initialize(nbCameras);

        std::vector<geo::Mat4d> calibrations;

        std::vector<std::vector<std::shared_ptr<DCDeprecatedFrame>>> frames;
        frames.resize(nbCameras);

        for(size_t id = 0; id < static_cast<size_t>(nbCameras); ++id){

            std::int32_t nbFrames;
            read(nbFrames, file);
            std::cout << "nbFrames: " << (int)nbFrames << "\n";
            frames[id].resize(nbFrames);

            for(auto &frame : frames[id]){
                frame = std::make_shared<DCDeprecatedFrame>();
            }

            geo::Mat4d mat;
            read_array(mat.array.data(), file, 16);
            std::cout << "mat:\n " << mat << "\n";
            calibrations.push_back(mat);

            video.set_transform(id, mat);
        }

        for(size_t idC = 0; idC < frames.size(); ++idC){
            for(size_t idF = 0; idF < frames[idC].size(); ++idF){

                std::cout << (1.f*idF/frames[idC].size()) << " ";
                auto frame = frames[idC][idF].get();
                frame->mode = DCMode::AK_C1280x720_DI640x576_NV12_F30;

                read(frame->idCapture, file);
                // std::cout << "idCapture: " << (int)frame->idCapture << "\n";

                read(frame->afterCaptureTS, file);
                // std::cout << "afterCaptureTS: " << (int)frame->afterCaptureTS << "\n";
                frame->receivedTS = frame->afterCaptureTS;

                std::int32_t validVerticesC;
                read(validVerticesC, file);
                frame->cloud.resize(validVerticesC);
                // std::cout << "validVerticesCount: " << (int)validVerticesC << "\n";

                std::int32_t cloudBufferSize;
                read(cloudBufferSize, file);

                // std::cout << "cloudBufferSize: " << (int)cloudBufferSize << "\n";
                std::vector<std::uint8_t> encodedCloudVerticesData;
                if(cloudBufferSize > 0){
                    encodedCloudVerticesData.resize(cloudBufferSize);
                    read_array(encodedCloudVerticesData.data(), file, encodedCloudVerticesData.size());
                }

                std::int16_t colorW, colorH;
                std::int32_t colorBufferSize;
                read(colorW, file);
                read(colorH, file);
                read(colorBufferSize, file);

                // std::cout << "colorW: " << (int)colorW << "\n";
                // std::cout << "colorH: " << (int)colorH << "\n";
                // std::cout << "colorBufferSize: " << (int)colorBufferSize << "\n";
                std::vector<std::uint8_t> encodedCloudColorData;

                ImageBuffer<ColorRGB8> image;
                if(colorBufferSize > 0){
                    encodedCloudColorData.resize(colorBufferSize);
                    read_array(encodedCloudColorData.data(), file, encodedCloudColorData.size());
                    auto bs = BinarySpan(reinterpret_cast<std::byte*>(encodedCloudColorData.data()) ,encodedCloudColorData.size());
                    jpgDecoder.decode(colorW, colorH, bs, image);

                    // stbi_write_jpg("D:/video_image.jpg", colorW, colorH, 3, image.get_data(), 90);
                    // image.
                }

                // # read audio
                std::int32_t audioBufferSize;
                read(audioBufferSize, file);
                if(audioBufferSize > 0){
                    frame->audioFrames.resize(audioBufferSize);
                    read_array(reinterpret_cast<float*>(frame->audioFrames.data()), file, audioBufferSize*7);
                }

                // # read imu
                DCImuSample imusSample;
                read_array(file, reinterpret_cast<char*>(&imusSample), sizeof (DCImuSample));

                if(encodedCloudVerticesData.size() > 0){
                    std::vector<std::uint16_t> decodedVertices;

                    // decode cloud
                    size_t uncompressedSize = frame->cloud.size()*3;
                    size_t rest = uncompressedSize % 128;
                    size_t paddedUncompressedSize = rest == 0 ? uncompressedSize : uncompressedSize + 128-rest;

                    // std::cout << "uncompressedSize: " << uncompressedSize << "\n";
                    // std::cout << "rest: " << rest << "\n";
                    // std::cout << "paddedUncompressedSize: " << paddedUncompressedSize << "\n";

                    if(decodedVertices.size() < paddedUncompressedSize){
                        decodedVertices.resize(paddedUncompressedSize);
                    }

                    // size_t decodedBytesNb = p4nzdec128v16(
                    //     encodedCloudVerticesData.data(),
                    //     paddedUncompressedSize,
                    //     decodedVertices.data());
                    // if(decodedBytesNb == 0){
                    //     Log::error("[K4FrameUncompressor::uncompress_lossless_16_bits_128padded_data] Error decoding data.\n");
                    // }

                    // decodedVertices.resize(uncompressedSize);


                    // auto vData = reinterpret_cast<geo::Pt3<std::uint16_t>*>(decodedVertices.data());

                    auto vvc = frame->cloud.size();
                    for(size_t idP = 0; idP < frame->cloud.size(); ++idP){
                        frame->cloud.vertices[idP]= Pt3f{
                                                         -(static_cast<float>(decodedVertices[        idP]) - 4096),
                                                         -(static_cast<float>(decodedVertices[vvc   + idP]) - 4096),
                                                         static_cast<float>(decodedVertices  [2*vvc + idP])
                                                     }*0.001f;

                        frame->cloud.colors[idP] = Pt3f{
                                                       static_cast<float>(image.values[idP].r()),
                                                       static_cast<float>(image.values[idP].g()),
                                                       static_cast<float>(image.values[idP].b())
                                                   }/255.f;
                    }
                }

                // auto cFrame = frameCompressor.compress(*frame, 80);
                // DCCompressedFrame *ptr = cFrame.release();
                // // cFrame.release();
                // auto s_ptr = std::shared_ptr<DCCompressedFrame>(ptr);
                // std::cout << "idCapture : " << s_ptr->idCapture << "\n";
                // std::cout << "vertices : " << s_ptr->validVerticesCount << "\n";
                // std::cout << "fpfColoredCloud : " << s_ptr->fpfColoredCloud.size() << "\n";

                // video.add_compressed_frame(idC, std::move(s_ptr));
            }
        }

        video.save_to_file(dest);

    }
}


auto process_kvid() -> void{


    using namespace tool;
    using namespace tool::geo;
    using namespace tool::cam;
    using namespace std::chrono;

    DCVideo video;
    std::string path = "E:/_kvid/30-07-24-fam.kvid";
    video.load_from_file(path);


    DCFrameGenerationSettings settings;
    settings.cloud = true;

    auto startTS = video.device_first_frame_timestamp(0);
    size_t idF = 50;
    // for(size_t idF = 0; idF < video.nb_frames(0); ++idF){

        auto firstCameraCurrentFrameTS = video.get_data_frame(0, idF).lock().get()->afterCaptureTS;
        auto diffNs = nanoseconds(firstCameraCurrentFrameTS - startTS);
        auto currentTimeMS = duration_cast<milliseconds>(diffNs);
        // auto firstCameraCurrentFrameTS = video.get_data_frame(0, idF).lock().;


        DCFrame d0Frame;
        video.generate_frame(settings, 0, idF, d0Frame);

        ColorCloud fullCloud;
        auto cp = d0Frame.volume_buffer<ColorCloud>(DCVolumeBufferType::ColoredCloud);
        std::cout << "fullCloud0: " << fullCloud.size() << " " << cp->size() << "\n";
        fullCloud.merge(*cp);
        std::cout << "fullCloud1: " << fullCloud.size() << "\n";
        fullCloud.merge(*cp);
        std::cout << "fullCloud2: " << fullCloud.size() << "\n";


        // std::cout << "idf " << idF << " time ms: " << currentTimeMS.count() << " " << cp->size() << "\n";

        // ColorVoxelGrid cGrid(0.0025f, {-2.f,-2.f,-2.f}, {+2.f,+2.f,+2.f});
        ColorVoxelGrid2 cGrid(0.002f, {-2.f,-2.f,-2.f}, {+4.f,+4.f,+4.f});
        cGrid.add_cloud(*cp, video.get_transform(0).conv<float>());

        // std::cout << "\t";
        for(size_t idC = 1; idC < video.nb_devices(); ++idC){
            auto cfid = video.closest_frame_id_from_time(idC, 1.0*currentTimeMS.count());


            DCFrame dIFrame;
            video.generate_frame(settings,idC, cfid, dIFrame);
            cp = dIFrame.volume_buffer<ColorCloud>(DCVolumeBufferType::ColoredCloud);
            std::cout << idC << " " << cfid << " " << cp->size() <<  " | ";



            cGrid.add_cloud(*cp, video.get_transform(idC).conv<float>());
            fullCloud.merge(*cp);
        }


        cGrid.mix();

        Buffer<CVoxel> voxels;
        cGrid.copy_to_cvoxels(voxels);
        Log::fmessage("voxels {}\n", voxels.size());

        ColorCloud cloud;
        cGrid.copy_to_cloud(cloud);

        io::CloudIO::save_cloud("E:/_clouds/mix_cloud.obj", cloud);
        cGrid.test();

        cGrid.copy_to_cloud(cloud);
        io::CloudIO::save_cloud("E:/_clouds/mix_cloud2.obj", cloud);

        Buffer<std::int16_t> vv;
        auto p = vv.cspan<std::int32_t>();

        return;

        // cGrid.compute_grid();
        // ColorCloud gridC;
        // cGrid.convert_to_cloud(gridC);


        // std::cout << "\n";
        // std::cout << "\t\tFULLSIZE: " << fullCloud.size() << " " << gridC.size() << "\n";

        if(idF < 1){
            // tool::data::FastPForEncoder encoder;

            // // Buffer<CVoxel> voxels;

            // // ColorCloud voxC;
            // // cGrid.convert_to_cloud(voxC);

            // Buffer<uint32_t> xyzrgb;
            // // Buffer<uint8_t> rgb;
            // cGrid.copy_to_buffer(xyzrgb);

            using namespace FastPForLib;
            {
                // // std::unique_ptr<CompositeCodec<SIMDFastPFor<8>, VariableByte>> codec = std::make_unique<CompositeCodec<SIMDFastPFor<8>, VariableByte>>();
                // auto codec = std::make_unique<CompositeCodec<SIMDFastPFor<8>, VariableByte>>();
                // // CompositeCodec<FastPFor<8>, VariableByte> codec;

                // Buffer<std::uint32_t> encodedB;
                // encodedB.resize(xyzrgb.size() + 1024);

                // size_t oSize = encodedB.size();

                // auto t1 = Time::nanoseconds_since_epoch();
                // codec->encodeArray(
                //     reinterpret_cast<std::uint32_t*>(xyzrgb.get_data()),
                //     xyzrgb.size(),
                //     encodedB.get_data(),
                //     oSize
                //     );
                // auto t2 = Time::nanoseconds_since_epoch();
                // // std::cout << "OSIZE " << oSize <<"\n";
                // encodedB.resize(oSize);



                // size_t outSize = xyzrgb.size();
                // Buffer<uint32_t> decodedXYZ;
                // decodedXYZ.resize(outSize*2);

                // auto t3 = Time::nanoseconds_since_epoch();
                // codec->decodeArray(
                //     encodedB.get_data(),
                //     encodedB.size(),
                //     decodedXYZ.get_data(),
                //     outSize
                //     );
                // auto t4 = Time::nanoseconds_since_epoch();
                // // std::cout << "OUTSIZE " << outSize <<"\n";
                // decodedXYZ.resize(outSize);


                // for(size_t ii = 0; ii < xyzrgb.size(); ++ii){
                //     if(xyzrgb[ii] != decodedXYZ[ii]){
                //         std::cout << "ERROR " << ii << " "<< xyzrgb[ii] << " " << decodedXYZ[ii] << "\n";
                //     }
                // }

                // std::cout << "END " << xyzrgb.size() << " " << " " << encodedB.size() << " " << oSize << " " <<
                //     Time::difference_micro_s(t1,t2).count() << " " << Time::difference_micro_s(t3,t4).count() << " " <<
                //     1.0*encodedB.size()/xyzrgb.size() <<
                //     "\n";

                // auto frameS = 1.0*encodedB.size()*4/1024/1024;
                // std::cout << "FRAME SIZE : " << frameS << "MB " << frameS*30*60 << "MB/m \n";


                // int src_len     = xyzrgb.bytes_size();
                // int max_len     = lzav_compress_bound( src_len );
                // void* comp_buf  = malloc( max_len );
                // auto t5 = Time::nanoseconds_since_epoch();
                // int comp_len = lzav_compress_default( xyzrgb.get_data(), comp_buf, src_len, max_len );
                // auto t6 = Time::nanoseconds_since_epoch();
                // if( comp_len == 0 && src_len != 0 )
                // {
                //     std::cout << "ERROR: " << comp_len << "\n";
                //     // Error handling.
                // }else{
                //     std::cout << "comp_len " << src_len << " " << comp_len << " " << 1.0 *comp_len / src_len << " " << Time::difference_micro_s(t5,t6).count() << "\n";
                // }

                // std::cout << "DESTROY\n";
                // codec = nullptr;
                // std::cout << "END DESTROY\n";

            }
            // auto sp = std::span(reinterpret_cast<std::uint64_t*>(voxels.get_data()), voxels.size());
            // std::sort(sp.begin(), sp.end());
            // std::uint64_t* d = reinterpret_cast<std::uint64_t*>(voxels.get_data());

            // for(int ii = 0; ii < voxels.size(); ++ii){
            //     // std::cout << d[ii] << " ";
            //     d[ii] = rand()%10000;
            // }
            // // std::sort(d, d + voxels.size()/8);


            // Buffer<std::byte> encodedBytes;
            // encoder.encode(std::span(reinterpret_cast<std::byte*>(voxels.get_data()), voxels.size()*8), encodedBytes);

            // Buffer<std::byte> bytes2;
            // bytes2.resize(voxels.bytes_size());
            // tool::data::FastPForDecoder decoder;
            // decoder.decode(encodedBytes, bytes2);

            // std::cout << "SIZE BYTES " << voxels.size() << " " << (1.0*voxels.size()/8/1024/1024)*30*60 << " " << encodedBytes.size()/8 <<" " << bytes2.size()/8  << "\n";

            // io::CloudIO::save_cloud("E:/_clouds/fusion1.obj", gridC);

        }else{
            // break;
        }

    // }

    Buffer<std::uint8_t> bb;

    std::uint8_t lvl1 = 0b11111111;
    bb.push_back(lvl1);

    std::uint8_t d1 = 0b01000000;
    if(lvl1 & d1){
        std::cout << "bouh\n";
    }
    // std::byte d1{0b10000000};
    // if((lvl1 & d1) ){

    // }
    // std::cout << lvl1  << "\n";


    std::cout << "-> " << (int)lvl1 << "\n";

    return;

    {
        Buffer<std::uint16_t> bi;
        bi.resize(10000);
        for(size_t ii = 0; ii < bi.size(); ++ii){
            bi[ii] = rand()%10000;
            // bi[ii] = 15;
            // bi[ii] = ii;
        }

        tool::data::FastPForEncoder encoder;
        Buffer<std::byte> encodedBytes;
        encoder.encode(std::span<std::byte>(reinterpret_cast<std::byte*>(bi.get_data()), bi.size()*2), encodedBytes);


        std::cout << "SIZE BYTES: " << bi.size()*2 << " " << bi.element_bytes_size()*bi.size() << " " << encodedBytes.size() << "\n";

        // std::sort(bi.begin(), bi.end());
        // Buffer<std::byte> encodedBytes2;
        // encoder.encode(std::span<std::byte>(reinterpret_cast<std::byte*>(bi.get_data()), bi.size()*4), encodedBytes2);

        // std::cout << "SIZE BYTES: " << bi.size() << " " << bi.element_bytes_size()*bi.size() << " " << encodedBytes.size() << "\n";
    }

    {
        Buffer<std::uint32_t> bi;
        bi.resize(10000);
        for(size_t ii = 0; ii < bi.size(); ++ii){
            bi[ii] = rand()%10000;
            // bi[ii] = 15;
            // bi[ii] = ii;
        }

        tool::data::FastPForEncoder encoder;
        Buffer<std::byte> encodedBytes;
        encoder.encode(std::span<std::byte>(reinterpret_cast<std::byte*>(bi.get_data()), bi.size()*4), encodedBytes);


        Buffer<std::byte> decodedBytes;
        decodedBytes.resize(bi.size()*4);

        std::cout << "SIZE BYTES: " << bi.size()*4 << " " << bi.element_bytes_size()*bi.size() << " " << encodedBytes.size() << "\n";

        tool::data::FastPForDecoder decoder;
        std::cout << "success : " << decoder.decode(encodedBytes, decodedBytes) << "\n";
    }

    {
        Buffer<std::uint64_t> bi;
        bi.resize(10000);
        for(size_t ii = 0; ii < bi.size(); ++ii){
            bi[ii] = rand()%10000;
            // bi[ii] = 15;
            // bi[ii] = ii;
        }


        Buffer<std::byte> encodedBytes;

        {
            // tool::data::FastPForEncoder encoder;
            // std::cout << "success : " << encoder.encode64(std::span<std::byte>(reinterpret_cast<std::byte*>(bi.get_data()), bi.size()*8), encodedBytes) << "\n";

            Buffer<std::uint32_t> encoded;
            tool::data::FastPForEncoder encoder;
            // std::cout << "success : " << encoder.encode64(bi.span(), encoded) << "\n";

            encoder.encode(std::span<std::byte>(reinterpret_cast<std::byte*>(bi.get_data()), bi.size()*8), encodedBytes);
            std::cout << "BYTES: " << bi.size() << " " << bi.bytes_size() << " "<<  encodedBytes.size() << " " << encodedBytes.size()/8<< " " <<  1.0* encodedBytes.size()/8 / bi.size()<< "\n";

            tool::data::FastPForDecoder decoder;

            Buffer<std::byte> decodedBytes;
            decodedBytes.resize(bi.bytes_size());

            decoder.decode(encodedBytes.span(), decodedBytes);

            auto dcb = std::span<std::uint64_t>(reinterpret_cast<std::uint64_t*>(decodedBytes.get_data()), bi.size());

            for(size_t id = 0; id < bi.size(); ++id){
                if(bi[id] != dcb[id]){
                    std::cout << "I";
                }else{
                    // std::cout << "W";
                }
            }
        }


        // Buffer<std::byte> decodedBytes;
        // decodedBytes.resize(bi.size()*8);

        // {
        //     tool::data::FastPForDecoder decoder;
        //     std::cout << "success : " << decoder.decode64(encodedBytes, decodedBytes) << "\n";
        // }

        // {
        //
        //     {
        //

        //
        //
        //         std::cout << "SIZE BYTES: " << bi.size()*8 << " " << bi.element_bytes_size()*bi.size() << " " << encodedBytes.size() << " "<< decodedBytes.size() << "\n";
        //     }
        // std::cout << "decode\n";
        // }
        // std::cout << "ok";

    }

    // // std::cout << "end\n";

    return;
    for(size_t idC = 0; idC < video.nb_devices(); ++idC){
        std::cout << "CAMERA " << idC << "\n";
        for(size_t idF = 0; idF < video.nb_frames(idC); ++idF){
            std::cout << idF << " " << std::flush;
            DCFrame frame;
            // std::cout << "uncompress " << idC << " " << idF << "\n";
            // video.generate_frame(settings, idC, idF, frame);

            // auto cb = frame.volume_buffer<ColorCloud>(DCVolumeBufferType::ColoredCloud);
            // std::cout << idF << " " << cb->size() << "\n";

            // frame.volume_buffer()
            // frame.cl

            // auto ccc = video.get_data_frame(idC, idF);
            // int v = ccc.lock().get()->validVerticesCount;

            // if(!initialized){
            //     mInfos.initialize(frame.mode);
            //     dIndices.initialize(mInfos.has_depth(), mInfos.depth_width(), mInfos.depth_height());
            //     initialized = true;

            //     std::cout << "cloud " << (int) frame.mode  << " " << frame.calibration.size() << " " << frame.depth.size() << " " << frame.rgbaDepthSizedColor.size() << " " <<  frame.cloud.empty() << std::endl;
            // }

            // std::vector<bool> filteringM(frame.depth.size(), false);

            // for(size_t idL = 0; idL < 3; ++idL){
            //     std::for_each(std::execution::par_unseq, std::begin(dIndices.depths1DNoBorders), std::end(dIndices.depths1DNoBorders), [&](size_t id){
            //         if(frame.depth[id] ==  dc_invalid_depth_value){
            //             return;
            //         }
            //         std::uint8_t count = 0;
            //         for(auto cId : dIndices.neighbours8Depth1D[id]){
            //             if(frame.depth[cId] != dc_invalid_depth_value){
            //                 ++count;
            //             }
            //         }
            //         filteringM[id] = count >= 6;
            //     });


            //     for(size_t id = 0; id < filteringM.size(); ++id){
            //         if(!filteringM[id]){
            //             frame.depth[id] = dc_invalid_depth_value;
            //         }
            //     }
            // }

            // size_t idValid = 0;
            // for(size_t id = 0; id < filteringM.size(); ++id){
            //     if(filteringM[id]){
            //         ++idValid;
            //     }
            // }

            // auto dFrame = std::make_shared<DCDataFrame>();
            // // compressor.compress(frame, 100, cFrame.get());
            // // cFrame->validVerticesCount = idValid;


            // // std::cout << "ccloud " << cFrame->calibration.size() << " " << (int)cFrame->mode << " "<< cFrame->fpfDepth.size() << " " << cFrame->validVerticesCount << " " << v <<  "\n";
            // video.replace_data_frame(idC, idF, std::move(dFrame));
            // // fIndices.neighbours8Depth1D;
            // // frame.depth.values;



            // // fIndices
            // frame.mode;
            // frame.calibration;
            // frame.depth


        }
    }

    String::replace_first(path, ".kvid", "-flo-modified.kvid");
    video.save_to_file(path);
}
