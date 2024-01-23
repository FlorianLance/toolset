
/*******************************************************************************
** Toolset-scaner-manager                                                     **
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

#include "manager_process_data_worker.hpp"

// Qt
#include <QCoreApplication>

// turbo-jpeg
#include <turbojpeg.h>

// base
#include "data/integers_encoder.hpp"

using namespace std::chrono;
using namespace tool::ui;
using namespace tool::cam;

struct ProcessDataWorker::Impl{

    Impl(size_t id){

        this->id = id;
        jpegDecompressor = tjInitDecompress();

        colorMap.emplace_back(QPair(QPair(0,0.5),     QPair(QColor(64,0,128),    QColor(255,0,0))));    // purple red
        colorMap.emplace_back(QPair(QPair(0.5,0.7),   QPair(QColor(255,0,0),     QColor(255,128,0))));    // red orange
        colorMap.emplace_back(QPair(QPair(0.7,1),     QPair(QColor(255,128,0),   QColor(255,255,0))));    // orange yellow
        colorMap.emplace_back(QPair(QPair(1,1.5),     QPair(QColor(255,255,0),   QColor(0,255,64))));     // yellow lightgreen
        colorMap.emplace_back(QPair(QPair(1.5,2),     QPair(QColor(0,255,64),    QColor(0,128,0))));      // lightgreen green
        colorMap.emplace_back(QPair(QPair(2,3),       QPair(QColor(0,128,0),     QColor(0,0,255))));      // green blue
        colorMap.emplace_back(QPair(QPair(3,4),       QPair(QColor(0,0,255),     QColor(0,255,255))));    // blue lightblue
        colorMap.emplace_back(QPair(QPair(4,5),       QPair(QColor(0,255,255),   QColor(255,128,192))));  // lightblue pink

        infraredMap.emplace_back(QPair(QPair(0,1000),       QPair(QColor(64,0,128),    QColor(255,0,0))));      // purple red
        infraredMap.emplace_back(QPair(QPair(1000,3000),    QPair(QColor(255,0,0),     QColor(255,128,0))));    // red orange
        infraredMap.emplace_back(QPair(QPair(3000,6000),    QPair(QColor(255,128,0),   QColor(255,255,0))));    // orange yellow
        infraredMap.emplace_back(QPair(QPair(6000,10000),   QPair(QColor(255,255,0),   QColor(0,255,64))));     // yellow lightgreen
        infraredMap.emplace_back(QPair(QPair(10000,15000),  QPair(QColor(0,255,64),    QColor(0,128,0))));      // lightgreen green
        infraredMap.emplace_back(QPair(QPair(15000,22000),  QPair(QColor(0,128,0),     QColor(0,0,255))));      // green blue
        infraredMap.emplace_back(QPair(QPair(22000,30000),  QPair(QColor(0,0,255),     QColor(0,255,255))));    // blue lightblue
        infraredMap.emplace_back(QPair(QPair(30000,66000),  QPair(QColor(0,255,255),   QColor(255,128,192))));  // lightblue pink

        bufferDecompressColor.resize(k2_color_width*k2_color_height*3);
        decodedDepth.resize(k2_depth_count*2);
        validityDepth2Rgb.resize(k2_depth_count);
        validIdPerPointMesh.resize(k2_depth_count);
    }

    // id grabber
    size_t id;

    // colormaps
    std::vector<QPair<QPair<qreal,qreal>, QPair<QColor, QColor>>> colorMap;
    std::vector<QPair<QPair<qreal,qreal>, QPair<QColor, QColor>>> infraredMap;

    // processing structurs
    std::vector<unsigned char> bufferDecompressColor;
    std::vector<std::uint16_t> decodedDepth;
    std::vector<bool> validityDepth2Rgb;
    std::vector<std::uint32_t> validIdPerPointMesh;

    // processed data for display
    cam::K2CloudDisplayData cloudData;
    cam::K2MeshDisplayData meshData;
    std::array<cam::K2BodyInfos,6> bodiesData;

    // time measurment
    std::int64_t allProcessTime = 0;

    // decompressors
    tjhandle jpegDecompressor;
    data::IntegersEncoder depthDecompressor;

    // data received
    cam::K2UdpHeader header;
    std::shared_ptr<K2Frame> frame = nullptr;

    // save data
    std::vector<std::shared_ptr<K2Frame>> savedFrames;
};

ProcessDataWorker::ProcessDataWorker(size_t id) : m_p(std::make_unique<Impl>(id)){
}

ProcessDataWorker::~ProcessDataWorker(){
    tjDestroy(m_p->jpegDecompressor);
}

K2CloudDisplayData *ProcessDataWorker::cloud_data(){
    return &m_p->cloudData;
}

K2MeshDisplayData *ProcessDataWorker::mesh_data(){
    return &m_p->meshData;
}

void ProcessDataWorker::process_data_from_udp_frame(cam::K2UdpHeader header, std::shared_ptr<K2Frame> frame){

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
    emit end_update_image_signal(m_p->allProcessTime = duration_cast<microseconds>(high_resolution_clock::now() - timeStart).count());
//    std::cout << "t: " << m_p->allProcessTime*0.001 << "\n";
}


void ProcessDataWorker::process_compressed_color_1920x1080(){

    // uncompress color
    tjDecompress2(m_p->jpegDecompressor, m_p->frame->compressedImage.data(), m_p->frame->jpegColorSize,
                  m_p->bufferDecompressColor.data(), k2_color_width, 0/*pitch*/, k2_color_height, TJPF_RGB, TJFLAG_FASTDCT);

    emit update_color_signal(QImage(m_p->bufferDecompressColor.data(), k2_color_width, k2_color_height, QImage::Format_RGB888));
}

void ProcessDataWorker::process_compressed_color_512x424(){

    // uncompress color
    tjDecompress2(m_p->jpegDecompressor, m_p->frame->compressedImage.data(), m_p->frame->jpegColorSize,
                  m_p->bufferDecompressColor.data(), k2_depth_width, 0/*pitch*/, k2_depth_height, TJPF_RGB, TJFLAG_FASTDCT);

    emit update_color_signal(QImage(m_p->bufferDecompressColor.data(), k2_depth_width, k2_depth_height, QImage::Format_RGB888));
}

void ProcessDataWorker::process_compressed_color_cloud(){

    // uncompress depth data
    m_p->depthDecompressor.decode(
        reinterpret_cast<std::uint32_t*>(m_p->frame->compressedDepthData.data()), m_p->frame->compressedDepthSize,
        reinterpret_cast<std::uint32_t*>(m_p->decodedDepth.data()), k2_depth_count/2
    );

    // copy joints
    m_p->bodiesData = m_p->frame->bodiesData;

    // uncompress color
    tjDecompress2(m_p->jpegDecompressor, m_p->frame->compressedImage.data(), m_p->frame->jpegColorSize,
                  m_p->bufferDecompressColor.data(), k2_depth_width, 0/*pitch*/, k2_depth_height, TJPF_RGB, TJFLAG_FASTDCT);

    // retrieve intrinsics
    const auto focalLengthX       = m_p->frame->intrinsics[0];
    const auto focalLengthY       = m_p->frame->intrinsics[1];
    const auto principalPointX    = m_p->frame->intrinsics[2];
    const auto principalPointY    = m_p->frame->intrinsics[3];

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

    emit update_cloud_data_signal(&m_p->cloudData);
    emit update_bodies_data_signal(&m_p->bodiesData);
    emit update_color_signal(QImage(m_p->bufferDecompressColor.data(), k2_depth_width, k2_depth_height, QImage::Format_RGB888));

    if(videoSavingState){
        emit new_frame_to_save_signal(m_p->frame);
    }
}

void ProcessDataWorker::process_compressed_color_mesh(){

    // uncompress depth data
    m_p->depthDecompressor.decode(
        reinterpret_cast<std::uint32_t*>(m_p->frame->compressedDepthData.data()), m_p->frame->compressedDepthSize,
        reinterpret_cast<std::uint32_t*>(m_p->decodedDepth.data()), k2_depth_count/2
    );

    // copy joints
    m_p->bodiesData = m_p->frame->bodiesData;

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

    emit update_mesh_data_signal(&m_p->meshData);
    emit update_bodies_data_signal(&m_p->bodiesData);
    emit update_color_signal(QImage(m_p->bufferDecompressColor.data(), k2_depth_width, k2_depth_height, QImage::Format_RGB888));
}

void ProcessDataWorker::process_depth(){


    QImage depthImage(k2_depth_width, k2_depth_height,QImage::Format_RGB888);
    depthImage.fill(Qt::black);
    size_t id = 0;

    auto depth = m_p->frame->depthData.get();

    for(size_t ii = 0; ii < k2_depth_height; ++ii){
        for(size_t jj = 0; jj < k2_depth_width; ++jj){

            auto d = (*depth)[id++];
            if(d == 0){
                continue;
            }

            qreal dValue = static_cast<qreal>(d*0.001);
            QColor col;
            for(auto cm : m_p->colorMap){
                if(!(dValue < cm.first.second)){
                    continue;
                }

                dValue = (dValue - cm.first.first) / (cm.first.second-cm.first.first);
                const QColor &c1 = cm.second.first;
                const QColor &c2 = cm.second.second;
                col.setRedF(dValue* c2.redF() + (1. - dValue) * c1.redF());
                col.setGreenF(dValue* c2.greenF() + (1. - dValue) * c1.greenF());
                col.setBlueF(dValue* c2.blueF() + (1. - dValue) * c1.blueF());
                depthImage.setPixelColor(static_cast<int>(jj), static_cast<int>(ii), col);
                break;
            }
        }
    }
    emit update_depth_signal(std::move(depthImage));
}

void ProcessDataWorker::process_infra(){

    QImage infraImage(k2_infrared_width, k2_infrared_height,QImage::Format_RGB888);
    infraImage.fill(Qt::black);
    size_t id = 0;

    auto infra = m_p->frame->infraData.get();

    auto *b = infraImage.bits();
    for(size_t ii = 0; ii < k2_infrared_count; ++ii){

        auto i = (*infra)[id++];
        if(i == 0){
            continue;
        }

        qreal iValue = static_cast<qreal>(i);
        for(auto cm : m_p->infraredMap){
            if(!(iValue < cm.first.second)){
                continue;
            }

            iValue = (iValue - cm.first.first) / (cm.first.second-cm.first.first);
            const QColor &c1 = cm.second.first;
            const QColor &c2 = cm.second.second;

            auto inv = (1. - iValue);
            b[ii*3+0]   = static_cast<std::uint8_t>(255 * (iValue * c2.redF()   + inv * c1.redF()));
            b[ii*3+1] = static_cast<std::uint8_t>(255   * (iValue * c2.greenF() + inv * c1.greenF()));
            b[ii*3+2] = static_cast<std::uint8_t>(255   * (iValue * c2.blueF()  + inv * c1.blueF()));
            break;
        }
    }


    emit update_infra_signal(std::move(infraImage));
}


#include "moc_manager_process_data_worker.cpp"



//    dzdx=(z(x+1,y)-z(x-1,y))/2.0;
//    dzdy=(z(x,y+1)-z(x,y-1))/2.0;
//    direction=(-dzdx,-dzdy,1.0)
//    magnitude=sqrt(direction.x**2 + direction.y**2 + direction.z**2)
//    normal=direction/magnitude
