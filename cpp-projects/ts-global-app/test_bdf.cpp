
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
#include <iomanip>

// local
#include "utility/bdf_header.hpp"

using namespace tool;


auto test_bdf_reader(const std::string &path) -> void{

    std::cout << "test_bdf_reader\n";

    std::ifstream file(path.data(), std::ios_base::binary);

    BDFHeader header;
    header.read_header(file);

    // int nbOfDataRecords = 1;// -1;
    // std::string durationDataRecord = "1";
    // std::string localSubjectId;
    // std::string localRecordingId;
    // std::string startDate = "dd.mm.yy";
    // std::string startTime = "hh.mm.ss";
    // std::string vDataFormat = "24BIT";
    // std::cout << std::format("{:>{}.2f}", values[i], ColLength);

    std::cout << "nbOfDataRecords [" << header.nbOfDataRecords << "]\n";
    std::cout << "durationDataRecord [" << header.durationDataRecord << "]\n";
    std::cout << "localSubjectId [" << header.localSubjectId << "]\n";
    std::cout << "localRecordingId [" << header.localRecordingId << "]\n";
    std::cout << "startDate [" << header.startDate << "]\n";
    std::cout << "startTime [" << header.startTime << "]\n";
    std::cout << "vDataFormat [" << header.vDataFormat << "]\n";
    std::cout << "channels size [" << header.channels.size() << "]\n";

    size_t idC = 0;
    for(const auto &channel : header.channels){
        std::cout << "### channel " << idC << "\n";
        std::cout << "label [" << channel.label << "]\n";
        std::cout << "transducerType [" << channel.transducerType << "]\n";
        std::cout << "physicalDimension [" << channel.physicalDimension << "]\n";
        std::cout << "physicalMinimum [" << channel.physicalMinimum << "]\n";
        std::cout << "physicalMaximum [" << channel.physicalMaximum << "]\n";
        std::cout << "digitalMinimum [" << channel.digitalMinimum << "]\n";
        std::cout << "digitalMaximum [" << channel.digitalMaximum << "]\n";
        std::cout << "prefiltering [" << channel.prefiltering << "]\n";
        std::cout << "recordNumberOfSamples [" << channel.recordNumberOfSamples << "]\n\n";
        ++idC;
    }

    tool::Buffer3D<double> data;
    header.read_data(file, data);


}

