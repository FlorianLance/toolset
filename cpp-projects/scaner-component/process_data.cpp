
/*******************************************************************************
** exvr-scaner-component                                                      **
** No license (to be defined)                                                 **
** Copyright (c) [2018] [Florian Lance][EPFL-LNCO]                            **
********************************************************************************/

#include "process_data.hpp"

// std
#include <chrono>

// turbo-jpeg
#include <turbojpeg.h>

// base
#include "data/integers_encoder.hpp"

using namespace std::chrono;
using namespace tool;
using namespace tool::camera;

struct ProcessData::Impl{

    Impl(size_t id){

        this->id = id;
        jpegDecompressor = tjInitDecompress();

        bufferDecompressColor.resize(k2_color_width*k2_color_height*4);
        decodedDepth.resize(k2_depth_count*2);
        validityDepth2Rgb.resize(k2_depth_count);
        validIdPerPointMesh.resize(k2_depth_count);
    }

    size_t id;


    // processed data for display
    std::vector<unsigned char> bufferDecompressColor;
    std::vector<std::uint16_t> decodedDepth;
    std::vector<bool> validityDepth2Rgb;
    std::vector<std::uint32_t> validIdPerPointMesh;

    // processed data for display
    camera::K2CloudDisplayData cloudData;
    camera::K2MeshDisplayData meshData;
    camera::K2BodiesDisplayData bodiesData;

    // time measurment
    std::int64_t allProcessTime = 0;


    // decompressors
    tjhandle jpegDecompressor;
    data::IntegersEncoder depthDecompressor;

    // data received
    K2UdpHeader header;
    std::shared_ptr<K2Frame> frame = nullptr;
};

ProcessData::ProcessData(size_t id) : m_p(std::make_unique<Impl>(id)){
}

ProcessData::~ProcessData(){
    tjDestroy(m_p->jpegDecompressor);
}

K2CloudDisplayData *ProcessData::cloud_data(){
    return &m_p->cloudData;
}

K2MeshDisplayData *ProcessData::mesh_data(){
    return &m_p->meshData;
}

void ProcessData::process_data_from_udp_frame(K2UdpHeader header, std::shared_ptr<K2Frame> frame){

    if(isProcessing){
        return;
    }

    auto timeStart = std::chrono::high_resolution_clock::now();
    isProcessing = true;

    m_p->frame = frame;
    m_p->header = std::move(header);
    if(frame->mode == K2FrameRequest::depth_512x424){
        process_depth();
    }else if(frame->mode == K2FrameRequest::infra_512x424 || frame->mode == K2FrameRequest::long_exposure_infra_512x424){
        process_infra();
    }else if(frame->mode == K2FrameRequest::compressed_color_1920x1080){
        process_compressed_color_1920x1080();
    }else if(frame->mode == K2FrameRequest::compressed_color_512x424){
        process_compressed_color_512x424();
    }else if(frame->mode == K2FrameRequest::compressed_color_cloud){
        process_compressed_color_cloud();
    }else if(frame->mode == K2FrameRequest::compressed_color_mesh){
        process_compressed_color_mesh();
    }

    isProcessing = false;        
    end_processing_signal(frame->timeStampGetFrame, m_p->allProcessTime = duration_cast<milliseconds>(high_resolution_clock::now() - timeStart).count());
}

void ProcessData::process_compressed_color_1920x1080(){

    // uncompress color
    tjDecompress2(m_p->jpegDecompressor, m_p->frame->compressedImage.data(), m_p->frame->jpegColorSize,
                  m_p->bufferDecompressColor.data(), k2_color_width, 0/*pitch*/, k2_color_height, TJPF_RGB, TJFLAG_FASTDCT);
}

void ProcessData::process_compressed_color_512x424(){

    // uncompress color
    tjDecompress2(m_p->jpegDecompressor, m_p->frame->compressedImage.data(), m_p->frame->jpegColorSize,
                  m_p->bufferDecompressColor.data(), k2_depth_width, 0/*pitch*/, k2_depth_height, TJPF_RGB, TJFLAG_FASTDCT);
}

void ProcessData::process_depth(){

//    QImage depthImage(kinect2_depth_width, kinect2_depth_height,QImage::Format_RGB888);
//    depthImage.fill(Qt::black);
//    size_t id = 0;

//    auto depth = m_p->frame->depthData.get();

//    for(size_t ii = 0; ii < kinect2_depth_height; ++ii){
//        for(size_t jj = 0; jj < kinect2_depth_width; ++jj){

//            auto d = (*depth)[id++];
//            if(d == 0){
//                continue;
//            }

//            qreal dValue = static_cast<qreal>(d*0.001);
//            QColor col;
//            for(auto cm : m_p->colorMap){
//                if(!(dValue < cm.first.second)){
//                    continue;
//                }

//                dValue = (dValue - cm.first.first) / (cm.first.second-cm.first.first);
//                const QColor &c1 = cm.second.first;
//                const QColor &c2 = cm.second.second;
//                col.setRedF(dValue* c2.redF() + (1. - dValue) * c1.redF());
//                col.setGreenF(dValue* c2.greenF() + (1. - dValue) * c1.greenF());
//                col.setBlueF(dValue* c2.blueF() + (1. - dValue) * c1.blueF());
//                depthImage.setPixelColor(static_cast<int>(jj), static_cast<int>(ii), col);
//                break;
//            }
//        }
//    }
//    emit update_depth_signal(std::move(depthImage));

    // TODO: use Kinect2ImageDisplayData
}


void ProcessData::process_infra(){

//    QImage infraImage(kinect2_infrared_width, kinect2_infrared_height,QImage::Format_RGB888);
//    infraImage.fill(Qt::black);
//    size_t id = 0;

//    auto infra = m_p->frame->infraData.get();

//    auto *b = infraImage.bits();
//    for(size_t ii = 0; ii < kinect2_infrared_count; ++ii){

//        auto i = (*infra)[id++];
//        if(i == 0){
//            continue;
//        }

//        qreal iValue = static_cast<qreal>(i);
//        QColor col;
//        for(auto cm : m_p->infraredMap){
//            if(!(iValue < cm.first.second)){
//                continue;
//            }

//            iValue = (iValue - cm.first.first) / (cm.first.second-cm.first.first);
//            const QColor &c1 = cm.second.first;
//            const QColor &c2 = cm.second.second;

//            auto inv = (1. - iValue);
//            b[ii*3+0]   = static_cast<std::uint8_t>(255 * (iValue * c2.redF()   + inv * c1.redF()));
//            b[ii*3+1] = static_cast<std::uint8_t>(255   * (iValue * c2.greenF() + inv * c1.greenF()));
//            b[ii*3+2] = static_cast<std::uint8_t>(255   * (iValue * c2.blueF()  + inv * c1.blueF()));
//            break;
//        }
//    }


//    emit update_infra_signal(std::move(infraImage));

    // TODO: use Kinect2ImageDisplayData
}


void ProcessData::process_compressed_color_cloud(){

    stack_trace_message_signal("dll_process_compressed_color_cloud_1");

    // uncompress depth data
    m_p->depthDecompressor.decode(
        reinterpret_cast<std::uint32_t*>(m_p->frame->compressedDepthData.data()), m_p->frame->compressedDepthSize,
        reinterpret_cast<std::uint32_t*>(m_p->decodedDepth.data()), k2_depth_count/2
    );

    stack_trace_message_signal("dll_process_compressed_color_cloud_2");

    // uncompress color
    tjDecompress2(m_p->jpegDecompressor, m_p->frame->compressedImage.data(), m_p->frame->jpegColorSize,
                  m_p->bufferDecompressColor.data(), k2_depth_width, 0/*pitch*/, k2_depth_height, TJPF_RGB, TJFLAG_FASTDCT);

    stack_trace_message_signal("dll_process_compressed_color_cloud_3");

    // retrieve intrinsics
    const auto focalLengthX       = m_p->frame->intrinsics[0];
    const auto focalLengthY       = m_p->frame->intrinsics[1];
    const auto principalPointX    = m_p->frame->intrinsics[2];
    const auto principalPointY    = m_p->frame->intrinsics[3];


    stack_trace_message_signal("dll_process_compressed_color_cloud_4");
    m_p->cloudData.dataLocker.lock();

    // cloud
    m_p->cloudData.sizePts = 0;
    size_t currentId = 0;
    size_t currentNbPoints = 0;
    for(size_t ii = 0; ii < k2_depth_height; ++ii){
        for(size_t jj = 0; jj < k2_depth_width; ++jj){

            std::uint16_t currentDepth = m_p->decodedDepth[currentId];

            if(currentDepth != 0){
                const float d = static_cast<float>(currentDepth)*0.001f;
                // position
                // x = (u - principalPointX) / focalLengthX * d;
                // y = (v - principalPointY) / focalLengthY * d;
                // z = d;
                (*m_p->cloudData.points)[currentNbPoints] = geo::Pt3f{
                        (jj - principalPointX) / focalLengthX *d,
                        -(ii - principalPointY) / focalLengthY *d,
                        d
                };

                // color
                (*m_p->cloudData.colors)[currentNbPoints] = {
                    m_p->bufferDecompressColor[currentId*3 + 0]/255.f,
                    m_p->bufferDecompressColor[currentId*3 + 1]/255.f,
                    m_p->bufferDecompressColor[currentId*3 + 2]/255.f
                };

                // process normal
                geo::Vec3f dir;
                if(ii != 0 && jj != 0 && ii != k2_depth_height -1 && jj != k2_depth_width-1){
                    const float dzdx = (m_p->decodedDepth[(jj+1)*k2_depth_width+(ii)] - m_p->decodedDepth[(jj-1)*k2_depth_width+(ii)]) / 2.f;
                    const float dzdy = (m_p->decodedDepth[(jj)*k2_depth_width+(ii+1)] - m_p->decodedDepth[(jj)*k2_depth_width+(ii-1)]) / 2.f;
                    dir = normalize(geo::Vec3f{-dzdx, -dzdy, 1.f});
                }
                (*m_p->cloudData.normals)[currentNbPoints] = dir;

                ++currentNbPoints;
            }
            ++currentId;
        }
    }

    m_p->cloudData.sizePts = currentNbPoints;
    m_p->cloudData.dataLocker.unlock();
    stack_trace_message_signal("dll_process_compressed_color_cloud_5");

    // copy joints
    m_p->bodiesData.dataLocker.lock();
    m_p->bodiesData.bodies = m_p->frame->bodiesData;
    m_p->bodiesData.dataLocker.unlock();

    stack_trace_message_signal("dll_process_compressed_color_cloud_6");
    update_cloud_data_signal(&m_p->cloudData);
    update_bodies_data_signal(&m_p->bodiesData);
}

void ProcessData::process_compressed_color_mesh(){

    // uncompress depth data
    m_p->depthDecompressor.decode(
        reinterpret_cast<std::uint32_t*>(m_p->frame->compressedDepthData.data()), m_p->frame->compressedDepthSize,
        reinterpret_cast<std::uint32_t*>(m_p->decodedDepth.data()), k2_depth_count/2
    );

    // uncompress color
    tjDecompress2(m_p->jpegDecompressor, m_p->frame->compressedImage.data(), m_p->frame->jpegColorSize,
                  m_p->bufferDecompressColor.data(), k2_depth_width, 0/*pitch*/, k2_depth_height, TJPF_RGB, TJFLAG_FASTDCT);

    // retrieve intrinsics
    const auto focalLengthX       = m_p->frame->intrinsics[0];
    const auto focalLengthY       = m_p->frame->intrinsics[1];
    const auto principalPointX    = m_p->frame->intrinsics[2];
    const auto principalPointY    = m_p->frame->intrinsics[3];

    m_p->meshData.dataLocker.lock();

    // check validity for each depth
    std::uint32_t idT = 0;
    for(size_t ii = 0; ii < k2_depth_count; ++ii){
        m_p->validityDepth2Rgb[ii] = m_p->decodedDepth[ii] != 0;
        if(m_p->validityDepth2Rgb[ii]){
            m_p->validIdPerPointMesh[ii] = idT++;
        }else{
            m_p->validIdPerPointMesh[ii] = idT;
            m_p->decodedDepth[ii] = 1000;
        }
    }

    // process point/normal/color
    size_t currentId = 0;
    size_t currentNbPoints = 0;
    for(size_t ii = 0; ii < k2_depth_height; ++ii){
        for(size_t jj = 0; jj < k2_depth_width; ++jj){

            std::uint16_t currentDepth = m_p->decodedDepth[currentId];
            if(m_p->validityDepth2Rgb[currentId]){
                // compute x,y coordinates from depth and intrinsics
                // x = (u - principalPointX) / focalLengthX * d;
                // y = (v - principalPointY) / focalLengthY * d;
                // z = d;
                const float d = static_cast<float>(currentDepth)*0.001f;
                (*m_p->meshData.points)[currentNbPoints] = geo::Pt3f{
                        (jj - principalPointX) / focalLengthX *d,
                        -(ii - principalPointY) / focalLengthY *d,
                        d
                };

                // process color
                (*m_p->meshData.colors)[currentNbPoints] = {
                    m_p->bufferDecompressColor[currentId*3 + 0]/255.f,
                    m_p->bufferDecompressColor[currentId*3 + 1]/255.f,
                    m_p->bufferDecompressColor[currentId*3 + 2]/255.f
                };

                // process normal
                geo::Vec3f dir;
                if(ii != 0 && jj != 0 && ii != k2_depth_height -1 && jj != k2_depth_width-1){
                    const float dzdx = (m_p->decodedDepth[(jj+1)*k2_depth_width+(ii)] - m_p->decodedDepth[(jj-1)*k2_depth_width+(ii)]) / 2.f;
                    const float dzdy = (m_p->decodedDepth[(jj)*k2_depth_width+(ii+1)] - m_p->decodedDepth[(jj)*k2_depth_width+(ii-1)]) / 2.f;
                    dir = normalize(geo::Vec3f{-dzdx, -dzdy, 1.f});
                }
                (*m_p->meshData.normals)[currentNbPoints] = dir;

                // increment
                ++currentNbPoints;
            }
            ++currentId;
        }
    }
    m_p->meshData.sizePts = currentNbPoints;

    idT = 0;
    for(std::uint32_t ii = 0; ii < k2_depth_width-1; ++ii){
        for(std::uint32_t jj = 0; jj < k2_depth_height-1; ++jj){

            const std::uint32_t idTopLeft     = jj*k2_depth_width+ii;
            const std::uint32_t idTopRight    = (jj)*k2_depth_width+(ii+1);
            const std::uint32_t idBottomLeft  = (jj+1)*k2_depth_width+(ii);
            const std::uint32_t idBottomRight = (jj+1)*k2_depth_width+(ii+1);

            const bool idTopLeftValid       = (m_p->decodedDepth[idTopLeft] != 0)     && m_p->validityDepth2Rgb[idTopLeft];
            const bool idTopRightValid      = (m_p->decodedDepth[idTopRight] != 0)    && m_p->validityDepth2Rgb[idTopRight];
            const bool idBottomLeftValid    = (m_p->decodedDepth[idBottomLeft] != 0)  && m_p->validityDepth2Rgb[idBottomLeft];
            const bool idBottomRightValid   = (m_p->decodedDepth[idBottomRight] != 0) && m_p->validityDepth2Rgb[idBottomRight];

            int invalidCount = (idTopLeftValid? 1 : 0) + (idTopRightValid? 1 : 0) + (idBottomLeftValid? 1 : 0) + (idBottomRightValid? 1 : 0);
            if(invalidCount < 3){
                continue;
            }

            if(invalidCount == 4){
                if(std::abs(m_p->decodedDepth[idTopLeft]-m_p->decodedDepth[idBottomRight]) < std::abs(m_p->decodedDepth[idTopRight] - m_p->decodedDepth[idBottomLeft])){
                    (*m_p->meshData.triangles)[idT++] = geo::Pt3<std::uint32_t>{
                        m_p->validIdPerPointMesh[idTopLeft],
                        m_p->validIdPerPointMesh[idBottomRight],
                        m_p->validIdPerPointMesh[idBottomLeft]
                    };
                    (*m_p->meshData.triangles)[idT++] = geo::Pt3<std::uint32_t>{
                        m_p->validIdPerPointMesh[idTopLeft],
                        m_p->validIdPerPointMesh[idTopRight],
                        m_p->validIdPerPointMesh[idBottomRight]
                    };
                }else{
                    (*m_p->meshData.triangles)[idT++] = geo::Pt3<std::uint32_t>{
                        m_p->validIdPerPointMesh[idTopLeft],
                        m_p->validIdPerPointMesh[idTopRight],
                        m_p->validIdPerPointMesh[idBottomLeft]
                    };
                    (*m_p->meshData.triangles)[idT++] = geo::Pt3<std::uint32_t>{
                        m_p->validIdPerPointMesh[idTopRight],
                        m_p->validIdPerPointMesh[idBottomRight],
                        m_p->validIdPerPointMesh[idBottomLeft]
                    };
                }
                continue;
            }

            if(!idTopLeftValid){
                (*m_p->meshData.triangles)[idT++] = geo::Pt3<std::uint32_t>{
                    m_p->validIdPerPointMesh[idTopRight],
                    m_p->validIdPerPointMesh[idBottomRight],
                    m_p->validIdPerPointMesh[idBottomLeft]
                };
            }
            if(!idTopRightValid){
                (*m_p->meshData.triangles)[idT++] = geo::Pt3<std::uint32_t>{
                    m_p->validIdPerPointMesh[idTopLeft],
                    m_p->validIdPerPointMesh[idBottomRight],
                    m_p->validIdPerPointMesh[idBottomLeft]
                };
            }
            if(!idBottomLeftValid){
                (*m_p->meshData.triangles)[idT++] = geo::Pt3<std::uint32_t>{
                    m_p->validIdPerPointMesh[idTopLeft],
                    m_p->validIdPerPointMesh[idTopRight],
                    m_p->validIdPerPointMesh[idBottomRight]
                };
            }
            if(!idBottomRightValid){
                (*m_p->meshData.triangles)[idT++] = geo::Pt3<std::uint32_t>{
                    m_p->validIdPerPointMesh[idTopLeft],
                    m_p->validIdPerPointMesh[idTopRight],
                    m_p->validIdPerPointMesh[idBottomLeft]
                };
            }

        }
    }
    m_p->meshData.sizeTris = idT;
    m_p->meshData.dataLocker.unlock();

    // copy joints
    m_p->bodiesData.dataLocker.lock();
    m_p->bodiesData.bodies = m_p->frame->bodiesData;
    m_p->bodiesData.dataLocker.unlock();

    update_mesh_data_signal(&m_p->meshData);
    update_bodies_data_signal(&m_p->bodiesData);
}
