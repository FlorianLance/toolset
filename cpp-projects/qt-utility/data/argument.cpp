
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

#include "argument.hpp"

using namespace tool::str;
using namespace tool::ex;

std::vector<Arg> Arg::split_value_to_atoms_args() const{

    auto list = split_value();
    std::vector<Arg> args;
    for(auto &v : list){
        Arg arg;
        arg.name           = name; // ?
        arg.m_unityType    = m_unityType;
        arg.m_value        = v;
        arg.m_separator    = "";
        arg.m_sizes        = {};

        arg.generator = generator;
        args.emplace_back(std::move(arg));
    }
    return args;
}

std::array<Arg, 3> Arg::to_transform_args() const{

    auto l = split_value();
    std::array<Arg, 3> args;
    for(auto &arg : args){
        arg.m_separator       = m_separator;
        arg.m_sizes           = {3};
    }
    args[0].m_value = l[0] + m_separator + l[1] + m_separator + l[2];
    args[1].m_value = l[3] + m_separator + l[4] + m_separator + l[5];
    args[2].m_value = l[6] + m_separator + l[7] + m_separator + l[8];

    return args;
}

void Arg::init_from_unknow(QString v){
    m_unityType    = UnityType::Unknow;
    m_separator    = "";
    m_value        = Convertor::to_str(v);
    m_sizes        = {};
}

void Arg::init_from(QString v){
    m_unityType = UnityType::System_string;
    m_separator = "";
    m_value     = Convertor::to_str(v);
    m_sizes     = {};
}

void Arg::init_from(QStringList list, QString sep){
    m_unityType = UnityType::System_string;
    m_separator = sep;
    m_value     = Convertor::to_str(list,m_separator);
    m_sizes     = {static_cast<int>(list.length())};
}

void Arg::init_from_bool_str(QString v){
    m_unityType = UnityType::System_boolean;
    m_separator = "";
    m_value     = v;
    m_sizes     = {};
}

void Arg::init_from_int_str(QString v){
    m_unityType = UnityType::System_int32;
    m_separator = "";
    m_value     = v;
    m_sizes     = {};
}

void Arg::init_from_float_str(QString v){
    m_unityType = UnityType::System_single;
    m_separator = "";
    m_value     = v;
    m_sizes     = {};
}

void Arg::init_from_double_str(QString v){
    m_unityType = UnityType::System_double;
    m_separator = "";
    m_value     = v;
    m_sizes     = {};
}

void Arg::init_from_vector3_str(QString v){
    m_unityType = UnityType::System_single;
    m_separator = " ";
    m_value     = v;
    m_sizes     = {3};
}

void Arg::init_from_color_str(QString v){
    m_unityType = UnityType::System_single;
    m_separator = " ";
    m_value     = v;
    m_sizes     = {4};
}

void Arg::init_from_curve_str(QString v, QString sep){
    m_unityType = UnityType::System_single;
    m_separator = sep;
    m_value     = v;
    m_sizes     = {static_cast<int>(v.count(m_separator) + 1)};
}

void Arg::init_from_transform_str(QString v){
    m_unityType = UnityType::System_single;
    m_separator = " ";
    m_value     = v;
    m_sizes     = {9};
}

void Arg::init_from_transform_args(Arg tr, Arg rot, Arg sc){

    m_unityType = UnityType::System_single;
    m_separator = " ";    
    m_value     = Convertor::to_str({tr.value(),rot.value(), sc.value()}, m_separator);
    m_sizes     = {9};
}


void Arg::init_from_args(std::vector<Arg> args, QString sep, UnityType uType){

    m_unityType = uType;
    m_separator = sep;
    m_value     = "";

    QStringList values;
    values.reserve(args.size());
    m_sizes = {static_cast<int>(args.size())};
    for(auto &arg : args){
        values << arg.value();
    }
    m_value = Convertor::to_str(values, m_separator);
}

Arg Arg::generate_item_ui_arg(QString name, UiType associatedUiType, bool hasGenerator, int generatorOrder){
    Arg arg;
    arg.name = name;
    if(hasGenerator){
        arg.generator = Generator(generatorOrder);
    }
    arg.m_associatedUiType = associatedUiType;
    return arg;
}

Arg Arg::generate_init_ui_arg(UiType uiType, QString name, int order){
    Arg arg;
    arg.generator = Generator(order);
    arg.m_associatedUiType = uiType;
    arg.name               = name;
    return arg;
}

Arg Arg::generate_from_loaded_xml_values(UiType uiType, QString name, QString value, QString separator, std::vector<int> sizes, UnityType type){
    Arg arg;
    arg.m_associatedUiType = uiType;
    arg.name               = name;
    arg.m_value            = value;
    arg.m_separator        = separator;
    arg.m_sizes            = sizes;
    arg.m_unityType        = type;
    return arg;
}

Arg Arg::generate_non_ui_arg(QString value, QString separator, QString name){
    Arg arg;
    arg.m_associatedUiType = UiType::Non_ui_labels;
    arg.name               = name;
    arg.m_value            = value;
    arg.m_separator        = separator;
    arg.m_sizes            = {static_cast<int>(value.count(separator)+1)};
    arg.m_unityType        = UnityType::System_string;
    return arg;
}

Arg Arg::copy(const Arg &argToCopy){
    Arg arg;
    arg.name               = argToCopy.name;
    arg.m_associatedUiType = argToCopy.m_associatedUiType;
    arg.m_value            = argToCopy.m_value;
    arg.m_separator        = argToCopy.m_separator;
    arg.m_sizes            = argToCopy.m_sizes;
    arg.generator          = argToCopy.generator;
    arg.m_unityType        = argToCopy.m_unityType;    
    return arg;
}


