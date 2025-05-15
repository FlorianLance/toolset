


/*******************************************************************************
** Toolset-base                                                               **
** MIT License                                                                **
** Copyright (c) [2024] [Florian Lance]                                       **
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

#pragma once

// std
#include <string>
#include <algorithm>

// local
#include "utility/io_fstream.hpp"
#include "utility/int_24.hpp"

using namespace std::literals;

struct BDFChannelInfo{
    std::string label;
    std::string transducerType        = "active electrode";
    std::string physicalDimension     = "nV";
    std::string physicalMinimum       = "-262144"; // Physical minimum in units of physical dimension
    std::string physicalMaximum       = "262143";  // Physical maximum in units of physical dimension
    std::string digitalMinimum        = "-8388608";
    std::string digitalMaximum        = "8388607";
    std::string prefiltering;
    std::string recordNumberOfSamples = "128"; // Number of samples in each data record (Sample-rate if Duration of data record = “1”)
};

struct BDFHeaderInfo{

    static constexpr std::uint8_t firstB = 255;
    static constexpr std::string_view idCode = "BIOSEMI"sv;

    int nbOfDataRecords = 1;// -1;
    std::string durationDataRecord = "1";
    std::string localSubjectId;
    std::string localRecordingId;
    std::string startDate = "dd.mm.yy";
    std::string startTime = "hh.mm.ss";
    std::string vDataFormat = "24BIT";

    std::vector<BDFChannelInfo> channels;

    static auto padding(std::string &text, size_t size, char fillC = ' ') -> void{

        if(text.size() > size){
            text.resize(size);
        }else if(text.size() == size){
            return;
        }else{
            size_t cSize = text.size();
            text.resize(size);
            std::fill(text.begin() + cSize, text.end(), fillC);
        }
    }

    static auto write_value(std::ofstream &file, double v) -> void{
        v = std::clamp(v, -8388608.0, 8388607.0);
        auto eb = signed_24_bit::encode(static_cast<std::int32_t>(v));
        file.write(reinterpret_cast<const char*>(&eb.use_by_the_24_bit_int[0]), 1);
        file.write(reinterpret_cast<const char*>(&eb.use_by_the_24_bit_int[1]), 1);
        file.write(reinterpret_cast<const char*>(&eb.use_by_the_24_bit_int[2]), 1);
    }


    auto write(std::ofstream &file) -> void{

        size_t nbOfChannels = channels.size();
        size_t headerSize = (nbOfChannels+1) * 256;
        // 8 + 80 + 80 + 8 + 8 + 8 + 44 + 8 + 8 + 4 +  // 256
        // nbOfChannels * (16+80+8+8+8+8+8+80+8+32); // N * 256

        tool::write(&firstB, file);
        tool::write_array(idCode.data(), file, idCode.size());

        padding(localSubjectId, 80);
        tool::write_array(localSubjectId.data(), file, localSubjectId.size());

        padding(localRecordingId, 80);
        tool::write_array(localRecordingId.data(), file, localRecordingId.size());

        padding(startDate, 8);
        tool::write_array(startDate.data(), file, startDate.size());

        padding(startTime, 8);
        tool::write_array(startTime.data(), file, startTime.size());

        auto nbOfBytesInHeader = std::to_string(headerSize);
        padding(nbOfBytesInHeader, 8);
        tool::write_array(nbOfBytesInHeader.data(), file, nbOfBytesInHeader.size());

        padding(vDataFormat, 44);
        tool::write_array(vDataFormat.data(), file, vDataFormat.size());

        auto nbOfDataRecordStr = std::to_string(nbOfDataRecords);
        padding(nbOfDataRecordStr, 8);
        tool::write_array(nbOfDataRecordStr.data(), file, nbOfDataRecordStr.size());

        padding(durationDataRecord, 8);
        tool::write_array(durationDataRecord.data(), file, durationDataRecord.size());

        auto nbOfChannelsStr = std::to_string(nbOfChannels);
        padding(nbOfChannelsStr, 4);
        tool::write_array(nbOfChannelsStr.data(), file, nbOfChannelsStr.size());

        std::string reserved = "";
        padding(reserved, 32);

        for(auto &c : channels){
            padding(c.label, 16);
            tool::write_array(c.label.data(), file, c.label.size());
        }

        for(auto &c : channels){
            padding(c.transducerType, 80);
            tool::write_array(c.transducerType.data(), file, c.transducerType.size());
        }

        for(auto &c : channels){
            padding(c.physicalDimension, 8);
            tool::write_array(c.physicalDimension.data(), file, c.physicalDimension.size());
        }

        for(auto &c : channels){
            padding(c.physicalMinimum, 8);
            tool::write_array(c.physicalMinimum.data(), file, c.physicalMinimum.size());
        }

        for(auto &c : channels){
            padding(c.physicalMaximum, 8);
            tool::write_array(c.physicalMaximum.data(), file, c.physicalMaximum.size());
        }

        for(auto &c : channels){
            padding(c.digitalMinimum, 8);
            tool::write_array(c.digitalMinimum.data(), file, c.digitalMinimum.size());
        }

        for(auto &c : channels){
            padding(c.digitalMaximum, 8);
            tool::write_array(c.digitalMaximum.data(), file, c.digitalMaximum.size());
        }

        for(auto &c : channels){
            padding(c.prefiltering, 80);
            tool::write_array(c.prefiltering.data(), file, c.prefiltering.size());
        }

        for(auto &c : channels){
            padding(c.recordNumberOfSamples, 8);
            tool::write_array(c.recordNumberOfSamples.data(), file, c.recordNumberOfSamples.size());
        }

        for(auto &c : channels){
            static_cast<void>(c);
            tool::write_array(reserved.data(), file, reserved.size());
        }
    }
};
