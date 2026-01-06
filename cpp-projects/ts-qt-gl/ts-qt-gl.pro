
# /*******************************************************************************
# ** Toolset-ts-qt-gl                                                           **
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
TARGET = ts-qt-gl
TEMPLATE = lib
CONFIG += staticlib
CONFIG -= console
CONFIG += qt
QT += core gui opengl widgets printsupport network
DEFINES += QWT_DLL

####################################### PRI
include(../ts-settings.pri)
include(../ts-projects.pri)
include(../ts-thirdparty.pri)
include(../ts-dependencies.pri)

####################################### INCLUDES

TS_QT_GL_DEP_INCLUDEPATH =\
    #
    $$TS_QT_GL_THIRDPARTY_INCLUDES\
    $$TS_QT_THIRDPARTY_INCLUDES\
    $$TS_IMGUI_GL_ENGINE_THIRDPARTY_INCLUDES\
    $$TS_BASE_THIRDPARTY_INCLUDES\
    $$TS_MESH_THIRDPARTY_INCLUDES\
    $$TS_OPENGL_THIRDPARTY_INCLUDES\
    #
    $$TS_QT_INCLUDES\
    $$TS_BASE_INCLUDES\
    $$TS_OPENGL_INCLUDES\
    $$TS_MESH_INCLUDES\

####################################### LIBS

TS_QT_GL_DEP_LIBS =\
    #
    $$TS_QT_GL_THIRDPARTY_LIBS\
    $$TS_QT_THIRDPARTY_LIBS\
    $$TS_IMGUI_GL_ENGINE_THIRDPARTY_LIBS\
    $$TS_BASE_THIRDPARTY_LIBS\
    $$TS_MESH_THIRDPARTY_LIBS\
    $$TS_OPENGL_THIRDPARTY_LIBS\
    #
    $$TS_QT_LIB\
    $$TS_BASE_LIB\
    $$TS_OPENGL_LIB\
    $$TS_MESH_LIB\

####################################### DEP

TS_QT_GL_PRE_TARGETDEPS =\
    $$TS_QT_LIB_FILE\
    $$TS_BASE_LIB_FILE\
    $$TS_OPENGL_LIB_FILE\
    $$TS_MESH_LIB_FILE\

####################################### GENERATE VARIABLES

include(../ts-gen-var.pri)

####################################### PROJECT FILES
HEADERS += \
    # global    
    gui/widgets/qt_base_sfml_gl_w.hpp \
    wrapper/qt_gl_wrapper.hpp \

SOURCES += \
    gui/widgets/qt_base_sfml_gl_w.cpp \

RESOURCES += \
    $$TOOLSET_CPP_RESOURCES_DIR"/resources.qrc" \

FORMS += \



