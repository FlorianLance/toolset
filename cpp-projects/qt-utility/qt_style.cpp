

/*******************************************************************************
** Toolset-qt-utility                                                         **
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

#include "qt_style.hpp"

using namespace tool::ui;

auto Styles::object_name_b(const OName &objectName) -> QString {
    return objectName.value.length() > 0 ? QString("[objectName=\"%1\"]").arg(objectName.value) : "";
}

auto Styles::background_color_b(const BGCol &bgCol) -> QString {
    return bgCol.hexValue.length() > 0 ? QString("background-color: %1;").arg(bgCol.hexValue) : "";
}

auto Styles::front_color_b(const FCol &fCol) -> QString {
    return fCol.hexValue.length() > 0 ? QString("color: %1;").arg(fCol.hexValue) : "";
}

auto Styles::border_color(const BDCol &bdCol) -> QString{
    return bdCol.hexValue.length() > 0 ? QString("border-color: %1;").arg(bdCol.hexValue) : "";
}

auto Styles::border_width(int pixelSize) -> QString{
    return pixelSize > -1 ? QString("border-width: %1;").arg(pixelSize) : "";
}

auto Styles::border_style(const QString &borderStyle) -> QString{
    return borderStyle.length() > 0 ? QString("border-style: %1;").arg(borderStyle) : "";
}

auto Styles::font_size(int fontSize) -> QString{
    return fontSize > -1 ? QString("font-size: %1px;").arg(fontSize) : "";
}

auto Styles::font_family(QString family)  -> QString{
    return family.length() > 0 ? QString("font-family: \"%1\";").arg(family) : "";
}

auto Styles::font_style(QFont::Style style)  -> QString{
    return QString("font-style: %1;").arg(style == QFont::Style::StyleNormal ? "normal" : (style == QFont::Style::StyleItalic ? "italic" : "oblique"));
}

auto Styles::font_weight(QFont::Weight weight) -> QString {
    using enum QFont::Weight ;
    int w =0;
    switch(weight){
    case Thin: w = 100; break;
    case ExtraLight: w = 200; break;
    case Light: w = 300; break;
    case Normal: w = 400; break;
    case Medium: w = 500; break;
    case DemiBold: w = 600; break;
    case Bold: w = 700; break;
    case ExtraBold: w = 800; break;
    case Black: w = 900; break;
    }
    return QString("font-weight: %1;").arg(w);
}

auto Styles::object_name_contains_b(const BName &baseName) -> QString{
    return baseName.value.length() > 0 ? QString("[objectName^=\"%1\"]").arg(baseName.value) : "";
}

auto Styles::qlabel(OName oName, BGCol bgColor, FCol fColor, QFont::Weight w, int sizeFont) -> QString{
    return QString(
        "QLabel%1{%2 %3 %4 %5}"
    ).arg(
        object_name_b(oName),
        background_color_b(bgColor),
        front_color_b(fColor),
        font_weight(w),
        font_size(sizeFont)
    );
}

auto Styles::qline_edit(OName oName, FCol fCol, BGCol bgCol, QFont::Weight w, int sizeFont) -> QString{
    return QString(
        "QLineEdit%1{%2 %3 %4 %5}"
    ).arg(
        object_name_b(oName),
        front_color_b(fCol),
        background_color_b(bgCol),
        font_weight(w),
        font_size(sizeFont)
    );
}


auto Styles::qtoolbutton(OName oName, BGCol bgColor) -> QString{
    return QString(
        "QToolButton%1:hover{%2}"
    ).arg(
        object_name_b(oName),
        background_color_b(bgColor)
    );
}

auto Styles::qtext_browser(OName oName, BGCol bgColor, FCol fColor, QFont::Weight w, int sizeFont) -> QString{
    return QString(
        "QTextBrowser%1{%2 %3 %4 %5}"
    ).arg(
        object_name_b(oName),
        background_color_b(bgColor),
        front_color_b(fColor),
        font_weight(w),
        font_size(sizeFont)
    );
}

auto Styles::qdock_widget(OName oName, BGCol bgColor, FCol fColor, QFont::Weight w, int sizeFont) -> QString{
    return QString(
       "QDockWidget%1{%2 %3 %4 %5}"
    ).arg(
        object_name_b(oName),
        background_color_b(bgColor),
        front_color_b(fColor),
        font_weight(w),
        font_size(sizeFont)
    );
}

auto Styles::qtoolbar(OName oName, BGCol bgColor) -> QString{
    return QString(
            "QToolBar%1{%2}"
        ).arg(
            object_name_b(oName),
            background_color_b(bgColor)
    );
}

auto Styles::qcheck_box(OName oName, FCol fCol, BGCol bgCol) -> QString{
    return QString(
        "QCheckBox%1{%2 %3}"
    ).arg(
        object_name_b(oName),
        front_color_b(fCol),
        background_color_b(bgCol)
    );
}

auto Styles::qwidget(OName oName, BGCol bgCol) -> QString{
    return QString(
        "QWidget%1{%2}"
    ).arg(
        object_name_b(oName),
        background_color_b(bgCol)
    );
}


//auto Styles::qgroup_box(OName oName, BGCol bgColor, FCol fColor, QFont::Weight w, int sizeFont) -> QString{
//    return QString(
//        "QGroupBox%1::title{%2 %3 %4 %5}"
//    ).arg(
//        object_name_b(oName),
//        background_color_b(bgColor),
//        front_color_b(fColor),
//        font_weight(w),
//        font_size(sizeFont)
//    );
//}

auto Styles::qspin_box(OName oName, BGCol bgCol) -> QString{
    return QString(
        "QSpinBox%1{%2}"
    ).arg(
        object_name_b(oName),
        background_color_b(bgCol)
    );
}

auto Styles::qdoublespin_box(OName oName, BGCol bgCol) -> QString{
    return QString(
       "QDoubleSpinBox%1{%2}"
       ).arg(
        object_name_b(oName),
        background_color_b(bgCol)
    );
}

auto Styles::qcombo_box(OName oName, BGCol bgCol) -> QString{
    return QString(
        "QComboBox%1{%2}"
    ).arg(
        object_name_b(oName),
        background_color_b(bgCol)
    );
}

auto Styles::qtext_edit(OName oName, FCol fCol, BGCol bgCol) -> QString{
    return QString(
        "QTextEdit%1{%2 %3}"
    ).arg(
        object_name_b(oName),
        front_color_b(fCol),
        background_color_b(bgCol)
    );
}




auto Styles::qabstract_item_view(OName oName, BGCol bgCol) -> QString{
    return QString(
        "QAbstractItemView%1{%2}"
    ).arg(
        object_name_b(oName),
        background_color_b(bgCol)
    );
}

auto Styles::qtable_widget(OName oName, BGCol bCol) -> QString{
    return QString(
        "QTableWidget%1{%2}"
    ).arg(
        object_name_b(oName),
        background_color_b(bCol)
    );
}

auto Styles::qheader_view(OName oName, BGCol bgCol, FCol fCol, QFont::Weight weight) -> QString{
    return QString(
        "QHeaderView%1::section{%2 %3 %4}"
    ).arg(
        object_name_b(oName),
        front_color_b(fCol),
        background_color_b(bgCol),
        font_weight(weight)
    );
}

auto Styles::qpush_button(OName oName,
    BGCol bgColor, BGCol bgHoverColor, BGCol bgPressedColor,
    BDCol bdColor, BDCol bdHoverColor, BDCol bdPressedColor,
    FCol fCol,     FCol fHoverCol,     FCol fPressedCol,
    QFont::Weight w, int sizeFont) -> QString{
    return QString(
       "QPushButton%1{%2 %3 %4 %11 %12}"
       "QPushButton%1:hover{%5 %6 %7}"
       "QPushButton%1:pressed {%8 %9 %10}"
    ).arg(
        object_name_b(oName),
        background_color_b(bgColor),        border_color(bdColor),          front_color_b(fCol),
        background_color_b(bgHoverColor),   border_color(bdHoverColor),     front_color_b(fHoverCol),
        background_color_b(bgPressedColor), border_color(bdPressedColor),   front_color_b(fPressedCol),
        font_weight(w),
        font_size(sizeFont)
    );
}

auto Styles::qtabbar(OName oName,
    BGCol bgColor, FCol fColor,
    BGCol bgHoverColor, FCol fHoverColor,
    BGCol bgSelectedColor, FCol fSelectedColor,
    BGCol bgSelectedHoverColor, FCol fSelectedHoverColor) -> QString{

    return QString(
        "QTabBar%1::tab{%2 %3}"
        "QTabBar%1::tab:!selected{%4 %5}"
        "QTabBar%1::tab:selected{%6 %7}"
        "QTabBar%1::tab:selected:hover{%8 %9}"
    ).arg(
        object_name_b(oName),
        background_color_b(bgColor),               front_color_b(fColor),
        background_color_b(bgHoverColor),          front_color_b(fHoverColor),
        background_color_b(bgSelectedColor),       front_color_b(fSelectedColor),
        background_color_b(bgSelectedHoverColor),  front_color_b(fSelectedHoverColor)
    );
}

auto Styles::qline(BName bName, BDCol bdColor) -> QString{
    return QString(
        "QFrame%1{border: none; border-bottom: 1px solid; %2}"
    ).arg(
        object_name_contains_b(bName),
        border_color(bdColor)
    );
}

auto Styles::qtab_widget(OName oName, BGCol bgColor) -> QString{
    return QString(
        "QTabWidget%1{%2}"
        "QTabWidget%1::pane{%2}"
    ).arg(
        object_name_b(oName),
        background_color_b(bgColor)
    );
}

auto Styles::qlist_widget(OName oName, BGCol bColor, BGCol bItemColor, BGCol bSelectedItemColor) -> QString{
    return QString(
        "QListWidget%1{%2}"
        "QListWidget%1:item{%3}"
        "QListWidget%1:item:selected{%4}"
    ).arg(
        object_name_b(oName),
        background_color_b(bColor),
        background_color_b(bItemColor),
        background_color_b(bSelectedItemColor)
    );
}

auto Styles::qmenu(OName oName, FCol fColor, FCol fItemColor, FCol fSelectedItemColor, BGCol bgColor, BGCol bgItemColor, BGCol bgSelectedItemColor, int sizeFont) -> QString{
    return QString(
        "QMenu%1{%8 %2 %5}"
        "QMenu%1::item{%3 %6}"
        "QMenu%1::item:selected{%4 %7}"
    ).arg(
        object_name_b(oName),
        front_color_b(fColor),front_color_b(fItemColor),front_color_b(fSelectedItemColor),
        background_color_b(bgColor),background_color_b(bgItemColor),background_color_b(bgSelectedItemColor),
        font_size(sizeFont)
    );
}
