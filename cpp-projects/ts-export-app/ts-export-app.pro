
# /*******************************************************************************
# ** Toolset-ts-export-app                                                      **
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
TARGET = ts-export-app
TEMPLATE = app
CONFIG += console
CONFIG -= qt

####################################### PRI
include(../ts-settings.pri)
include(../ts-projects.pri)
include(../ts-thirdparty.pri)
include(../ts-dependencies.pri)

####################################### INCLUDES

TS_EXPORT_APP_DEP_INCLUDEPATH =\
    #
    $$TS_EXPORT_APP_THIRDPARTY_INCLUDES\
    $$TS_EXPORT_THIRDPARTY_INCLUDES\
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
    $$TS_EXPORT_INCLUDES\

####################################### LIBS

TS_EXPORT_APP_DEP_LIBS =\
    #
    $$TS_EXPORT_APP_THIRDPARTY_LIBS\
    $$TS_EXPORT_THIRDPARTY_LIBS\
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
    $$TS_EXPORT_OBJ"\*.obj"\

####################################### DEP

TS_EXPORT_APP_PRE_TARGETDEPS =\
    $$TS_EXPORT_PRE_TARGETDEPS\

####################################### GENERATE VARIABLES

include(../ts-gen-var.pri)

######################################## PROJECT FILES
HEADERS += \

SOURCES += \
    ts_export_app_main.cpp\

