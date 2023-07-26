
# /*******************************************************************************
# ** Toolset-k4-scaner-manager                                                  **
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
    TARGET = k4-scaner-managerd
}
equals(CFG, "release"){
    TARGET = k4-scaner-manager
}

####################################### TEMPLATE
TEMPLATE = app
CONFIG += console
CONFIG += opengl
CONFIG -= qt

####################################### BUILD FILES
OBJECTS_DIR = $$K4_SCANER_MANAGER_OBJ
DESTDIR     = $$K4_SCANER_MANAGER_DEST

####################################### INCLUDES

INCLUDEPATH += \
    # tool
    $$BASE_INCLUDES \
    $$OPENGL_UTILITY_INCLUDES \
    $$3D_ENGINE_INCLUDES \
    # third party
    $$KINECT4_INCLUDES \
    $$OPENCV_INCLUDES \
    $$SFML_INCLUDES \
    $$GLEW_INCLUDES \
    $$BOOST_INCLUDES \
    $$TURBOJPG_INCLUDES \
    $$FASTPFOR_INCLUDES \
    $$ASSIMP_INCLUDES \
    $$GLM_INCLUDES \
    $$LIBSOUNDIO_INCLUDES \
    $$LIBUSB_INCLUDES \
    $$EIGEN_INCLUDES \
    $$OPEN3D_INCLUDES \
    $$TURBOPFOR_INCLUDES \
    $$GLFW_INCLUDES \

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
    $$GLEW_LIBS \
    $$SFML_LIBS \
    $$ASSIMP_LIBS \
    $$TURBOJPG_LIBS \
    $$FASTPFOR_LIBS \
    $$KINECT4_LIBS \
    $$BOOST_LIBS \
    $$LIBSOUNDIO_LIBS \
    $$LIBUSB_LIBS \
    $$EIGEN_LIBS \
    $$OPEN3D_LIBS \
    $$TURBOPFOR_LIBS \
    $$WINDOWS_LIBS \

####################################### PROJECT FILES

HEADERS += \
    k4sm_controller.hpp \
    k4sm_model.hpp \
    k4sm_gl_window.hpp \
    k4sm_signals.hpp \
    # data
    data/k4sm_misc.hpp \
    data/k4sm_paths.hpp \
    data/k4sm_settings.hpp \
    data/k4sm_ui_settings.hpp \
    # drawers
    drawers/k4sm_left_panel_child_drawer.hpp \
    drawers/k4sm_middle_panel_child_drawer.hpp \
    drawers/k4sm_main_window_drawer.hpp \    

SOURCES += \
    k4sm_model.cpp \
    k4sm_signals.cpp \
    k4sm_main.cpp \
    k4sm_controller.cpp \
    k4sm_gl_window.cpp \
    # drawers
    drawers/imgui_demo.cpp \
    drawers/k4sm_main_window_drawer.cpp \
    drawers/k4sm_left_panel_child_drawer.cpp \
    drawers/k4sm_middle_panel_child_drawer.cpp \
    # data
    data/k4sm_settings.cpp \

DISTFILES += \
    ../_build/bin/k4-scaner-manager/resources/shaders/cloud.fs \
    ../_build/bin/k4-scaner-manager/resources/shaders/cloud.vs \
    ../_build/bin/k4-scaner-manager/resources/shaders/solid.fs \
    ../_build/bin/k4-scaner-manager/resources/shaders/solid.vs \
    ../_build/bin/k4-scaner-manager/resources/shaders/voxelcloud.fs \
    ../_build/bin/k4-scaner-manager/resources/shaders/voxelcloud.gs \
    ../_build/bin/k4-scaner-manager/resources/shaders/voxelcloud.vs

