
# /*******************************************************************************
# ** Toolset-scaner-manager                                                     **
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
    TARGET = scaner-managerd
}
equals(CFG, "release"){
    TARGET = scaner-manager
}

####################################### TEMPLATE
TEMPLATE = app
CONFIG += console

####################################### BUILD FILES
OBJECTS_DIR = $$SCANER_MANAGER_OBJ
MOC_DIR     = $$SCANER_MANAGER_MOC
RCC_DIR     = $$SCANER_MANAGER_RCC
UI_DIR      = $$SCANER_MANAGER_UI
DESTDIR     = $$SCANER_MANAGER_DEST

####################################### CONFIG
QT  += core gui network widgets

####################################### DEPS
INCLUDEPATH += $$SCANER_MANAGER_DEP_INCLUDEPATH
LIBS +=  $$SCANER_MANAGER_APP_DEP_LIBS
PRE_TARGETDEPS += $$SCANER_MANAGER_APP_PRE_TARGETDEPS

####################################### PROJECT FILES

HEADERS += \
    # managers
    managers/calibration_manager.hpp \
    managers/grabber_controller.hpp \
    managers/udp_reader_manager.hpp \
    # workers
    widgets/scaner_manager_window.h \
    workers/frame_reader_worker.hpp \
    workers/interface_udp_receiver_worker.hpp \
    workers/manager_process_data_worker.hpp \
    workers/tcp_sender_worker.hpp \
    # widgets
    widgets/multi_depth_camera_gl_widget.hpp

SOURCES += \
    # managers
    managers/calibration_manager.cpp \
    managers/grabber_controller.cpp \
    managers/udp_reader_manager.cpp \
    # workers    
    scaner_manager_main.cpp \
    widgets/scaner_manager_window.cpp \
    workers/frame_reader_worker.cpp \
    workers/interface_udp_receiver_worker.cpp \
    workers/manager_process_data_worker.cpp \
    workers/tcp_sender_worker.cpp \
    # widgets
    widgets/multi_depth_camera_gl_widget.cpp \
    # main

FORMS += \
    ui/ManagerW.ui \

RESOURCES += \
    $$TOOLSET_CPP_RESOURCES_DIR"/resources.qrc" \
