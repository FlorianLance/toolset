
# /*******************************************************************************
# ** Toolset-ts-qt-app                                                          **
# ** MIT License                                                                **
# ** Copyright (c) [2018] [Florian Lance]                                       **
# **                                                                            **
# ** Permission is hereby granted, free of charge, to any person obtaining a    **
# ** copy of this software and associated documentation files (the "Software"), **
# ** to deal in the Software without restriction, including without limitation  **
# ** the rights to use, copy, modify, merge, publish, distribute, sublicense,   **
# ** and/or sell copies of the Software, and to permit persons to whom the      **
# ** Software is furnished to do so, subject to the following conditions:       **
# **                                                                            **
# ** The above copyright notice and this permission notice shall be included in **
# ** all copies or substantial portions of the Software.                        **
# **                                                                            **
# ** THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR **
# ** IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,   **
# ** FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL    **
# ** THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER **
# ** LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING    **
# ** FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER        **
# ** DEALINGS IN THE SOFTWARE.                                                  **
# **                                                                            **
# ********************************************************************************/

####################################### repo
TOOLSET_REPOSITORY_DIR      = $$PWD"/../.."

####################################### TARGET/TEMPMATE/CONFIG
TARGET = ts-qt-app
TEMPLATE = app
CONFIG += console
CONFIG += qt

QT += core gui opengl widgets printsupport network
DEFINES += QWT_DLL

win32-msvc*: DEFINES += _USE_MATH_DEFINES
win32-msvc*: DEFINES += NOMINMAX
DEFINES += JKQTCOMMON_LIB_IN_DLL JKQTFASTPLOTTER_LIB_IN_DLL JKQTMATHTEXT_LIB_IN_DLL JKQTPLOTTER_LIB_IN_DLL

####################################### PRI
include(../ts-settings.pri)
include(../ts-projects.pri)
include(../ts-thirdparty.pri)
include(../ts-dependencies.pri)

####################################### INCLUDES

TS_QT_APP_DEP_INCLUDEPATH =\
    #
    $$TS_QT_APP_THIRDPARTY_INCLUDES\
    $$TS_QT_GL_THIRDPARTY_INCLUDES\
    $$TS_QT_THIRDPARTY_INCLUDES\
    $$TS_IMGUI_GL_ENGINE_THIRDPARTY_INCLUDES\
    $$TS_BASE_THIRDPARTY_INCLUDES\
    $$TS_MESH_THIRDPARTY_INCLUDES\
    $$TS_OPENGL_THIRDPARTY_INCLUDES\
    #
    $$TS_QT_GL_INCLUDES\
    $$TS_QT_INCLUDES\
    $$TS_BASE_INCLUDES\
    $$TS_OPENGL_INCLUDES\
    $$TS_MESH_INCLUDES\

####################################### LIBS

TS_QT_APP_DEP_LIBS =\
    #
    $$TS_QT_APP_THIRDPARTY_LIBS\
    $$TS_QT_GL_THIRDPARTY_LIBS\
    $$TS_QT_THIRDPARTY_LIBS\
    $$TS_IMGUI_GL_ENGINE_THIRDPARTY_LIBS\
    $$TS_BASE_THIRDPARTY_LIBS\
    $$TS_MESH_THIRDPARTY_LIBS\
    $$TS_OPENGL_THIRDPARTY_LIBS\
    #
    $$TS_QT_GL_LIB\
    $$TS_QT_LIB\
    $$TS_BASE_LIB\
    $$TS_OPENGL_LIB\
    $$TS_MESH_LIB\

####################################### DEP

TS_QT_APP_PRE_TARGETDEPS =\
    $$TS_QT_GL_LIB_FILE\
    $$TS_QT_LIB_FILE\
    $$TS_BASE_LIB_FILE\
    $$TS_OPENGL_LIB_FILE\
    $$TS_MESH_LIB_FILE\

####################################### GENERATE VARIABLES

include(../ts-gen-var.pri)

####################################### PROJECT FILES
HEADERS += \
    __JKQtPlotterTest.hpp \
    lines_counter.hpp

SOURCES += \
    __JKQtPlotterTest.cpp \
    qt_utility_main.cpp \



