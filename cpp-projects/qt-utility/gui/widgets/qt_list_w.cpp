

/*******************************************************************************
** Toolbox-qt-utility                                                         **
** MIT License                                                                **
** Copyright (c) [2025] [Florian Lance]                                       **
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


#include "qt_list_w.hpp"


using namespace tool::ui;

QtListW::QtListW(){
    setWidgetResizable(true);

    m_layout = new QVBoxLayout();
    m_layout->setContentsMargins(2,2,2,2);
    m_layout->addStretch(1);

    m_content = new QWidget();
    m_content->setLayout(m_layout);

    setWidget(m_content);
}

auto QtListW::add(QWidget *w) -> void{

    for(auto cw : m_widgets){
        if(cw == w){
            // error
            return;
        }
    }
    m_widgets.push_back(w);
    if(count() != 0){
        m_layout->insertWidget(count()-1,w);
    }else{
        m_layout->insertWidget(0,w);
    }
}

auto QtListW::get_at(size_t id) -> QWidget*{

    if(id < count()){
        return m_widgets[id];
    }
    // error
    return nullptr;
}

auto QtListW::get_id(QWidget *w) -> std::optional<size_t>{

    for(size_t ii = 0; ii < count(); ++ii){
        if(m_widgets[ii] == w){
            return {ii};
        }
    }
    return {};
}

auto QtListW::take_at(size_t id) -> QWidget*{

    if(id < count()){
        m_widgets.erase(m_widgets.begin() + id, m_widgets.begin() + id + 1);
        return m_layout->takeAt(static_cast<int>(id))->widget();
    }
    // error
    return nullptr;
}

auto QtListW::delete_at(size_t id) -> void{

    if(auto w = take_at(id); w != nullptr){
        delete w;
    }
}

auto QtListW::delete_last() -> void{

    if(count() > 0){
        delete_at(count()-1);
    }
}

auto QtListW::delete_all() -> void{

    while(count() > 0){
        delete_at(0);
    }
}

auto QtListW::count() -> size_t{
    return m_widgets.size();
}
