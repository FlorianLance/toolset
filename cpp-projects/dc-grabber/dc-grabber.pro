
# /*******************************************************************************
# ** Toolset-dc-grabber                                                  **
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

####################################### TARGET
equals(CFG, "debug"){
    TARGET = dc-grabberd
}
equals(CFG, "release"){
    TARGET = dc-grabber
}

####################################### TEMPLATE
TEMPLATE = app
CONFIG += console
CONFIG += opengl
CONFIG -= qt

####################################### BUILD FILES
OBJECTS_DIR = $$DC_GRABBER_OBJ
DESTDIR     = $$DC_GRABBER_DEST

####################################### INCLUDES

INCLUDEPATH += \
    # tool
    $$BASE_INCLUDES \
    $$OPENGL_UTILITY_INCLUDES \
    $$3D_ENGINE_INCLUDES \
    # third party
    $$KINECT4_INCLUDES\
    $$OPENCV_INCLUDES\
    $$SFML_INCLUDES\
    $$GLEW_INCLUDES\
    $$BOOST_INCLUDES\
    $$TURBOJPG_INCLUDES\
    $$FASTPFOR_INCLUDES\
    $$ASSIMP_INCLUDES\
    $$GLM_INCLUDES\
    $$LIBSOUNDIO_INCLUDES \
    $$LIBUSB_INCLUDES \
    $$EIGEN_INCLUDES\
    $$OPEN3D_INCLUDES \
    $$TURBOPFOR_INCLUDES \
    $$ORBBEC_INCLUDES \
    $$OPENCV_INCLUDES \

####################################### LIBRAIRIES

PRE_TARGETDEPS += \
    # tool
    $$BASE_LIB_DEP \
    $$OPENGL_UTILITY_LIB_DEP \
    $$3D_ENGINE_LIB_DEP \

LIBS +=  \
    # tool
    $$BASE_LIB \
    $$OPENGL_UTILITY_LIB \
    $$3D_ENGINE_LIB \
    # thirdparty
    $$WINDOWS_LIBS \
    $$GLEW_LIBS \
    $$SFML_LIBS \
    $$ASSIMP_LIBS \
    $$TURBOJPG_LIBS \
    $$FASTPFOR_LIBS \
    $$KINECT4_LIBS \
    $$BOOST_LIBS \
    $$LIBSOUNDIO_LIBS \
    $$LIBUSB_LIBS \
    $$EIGEN_LIBS\
    $$OPEN3D_LIBS \
    $$TURBOPFOR_LIBS \
    $$ORBBEC_LIBS \
    $$OPENCV_LIBS \

####################################### PROJECT FILES

HEADERS += \
    dcg_model.hpp \
    dcg_view.hpp \
    dcg_controller.hpp \
    dcg_signals.hpp \
    # data    
    data/dcg_paths.hpp \
    data/dcg_settings.hpp \
    data/dcg_ui_settings.hpp \
    # ui
    ui/dcg_main_w.hpp \
    ui/dcg_gl_window.hpp \
    ui/dcg_left_panel_child_drawer.hpp \
    ui/dcg_main_menu_bar_drawer.hpp \

SOURCES += \    
    dcg_main.cpp \
    dcg_controller.cpp \
    dcg_model.cpp \
    dcg_signals.cpp \
    dcg_view.cpp \
    # data
    data/dcg_settings.cpp \
    # ui    
    ui/dcg_main_w.cpp \
    ui/dcg_gl_window.cpp \
    ui/dcg_left_panel_child_drawer.cpp \
    ui/dcg_main_menu_bar_drawer.cpp \
    ui/imgui_demo.cpp \

DISTFILES += \
    ../_build/bin/dc-grabber/resources/shaders/cloud.fs \
    ../_build/bin/dc-grabber/resources/shaders/cloud.vs \
    ../_build/bin/dc-grabber/resources/shaders/solid.fs \
    ../_build/bin/dc-grabber/resources/shaders/solid.vs \
    ../_build/bin/dc-grabber/resources/shaders/voxelcloud.fs \
    ../_build/bin/dc-grabber/resources/shaders/voxelcloud.gs \
    ../_build/bin/dc-grabber/resources/shaders/voxelcloud.vs

