
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


#pragma once

// std
#include <thread>
#include <memory>
#include <atomic>
#include <string>
#include <vector>
#include <mutex>

class BiopacDevice{

public:

    bool connect(const std::string &serial = "auto");
    void disconnect();

    void start_acquisition();
    void stop_acquisition();

    void start_reading_thread();
    void stop_reading_thread();

    inline bool is_connected() const{
        return m_connected;
    }

    inline bool acquisition_started() const{
        return m_acquisitionStarted;
    }

    inline bool thread_running() const{
        return running;
    }

private:

    void process();

public:

    std::mutex m;

    bool usb  = true;
    bool mp36 = false;
    bool automaticSerial = true;

    std::vector<int> analogCH = std::vector<int>(16);
    double timePerSample = 5.0;
    int numSamples = 100;

    // channels x array of data
    std::vector<std::vector<std::pair<long, double>>> channelsData;

    std::vector<std::vector<double>> channelsLastData;

private:

    bool m_connected = false;
    bool m_acquisitionStarted = false;
    std::atomic_bool running = false;
    std::unique_ptr<std::thread> m_biopacReader = nullptr;
};


