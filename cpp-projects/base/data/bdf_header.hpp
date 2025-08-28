
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

// local
#include "utility/buffer.hpp"

namespace tool::data{

using namespace std::literals;

// Length in bytes 	BDF Header:                     EDF Header:             Description
// 8 bytes          Byte 1: "255" (non ascii)       Byte 1: "0" (ASCII) 	Identification code
//                  Bytes 2-8 : "BIOSEMI" (ASCII) 	Bytes 2-8 : " "(ASCII)
// 80 bytes         User text input (ASCII)                                 Local subject identification
// 80 bytes         User text input (ASCII)                                 Local recording identification
// 8 bytes          dd.mm.yy (ASCII)                                        Startdate of recording
// 8 bytes          hh.mm.ss (ASCII)                                        Starttime of recording
// 8 bytes          (ASCII)                                                 Number of bytes in header record
// 44 bytes         "24BIT" (ASCII)                 "BIOSEMI" (ASCII)       Version of data format.
// 8 bytes          (ASCII)                                                 Number of data records "-1" if unknown
// 8 bytes          e.g.: "1" (ASCII)                                       Duration of a data record, in seconds
// 4 bytes          e.g.: "257" or "128" (ASCII)                            Number of channels (N) in data record
// N x 16 bytes     e.g.: "Fp1", "Fpz", "Fp2", etc (ASCII)                  Labels of the channels
// N x 80 bytes     e.g.: "active electrode", "respiration belt" (ASCII)    Transducer type
// N x 8 bytes      e.g.: "uV", "Ohm" (ASCII)                               Physical dimension of channels
// N x 8 bytes      e.g.: "-262144" (ASCII)         e.g.: "-32768" (ASCII) 	Physical minimum in units of physical dimension
// N x 8 bytes      e.g.: "262143" (ASCII)          e.g.: "32767" (ASCII) 	Physical maximum in units of physical dimension
// N x 8 bytes      e.g.: "-8388608" (ASCII)        e.g.: "-32768" (ASCII) 	Digital minimum
// N x 8 bytes      e.g.: "8388607" (ASCII)         e.g.: "32767" (ASCII) 	Digital maximum
// N x 80 bytes 	e.g.: "HP:DC; LP:410"           e.g.: "HP:0,16; LP:500" Prefiltering
// N x 8 bytes      For example: "2048" (ASCII)                             Number of samples in each data record (Sample-rate if Duration of data record = "1")
// N x 32 bytes 	(ASCII)                                                 Reserved

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

struct BDFHeader{

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

    auto write_header(std::ofstream &file) -> void;
    static auto write_value(std::ofstream &file, double v) -> void;
    static auto write_values(std::ofstream &file, std::span<double> values) -> void;

    auto read_header(std::ifstream &file) -> void;
    auto read_data(std::ifstream &file, Buffer3D<double> &data) -> void;

private:

    static auto padding(std::string &text, size_t size, char fillC = ' ') -> void;
};
}
