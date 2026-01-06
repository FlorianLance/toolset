


#include "qt_plot_graphic_item.hpp"

using namespace tool::ui;

PlotGraphicsItem::PlotGraphicsItem(QGraphicsItem *parent) : QGraphicsItem(parent){

    middleLinePen.setColor(QColor(122,122,122,122));
    rawPlotPen.setColor(QColor(255,0,0,255));
    processedPlotPen.setColor(QColor(0,0,255,255));
    labelPen.setColor(QColor(0,0,0,255));

    rawPlotPen.setWidthF(0.5);
    processedPlotPen.setWidthF(0.5);
}


auto PlotGraphicsItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) -> void{

    static_cast<void>(option);
    static_cast<void>(widget);

    // draw background
    painter->fillRect(boundingRect(), backgroundColor);

    // middle line
    painter->setPen(middleLinePen);    
    painter->drawLine(QPointF(0, height/2), QPointF(width, height/2));

    // draw raw plot
    painter->setPen(rawPlotPen);
    painter->drawPolyline(rawPlot);

    // draw processed plot
    // painter->setPen(processedPlotPen);
    // painter->drawPolyline(processedPlot);

    // label
    painter->setPen(labelPen);
    painter->drawText(QRectF(boundingRect().topLeft(), QSize(150,25)), name);

    // test
    //  painter->fillRect(width/2-5,0,width/2+5, 2*height/3.0, Qt::blue);
    //        painter->fillRect(0,  2*height/3.0, width/4, 1*height/3.0, Qt::blue);
    //0,0,10,2*height/3.0);
}

auto PlotGraphicsItem::reset_data() -> void{
    rawPlot.clear();
}


auto PlotGraphicsItem::set_raw_data(const tool::Buffer<double> &times, const tool::Buffer<double> &values) -> void{

    if(times.size() < 2 || (times.size() != values.size())){
        return;
    }

    double minRY;
    double maxRY;
    if(automaticRangeY){
        minRY = *std::min_element(values.cbegin(), values.cend());
        maxRY = *std::max_element(values.cbegin(), values.cend());
    }else{
        minRY = minRangeY;
        maxRY = maxRangeY;
    }


    auto firstX = times.front();
    auto lastX  = times.back();
    // if(lastX < maxRangeX){
    //     lastX = maxRangeX;
    // }
    auto diffX = lastX - firstX;
    auto diffY = maxRY - minRY;

    rawPlot.resize(times.size());
    for(size_t ii = 0; ii < times.size(); ++ii){

        rawPlot[ii].rx() = width * (times[ii] - firstX) / diffX;

        auto y = values[ii];
        if(y < minRY){
            y = minRY;
        }else if (y > maxRY){
            y = maxRY;
        }
        rawPlot[ii].ry() = height*(y - minRY)/diffY;
    }


//     scale eeg score




//     def scale_eeg_zscore(eeg_signal):
//         mean = np.mean(eeg_signal)
//                std = np.std(eeg_signal)
//           return (eeg_signal - mean) / std

// # Example usage
//               signal = np.array([...])  # Your EEG signal
//               scaled_signal = scale_eeg_zscore(signal)

//     Pros

//     Preserves relative relationships between features
//     Reduces impact of absolute voltage differences
//     Maintains zero mean for better statistical analysis

//         Common approach in EEG research

//     Cons

//     Sensitive to outliers
//     May not preserve physiological amplitude relationships

//     Z-score scaling normalizes the signal to have zero mean and unit variance. This approach is particularly useful for comparative analyses across different EEG recordings or subjects, as it reduces the impact of absolute voltage differences while preserving relative relationships between features.



//     def scale_eeg_fixed_range(eeg_signal):
//         min_val = np.min(eeg_signal)
//                   max_val = np.max(eeg_signal)
//               return 2 * ((eeg_signal - min_val) / (max_val - min_val)) - 1
//     Pros

//     Simple to interpret (-1 to +1 range)
//     Preserves signal extremes

//         Good for visualization purposes

//     Cons

//     Sensitive to outliers
//     May lose relative relationships
//     Not suitable for statistical analysis

//     Fixed range scaling compresses the signal to a fixed range (-1 to +1). While simple and intuitive, this approach can be sensitive to outliers and may not preserve important relative relationships between different parts of the signal.


//     def scale_eeg_robust(eeg_signal):
//         lower_bound = np.percentile(eeg_signal, 1)
//                       upper_bound = np.percentile(eeg_signal, 99)
//           return (np.clip(eeg_signal, lower_bound, upper_bound) - lower_bound) / \
//           (upper_bound - lower_bound)

// # Example usage
//           signal = np.array([...])  # Your EEG signal
//               scaled_signal = scale_eeg_robust(signal)

//     Robust scaling uses percentiles rather than absolute minimum/maximum values, making it more resistant to outliers. This approach is particularly useful for noisy EEG recordings where artifacts might skew the scaling.

// Pros

// Resistant to outliers
// Preserves most of the signal information

//     Better for noisy recordings

// Cons

// Discards extreme values
}

auto PlotGraphicsItem::set_raw_data2(const tool::NumericBuffer<double> &times, const tool::NumericBuffer<double> &values) -> void{

    if(times.size() < 2 || (times.size() != values.size())){
        return;
    }

    // retrieve min / max for first axis
    auto firstX = times.min();
    auto lastX  = times.max();

    // resize plot according it
    rawPlot.resize(times.size());

    // generate data x positions
    for(size_t ii = 0; ii < times.size(); ++ii){
        rawPlot[ii].rx() = (1.0 * ii / times.size()) * width;
    }

    double minRY = minRangeY;
    double maxRY = maxRangeY;
    if(automaticRangeY){
        minRY = values.min();
        maxRY = values.max();
    }else {

        if(addTempMOffset){
            auto mean = values.mean();
            minRY += mean;
            maxRY += mean;
        }
    }

    for(size_t ii = 0; ii < times.size(); ++ii){
        rawPlot[ii].ry() = height * (values[ii] - minRY)/(maxRY-minRY);
    }


    // // compute y plot values with z-scole scaling
    // // qDebug() << "compute y plot values with z-scole scaling";
    // auto meanV = values.mean();
    // auto stdV  = values.standard_deviation();
    // yTemp.resize(times.size());
    // for(size_t ii = 0; ii < times.size(); ++ii){
    //     yTemp[ii] = (values[ii] - meanV) / stdV;
    // }

    // // set y plot values
    // // qDebug() << "set y plot values";
    // auto minY    = yTemp.min();
    // auto maxY    = yTemp.max();
    // auto diffY   = maxY-minY;
    // for(size_t ii = 0; ii < times.size(); ++ii){
    //     rawPlot[ii].ry() = height*(yTemp[ii] - minY)/diffY;
    // }
    //qDebug() << "MIN " << firstX << " MAX " << lastX << " DIFF " << diffX << " minRY " << minRY << " maxRY " << maxRY;

    //     def scale_eeg_zscore(eeg_signal):
    //         mean = np.mean(eeg_signal)
    //                std = np.std(eeg_signal)
    //           return (eeg_signal - mean) / std

    // double minRY;
    // double maxRY;
    // if(automaticRangeY){
    //     minRY = *std::min_element(values.cbegin(), values.cend());
    //     maxRY = *std::max_element(values.cbegin(), values.cend());
    // }else{
    // minRY = minRangeY;
    // maxRY = maxRangeY;
    // }



    // for(size_t ii = 0; ii < times.size(); ++ii){

        // auto diffY = maxRY - minRY;
    //     // rawPlot[ii].ry() = height*y;
    //     // rawPlot[ii].ry() = height*(y)/10.0;

    //     // if(ii == 0){
    //     //     qDebug() << "ii " << ii << y << height*y << meanV << stdV;
    //     // }
    //     // rawPlot[ii].ry() = height*((rand()%1000) - 500)/500.0;
    // }

    // qDebug() << "MEAN " << cBuffer.mean() << "STD"<< cBuffer.standard_deviation();



    // double mean = 0.0;
    // for(size_t ii = 0; ii < times.size(); ++ii){
    //     mean += rawPlot[ii].ry();
    // }
    // mean /= times.size();
    // qDebug() << "MEAN " << mean;
}

auto PlotGraphicsItem::boundingRect() const -> QRectF{
    return QRectF(0,0,width,height);
}
