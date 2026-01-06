
#pragma once

// Qt
#include <QWidget>

// local
#include "utility/times_data_buffer.hpp"


namespace tool::ui{

struct EEGScreenListW : public QWidget{
    Q_OBJECT

public:

    EEGScreenListW();
    auto resize(size_t nbElectrodes) -> void;
    auto update_display_settings(double heightFactor) -> void;
    auto update_id(int idUser) -> void;

public slots:

    auto update_data(std::shared_ptr<tool::TimeChannelsNBuffer<double>> data) -> void;
    auto update_channels(size_t nbChannels, std::span<QString> channelsNames, std::span<QColor> channelsColors) -> void;
    auto add_marker(double time) -> void;

private:

    int m_idUser = 0;

    tool::Buffer<QColor> m_curvesColors;

    QMenu *contextMenu = nullptr;
    QAction *switchInterpolateA = nullptr;
    int m_lastClickID = 0;

    QWidget *m_plotW = nullptr;
    std::shared_ptr<tool::TimeChannelsNBuffer<double>> m_data  = nullptr;
    // tool::Buffer<double> m_plotXData;
    tool::Buffer<tool::Buffer<double>> m_plotYData;
    tool::Buffer<double> m_minB;
    tool::Buffer<double> m_maxB;
    bool m_auto = true;
    double m_heightFactor = 1.00;
};

}
