
/*******************************************************************************
** Toolset-ts-qt                                                              **
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

#include "id_key.hpp"

// local
#include <qt_logger.hpp>

using namespace tool::ex;
using namespace Qt::Literals::StringLiterals;

IdKey::IdKey(IdKey::Type type, int id) : m_type(type){

    if(m_type == Type::NotUsed){
        return;
    }

    auto sId = static_cast<int>(m_source);

    if(id == -1){
        m_id = m_currentId[m_type][sId];
        m_currentId[m_type][sId]++;
    }else{
        m_id = id;
        if(m_id >= m_currentId[m_type][sId]){
            m_currentId[m_type][sId] = m_id + 1;
        }
    }

    if(m_keys[m_type][sId].count(m_id) == 0){
        m_keys[m_type][sId].insert(m_id);
    }else{

        if(m_type != Type::FlowElement && m_type != Type::ButtonFlowElement){

            int idToTest = 0;
            while(m_keys[m_type][sId].count(idToTest) == 0){
                ++idToTest;
            }
            m_keys[m_type][sId].insert(m_id = idToTest);
        }
    }

}

IdKey::~IdKey(){

    if(m_type == Type::NotUsed){
        return;
    }

    auto sId = static_cast<int>(m_source);


    if(m_type != Type::FlowElement && m_type != Type::ButtonFlowElement){
        if(m_keys[m_type][sId].contains(m_id)){
            m_keys[m_type][sId].erase(m_id);
        }else{
            QtLog::error(u"Id [%1] of type [%2] cannot be removed from set."_s.arg(QString::number(m_id), from_view(type_name())));
        }
    }
}

void IdKey::reset(){

    auto sId = static_cast<int>(m_source);

    for (auto& p : m_types.data) {
        m_currentId[std::get<0>(p)][sId] = 0;
    }

    for (auto& p : m_types.data) {
        m_keys[std::get<0>(p)][sId].clear();
    }
}

void IdKey::debug_display(){
    QtLog::message(QSL("[START ################# IdKeys::debug_display]"));
    QtLog::message(QSL("Current id:"));
    for(const auto &pId : m_currentId){
        QtLog::message(u"Type: [%1] Id: [c:%2|i:%2]"_s.arg(from_view(to_string(pId.first)), QString::number(pId.second[0]), QString::number(pId.second[1])));
    }
    QtLog::message(u"Set size:"_s);
    for(const auto &pId : m_keys){

        int maxC = -10;
        int minC = 100000000;
        int maxI = -10;
        int minI = 100000000;
        for(const auto &val : pId.second[0]){
            if(val < minC){
                minC = val;
            }else if (val > maxC){
                maxC = val;
            }
        }
        for(const auto &val : pId.second[1]){
            if(val < minI){
                minI = val;
            }else if (val > maxI){
                maxI = val;
            }
        }
        QtLog::message(u"Type: [%1] Size: [c:%2|i:] Min: [c:%3|i:%4] Max: [c:|i:%5]"_s.arg(
            from_view(to_string(pId.first)),
            QString::number(pId.second[0].size()),QString::number(minC), QString::number(minI), QString::number(maxC), QString::number(maxI)));
    }
    QtLog::message(u"[END ################# IdKeys::debug_display]"_s);
}

constexpr IdKey::TypeStr IdKey::type_name() const {
    return to_string(m_type);
}
