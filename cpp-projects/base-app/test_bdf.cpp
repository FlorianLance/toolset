
#include "tests.hpp"

// std
#include <iostream>
#include <iomanip>

// local
#include "data/bdf_header.hpp"

using namespace tool::data;



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

