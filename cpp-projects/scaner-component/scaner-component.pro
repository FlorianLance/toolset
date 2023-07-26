
# /*******************************************************************************
# ** Toolset-scaner-component                                                   **
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
TOOLBOX_REPOSITORY_DIR      = $$PWD"/../.."

####################################### PRI
# defines compiling options
include(../ts-settings.pri)
# defines projects paths and variables
include(../ts-projects.pri)
# defines thirdparty includes and libs
include(../ts-thirdparty.pri)

####################################### TARGET
equals(CFG, "debug"){
    TARGET = scaner-componentd
}
equals(CFG, "release"){
    TARGET = scaner-component
}

####################################### TEMPLATE
TEMPLATE = lib
CONFIG += staticlib
CONFIG -= console

####################################### BUILD FILES
OBJECTS_DIR = $$SCANER_COMPONENT_OBJ
DESTDIR     = $$SCANER_COMPONENT_DEST

####################################### CONFIG
CONFIG -= qt

####################################### INCLUDES
INCLUDEPATH += \
    # base
    $$BASE_INCLUDES \
    $$KINECT2_INCLUDES \
    $$BOOST_INCLUDES \
    $$TURBOJPG_INCLUDES \

####################################### LIBRAIRIES

PRE_TARGETDEPS += \
    # tool
    $$BASE_LIB_DEP \

LIBS +=  \
    # tool
    $$BASE_LIB\
    # third party
    $$KINECT2_LIBS \
    $$BOOST_LIBS \
    $$TURBOJPG_LIBS\
    $$FASTPFOR_LIBS\

####################################### PROJECT FILES

HEADERS += \
    grabber_controller.hpp \
    network/scaner_tcp_sender.hpp \
    network/scaner_udp_reader.hpp \
    process_data.hpp \
    network/frame_reader.hpp \
    network/udp_reader_manager.hpp \

SOURCES += \
    export_main_test.cpp \
    grabber_controller.cpp \
    network/scaner_tcp_sender.cpp \
    network/scaner_udp_reader.cpp \
    process_data.cpp \
    network/frame_reader.cpp \
    network/udp_reader_manager.cpp \


DISTFILES += \
