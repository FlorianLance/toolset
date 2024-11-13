
#include "tests.hpp"

// base
#include "depth-camera/settings/dc_server_settings.hpp"
#include "depth-camera/settings/dc_client_settings.hpp"

using namespace tool;
using namespace tool::geo;
using namespace tool::cam;
using namespace std::chrono;

auto dc_settings() -> void{
    std::string baseGrabberPath = "C:/Users/SDKim/Desktop/conversions/copy/dc-grabber/config/";
    std::string baseManagerPath = "C:/Users/SDKim/Desktop/conversions/copy/dc-manager/config/";
    //
    // settings/device_ALL_HERE_NUC2_G0.json
    // settings/device_ALL_HERE_NUC3_G0.json
    // settings/device_ALL_HERE_NUC4_G0.json

    // network/network_ALL_HERE_NUC1_G0.config
    // id_interface | reading_port | ipv4/ipv6
    // 0 8888 ipv4
    // 0 8890 ipv4
    // 0 8892 ipv4
    // 0 8894 ipv4

    DCServerSettings serverS;
    serverS.udpServerS.protocol = net::Protocol::ipv4;
    serverS.udpServerS.readingInterfaceId = 0;
    serverS.udpServerS.readingPort = 8888;
    serverS.udpServerS.anyReadingInterface = false;
    // serverS.deviceS.load_from_file(baseGrabberPath + "settings/device_ALL_HERE_NUC1_G0.json");
    auto &c = serverS.deviceS.configS;
    c.openDevice    = true;
    c.startReading  = true;
    c.typeDevice    = DCType::FemtoBolt;
    c.idDevice      = 0;
    c.mode          = DCMode::FB_C1280x720_DI640x576_NV12_F30;

    auto &d = serverS.deviceS.dataS;
    auto &cap = d.server.capture;
    cap.depth = true;
    cap.color = true;

    auto &send = d.server.sending;
    send.addDepth               = true;
    send.addDepthSizedColor     = true;
    send.addOriginalSizeColor   = false;
    send.addInfra               = false;
    send.addBodiesId            = false;
    send.addBodiesSkeleton      = false;
    send.depthCM                = DCCompressionMode::FastPFor;
    send.depthSizedColorCM      = DCCompressionMode::JPEG;
    d.server.generation.depthSizedColorImage = true;
    d.server.generation.cloud                = true;

    serverS.save_to_json_str_file("C:/Users/SDKim/Desktop/server_ALL_HERE_NUC1_0.json");
    serverS.udpServerS.readingPort = 8890;
    serverS.save_to_json_str_file("C:/Users/SDKim/Desktop/server_ALL_HERE_NUC2_0.json");
    serverS.udpServerS.readingPort = 8892;
    serverS.save_to_json_str_file("C:/Users/SDKim/Desktop/server_ALL_HERE_NUC3_0.json");
    serverS.udpServerS.readingPort = 8894;
    serverS.save_to_json_str_file("C:/Users/SDKim/Desktop/server_ALL_HERE_NUC4_0.json");

    // DCDeviceDataSettings server;
    // DCFrameGenerationSettings clientGeneration;

    // std::cout << serverS.deviceS.configS.


    DCClientSettings clientS;
    clientS.devicesS.resize(4);

    auto &co1 = clientS.devicesS[0].connectionS;
    co1.connectionType      = DCClientType::Remote;
    co1.protocol            = net::Protocol::ipv4;
    co1.readingPort         = 8889;
    co1.sendingPort         = 8888;
    co1.sendingAddress      = "ALL_HERE_NUC1";
    co1.idReadingInterface  = 0;
    co1.anyReadingInterface = false;

    auto &co2 = clientS.devicesS[1].connectionS;
    co2.connectionType      = DCClientType::Remote;
    co2.protocol            = net::Protocol::ipv4;
    co2.readingPort         = 8891;
    co2.sendingPort         = 8890;
    co2.sendingAddress      = "ALL_HERE_NUC2";
    co2.idReadingInterface  = 0;
    co2.anyReadingInterface = false;

    auto &co3 = clientS.devicesS[2].connectionS;
    co3.connectionType      = DCClientType::Remote;
    co3.protocol            = net::Protocol::ipv4;
    co3.readingPort         = 8893;
    co3.sendingPort         = 8892;
    co3.sendingAddress      = "ALL_HERE_NUC3";
    co3.idReadingInterface  = 0;
    co3.anyReadingInterface = false;

    auto &co4 = clientS.devicesS[3].connectionS;
    co4.connectionType      = DCClientType::Remote;
    co4.protocol            = net::Protocol::ipv4;
    co4.readingPort         = 8895;
    co4.sendingPort         = 8894;
    co4.sendingAddress      = "ALL_HERE_NUC4";
    co4.idReadingInterface  = 0;
    co4.anyReadingInterface = false;

    clientS.devicesS[0].deviceS.load_from_file(baseManagerPath + "settings/device_ALL_HERE_VRPC_G0.json");
    clientS.devicesS[1].deviceS.load_from_file(baseManagerPath + "settings/device_ALL_HERE_VRPC_G1.json");
    clientS.devicesS[2].deviceS.load_from_file(baseManagerPath + "settings/device_ALL_HERE_VRPC_G2.json");
    clientS.devicesS[3].deviceS.load_from_file(baseManagerPath + "settings/device_ALL_HERE_VRPC_G3.json");

    clientS.devicesS[0].filtersS.load_from_file(baseManagerPath + "settings/filters_ALL_HERE_VRPC_G0.json");
    clientS.devicesS[1].filtersS.load_from_file(baseManagerPath + "settings/filters_ALL_HERE_VRPC_G1.json");
    clientS.devicesS[2].filtersS.load_from_file(baseManagerPath + "settings/filters_ALL_HERE_VRPC_G2.json");
    clientS.devicesS[3].filtersS.load_from_file(baseManagerPath + "settings/filters_ALL_HERE_VRPC_G3.json");

    clientS.devicesS[0].calibrationFiltersS.load_from_file(baseManagerPath + "settings/calibration_filters_ALL_HERE_VRPC_G0.config");
    clientS.devicesS[1].calibrationFiltersS.load_from_file(baseManagerPath + "settings/calibration_filters_ALL_HERE_VRPC_G1.config");
    clientS.devicesS[2].calibrationFiltersS.load_from_file(baseManagerPath + "settings/calibration_filters_ALL_HERE_VRPC_G2.config");
    clientS.devicesS[3].calibrationFiltersS.load_from_file(baseManagerPath + "settings/calibration_filters_ALL_HERE_VRPC_G3.config");

    clientS.devicesS[0].colorS.load_from_file(baseManagerPath + "settings/color_ALL_HERE_VRPC_G0.json");
    clientS.devicesS[1].colorS.load_from_file(baseManagerPath + "settings/color_ALL_HERE_VRPC_G1.json");
    clientS.devicesS[2].colorS.load_from_file(baseManagerPath + "settings/color_ALL_HERE_VRPC_G2.json");
    clientS.devicesS[3].colorS.load_from_file(baseManagerPath + "settings/color_ALL_HERE_VRPC_G3.json");

    clientS.devicesS[0].modelS.load_from_file(baseManagerPath + "calibration/model_ALL_HERE_VRPC_G0.json");
    clientS.devicesS[1].modelS.load_from_file(baseManagerPath + "calibration/model_ALL_HERE_VRPC_G1.json");
    clientS.devicesS[2].modelS.load_from_file(baseManagerPath + "calibration/model_ALL_HERE_VRPC_G2.json");
    clientS.devicesS[3].modelS.load_from_file(baseManagerPath + "calibration/model_ALL_HERE_VRPC_G3.json");

    clientS.save_to_json_str_file("C:/Users/SDKim/Desktop/client_ALL_HERE_VRPC_0.json");
}
