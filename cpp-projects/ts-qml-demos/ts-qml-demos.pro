

# /*******************************************************************************
# ** ts-qml-demos                                                               **
# ** MIT License                                                                **
# ** Copyright (c) [2026] [Florian Lance]                                       **
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
TARGET = ts-qml-demos
TEMPLATE = app
QT += qml quick quickcontrols2 core gui widgets opengl
CONFIG += qtquickcompiler console

#QML_IMPORT_PATH = $$PWD/qml $$PWD/qml/settings $$PWD/qml/utility

####################################### PRI
include(../ts-settings.pri)
include(../ts-projects.pri)
include(../ts-thirdparty.pri)
include(../ts-dependencies.pri)

RESOURCES += \
    resources.qrc

####################################### INCLUDES

TS_QML_DEMOS_DEP_INCLUDEPATH =\
    #
    $$TS_QML_DEMOS_THIRDPARTY_INCLUDES\
    $$TS_BASE_THIRDPARTY_INCLUDES\
    $$TS_QT_THIRDPARTY_INCLUDES\
    $$TS_DEPTH_CAMERA_THIRDPARTY_INCLUDES\
    $$TS_OPENGL_THIRDPARTY_INCLUDES\
    $$TS_MESH_THIRDPARTY_INCLUDES\
    $$TS_NETWORK_THIRDPARTY_INCLUDES\
    $$TS_DATA_THIRDPARTY_INCLUDES\
    #
    $$TS_BASE_INCLUDES\
    $$TS_OPENGL_INCLUDES\
    $$TS_QT_INCLUDES\
    $$TS_NETWORK_INCLUDES\
    $$TS_DEPTH_CAMERA_INCLUDES\
    $$TS_MESH_INCLUDES\
    $$TS_DATA_INCLUDES\

####################################### LIBS

TS_QML_DEMOS_DEP_LIBS =\
    #
    $$TS_QML_DEMOS_THIRDPARTY_LIBS\
    $$TS_BASE_THIRDPARTY_LIBS\
    $$TS_OPENGL_THIRDPARTY_LIBS\
    $$TS_QT_THIRDPARTY_LIBS\
    $$TS_DEPTH_CAMERA_THIRDPARTY_LIBS\
    $$TS_MESH_THIRDPARTY_LIBS\
    $$TS_NETWORK_THIRDPARTY_LIBS\
    $$TS_DATA_THIRDPARTY_LIBS\
    #
    $$TS_BASE_LIB\
    $$TS_OPENGL_LIB\
    $$TS_NETWORK_LIB\
    $$TS_DEPTH_CAMERA_LIB\
    $$TS_DATA_LIB\
    $$TS_MESH_LIB\
    $$TS_QT_LIB\

####################################### DEP

TS_QML_DEMOS_PRE_TARGETDEPS =\
    $$TS_BASE_LIB_FILE\
    $$TS_QT_LIB_FILE\
    $$TS_OPENGL_LIB_FILE\
    $$TS_NETWORK_LIB_FILE\
    $$TS_DEPTH_CAMERA_LIB_FILE\
    $$TS_DATA_LIB_FILE\
    $$TS_MESH_LIB_FILE\

####################################### GENERATE VARIABLES

include(../ts-gen-var.pri)

####################################### PROJECT FILES
HEADERS += \
    items/base_3d_item_viewer.hpp \
    items/sample_3d_item_viewer.hpp \
    samples/base_sample.hpp \
    samples/common.hpp \
    samples/sample_renderer.hpp \
    scenes/base_scene.hpp \
    tqd_controller.hpp

SOURCES += \
    items/base_3d_item_viewer.cpp \
    items/sample_3d_item_viewer.cpp \
    samples/sample_renderer.cpp \
    tqd_controller.cpp \
    tqd_main.cpp


