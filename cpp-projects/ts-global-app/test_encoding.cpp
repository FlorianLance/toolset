
/*******************************************************************************
** Toolset-ts-global-app                                                      **
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

#include "tests.hpp"

// std
#include <iostream>

// fastfor
#include "fastpfor/fastpfor.h"
#include "fastpfor/simdfastpfor.h"
#include "fastpfor/compositecodec.h"
#include "fastpfor/variablebyte.h"

// base
#include "depth-camera/dc_video_player.hpp"
#include "geometry/color_voxel_grid.hpp"
#include "data/fastpfor_encoding.hpp"
#include "utility/logger.hpp"

using namespace tool;
using namespace tool::geo;
using namespace tool::cam;
using namespace tool::data;

using namespace std::chrono;


struct DD{

    std::uint64_t
        l1  : 3,
        l2  : 3,
        l3  : 3,
        l4  : 3,
        l5  : 3,
        l6  : 3,
        l7  : 3,
        l8  : 3,
        l9  : 3,
        l10 : 3,
        l11 : 3,
        l12 : 3,
        l13 : 3,
        l14 : 3,
        l15 : 3,
        l16 : 3,
        l17 : 3,
        l18 : 3,
        l19 : 3,
        l20 : 3,
        l21 : 3;
};

struct LLL{
    std::uint64_t
        level : 5, // from 0 - 31
        encoding : 60;
};


auto encode1() -> void{

    // load video
    DCVideo video;
    std::string path = "E:/_kvid/30-07-24-fam.kvid";
    video.load_from_file(path);

    DCFrameGenerationSettings settings;
    settings.cloud = true;

    // generate full cloud from all cameras one frame
    ColorCloud fullCloud;
    auto startTS = video.device_first_frame_timestamp(0);
    for(size_t idF = 50; idF < video.nb_frames(0); ++idF){

        auto firstCameraCurrentFrameTS = video.get_data_frame(0, idF).lock().get()->afterCaptureTS;
        auto diffNs = nanoseconds(firstCameraCurrentFrameTS - startTS);
        auto currentTimeMS = duration_cast<milliseconds>(diffNs);


        DCFrame d0Frame;
        video.generate_frame(settings, 0, idF, d0Frame);


        auto cp = d0Frame.volume_buffer<ColorCloud>(DCVolumeBufferType::ColoredCloud);
        fullCloud.merge(*cp);

        // std::cout << "\t";
        for(size_t idC = 1; idC < video.nb_devices(); ++idC){
            auto cfid = video.closest_frame_id_from_time(idC, 1.0*currentTimeMS.count());

            DCFrame dIFrame;
            video.generate_frame(settings,idC, cfid, dIFrame);
            cp = dIFrame.volume_buffer<ColorCloud>(DCVolumeBufferType::ColoredCloud);

            cp->vertices.apply_transformation(video.get_transform(idC).conv<float>());

            fullCloud.merge(*cp);
        }
        break;
    }

    size_t nbLevels = 12;
    Buffer<std::uint64_t> ids;

    // 0 1m 1
    // 1 0.5m 8
    // 2 0.25m 64
    // 3 0.12m 512
    // 4 0.05m 4098
    // 5 0.025m 32768
    // 6 0.0125m 262144
    // 7 0.006m 2M
    // 8 3mm 16M
    // 9 1.5mm 128M
    // 10 0.75mm 1MM

    Pt3f volumeOrigin = {-2.f,-2.f,-2.f};
    Pt3f volumeSize   = {4.f,4.f,4.f};
    Pt3f max = volumeOrigin + volumeSize;
    std::vector<double> lFactors;
    std::cout << "factors: ";
    for(int idL = 0; idL < nbLevels; ++idL){
        lFactors.push_back(1.0 / std::pow(2, idL+1));
        std::cout << idL << " " << lFactors.back() << "\n";
    }

    std::cout << "SIZE " << fullCloud.size() << "\n";

    Buffer<Buffer<std::uint8_t>> encoding;
    Vertices3D nb;
    nb.reserve(fullCloud.vertices.size());
    encoding.reserve(fullCloud.vertices.size());
    Pt3f diff = max - volumeOrigin;
    for(auto &v : fullCloud.vertices){

        // Pt3f diff =  v - volumeOrigin;
        if(v < volumeOrigin || v > max){
            // outside
            continue;
        }

        v -= volumeOrigin;
        v.x() /= diff.x();
        v.y() /= diff.y();
        v.z() /= diff.z();
        double cfX = 0.5;
        double cfY = 0.5;
        double cfZ = 0.5;
        // std::int32_t tId = 0;


        // Buffer<std::uint8_t> ids;
        // ids.resize(nbLevels);

        std::uint64_t id64 = 0;

        // std::uint64_t idE = 0;
        for(int idL = 0; idL < nbLevels; ++idL){

            int c1 = (v.x() > cfX);
            int c2 = (v.y() > cfY);
            int c3 = (v.z() > cfZ);

            // int b0 = !c1 && !c2 && !c3;
            int b1 = c1 && !c2 && !c3;
            int b2 = !c1 && c2 && !c3;
            int b3 = !c1 && !c2 && c3;

            int b4 = c1 && c2 && !c3;
            int b5 = c1 && !c2 && c3;
            int b6 = !c1 && c2 && c3;
            int b7 = c1 && c2 && c3;

            std::uint8_t idd = b1 * 1 + b2 * 2 + b3 * 3 + b4 * 4 + b5 * 5 + b6 * 6 + b7 * 7;
            // ids[idL] = idd;
            id64 += idd << (3*idL);


            auto cf = lFactors[idL];
            cfX += (-0.5 + c1) * cf;
            cfY += (-0.5 + c2) * cf;
            cfZ += (-0.5 + c3) * cf;
        }

        ids.push_back(id64);
    }


    std::cout << "SIZE2: " << ids.size() << "\n";

    // Buffer<std::uint64_t> ids;
    // ids.resize(100000);

    // for(size_t idV = 0; idV < ids.size(); ++idV){
    //     std::uint64_t tId = 0;
    //     for(size_t idL = 0; idL < nbLevels; ++idL){
    //         // add level?
    //         tId |= static_cast<std::uint64_t>(rand()%8) << ((nbLevels-1-idL)*3);
    //     }
    //     ids[idV] = tId;
    // }


    // std::sort(ids.begin(), ids.end());

    std::vector<std::uint64_t> fff;
    for(size_t idL = 0; idL < nbLevels; ++idL){
        fff.push_back(std::uint64_t(0b111) << ((nbLevels-1-idL)*3));
    }

    struct Node{
        std::array<std::unique_ptr<Node>, 8> children;
        std::uint8_t idLevel = 0;
        std::uint8_t value = 0;
    };

    {


        std::vector<size_t> countPerLevel;
        countPerLevel.resize(nbLevels);
        std::fill(countPerLevel.begin(), countPerLevel.end(), 0);

        Node root;
        root.idLevel = 0;

        Buffer<Node*> allNodes;
        size_t nbNodes = 0;
        for(const auto &id : ids){

            Node* currentNode = &root;
            for(size_t idL = 0; idL < nbLevels; ++idL){
                std::uint8_t v = (id & fff[idL]) >> ((nbLevels-1-idL)*3);
                if(currentNode->children[v] == nullptr){
                    currentNode->children[v] = std::make_unique<Node>();
                    currentNode->children[v]->idLevel = idL;
                    currentNode->children[v]->value   = v;
                    allNodes.push_back(currentNode->children[v].get());
                    countPerLevel[idL]++;
                    ++nbNodes;
                }
                currentNode = currentNode->children[v].get();
            }
        }



        std::cout << "Nb nodes: " << nbNodes << " " << (nbNodes/1024.0/1024.0) << " " << ids.size()*4*3/1024.0/1024.0 << " " << ids.size()*8/1024.0/1024.0 << "\n";
        size_t idLL = 0;
        float fullSize = 4.f;
        for(const auto &nb : countPerLevel){
            std::cout << idLL << " -> " << nb << " " << fullSize/ (std::pow(2,idLL)) << "\n";
            ++idLL;
        }

        // 4*8

        // 10m
        // 1cm => 0.01m
        //



        // {
        //     Node* currentNode = &root;
        //     while(currentNode != nullptr){
        //         for(size_t id = 0; id < 8; ++id){
        //             if(currentNode->children[id]){

        //             }
        //         }
        //     }

        // }
    }

    // 16
    // 16
    // 16


    // tester hauffman coding

    return;

    // 3*15 -> 45
    // 3*14 -> 42
    // 3*13 -> 39 + 24 -> 63

    // 45

    // 1_32b: 30 (r2)
    // 2_32b: 15 + 15 (r2)
    // 3_32b: 30 (r2)
    // 4_32B: 24c + 8c

    // N * 24

    // N0
    // N1 N2 N3 N4
    std::cout << "MIN_u16 " << std::numeric_limits<std::uint16_t>::min() << "\n";
    std::cout << "MAX_u16 " << std::numeric_limits<std::uint16_t>::max() << "\n";
    std::cout << "LOW_u16 " << std::numeric_limits<std::uint16_t>::lowest() << "\n";
    std::cout << "INF_u16 " << std::numeric_limits<std::uint16_t>::infinity() << "\n";

    CVoxel vox;
    vox.xIndex = 16383;
    vox.yIndex = 4095;
    vox.zIndex = 4096*2-1;

    ColorVoxelGrid grid(0.001f, {-2.f,-2.f,-2.f}, {4.f,4.f,4.f});
    grid.add_cloud(fullCloud, geo::Mat4f::identity());
    grid.compute_grid();

    Buffer<CVoxel> voxels;
    grid.copy_to_buffer(voxels);


    tool::data::FastPForEncoder encoding2;

    BinaryBuffer encodedBuffer;




    using namespace FastPForLib;
    {
        CompositeCodec<FastPFor<8>, VariableByte> codec;

        Buffer<std::uint32_t> encoded2;
        encoded2.resize(voxels.size()*2 + 1024);

        size_t encodedSize = encoded2.size();
        codec.encodeArray(
            reinterpret_cast<std::uint64_t*>(voxels.get_data()),
            voxels.size(),
            encoded2.get_data(),
            encodedSize
            );
        std::cout << "inputsize 32: " << encoded2.size() << "\n";
        std::cout << "encodedSize 32: " << encodedSize << "\n";
        std::cout << "encodedSize MB: " << encodedSize/4.0/1024./1024. << "\n";
    }


    //


    {

        // compute_deltas_inplace(datain, length, 0);// compute the deltas in-place
        // compute_prefix_sum_inplace(datain, length, 0);// under the delta computations (datain is back)
    }

    // auto ss = std::span<std::byte>(voxels.get_byte_data(), voxels.size()*8);
    // std::cout << "encoding " << ss.size() << "\n";
    // encoding2.encode(ss, encodedBuffer);
    // std::cout << "voxels size " << fullCloud.size() << " " << voxels.size()*8.0/1024.0/1024.0 << " " << encodedBuffer.size()/1024.0/1024.0 << "\n";

    // 16: x
    // 16: y
    // 16: z
    // 16: rb
    // 16: ga

    // 24 :
    //0 16000
    //1 8000
    //2 4000
    //3 2000
    //4 1000
    //5 500
    //6 250
    //7 125
    //8 62
    //9 30
    //10 15
    //11 7
    //12 3.5
    //13 1.7
    //14 0.8

    // 1


    // 8m

    // 8000
    // 111 111
    //

    // std::cout << "vox.xIndex " << vox.xIndex << " " << vox.yIndex << " " << vox.zIndex << "\n";
    // 16: 0 - 65000
    // 15: 0 - 32000
    // 14: 0 - 16000
    // 13: 0 - 8000

    // 14 * 3 -> 42
    // 13 * 2 -> 39
    std::cout <<"end\n";
}

// 0 0 0 0 0 0 1 2 3 4 5
// 0 0 0 1 0 0 0 0 1 0 0

// x
// y
// z
// rgb


// lossless mpeg



// 3 bits / lvl
// 0 10m
// 1 5m
// 2 1.5m
// 3 0.75m
// 4 32cm
// 5 16cm
// 6 8cm
// 7 4cm
// 8 2cm
// 10 1cm
// 11 0.5cm
// 12 2.5mm
// 13 1.25mm
// 14 0.62mm

// 13 -> 5m / 5m / 5m
// 13 * 3 -> 39 + 24 -> 63

// 14 + 14 + 14
// -> 42 bits


auto encode2() -> void{


    Log::message("Load video\n");
    DCVideo video;
    std::string path = "E:/_kvid/30-07-24-fam.kvid";
    video.load_from_file(path);

    DCFrameGenerationSettings settings;
    settings.cloud = true;

    Log::message("Generate full cloud from one frame\n");
    auto startTS = video.device_first_frame_timestamp(0);
    size_t idF = 50;

    auto firstCameraCurrentFrameTS = video.get_data_frame(0, idF).lock().get()->afterCaptureTS;
    auto diffNs = nanoseconds(firstCameraCurrentFrameTS - startTS);
    auto currentTimeMS = duration_cast<milliseconds>(diffNs);
    // auto firstCameraCurrentFrameTS = video.get_data_frame(0, idF).lock().;


    DCFrame d0Frame;
    video.generate_frame(settings, 0, idF, d0Frame);

    ColorCloud fullCloud;
    auto cp = d0Frame.volume_buffer<ColorCloud>(DCVolumeBufferType::ColoredCloud);
    fullCloud.merge(*cp);

    // std::cout << "\t";
    for(size_t idC = 1; idC < video.nb_devices(); ++idC){
        auto cfid = video.closest_frame_id_from_time(idC, 1.0*currentTimeMS.count());

        DCFrame dIFrame;
        video.generate_frame(settings,idC, cfid, dIFrame);
        cp = dIFrame.volume_buffer<ColorCloud>(DCVolumeBufferType::ColoredCloud);

        cp->vertices.apply_transformation(video.get_transform(idC).conv<float>());

        fullCloud.merge(*cp);
    }

    Log::message("Save full cloud\n");
    CloudIO::save_cloud("E:/_clouds/fullcloud50.obj", fullCloud);

    Pt3f volumeOrigin = {-2.f,-2.f,-2.f};
    Pt3f volumeSize   = {4.f,4.f,4.f};
    Pt3f max = volumeOrigin + volumeSize;

    constexpr int nbLevels = 14;

    Log::message("Generate levels factors\n");
    std::vector<double> lFactors;
    for(int idL = 0; idL < nbLevels; ++idL){
        lFactors.push_back(1.0 / std::pow(2, idL+1));
        std::cout << "\t" << idL << " " << lFactors.back() << "\n";
    }

    Buffer<Buffer<std::uint8_t>> encoding;
    Vertices3D nb;
    nb.reserve(fullCloud.vertices.size());
    encoding.reserve(fullCloud.vertices.size());
    Pt3f diff = max - volumeOrigin;
    for(auto &v : fullCloud.vertices){

        // Pt3f diff =  v - volumeOrigin;
        if(v < volumeOrigin || v > max){
            // outside
            continue;
        }

        v -= volumeOrigin;
        v.x() /= diff.x();
        v.y() /= diff.y();
        v.z() /= diff.z();

        double cfX = 0.5;
        double cfY = 0.5;
        double cfZ = 0.5;

        Buffer<std::uint8_t> ids;
        ids.resize(nbLevels);

        std::uint64_t id64 = 0;

        // std::uint64_t idE = 0;
        for(int idL = 0; idL < nbLevels; ++idL){

            int c1 = (v.x() > cfX);
            int c2 = (v.y() > cfY);
            int c3 = (v.z() > cfZ);

            // int b0 = !c1 && !c2 && !c3;
            int b1 = c1 && !c2 && !c3;
            int b2 = !c1 && c2 && !c3;
            int b3 = !c1 && !c2 && c3;

            int b4 = c1 && c2 && !c3;
            int b5 = c1 && !c2 && c3;
            int b6 = !c1 && c2 && c3;
            int b7 = c1 && c2 && c3;

            std::uint8_t idd = b1 * 1 + b2 * 2 + b3 * 3 + b4 * 4 + b5 * 5 + b6 * 6 + b7 * 7;
            ids[idL] = idd;

            id64 += idd << (3*idL);


            // idE = (int)idd + 10*idE;

            // std::cout << idE << "\n";
            // std::cout << (int)idd << " -> " << cfX << " " << cfY << " "<< cfZ << "\n";
            // std::cout << b0 << b1 << b2 << b3 << b4 << b5 << b6 << b7 << "\n";
            // std::cout << c1 << c2 << c3 << "|";

            auto cf = lFactors[idL];
            cfX += (-0.5 + c1) * cf;
            cfY += (-0.5 + c2) * cf;
            cfZ += (-0.5 + c3) * cf;

            // std::uint8_t encoding = 0;
            // encoding = 0b00000001 * c1;


            // if(v.x() < cfX && v.y() < cfY  && v.z() < cfZ){
            //     cfX -= cfX*0.5f;
            //     cfY -= cfY*0.5f;
            //     cfZ -= cfZ*0.5f;
            //     std::cout << "0";
            // }else{
            //     cfX += cfX*0.5f;
            //     std::cout << "1";
            // }
        }

        // std::cout << "id: " << idE << " : ";
        // std::cout << "v " << v.x() << " " << v.y() << " " << v.z() << " : " << idE << "\n";
        // for(auto &id : ids){
        //     std::cout << (int)id;
        // }
        // std::cout << "\n";


        encoding.push_back(std::move(ids));
        // if(!values.contains(idE)){
        //     values.insert(idE);
        // }else{
        //     std::cout << "already inside\n";

        // }


        cfX = 0.5;
        cfY = 0.5;
        cfZ = 0.5;

        for(int idL = 0; idL < nbLevels; ++idL){

            // double vv =(idE / std::pow(10, nbLevels-idL-1));
            // auto cId  = static_cast<uint8_t>(std::trunc(fmod(vv,10)));

            auto cId = encoding.back()[idL];

            int c1 = 0;
            int c2 = 0;
            int c3 = 0;

            switch(cId){
            case 0:
                c1 = 0;
                c2 = 0;
                c3 = 0;
                break;
            case 1:
                c1 = 1;
                c2 = 0;
                c3 = 0;
                break;
            case 2:
                c1 = 0;
                c2 = 1;
                c3 = 0;
                break;
            case 3:
                c1 = 0;
                c2 = 0;
                c3 = 1;
                break;
            case 4:
                c1 = 1;
                c2 = 1;
                c3 = 0;
                break;
            case 5:
                c1 = 1;
                c2 = 0;
                c3 = 1;
                break;
            case 6:
                c1 = 0;
                c2 = 1;
                c3 = 1;
                break;
            case 7:
                c1 = 1;
                c2 = 1;
                c3 = 1;
                break;
            }

            auto cf = lFactors[idL];
            cfX += (-0.5 + c1) * cf;
            cfY += (-0.5 + c2) * cf;
            cfZ += (-0.5 + c3) * cf;

            // std::cout << "V: " << vv << " " << fmod(vv,10)  << " "<< (int)cId << " " << cfX << " " << cfY << " " << cfZ << "\n";
            // 1 is 12345 / 10000 % 10
        }
        nb.push_back(geo::Pt3d{cfX, cfY, cfZ}.conv<float>());

    }

    for(size_t ii = 0; ii < nb.size(); ++ii){
        const auto &ov = fullCloud.vertices[ii];
        const auto &nv = nb[ii];
        if(norm(ov-nv) > 0.0001){
            std::cout << ov << " " << nv  << "\n";
        }

        // if(norm(ov-nv)){

        // }
    }


    for(size_t ii = 0; ii < encoding.size(); ++ii){
        std::uint64_t total = 0;
        for(int jj = 0; jj < encoding[ii].size(); ++jj){
            total += encoding[ii][jj] << (3*jj);
        }

    }
}


