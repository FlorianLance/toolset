
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
TOOLSET_REPOSITORY_DIR      = $$PWD"/../.."

####################################### PRI
# defines compiling options
include(../ts-settings.pri)
# defines projects paths and variables
include(../ts-projects.pri)
# defines thirdparty includes and libs
include(../ts-thirdparty.pri)
# defines dependencies
include(../ts-dependencies.pri)

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

####################################### DEPS
INCLUDEPATH += $$SCANER_COMPONENT_DEP_INCLUDEPATH
LIBS +=  $$SCANER_COMPONENT_DEP_LIBS
PRE_TARGETDEPS += $$SCANER_COMPONENT_PRE_TARGETDEPS

####################################### PROJECT FILES

HEADERS += \
    k2_grabber_controller.hpp \
    k2_process_data.hpp \
    network/k2_frame_reader.hpp \
    network/k2_scaner_tcp_sender.hpp \
    network/k2_scaner_udp_reader.hpp \
    network/k2_udp_reader_manager.hpp \

SOURCES += \
    export_main_test.cpp \
    k2_grabber_controller.cpp \
    k2_process_data.cpp \
    network/k2_frame_reader.cpp \
    network/k2_scaner_tcp_sender.cpp \
    network/k2_scaner_udp_reader.cpp \
    network/k2_udp_reader_manager.cpp \


DISTFILES += \
