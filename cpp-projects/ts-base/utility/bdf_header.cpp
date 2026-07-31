
/*******************************************************************************
** Toolset-ts-base                                                            **
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

#include "bdf_header.hpp"

// std
#include <algorithm>

// local
#include "io_fstream.hpp"
#include "int_24.hpp"
#include "string.hpp"

using namespace tool;


auto BDFHeader::read_header(std::ifstream &file) -> void{

    // Identification code (8 bytes)
    read_array<std::byte>(file, 8);

    // Local subject identification (80 bytes)
    read_text(localSubjectId, file, 80);
    str::right_trim(localSubjectId, ' ');

    // Local recording identification (80 bytes)
    read_text(localRecordingId, file, 80);
    str::right_trim(localRecordingId, ' ');

    // Startdate of recording (8 bytes)
    read_text(startDate, file, 8);
    str::right_trim(startDate, ' ');

    // Starttime of recording (8 bytes)
    read_text(startTime, file, 8);
    str::right_trim(startTime, ' ');

    // Number of bytes in header record (8 bytes)
    std::string nbOfBytesInHeader;
    read_text(nbOfBytesInHeader, file, 8);
    str::right_trim(nbOfBytesInHeader, ' ');
    size_t headerSize = std::stoi(nbOfBytesInHeader);
    static_cast<void>(headerSize);

    // Version of data format (44 bytes)
    read_text(vDataFormat, file, 44);
    str::right_trim(vDataFormat, ' ');

    // Number of data records "-1" if unknown (8 bytes)
    std::string nbOfDataRecordStr;
    read_text(nbOfDataRecordStr, file, 8);
    str::right_trim(nbOfDataRecordStr, ' ');
    nbOfDataRecords = std::stoi(nbOfDataRecordStr);

    // Duration of a data record, in seconds (8 bytes)
    read_text(durationDataRecord, file, 8);
    str::right_trim(durationDataRecord, ' ');

    // Number of channels (N) in data record (4 bytes)
    std::string nbOfChannelsStr;
    read_text(nbOfChannelsStr, file, 4);
    str::right_trim(nbOfChannelsStr, ' ');
    size_t nbOfChannels = std::stoi(nbOfChannelsStr);

    channels.resize(nbOfChannels);

    // Labels of the channels (N x 16 bytes)
    for(auto &channel : channels){
        read_text(channel.label, file, 16);
        str::right_trim(channel.label, ' ');
    }

     // Transducer type (N x 80 bytes)
    for(auto &channel : channels){
        read_text(channel.transducerType, file, 80);
        str::right_trim(channel.transducerType, ' ');
    }

    // Physical dimension of channels (N x 8 bytes)
    for(auto &channel : channels){
        read_text(channel.physicalDimension, file, 8);
        str::right_trim(channel.physicalDimension, ' ');
    }

    // Physical minimum in units of physical dimension  (N x 8 bytes)
    for(auto &channel : channels){
        std::string physicalMinimumStr;
        read_text(physicalMinimumStr, file, 8);
        str::right_trim(physicalMinimumStr, ' ');
        channel.physicalMinimum = std::stoi(physicalMinimumStr);
    }

    // Physical maximum in units of physical dimension (N x 8 bytes)
    for(auto &channel : channels){
        std::string physicalMaximumStr;
        read_text(physicalMaximumStr, file, 8);
        str::right_trim(physicalMaximumStr, ' ');
        channel.physicalMaximum = std::stoi(physicalMaximumStr);
    }

    // Digital minimum (N x 8 bytes)
    for(auto &channel : channels){
        std::string digitalMinimumStr;
        read_text(digitalMinimumStr, file, 8);
        str::right_trim(digitalMinimumStr, ' ');
        channel.digitalMinimum = std::stoi(digitalMinimumStr);
    }

    // Digital maximum (N x 8 bytes)
    for(auto &channel : channels){
        std::string digitalMaximumStr;
        read_text(digitalMaximumStr, file, 8);
        str::right_trim(digitalMaximumStr, ' ');
        channel.digitalMaximum = std::stoi(digitalMaximumStr);
    }

    // Prefiltering (N x 80 bytes)
    for(auto &channel : channels){
        read_text(channel.prefiltering, file, 80);
        str::right_trim(channel.prefiltering, ' ');
    }

    // Number of samples in each data record (Sample-rate if Duration of data record = "1") (N x 8 bytes)
    for(auto &channel : channels){
        std::string recordNumberOfSamplesStr;
        read_text(recordNumberOfSamplesStr, file, 8);
        str::right_trim(recordNumberOfSamplesStr, ' ');
        channel.recordNumberOfSamples = std::stoi(recordNumberOfSamplesStr);
    }

    // Reserved (N x 32 bytes)
    std::string reserved(32, ' ');
    for(auto &channel : channels){
        static_cast<void>(channel);
        tool::read_array<char>(reserved.data(), file, reserved.size());
    }
}

auto BDFHeader::read_data(std::ifstream &file, Buffer3D<double> &data) -> void{

    if(nbOfDataRecords <= 0){
        return;
    }

    data.resize(nbOfDataRecords);

    for(size_t idR = 0; idR < static_cast<size_t>(nbOfDataRecords); ++idR){

        data[idR].resize(channels.size());

        for(size_t idC = 0; idC < channels.size(); ++idC){;

            data[idR][idC].resize(channels[idC].recordNumberOfSamples);

            for(size_t idS = 0; idS < data[idR][idC].size(); ++idS){
                signed_24_bit i24;
                tool::read(i24.use_by_the_24_bit_int[0], file);
                tool::read(i24.use_by_the_24_bit_int[1], file);
                tool::read(i24.use_by_the_24_bit_int[2], file);
                data[idR][idC][idS] = channels[idC].compute_physical_from_digital(signed_24_bit::decode(i24));
                // todo depending physicalDimension scale and transducer
                // ...
            }
        }
    }
}

auto BDFHeader::write_header(std::ofstream &file) -> void{

    size_t nbOfChannels = channels.size();
    size_t headerSize = (nbOfChannels+1) * 256;
    // 8 + 80 + 80 + 8 + 8 + 8 + 44 + 8 + 8 + 4 +  // 256
    // nbOfChannels * (16+80+8+8+8+8+8+80+8+32); // N * 256

    // Identification code (8 bytes)
    tool::write(&firstB, file);
    tool::write_array(idCode.data(), file, idCode.size());

    // Local subject identification (80 bytes)
    padding(localSubjectId, 80);
    tool::write_array(localSubjectId.data(), file, localSubjectId.size());

    // Local recording identification (80 bytes)
    padding(localRecordingId, 80);
    tool::write_array(localRecordingId.data(), file, localRecordingId.size());

    // Startdate of recording (8 bytes)
    padding(startDate, 8);
    tool::write_array(startDate.data(), file, startDate.size());

    // Starttime of recording (8 bytes)
    padding(startTime, 8);
    tool::write_array(startTime.data(), file, startTime.size());

    // Number of bytes in header record (8 bytes)
    auto nbOfBytesInHeader = std::to_string(headerSize);
    padding(nbOfBytesInHeader, 8);
    tool::write_array(nbOfBytesInHeader.data(), file, nbOfBytesInHeader.size());

    // Version of data format (44 bytes)
    padding(vDataFormat, 44);
    tool::write_array(vDataFormat.data(), file, vDataFormat.size());

    // Number of data records "-1" if unknown (8 bytes)
    auto nbOfDataRecordStr = std::to_string(nbOfDataRecords);
    padding(nbOfDataRecordStr, 8);
    tool::write_array(nbOfDataRecordStr.data(), file, nbOfDataRecordStr.size());

    // Duration of a data record, in seconds (8 bytes)
    padding(durationDataRecord, 8);
    tool::write_array(durationDataRecord.data(), file, durationDataRecord.size());

    // Number of channels (N) in data record (4 bytes)
    auto nbOfChannelsStr = std::to_string(nbOfChannels);
    padding(nbOfChannelsStr, 4);
    tool::write_array(nbOfChannelsStr.data(), file, nbOfChannelsStr.size());

    // Labels of the channels (N x 16 bytes)
    for(auto &c : channels){
        padding(c.label, 16);
        tool::write_array(c.label.data(), file, c.label.size());
    }

    // Transducer type (N x 80 bytes)
    for(auto &c : channels){
        padding(c.transducerType, 80);
        tool::write_array(c.transducerType.data(), file, c.transducerType.size());
    }

    // Physical dimension of channels (N x 8 bytes)
    for(auto &c : channels){
        padding(c.physicalDimension, 8);
        tool::write_array(c.physicalDimension.data(), file, c.physicalDimension.size());
    }

    // Physical minimum in units of physical dimension  (N x 8 bytes)
    for(auto &c : channels){
        std::string physicalMinimumStr = std::to_string(c.physicalMinimum);
        padding(physicalMinimumStr, 8);
        tool::write_array(physicalMinimumStr.data(), file, physicalMinimumStr.size());
    }

    // Physical maximum in units of physical dimension (N x 8 bytes)
    for(auto &c : channels){
        std::string physicalMaximumStr = std::to_string(c.physicalMaximum);
        padding(physicalMaximumStr, 8);
        tool::write_array(physicalMaximumStr.data(), file, physicalMaximumStr.size());
    }

    // Digital minimum (N x 8 bytes)
    for(auto &c : channels){
        std::string digitalMinimumStr = std::to_string(c.digitalMinimum);
        padding(digitalMinimumStr, 8);
        tool::write_array(digitalMinimumStr.data(), file, digitalMinimumStr.size());
    }

    // Digital maximum (N x 8 bytes)
    for(auto &c : channels){
        std::string digitalMaximumStr = std::to_string(c.digitalMaximum);
        padding(digitalMaximumStr, 8);
        tool::write_array(digitalMaximumStr.data(), file, digitalMaximumStr.size());
    }

    // Prefiltering (N x 80 bytes)
    for(auto &c : channels){
        padding(c.prefiltering, 80);
        tool::write_array(c.prefiltering.data(), file, c.prefiltering.size());
    }

    // Number of samples in each data record (Sample-rate if Duration of data record = "1") (N x 8 bytes)
    for(auto &c : channels){
        std::string recordNumberOfSamplesStr = std::to_string(c.recordNumberOfSamples);
        padding(recordNumberOfSamplesStr, 8);
        tool::write_array(recordNumberOfSamplesStr.data(), file, recordNumberOfSamplesStr.size());
    }

    // Reserved (N x 32 bytes)
    std::string reserved = "";
    padding(reserved, 32);
    for(auto &c : channels){
        static_cast<void>(c);
        tool::write_array(reserved.data(), file, reserved.size());
    }
}

auto BDFHeader::write_value(std::ofstream &file, double v) -> void{
    v = std::clamp(v, -8388608.0, 8388607.0);
    auto eb = signed_24_bit::encode(static_cast<std::int32_t>(v));
    file.write(reinterpret_cast<const char*>(&eb.use_by_the_24_bit_int[0]), 1);
    file.write(reinterpret_cast<const char*>(&eb.use_by_the_24_bit_int[1]), 1);
    file.write(reinterpret_cast<const char*>(&eb.use_by_the_24_bit_int[2]), 1);
}


auto BDFHeader::padding(std::string &text, size_t size, char fillC) -> void{

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
