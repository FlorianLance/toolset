

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

#pragma once

// Qt
#include <QFont>

// local
#include "qt_str.hpp"

namespace tool::ui {

struct Colors{

    static inline const QString whiteH     = "#FFFFFF";
    static inline const QString lightGrayH = "#BDBDBD";
    static inline const QString darkGrayH  = "#2D2D2D";
    static inline const QString blackH     = "#000000";

    static inline const QString redH       = "#FF0000";
    static inline const QString greenH     = "#279340";
    static inline const QString orangeH    = "#FF8C00";

    static inline const QString lightBlueH = "#8FBBBC";
    static inline const QString blueH      = "#5C8EBC";
    static inline const QString navyBlueH  = "#427B89";
};

struct OName{QString value;};
struct BName{QString value;};
struct FCol{QString hexValue;};
struct BGCol{QString hexValue;};
struct BDCol{QString hexValue;};

struct Styles{

    enum class Theme {
        Light, Dark
    };

    using C = Colors;
    // object
    static auto object_name_b(const OName &objectName) -> QString;
    static auto object_name_contains_b(const BName &baseName) -> QString;
    // color
    static auto front_color_b(const FCol &fCol) -> QString;
    static auto background_color_b(const BGCol &bgCol) -> QString;
    // border
    static auto border_color(const BDCol &bdCol) -> QString;
    static auto border_width(int pixelSize) -> QString;
    static auto border_style(const QString &borderStyle) -> QString;
    // font
    static auto font_size(int fontSize) -> QString;
    static auto font_weight(QFont::Weight weight) -> QString;
    static auto font_family(QString family) -> QString;
    static auto font_style(QFont::Style style = QFont::Style::StyleNormal) -> QString;

    // ui elements
    static auto qwidget(OName oName, BGCol bgCol) -> QString;
//    static auto qgroup_box(OName oName, BGCol bgCol, FCol fColor, QFont::Weight w = QFont::Weight::Normal, int sizeFont = -1) -> QString;
    static auto qlabel(OName oName, BGCol bgColor, FCol fColor, QFont::Weight w = QFont::Weight::Normal, int sizeFont = -1) -> QString;
    static auto qcheck_box(OName oName, FCol fCol, BGCol bgCol) -> QString;
    static auto qspin_box(OName oName, BGCol bgCol) -> QString;
    static auto qdoublespin_box(OName oName, BGCol bgCol) -> QString;
    static auto qabstract_item_view(OName oName, BGCol bgCol) -> QString;
    static auto qcombo_box(OName oName, BGCol bgCol) -> QString;
    static auto qtext_edit(OName oName, FCol fCol, BGCol bgCol) -> QString;
    static auto qline_edit(OName oName, FCol fCol, BGCol bgCol, QFont::Weight w = QFont::Weight::Normal, int sizeFont = -1) -> QString;
    static auto qtable_widget(OName oName, BGCol bCol) -> QString;
    static auto qheader_view(OName oName, BGCol bgCol, FCol fCol, QFont::Weight weight = QFont::Weight:: Bold) -> QString;
    static auto qline(BName bName, BDCol bdColor) -> QString;
    static auto qtab_widget(OName oName, BGCol bgColor) -> QString;
    static auto qlist_widget(OName oName, BGCol bColor, BGCol bItemColor, BGCol bSelectedItemColor) -> QString;
    static auto qtoolbar(OName oName, BGCol bgColor) -> QString;
    static auto qtoolbutton(OName oName, BGCol bgColor) -> QString;

    static auto qpush_button(
        OName oName,
        BGCol bgColor, BGCol bgHoverColor, BGCol bgPressedColor,
        BDCol bdColor, BDCol bdHoverColor, BDCol bdPressedColor,
        FCol fCol,     FCol fHoverCol,     FCol fPressedCol,
        QFont::Weight w = QFont::Weight::Normal, int sizeFont = -1) -> QString;

    static auto qtabbar(
        OName oName,
        BGCol bgColor, FCol fColor,
        BGCol bgHoverColor, FCol fHoverColor,
        BGCol bgSelectedColor, FCol fSelectedColor,
        BGCol bgSelectedHoverColor, FCol fSelectedHoverColor) -> QString;

    static auto qtext_browser(OName oName, BGCol bgColor, FCol fColor, QFont::Weight w = QFont::Weight::Normal, int sizeFont = -1) -> QString;

    static auto qdock_widget(
        OName oName,
        BGCol bgColor,
        FCol fColor,
        QFont::Weight w = QFont::Weight::Normal, int sizeFont = -1) -> QString;


    static auto qmenu(OName oName, FCol fColor, FCol fItemColor, FCol fSelectedItemColor, BGCol bgColor, BGCol bgItemColor, BGCol bgSelectedItemColor, int sizeFont) -> QString;


    static inline auto menu(Theme theme) -> QString{
        switch (theme) {
        case Theme::Light:
            return {};
        case Theme::Dark:
            return qmenu(OName{""}, FCol{""}, FCol{C::whiteH}, FCol{C::blackH}, BGCol{C::darkGrayH}, BGCol{C::darkGrayH}, BGCol{C::whiteH}, 12);
        }
        return {};
    }

    static inline auto toolbar(Theme theme) -> QString{
        switch (theme) {
        case Theme::Light:
            return {};
        case Theme::Dark:
            return qtoolbar(OName{""}, BGCol{C::darkGrayH}) % qtoolbutton(OName{""}, BGCol{C::lightGrayH});
        }
        return {};
    }

    static inline auto logger(Theme theme, QFont::Weight w = QFont::Weight::Normal, int sizeFont = 11) -> QString{
        switch (theme) {
        case Theme::Light:
            return qtext_browser(OName{""}, BGCol{C::whiteH}, FCol{C::blackH}, w, sizeFont);
        case Theme::Dark:
            return qtext_browser(OName{""}, BGCol{C::darkGrayH}, FCol{C::whiteH}, w, sizeFont);
        }
        return {};
    }

    static inline auto global(Theme theme) -> QString{
        auto n = OName{""};
        auto bg = BGCol{C::darkGrayH};
        switch (theme) {
        case Theme::Light:
            return {};
        case Theme::Dark:
            return
                qwidget(n, bg) %
                qlabel(n, bg, FCol{C::whiteH}) %
                qcombo_box(n, bg);
        }
        return {};
    }

};
}
