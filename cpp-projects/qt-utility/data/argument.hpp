
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

// std
#include <optional>

// Qt
#include <QVector>

// local
#include "qt_ui.hpp"
#include "id_key.hpp"
#include "ex_widgets_types.hpp"
#include "unity_types.hpp"
#include "qt_convertors.hpp"

namespace tool::ex {

struct Arg;

struct Generator{

    Generator() = default;
    Generator(int order) : order(order){}

    int order = -1;
    std::optional<QString> info     = std::nullopt;
    std::optional<QString> decimals = std::nullopt;
    std::optional<QString> min      = std::nullopt;
    std::optional<QString> max      = std::nullopt;
    std::optional<QString> step     = std::nullopt;
};

struct Arg{

    // generate
    static Arg generate_item_ui_arg(QString name, UiType associatedUiType, bool hasGenerator, int generatorOrder);
    static Arg generate_init_ui_arg(UiType uiType, QString name, int order);
    static Arg generate_from_loaded_xml_values(UiType uiType, QString name, QString value, QString separator, std::vector<int> sizes, UnityType type);
    static Arg generate_non_ui_arg(QString value, QString separator, QString name);

    // copy
    static Arg copy(const Arg &argToCopy);

    inline int count() const {return m_value.count(m_separator)+1;}

    // to value
    inline bool to_bool_value() const{return str::Convertor::to_bool(m_value);}
    inline int to_int_value() const{return str::Convertor::to_int(m_value);}
    inline float to_float_value() const{return str::Convertor::to_float(m_value);}
    inline double to_double_value() const{return str::Convertor::to_double(m_value);}
    inline QColor to_color_value() const{return str::Convertor::to_color(m_value);}
    inline QString to_string_value() const{return str::Convertor::to_str(m_value);}
    inline std::pair<std::vector<double>, std::vector<double>> to_curve_value() const{ return str::Convertor::to_curve(m_value, m_separator);}
    inline std::vector<std::pair<std::vector<double>, std::vector<double>>> to_curves_value() const {return str::Convertor::to_curves(m_value, "*", m_separator);}

    inline QStringList split_value() const{return m_value.split(m_separator);}
    inline QStringList split_value(QString sep) const{return m_value.split(sep);}

    // to args
    std::vector<Arg> split_value_to_atoms_args() const;
    std::array<Arg,3> to_transform_args() const;

    // from values
    inline void init_from(bool v){init_from_bool_str(str::Convertor::to_str(v));}
    inline void init_from(int v){init_from_int_str(str::Convertor::to_str(v));}
    inline void init_from(float v){init_from_float_str(str::Convertor::to_str(v));}
    inline void init_from(double v){init_from_double_str(str::Convertor::to_str(v));}
    inline void init_from(QColor v){init_from_color_str(str::Convertor::to_str(v));}
    inline void init_from_curve(std::vector<double> *xCoords, std::vector<double> *yCoords, QString sep){
        init_from_curve_str(str::Convertor::curve_to_str(xCoords,yCoords,sep), sep);
    }

    void init_from_unknow(QString v);
    void init_from(QString v);
    void init_from(QStringList list, QString sep = "|");

    // from string
    void init_from_bool_str(QString v);
    void init_from_int_str(QString v);
    void init_from_float_str(QString v);
    void init_from_double_str(QString v);
    void init_from_vector3_str(QString v);
    void init_from_color_str(QString v);
    void init_from_curve_str(QString v, QString sep);
    void init_from_transform_str(QString v);

    // from args
    void init_from_transform_args(Arg tr, Arg rot, Arg sc);
    void init_from_args(std::vector<Arg> args, QString sep, UnityType uType = UnityType::Mixed);


    inline QString value() const{return m_value;}
    inline QString separator() const{return m_separator;}
    inline int dimensions_nb() const {return static_cast<int>(m_sizes.size());}
    inline int size_dimension(int id)const{return m_sizes[id];}
    inline UiType associated_ui_type() const {return m_associatedUiType;}
    inline UnityType unity_type() const {return m_unityType;}

    inline void set_value(QString value){m_value = value;}
    inline void set_separator(QString sep){m_separator = sep;}

    QString name; // only updated when reading

    // generator
    std::optional<Generator> generator = {};

private:

    UiType m_associatedUiType   = UiType::Unspecified;
    QString m_value             = "";
    QString m_separator         = "";
    std::vector<int> m_sizes    = {};
    UnityType m_unityType       = UnityType::Unknow;
};

}
