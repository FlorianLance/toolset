

/*******************************************************************************
** Toolset-qt-utility                                                         **
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

// Qt
#include <QVBoxLayout>
#include <QDoubleSpinBox>
#include <QLineEdit>
#include <QRadioButton>
#include <QPushButton>
#include <QLabel>
#include <QCheckBox>
#include <QTextEdit>
#include <QSpinBox>
#include <QComboBox>
#include <QDateTimeEdit>

namespace tool {

// names types
template<typename T>
struct MinV {T v;};
template<typename T>
struct V{T v;};
template<typename T>
struct MaxV {T v;};
template<typename T>
struct StepV {T v;};
struct LMargins{bool v;};
struct LStretch {bool v;};

using MinI  = MinV<int>;
using MaxI  = MaxV<int>;
using ValI  = V<int>;
using StepI = StepV<int>;

using MinR  = MinV<qreal>;
using MaxR  = MaxV<qreal>;
using ValR  = V<qreal>;
using StepR = StepV<qreal>;

struct LMarginsD{
    int left = 2;
    int top = 2;
    int right = 2;
    int bottom = 2;
    int inter = 2;
};

struct LStretchD{
    int spacerStretch = 10;
    std::vector<int> elementsStretch;
};

struct DsbSettings{
    MinR  min         = {-1000.};
    ValR  value       = {0.};
    MaxR  max         = {1000.};
    StepR singleStep  = {0.1};
    int   decimals    = 2;
};

struct Vector2dSettings{
    DsbSettings sX = {MinR{-1000.}, ValR{0}, MaxR{1000.}, StepR{0.01}, 2};
    DsbSettings sY = {MinR{-1000.}, ValR{0}, MaxR{1000.}, StepR{0.01}, 2};
};

struct Vector3dSettings{
    DsbSettings sX = {MinR{-1000.}, ValR{0}, MaxR{1000.}, StepR{0.01}, 2};
    DsbSettings sY = {MinR{-1000.}, ValR{0}, MaxR{1000.}, StepR{0.01}, 2};
    DsbSettings sZ = {MinR{-1000.}, ValR{0}, MaxR{1000.}, StepR{0.01}, 2};
};
}

namespace tool::ui {

template<class T> class Blocker {
    T *blocked;
    bool previous;
public:
    Blocker(T *blocked) : blocked(blocked), previous(blocked->blockSignals(true)) {}
    ~Blocker() { blocked->blockSignals(previous); }
    T *operator->() { return blocked; }
};

template<class T> inline auto w_blocking(T *blocked) -> Blocker<T> {
    return Blocker<T>(blocked);
}

struct W{

    // init
    static auto init_frame_on_off(QFrame *f, bool on, bool enabled = true) -> QFrame*;
    static auto init(QComboBox *cb, QStringList items, int index=0, bool enabled = true) -> QComboBox*;
    static auto init(QComboBox *cb, QStringList items, QString currentItem, bool enabled = true) -> QComboBox*;
    static auto init(QCheckBox *cb, const QString &txt, bool checked, bool enabled = true) -> QCheckBox*;
    static auto init(QRadioButton *rb, const QString &txt, bool checked, bool enabled = true) -> QRadioButton*;
    static auto init(QSlider *sl, MinV<int> min, V<int> value, MaxV<int> max, StepV<int> singleStep, bool enabled = true) ->  QSlider*;
    static auto init(QPushButton *pb, const QString &txt, bool enabled = true)-> QPushButton*;
    static auto init(QTextEdit *te, const QString &txt, Qt::TextFormat tf, bool enabled = true) -> QTextEdit*;
    static auto init(QLineEdit *le, const QString &txt, bool enabled = true) -> QLineEdit*;
    static auto init(QLabel *la, const QString &txt, bool enabled = true) -> QLabel*;
    static auto init(QSpinBox *sb, MinV<int> min, V<int> value, MaxV<int> max, StepV<int> singleStep, bool enabled = true) -> QSpinBox*;
    static auto init(QDoubleSpinBox *dsb, MinV<qreal> min, V<qreal> value, MaxV<qreal> max, StepV<qreal> singleStep, int decimals, bool enabled = true) -> QDoubleSpinBox*;
    // set
    static auto set(QLabel *la, const QString &txt) -> void;
    static auto set(QSpinBox *sb, int value) -> void;
    static auto set(QDoubleSpinBox *dsb, double value) -> void;
    static auto set(QComboBox *cb, int index) -> void;
    static auto set(QComboBox *cb, const QString &txt) -> void;
    static auto set(QLineEdit *le, const QString &txt) -> void;
    static auto set(QCheckBox *cb, bool checked) -> void;
    static auto set(QRadioButton *rb, bool checked) -> void;
    static auto set(QDateTimeEdit *dte, QDateTime dateTime) -> void;
    static auto set(QDateTimeEdit *dte, QTime time) -> void;

    static auto box_value(const QComboBox *box) -> QVariant{
        int idx = box->currentIndex();
        if (idx == -1){
            return QVariant();
        }
        return box->itemData(idx);
    }

    static auto set_box_value(QComboBox *box, int v) -> void{
        for(int id = 0; id < box->count(); ++id){
            if(box->itemData(id).toInt() == v){
                set(box, id);
                return;
            }
        }
    }

    static auto set_box_value(QComboBox *box, const QString &v) -> void{
        for(int id = 0; id < box->count(); ++id){
            if(box->itemData(id) == v){
                set(box, id);
                return;
            }
        }
    }

    // generate
    [[nodiscard]] static inline auto empty() -> QWidget*{return new QWidget();}
    [[nodiscard]] static inline auto button() -> QPushButton*{return new QPushButton();}
    [[nodiscard]] static inline auto txt(const QString &txt) -> QLabel* {return new QLabel(txt);}
    [[nodiscard]] static auto txt(const QString &txt, Qt::Alignment alignment) -> QLabel*;
    [[nodiscard]] static auto icon(QIcon icon, QSize size) -> QLabel*;
    [[nodiscard]] static auto icon(QColor color, QSize size) -> QLabel*;
};

struct L{
    static auto stretch(QLayout *layout, int stretchF = 0) -> void;
    [[nodiscard]] static inline auto VB() -> QVBoxLayout* {return new QVBoxLayout();}
    [[nodiscard]] static inline auto HB() -> QHBoxLayout*{return new QHBoxLayout();}
    [[nodiscard]] static inline auto G() -> QGridLayout*{return new QGridLayout();}
};

struct A{
    [[nodiscard]] static constexpr auto t_l() noexcept -> QFlags<Qt::AlignmentFlag>{return Qt::AlignLeft | Qt::AlignTop;}
    [[nodiscard]] static constexpr auto t_r() noexcept -> QFlags<Qt::AlignmentFlag>{return Qt::AlignRight | Qt::AlignTop;}
    [[nodiscard]] static constexpr auto t_c() noexcept -> QFlags<Qt::AlignmentFlag>{return Qt::AlignHCenter | Qt::AlignTop;}
    [[nodiscard]] static constexpr auto b_l() noexcept -> QFlags<Qt::AlignmentFlag>{return Qt::AlignLeft | Qt::AlignBottom;}
    [[nodiscard]] static constexpr auto b_r() noexcept -> QFlags<Qt::AlignmentFlag>{return Qt::AlignRight | Qt::AlignBottom;}
    [[nodiscard]] static constexpr auto b_c() noexcept -> QFlags<Qt::AlignmentFlag>{return Qt::AlignHCenter | Qt::AlignBottom;}
    [[nodiscard]] static constexpr auto l() noexcept -> QFlags<Qt::AlignmentFlag>{return Qt::AlignLeft | Qt::AlignVCenter;}
    [[nodiscard]] static constexpr auto r() noexcept -> QFlags<Qt::AlignmentFlag>{return Qt::AlignRight | Qt::AlignVCenter;}
    [[nodiscard]] static constexpr auto c() noexcept -> QFlags<Qt::AlignmentFlag>{return Qt::AlignVCenter | Qt::AlignHCenter;}
};


struct F{


    struct WL{
        QWidget *w = nullptr;
        int stretchFactor = 0;
    };

    struct WA{
        QWidget *w = nullptr;
        Qt::Alignment a;
    };

    struct WAP{
        QWidget *w = nullptr;
        int rowP=-1;
        int colP=-1;
        int rowsS=1;
        int colsS=1;
        Qt::Alignment a;
    };

    [[nodiscard]] static auto h_line() -> QFrame*;

    static auto horiz(
        std::vector<WL> widgets,
        int spacerStretch = 0,
        LMarginsD margins = LMarginsD{},
        QFrame::Shape shape = QFrame::Shape::NoFrame,
        QFrame::Shadow shadow = QFrame::Raised
    ) -> QFrame*;

    static auto vert(
        std::vector<WL> widgets,
        int spacerStretch = 0,
        LMarginsD margins = LMarginsD{},
        QFrame::Shape shape = QFrame::Shape::NoFrame,
        QFrame::Shadow shadow = QFrame::Raised
    ) -> QFrame*;

    static auto grid(
        std::vector<WA> widgets,
        std::vector<int> rowsS,
        std::vector<int> colsS,
        int hStretch = 0,
        int vStretch = 0,
        LMarginsD margins = LMarginsD{},
        QFrame::Shape shape = QFrame::Shape::NoFrame,
        QFrame::Shadow shadow = QFrame::Raised) -> QFrame*;

    static auto grid2(
        std::vector<WAP> widgets,
        std::vector<int> rowsS,
        std::vector<int> colsS,
        int addRowStretch = 0,
        int addColStretch = 0,
        LMarginsD margins = LMarginsD{},
        QFrame::Shape shape = QFrame::Shape::NoFrame,
        QFrame::Shadow shadow = QFrame::Raised) -> QFrame*;


    // deprecated
    static QFrame *old_gen_frame(QLayout *layout, std::vector<std::pair<QWidget*,int>> widgetsStretch, int stretchFactor = 0, LMarginsD margins = LMarginsD{},
                       QFrame::Shape shape = QFrame::Shape::NoFrame, QFrame::Shadow shadow = QFrame::Raised);
    static QFrame *old_gen_frame(QLayout *layout, std::vector<QWidget*> widgets, LStretchD strech = LStretchD{}, LMarginsD margins = LMarginsD{},
                       QFrame::Shape shape = QFrame::Shape::NoFrame, QFrame::Shadow shadow = QFrame::Raised);
    static QFrame* old_gen(QLayout *layout, std::vector<QWidget*> widgets, LStretch stretch = LStretch{true}, LMargins margins = LMargins{true},
                       QFrame::Shape shape = QFrame::Shape::NoFrame, QFrame::Shadow shadow = QFrame::Raised);

private:

    static auto gen(
        QLayout *layout,
        std::vector<WL> widgets,
        int spacerStretch = 0,
        LMarginsD margins = LMarginsD{},
        QFrame::Shape shape = QFrame::Shape::NoFrame,
        QFrame::Shadow shadow = QFrame::Raised
    ) -> QFrame*;
};

}

