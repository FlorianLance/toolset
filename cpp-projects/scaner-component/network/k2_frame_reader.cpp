
/*******************************************************************************
** exvr-scaner-component                                                      **
** No license (to be defined)                                                 **
** Copyright (c) [2018] [Florian Lance][EPFL-LNCO]                            **
********************************************************************************/

#include "k2_frame_reader.hpp"

// std
#include <iostream>
#include <execution>
#include <chrono>

using namespace std::chrono_literals;
using namespace std::chrono;

using namespace tool;
using namespace tool::network;
using namespace tool::camera;

struct K2FrameReader::Impl{

    K2UdpHeader firstHeader;
    K2UdpHeader lastHeader;
    std::vector<char> bufferData;

};

K2FrameReader::K2FrameReader() : m_p(std::make_unique<Impl>()){
}

K2FrameReader::~K2FrameReader(){}

auto K2FrameReader::process_packets(std::vector<std::vector<char> *> *data) -> void{

    if(data->size() < 2){
        // error...
        return;
    }

    stack_trace_message_signal("dll_process_packets_1");

    using KFR = K2FrameRequest;
    auto timeStart = high_resolution_clock::now();

    // get first header
    char *currentP = (*data)[0]->data();
    std::copy(currentP, currentP + sizeof (K2UdpHeader), reinterpret_cast<char*>(&m_p->firstHeader));

    // get last header
    currentP = (*data)[data->size()-1]->data();
    std::copy(currentP, currentP + sizeof (K2UdpHeader), reinterpret_cast<char*>(&m_p->lastHeader));

    // retrieve times from headers
    const auto tFirst   = nanoseconds(m_p->firstHeader.timeStamp);
    const auto tLast    = nanoseconds(m_p->lastHeader.timeStamp);
    const auto timeStampGet = nanoseconds(high_resolution_clock::now().time_since_epoch()) - tLast;

    // resize if necessary
    if(m_p->bufferData.size() < m_p->firstHeader.sizeFullData){
        m_p->bufferData.resize(m_p->firstHeader.sizeFullData);
    }

    stack_trace_message_signal("dll_process_packets_2");

    // read data
    std::for_each(std::execution::par_unseq, std::begin(*data), std::end(*data), [&](std::vector<char> *buffer){

        char *currentP = buffer->data();

        // read header
        K2UdpHeader header;
        std::move(currentP, currentP + sizeof (K2UdpHeader), reinterpret_cast<char*>(&header));
        currentP +=  sizeof (K2UdpHeader);

        // read raw data
        std::move(currentP, currentP + header.sizePacketData, m_p->bufferData.data() + header.offset);
    });


    stack_trace_message_signal("dll_process_packets_3");

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

    stack_trace_message_signal("dll_process_packets_4");

    // update frame from header
    const auto tSend = duration_cast<milliseconds>(tLast-tFirst).count();
    const auto tProcessingFrame = duration_cast<milliseconds>(high_resolution_clock::now() - timeStart);
    (void)tSend;
    (void)tProcessingFrame;
    frame->timeStampGetFrame = timeStampGet.count();
    frame->frameId = m_p->firstHeader.frameId;
    frame->intrinsics = {m_p->firstHeader.intrinsics1,m_p->firstHeader.intrinsics2,m_p->firstHeader.intrinsics3,m_p->firstHeader.intrinsics4};

    // check if frame not too old    
    stack_trace_message_signal("dll_process_packets_5");
    const auto timeFrame = duration_cast<milliseconds>(nanoseconds(high_resolution_clock::now().time_since_epoch())- nanoseconds(frame->timeStampGetFrame));
    if(timeFrame.count() > 200){
        std::cerr << "Drop frame too old\n";
        return;
    }

    frame_received_signal(m_p->firstHeader, frame);
}


