
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


#include "dc_device_array.hpp"

// local
#include "depth-camera/impl/azure_kinect_device_impl.hpp"
#include "depth-camera/impl/femto_bolt_device_impl.hpp"
#include "depth-camera/impl/femto_mega_ethernet_device_impl.hpp"
#include "depth-camera/impl/femto_mega_usb_device_impl.hpp"
#include "utility/logger.hpp"

using namespace tool::cam;

struct DCDeviceArray::Impl{

    // // settings
    // DCDeviceSettings deviceS;
    // DCFiltersSettings filtersS;
    // DCColorSettings colorsS;
    // DCMiscSettings delayS;

    std::vector<std::unique_ptr<DCDeviceImpl>> devices;
};

DCDeviceArray::DCDeviceArray(): i(std::make_unique<Impl>()){
}

DCDeviceArray::~DCDeviceArray(){
}


auto DCDeviceArray::initialize(std::span<const DCDeviceSettings> devicesS) -> void{

    // clean
    // ...

    // create devices
    i->devices.resize(devicesS.size());
    size_t idD = 0;
    for(auto &device : i->devices){
        // device = std::make
        auto type = devicesS[idD].configS.typeDevice;
        if(type == DCType::AzureKinect){
            device = std::make_unique<AzureKinectDeviceImpl>();
        }else if(type == DCType::FemtoBolt){
            device = std::make_unique<FemtoBoltDeviceImpl>();
        }else if(type == DCType::FemtoMegaEthernet){
            device = std::make_unique<FemtoMegaEthernetDeviceImpl>();
        }else if(type == DCType::FemtoMegaUSB){
            device = std::make_unique<FemtoMegaUSBDeviceImpl>();
        }
        ++idD;
    }

    // open devices
    idD = 0;
    for(auto &device : i->devices){
        const auto &deviceS = devicesS[idD];
        if(deviceS.configS.openDevice){
            if(device->open(deviceS.configS)){
                // device->set_data_settings(deviceS.dataS.server);
                // device->set_filters_settings(deviceS.)
            }
        }
        ++idD;
    }

    // start reading
            // if(devicesS[idD].configS.startReading)
}

auto DCDeviceArray::open() -> void{

}
