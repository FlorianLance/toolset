
/*******************************************************************************
** Toolset-ts-demos                                                           **
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


#include "biopac.hpp"

// std
#include <iostream>
#include <chrono>

// biopac
#include "mpdev.h"


std::string convert_to_str(MP_RETURN_CODE code){

    switch(code){
    case MPSUCCESS:
        return " [successful execution] ";
    case MPDRVERR:
        return " [error communicating with the device drivers] ";
    case MPDLLBUSY:
        return " [a process is attached to the DLL, only one process may use the DLL] ";
    case MPINVPARA:
        return " [invalid parameter(s)] ";
    case MPNOTCON:
        return " [MP device is not connected] ";
    case MPREADY:
        return " [MP device is ready] ";
    case MPWPRETRIG:
        return " [MP device is waiting for pre-trigger (pre-triggering is not implemented)] ";
    case MPWTRIG:
        return " [MP device is waiting for trigger] ";
    case MPBUSY:
        return " [MP device is busy] ";
    case MPNOACTCH:
        return " [there are no active channels, in order to acquire data at least one analog channel must be active] ";
    case MPCOMERR:
        return " [generic communication error] ";
    case MPINVTYPE:
        return " [the function is incompatible with the selected MP device or communication method] ";
    case MPNOTINNET:
        return " [the specified MP150 is not in the network] ";
    case MPSMPLDLERR:
        return " [MP device overwrote samples that had not been transferred from the device (buffer overflow)] ";
    case MPMEMALLOCERR:
        return " [error allocating memory] ";
    case MPSOCKERR:
        return " [internal socket error] ";
    case MPUNDRFLOW:
        return " [MP device returned a data pointer that is less than the last data pointer] ";
    case MPPRESETERR:
        return " [error with the specified preset file] ";
    case MPPARSERERR:
        return " [preset file parsing error, the XML file must be valid according to the schema] ";
    default:
        return "";
    }
}

bool BiopacDevice::connect(const std::string &serial){

    // configure the API and connect to the MP Device
    if(mp36){
        std::cout << "Connect to MP36 device.\n";
    }else{
        std::cout << "Connect to MP150 device using UDP with serial \"" << serial << "\"\n";
    }

    const auto device = mp36 ? MP36 : MP150;
    const auto mode = usb ? MPUSB : MPUDP;
    auto retval = connectMPDev(device, mode, automaticSerial ? "auto" : serial.c_str());

    m_connected = (retval == MPSUCCESS);
    if(!m_connected){

        std::cerr << "Program failed to connect to MP Device.\n";
        std::cerr << "connectMPDev returned with " << convert_to_str(retval) << " as a return code.\n";

        if(!mp36){
            std::cout << "Try to find connected MP150 devices...\n";

            // Finds all the MP150s routable in the current network configuration
            // and caches their serial numbers which can be read by calling readAvailableMP150SN().
            if(retval == findAllMP150()){
                const DWORD numCharToRead = 13;
                DWORD numCharRead   = 0;
                std::vector<char> buffer(14*10);
                std::fill(std::begin(buffer), std::end(buffer), ' ');

                if(retval == readAvailableMP150SN(buffer.data(), numCharToRead, &numCharRead)){
                    std::string s(buffer.begin(), buffer.end());
                    std::cout << "Find MP150 with serial: " << s << "\n";
                }else{
                    std::cerr << "Cannot read serial from detected MPR150. \n";
                    std::cerr << "readAvailableMP150SN returned with " << convert_to_str(retval) << " as a return code.\n";
                }

            }else{
                std::cerr << "Cannot find any MP150 connected. \n";
                std::cerr << "findAllMP150 returned with " << convert_to_str(retval) << " as a return code..\n";
            }
        }

        disconnectMPDev();
    }else{
        std::cout << "Device is connected.\n";
    }

    return m_connected;
}

void BiopacDevice::disconnect(){

    if(m_connected){

        if(m_biopacReader != nullptr){
            stop_reading_thread();
        }

        std::cout << "Disconnecting...\n";
        auto retval = disconnectMPDev();
        if(retval != MPSUCCESS){
            std::cerr << "Program failed to disconnect\n";
            std::cerr << "disconnectMPDev returned with " << convert_to_str(retval) << " as a return code.\n";
        }

        m_connected          = false;
        m_acquisitionStarted = false;

    }else{
        std::cerr << "Biopac already disconnected.\n";
    }
}

void BiopacDevice::start_acquisition(){

    if(!m_connected){
        std::cerr << "Biopac not connected.\n";
        return;
    }

    if(m_acquisitionStarted){
        std::cerr << "Acquisition already started.\n";
        return;
    }

    std::cout << "Setting acquisition channels...\n";
    auto retval = setAcqChannels(analogCH.data());

    if(retval != MPSUCCESS){
        std::cout << "Program failed to set Acquisition Channels.\n";
        std::cout << "setAcqChannels returned with " << convert_to_str(retval) << " as a return code.\n";
        return;
    }

    std::cout << "Setting sample rate.\n";
    retval = setSampleRate(timePerSample);
    if(retval != MPSUCCESS){
        std::cout << "Program failed to set Sample Rate.\n";
        std::cout << "setSampleRate returned with " << convert_to_str(retval) << " as a return code.\n";
        return;
    }

    std::cout << "Starting acquisition Daemon...\n";
    retval = startMPAcqDaemon();
    if(retval != MPSUCCESS){
        std::cerr << "Program failed to Start Acquisition Daemon\n";
        std::cerr << "startMPAcqDaemon returned with " << convert_to_str(retval) << " as a return code.\n";
        std::cout << "Stopping...\n";;
        stopAcquisition();
        return;
    }
    std::cout << "Daemon started\n";

    std::cout << "Starting acquisition...\n";
    retval = startAcquisition();
    if(retval != MPSUCCESS){
        std::cout << "Program failed to Start Acquisition\n";
        std::cerr << "startAcquisition returned with " << convert_to_str(retval) << " as a return code.\n";
        std::cout << "Stopping...\n";
        stopAcquisition();
        return;
    }
    std::cout << "Acquisition started.\n";

    m_acquisitionStarted = true;
}

void BiopacDevice::stop_acquisition(){

    if(!m_connected){
        std::cerr << "Biopac not connected.\n";
        return;
    }

    if(!m_acquisitionStarted){
        std::cerr << "Acquisition not started.\n";
        return;
    }

    auto retval = stopAcquisition();
    if(retval != MPSUCCESS){
        std::cerr << "Program failed to stop acquisition\n";
        std::cerr << "stopAcquisition returned with " << convert_to_str(retval) << " as a return code.\n";
    }

    m_acquisitionStarted = false;
}

void BiopacDevice::start_reading_thread(){


    if(!m_connected){
        std::cerr << "Biopac not connected.\n";
        return;
    }

    if(!m_acquisitionStarted){
        std::cerr << "Acquisition must be started before.\n";
        return;
    }

    if(m_biopacReader != nullptr || running){
        std::cerr << "Acquisition already started.\n";
        return;
    }

    std::cout << "Acquiring...\n";
    m_biopacReader = std::make_unique<std::thread>(&BiopacDevice::process, this);
}

void BiopacDevice::stop_reading_thread(){


    using namespace std::chrono_literals;

    if(m_biopacReader == nullptr || !running){
        std::cerr << "Acquisition not started.\n";
        return;
    }

    std::cout << "Stopping...\n";
    running = false;
    std::this_thread::sleep_for(1000ms);

    if(m_biopacReader->joinable()){
        m_biopacReader->join();
    }

    m_biopacReader = nullptr;
}

void BiopacDevice::process(){

    using namespace std::chrono_literals;

    int nbChannels = 0;
    for(const auto &channel : analogCH){
        if(channel == 1){
            ++nbChannels;
        }
    }

    // resize
    std::vector<double> data;
    data.resize(static_cast<size_t>(numSamples * nbChannels));
    channelsLastData.resize(nbChannels);

    DWORD valuesRead = 0;
    DWORD numValuesToRead = numSamples * nbChannels;

    DWORD totalRead = 0;

    running = true;
    while(running){

        auto retval = receiveMPData(data.data(), numValuesToRead, &valuesRead);
        totalRead += valuesRead;
        if(retval != MPSUCCESS){
            std::cerr << "Failed to receive MP data.\n";
            running = false;

            //            // using of getMPDaemonLAstError is a good practice
            //            char szbuff3[512];
            //            memset(szbuff3,0,512);
            //            sprintf(szbuff3,"Failed to Recv data from Acq Daemon. Last ERROR=%d, Read=%d", getMPDaemonLastError(), valuesRead);
            //            cout << szbuff3 << endl;

            //            stopAcquisition();

            return;
        }

        m.lock();
        for(size_t ii = 0; ii < channelsLastData[ii].size(); ++ii){
            channelsLastData[ii].clear();
        }

        m.unlock();
        std::this_thread::sleep_for(5ms);
    }

    std::cout << "Total nb values read: " << totalRead << "\n";

    //void startAcqDaemonDemo()
    //{

    //    cout << "Acquiring..." << endl;

    //    long numsamples = 5000;

    //    cout << "Getting " << numsamples << " samples from the MP Device... " << endl;

    //    DWORD valuesRead = 0;
    //    DWORD numValuesToRead = 0;
    //    //remember that data will be interleaved
    //    //therefore we need to mulitply the number of samples
    //    //by the number of active channels to acquire the necessary
    //    //data points from the active channels
    //    DWORD remainValues = 3*numsamples;
    //    double * buff = new double[remainValues];
    //    DWORD offset = 0;

    //    cout << "Acquiring...\n" << endl;

    //    while(remainValues > 0)
    //    {
    //        //read 1 second of data at a time
    //        //frequency times the number of active channels
    //        numValuesToRead = 3*200;

    //        //if there are more values to be read than the remaing number of values we want read then just read the reamining needed
    //        numValuesToRead = (numValuesToRead > remainValues) ? remainValues : numValuesToRead;


    //        if(receiveMPData(buff+offset,numValuesToRead, &valuesRead) != MPSUCCESS)
    //        {
    //            cout << "Failed to receive MP data" << endl;


    //            // using of getMPDaemonLAstError is a good practice
    //            char szbuff3[512];
    //            memset(szbuff3,0,512);
    //            sprintf(szbuff3,"Failed to Recv data from Acq Daemon. Last ERROR=%d, Read=%d", getMPDaemonLastError(), valuesRead);
    //            cout << szbuff3 << endl;

    //            stopAcquisition();

    //            break;
    //        }

    //        offset += numValuesToRead;
    //        remainValues -= numValuesToRead;

    //        //show status
    //        printf("                                                      \r");
    //        printf("Remaining Values: %d\r", remainValues );
    //    }

    //    cout << endl;

    //    for(int j=0; j<numsamples; j++)
    //    {
    //        cout << "Sample: " << j+1 << endl;

    //        for(int i=0; i < 3; i++)
    //            switch(i)
    //            {
    //            case 0:
    //                cout << "CH2: " << buff[i+(3*j)];
    //                break;
    //            case 1:
    //                cout << " CH7: " << buff[i+(3*j)];
    //                break;
    //            case 2:
    //                cout << " CH11: " << buff[i+(3*j)] << endl;
    //                break;
    //            default:
    //                break;

    //            }
    //    }

    //    //stop
    //    cout << "Stopping..." << endl;

    //    stopAcquisition();

    //    //free Memory
    //    delete[] buff;
    //}


}
