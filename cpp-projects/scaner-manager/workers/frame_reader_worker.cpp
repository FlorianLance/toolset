
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

#include "frame_reader_worker.hpp"

// std
#include <iostream>
#include <execution>
#include <chrono>

// Qt
#include <QDateTime>
#include <QThread>
#include <QDebug>


// qt-utility
#include "qt_logger.hpp"


using namespace std::chrono;
using namespace tool;
using namespace tool::network;
using namespace tool::camera;

struct FrameReader::Impl{

    K2UdpHeader firstHeader;
    K2UdpHeader lastHeader;
    std::vector<char> bufferData;
    size_t nbFramesReceived = 0;
};

FrameReader::FrameReader() : m_p(std::make_unique<Impl>()){
}


void FrameReader::process_packets(std::vector<QByteArray *> data){

    if(data.size() < 2){
        // error...
        return;
    }

    using KFR = K2FrameRequest;
    auto timeStart = high_resolution_clock::now();

    // get first header
    char *currentP = data[0]->data();
    std::copy(currentP, currentP + sizeof (K2UdpHeader), reinterpret_cast<char*>(&m_p->firstHeader));

    // get last header
    currentP = data[data.size()-1]->data();
    std::copy(currentP, currentP + sizeof (K2UdpHeader), reinterpret_cast<char*>(&m_p->lastHeader));

    // retrieve times from headers
    const auto tFirst   = nanoseconds(m_p->firstHeader.timeStamp);
    const auto tLast    = nanoseconds(m_p->lastHeader.timeStamp);
    const auto timeStampGet = nanoseconds(high_resolution_clock::now().time_since_epoch()) - tLast;

    // resize if necessary
    if(m_p->bufferData.size() < m_p->firstHeader.sizeFullData){
        m_p->bufferData.resize(m_p->firstHeader.sizeFullData);
    }

    // read data
    std::for_each(std::execution::par_unseq, std::begin(data), std::end(data), [&](QByteArray *buffer){

        char *currentP = buffer->data();

        // read header
        K2UdpHeader header;
        std::move(currentP, currentP + sizeof (K2UdpHeader), reinterpret_cast<char*>(&header));
        currentP +=  sizeof (K2UdpHeader);

        // read raw data
        std::move(currentP, currentP + header.sizePacketData, m_p->bufferData.data() + header.offset);
    });

    // process data
    std::shared_ptr<K2Frame> frame = std::make_shared<K2Frame>(false);
    frame->mode = m_p->firstHeader.frameMode;

    if(frame->mode == KFR::compressed_color_1920x1080 || frame->mode == KFR::compressed_color_512x424){

        frame->jpegColorSize = m_p->firstHeader.sizeFullData;
        frame->compressedImage.resize(frame->jpegColorSize);

        auto colorP = m_p->bufferData.data();
        std::copy(colorP, colorP +  frame->jpegColorSize, frame->compressedImage.data());

    }else if(frame->mode == KFR::compressed_color_cloud || frame->mode == KFR::compressed_color_mesh){

        frame->compressedDepthSize = m_p->firstHeader.size1;
        frame->jpegColorSize = m_p->firstHeader.size2;

        const size_t sizeDepth16    = frame->compressedDepthSize*2;
        const size_t sizeDepth8     = sizeDepth16*2;
        const size_t sizeColor8     = frame->jpegColorSize;

        frame->compressedDepthData.resize(sizeDepth16);
        frame->compressedImage.resize(frame->jpegColorSize);

        // copy depth values
        auto depthP = reinterpret_cast<std::uint16_t*>(m_p->bufferData.data());
        std::copy(depthP, depthP + sizeDepth16, frame->compressedDepthData.data());

        // copy color values
        auto colorP = m_p->bufferData.data() + sizeDepth8;
        std::copy(colorP, colorP +  frame->jpegColorSize, frame->compressedImage.data());

        // copy bodies
        size_t id = 0;
        K2Joint4x64 *bodyP = reinterpret_cast<K2Joint4x64*>(&m_p->bufferData[sizeDepth8 + sizeColor8]);
        for(auto &bodyInfo : frame->bodiesData){
            for(size_t jj = 0; jj < 25; ++jj){
                auto [bodyId,bodyTracked,bodyRestricted,jointType,joint] = int_to_joint(bodyP[id++]);
                bodyInfo.id = bodyId;
                bodyInfo.tracked = bodyTracked;
                bodyInfo.restricted = bodyRestricted;
                bodyInfo.joints[jointType] = joint;
            }

        }

    }else if(frame->mode == KFR::depth_512x424){

        if(frame->depthData == nullptr){
            frame->depthData = std::make_unique<K2DepthData>();
        }

        auto depthP = reinterpret_cast<std::uint16_t*>(m_p->bufferData.data());
        std::copy(depthP, depthP + k2_depth_count, frame->depthData->data());

    }else if(frame->mode == KFR::infra_512x424 || frame->mode == KFR::long_exposure_infra_512x424){

        if(frame->infraData == nullptr){
            frame->infraData = std::make_unique<K2InfraData>();
        }

        auto infraP = reinterpret_cast<std::uint16_t*>(m_p->bufferData.data());
        std::copy(infraP, infraP + k2_infrared_count, frame->infraData->data());
    }

    // udpate frame from header
    const auto tSend = duration_cast<milliseconds>(tLast-tFirst).count();
    const auto tProcessingFrame = duration_cast<milliseconds>(high_resolution_clock::now() - timeStart);
    (void)tSend;
    (void)tProcessingFrame;
    frame->timeStampGetFrame = timeStampGet.count();
    frame->frameId = m_p->firstHeader.frameId;
    frame->intrinsics = {m_p->firstHeader.intrinsics1,m_p->firstHeader.intrinsics2,m_p->firstHeader.intrinsics3,m_p->firstHeader.intrinsics4};

    // check if frame not too old
    const auto timeFrame = duration_cast<milliseconds>(nanoseconds(high_resolution_clock::now().time_since_epoch())- nanoseconds(frame->timeStampGetFrame));

    if(timeFrame.count() > 100){
        // frame too old
        QtLogger::warning(QSL("Frame too old with id ") % QString::number(frame->frameId) % QSL(" too old."));
        return;
    }

    m_p->nbFramesReceived++;
    emit frame_received_signal(m_p->firstHeader, frame);
}


#include "moc_frame_reader_worker.cpp"
