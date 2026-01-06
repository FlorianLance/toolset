
# /*******************************************************************************
# ** Toolset-ts-global-app                                                      **
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


####################################### REPOSITORY
TOOLSET_REPOSITORY_DIR      = $$PWD"/../.."

####################################### TARGET/TEMPMATE/CONFIG
TARGET = ts-global-app
TEMPLATE = app
CONFIG += console
CONFIG -= qt

####################################### PRI
include(../ts-settings.pri)
include(../ts-projects.pri)
include(../ts-thirdparty.pri)
include(../ts-dependencies.pri)

####################################### INCLUDES

TS_GLOBAL_APP_DEP_INCLUDEPATH =\
    #
    $$TS_GLOBAL_APP_THIRDPARTY_INCLUDES\
    $$TS_BASE_THIRDPARTY_INCLUDES\
    $$TS_DATA_THIRDPARTY_INCLUDES\
    $$TS_NETWORK_THIRDPARTY_INCLUDES\
    $$TS_MESH_THIRDPARTY_INCLUDES\
    $$TS_DEPTH_CAMERA_THIRDPARTY_INCLUDES\
    #
    $$TS_BASE_INCLUDES\
    $$TS_MESH_INCLUDES\
    $$TS_DATA_INCLUDES\
    $$TS_NETWORK_INCLUDES\
    $$TS_DEPTH_CAMERA_INCLUDES\

####################################### LIBS

TS_GLOBAL_APP_DEP_LIBS =\
    #
    $$TS_GLOBAL_APP_THIRDPARTY_LIBS\
    $$TS_BASE_THIRDPARTY_LIBS\
    $$TS_DATA_THIRDPARTY_LIBS\
    $$TS_NETWORK_THIRDPARTY_LIBS\
    $$TS_MESH_THIRDPARTY_LIBS\
    $$TS_DEPTH_CAMERA_THIRDPARTY_LIBS\
    #
    $$TS_BASE_LIB\
    $$TS_DATA_LIB\
    $$TS_NETWORK_LIB\
    $$TS_MESH_LIB\
    $$TS_DEPTH_CAMERA_LIB\

####################################### DEP

TS_GLOBAL_APP_PRE_TARGETDEPS =\
    $$TS_BASE_LIB_FILE\
    $$TS_DATA_LIB_FILE\
    $$TS_NETWORK_LIB_FILE\
    $$TS_MESH_LIB_FILE\
    $$TS_DEPTH_CAMERA_LIB_FILE\

####################################### GENERATE VARIABLES

include(../ts-gen-var.pri)

####################################### PROJECT FILES
HEADERS += \
    ob_utility.hpp \    
    tests.hpp \

SOURCES += \
    # main
    ts_global_app_main.cpp \
    test_bdf.cpp \
    test_dc_settings.cpp \
    test_encoding.cpp \
    test_mic.cpp \
    test_ak_device.cpp \
    test_dc_player.cpp \
    test_multi_devices.cpp \
    test_pcl.cpp \
    test_k2_device.cpp \
    test_kvid.cpp \
    test_simd.cpp \
    test_time.cpp \
    test_ob_device.cpp


