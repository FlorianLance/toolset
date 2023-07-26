
# /*******************************************************************************
# ** Toolset-scaner-grabber                                                     **
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
    TARGET = scaner-grabberd
}
equals(CFG, "release"){
    TARGET = scaner-grabber
}

####################################### TEMPLATE
TEMPLATE = app
CONFIG += console

####################################### BUILD FILES
OBJECTS_DIR = $$SCANER_GRABBER_OBJ
MOC_DIR     = $$SCANER_GRABBER_MOC
RCC_DIR     = $$SCANER_GRABBER_RCC
UI_DIR      = $$SCANER_GRABBER_UI
DESTDIR     = $$SCANER_GRABBER_DEST

####################################### CONFIG
CONFIG += opengl
QT  += core gui widgets network

####################################### INCLUDES

INCLUDEPATH += \
    # tool
    $$BASE_INCLUDES\
    $$OPENGL_UTILITY_INCLUDES\
    $$QT_UTILITY_INCLUDES \
    $$QT_UTILITY_UI \
    # third party
    $$KINECT2_INCLUDES\
    $$OPENCV_INCLUDES\
    $$SFML_INCLUDES\
    $$GLEW_INCLUDES\
    $$BOOST_INCLUDES\
    $$TURBOJPG_INCLUDES\
    $$FASTPFOR_INCLUDES\
    $$ASSIMP_INCLUDES\
    $$GLM_INCLUDES\

####################################### LIBRAIRIES

PRE_TARGETDEPS += \
    # tool
    $$BASE_LIB_DEP \
    $$OPENGL_UTILITY_LIB_DEP \
    $$QT_UTILITY_LIB_DEP \

LIBS +=  \
    # tool
    $$BASE_LIB\
    $$OPENGL_UTILITY_LIB\
    $$QT_UTILITY_LIB\
    # thirdparty
    $$BOOST_LIBS\
    $$KINECT2_LIBS\
    $$GLEW_LIBS\
    $$OPENCV_LIBS\
    $$SFML_LIBS\
    $$TURBOJPG_LIBS\
    $$FASTPFOR_LIBS\
    $$ASSIMP_LIBS\
    -lopengl32\

####################################### PROJECT FILES

HEADERS += \
    scaner_grabber_window.hpp \
    workers/grabber_process_data_worker.hpp \
    workers/udp_sender_worker.hpp \
    workers/tcp_reader_worker.hpp \
    workers/camera_worker.hpp \
    widgets/depth_camera_gl_widget.hpp \

SOURCES += \    
    scaner_grabber_window.cpp \
    workers/grabber_process_data_worker.cpp \
    workers/udp_sender_worker.cpp \
    workers/tcp_reader_worker.cpp \
    workers/camera_worker.cpp \
    widgets/depth_camera_gl_widget.cpp \

FORMS += \
    ui/GrabberW.ui \
    ui/about_dialog.ui \

RESOURCES += \
    $$TOOLBOX_CPP_RESOURCES_DIR"/resources.qrc" \

DISTFILES += \
    ../resources/shaders/scaner/scanercloud.fs \
    ../resources/shaders/scaner/scanercloud.vs \
    ../resources/shaders/scaner/scanermesh.fs \
    ../resources/shaders/scaner/scanermesh.vs \

