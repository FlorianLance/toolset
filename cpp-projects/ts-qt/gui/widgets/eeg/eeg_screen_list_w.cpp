
#include "eeg_screen_list_w.hpp"

// Qt
#include <QGridLayout>
#include <QResizeEvent>
#include <QToolTip>
#include <QMouseEvent>
#include <QMenu>

// local
#include "gui/widgets/qt_fast_multi_curves_plot_w.hpp"

using namespace tool::ui;
using namespace Qt::Literals::StringLiterals;

EEGScreenListW::EEGScreenListW(){

    auto layout = new QGridLayout();
    setLayout(layout);
    layout->setContentsMargins(0,0,0,0);
    layout->addWidget(m_plotW = new QtFastMultiCurvesPlotW());

    contextMenu = new QMenu(this);
    // switchInterpolateA = new QAction(u"Switch interpolation"_s);
    // connect(switchInterpolateA, &QAction::triggered, this, [&](){
    //     emit AhBciSignals::get()->vd_channel_interpolation_state_switched_signal(m_idUser, m_lastClickID);
    // });
    // contextMenu->addAction(switchInterpolateA);

    auto plotW = dynamic_cast<QtFastMultiCurvesPlotW*>(m_plotW);
    connect(plotW, &QtFastMultiCurvesPlotW::left_click_pressed_signal, this, [&](QPoint globalPos, int id, const QString &text){
        if(id < m_maxB.size()){
            QToolTip::showText(globalPos, u"E%1 : %2\nMin: %3\nMax: %4"_s.arg(QString::number(id),text, QString::number(m_minB[id]),QString::number(m_maxB[id])));
        }
    });
    connect(plotW, &QtFastMultiCurvesPlotW::left_click_released_signal, this, [&](){
        QToolTip::hideText();
    });

    connect(plotW, &QtFastMultiCurvesPlotW::right_click_pressed_signal, this, [&](QPoint globalPos, int id, const QString &text){
        if(id < m_maxB.size()){
            m_lastClickID = id;
            contextMenu->exec(globalPos);
        }
    });

    plotW->set_left_title("");
    plotW->set_x_axis_scaling_infos(true, 0.0 , 1.0, 0.0, false);
    plotW->set_y_axis_scaling_infos(false, 0.0, 1.0, false, 1.0, false);
    plotW->set_grid_lines_display(false, false, false, false);
}

auto EEGScreenListW::resize(size_t nbElectrodes) -> void{

    auto plotW = dynamic_cast<QtFastMultiCurvesPlotW*>(m_plotW);
    m_minB.resize(nbElectrodes);
    m_maxB.resize(nbElectrodes);
    m_curvesColors.resize(nbElectrodes);
    plotW->set_nb_horizontal_markers(nbElectrodes);

    plotW->set_nb_curves(nbElectrodes, false);
    plotW->set_curves_identical_width(1.2, true);
}

auto EEGScreenListW::update_data(std::shared_ptr<TimeChannelsNBuffer<double>> data) -> void{

    m_data = std::move(data);



    if(m_data == nullptr){
        return;
    }
    if(m_data->nb_channels() == 0 || m_data->nb_samples() == 0){
        return;
    }


    // resize data
    if(m_plotYData.size() != m_data->nb_channels()){
        m_plotYData.resize(m_data->nb_channels());
        // m_plotXData.resize(m_data->nb_samples());
        for(auto &pData : m_plotYData){
            pData.resize(m_data->nb_samples());
        }
    }else if(m_plotYData[0].size() != m_data->nb_samples()){
        // m_plotXData.resize(m_data->nb_samples());
        for(auto &pData : m_plotYData){
            pData.resize(m_data->nb_samples());
        }
    }

    // retrieve min/max/diff
    double min = std::numeric_limits<double>::max();
    double max = std::numeric_limits<double>::min();
    double diff = 0.0;
    for(size_t idC = 0; idC < m_data->nb_channels(); ++idC){
        auto cMin = m_data->channels[idC].min();
        auto cMax = m_data->channels[idC].max();

        if(min > cMin){
            min = cMin;
        }
        if(max < cMax){
            max = cMax;
        }

        auto cDiff = cMax - cMin;
        if(diff < cDiff){
            diff = cDiff;
        }
        m_minB[idC] = cMin;
        m_maxB[idC] = cMax;
    }


    // set automatic scale
    auto plotW = dynamic_cast<QtFastMultiCurvesPlotW*>(m_plotW);
    double offsetY  = 1.0 / (m_data->nb_channels() + 1);
    double currentY = 1.0 - offsetY;
    for(size_t idC = 0; idC < m_data->nb_channels(); ++idC){
        for(size_t idS = 0; idS < m_data->nb_samples(); ++idS){
            auto d = (m_data->channels[idC][idS]);
            d -= m_minB[idC];
            d -= (m_maxB[idC] - m_minB[idC]) * 0.5;
            d /= diff;
            m_plotYData[idC][idS] = m_heightFactor*d / m_data->nb_channels() + currentY;
        }

        plotW->set_curve_points(idC, m_data->times.span(), m_plotYData[idC].span());
        currentY -= offsetY;
    }

    auto diffT = m_data->times.back() - m_data->times.front();
    plotW->set_x_axis_scaling_infos(true, m_data->times.front(),  m_data->times.back(), diffT*0.1, true);
}

auto EEGScreenListW::update_display_settings(double heightFactor) -> void{
    // m_auto = automatic;
    m_heightFactor = heightFactor;
}

auto EEGScreenListW::update_id(int idUser) -> void{
    m_idUser = idUser;
}


auto EEGScreenListW::update_channels(size_t nbChannels, std::span<QString> channelsNames, std::span<QColor> channelsColors) -> void{

    resize(nbChannels);

    // set y axe to 0 - 1
    auto plotW = dynamic_cast<QtFastMultiCurvesPlotW*>(m_plotW);
    double offsetY = 1.0 / (nbChannels + 1);
    double currentY = 1.0 - offsetY;

    plotW->set_left_title("");
    // plotW->set_x_axis_scaling_infos(false, -0.2, 1.0);
    // plotW->set_x_axis_scaling_infos(true, 0.0 , 1.0, 0.0, false);


    plotW->set_y_axis_scaling_infos(false, 0.0, 1.0, false, 1.0, false);
    plotW->set_grid_lines_display(false, false, false, false);

    // size_t maxSize = std::min(nbChannels, std::min(channelsNames.size(), channelsColors.size()));
    for(size_t idC =  0; idC < nbChannels; ++idC){
        plotW->set_horizontal_marker_info(idC, currentY, channelsNames[idC]);
        m_curvesColors[idC] = channelsColors[idC];
        currentY -= offsetY;
    }
    plotW->set_curves_colors(m_curvesColors, true);
}

auto EEGScreenListW::add_marker(double time) -> void{
    auto plotW = dynamic_cast<QtFastMultiCurvesPlotW*>(m_plotW);
    plotW->add_vertical_marker(time, "t");
}
