
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


