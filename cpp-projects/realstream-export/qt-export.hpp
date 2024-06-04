
/*******************************************************************************
** RealStream-export                                                          **
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
#include <QCoreApplication>

// base
#include "utility/export.hpp"

struct TestC : public QObject{
    Q_OBJECT
public:
    TestC(){

    }
signals:

    auto increment_signal() -> void;

public slots:

    auto increment() -> void{
        ++m_value;
    }
private:

    int m_value = 0;
};



struct QtApp{

    QtApp(std::string applicationPath);
    ~QtApp();

    auto exec()-> int;

    std::unique_ptr<QCoreApplication> app = nullptr;
    std::string m_applicationPath;
    char *m_applicationPathData = nullptr;
};


extern "C"{

    DECL_EXPORT QtApp* create__qt_app(const char *applicationPath);
    DECL_EXPORT void delete__qt_app(QtApp *qtCoreApp);
    DECL_EXPORT int exec__qt_app(QtApp *qtCoreApp);
    DECL_EXPORT void exit__qt_app(QtApp *qtCoreApp);
}
