
# /*******************************************************************************
# ** Toolset-dc-manager                                                         **
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
    TARGET = dc-managerd
}
equals(CFG, "release"){
    TARGET = dc-manager
}

####################################### TEMPLATE
TEMPLATE = app
CONFIG += console
CONFIG += opengl
CONFIG -= qt

####################################### BUILD FILES
OBJECTS_DIR = $$DC_MANAGER_OBJ
DESTDIR     = $$DC_MANAGER_DEST

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
    $$ORBBEC_INCLUDES \

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
    $$ORBBEC_LIBS \

####################################### PROJECT FILES

HEADERS += \
    dcm_view.hpp \
    dcm_controller.hpp \    
    dcm_model.hpp \
    dcm_signals.hpp \
    # data
    data/dcm_paths.hpp \
    data/dcm_settings.hpp \
    data/dcm_types.hpp \
    data/dcm_ui_settings.hpp \
    # drawers
    ui/dcm_gl_window.hpp \
    drawers/dcm_left_panel_child_drawer.hpp \
    drawers/dcm_main_window_drawer.hpp \
    drawers/dcm_middle_panel_child_drawer.hpp \

SOURCES += \    
    dcm_controller.cpp \    
    dcm_main.cpp \
    dcm_model.cpp \
    dcm_signals.cpp \
    dcm_view.cpp \
    # drawers
    ui/dcm_gl_window.cpp \
    ui/dcm_left_panel_child_drawer.cpp \
    ui/dcm_main_window_drawer.cpp \
    ui/dcm_middle_panel_child_drawer.cpp \
    ui/imgui_demo.cpp \
    # data
    data/dcm_settings.cpp \

DISTFILES += \
    ../_build/bin/dc-manager/resources/shaders/cloud.fs \
    ../_build/bin/dc-manager/resources/shaders/cloud.vs \
    ../_build/bin/dc-manager/resources/shaders/solid.fs \
    ../_build/bin/dc-manager/resources/shaders/solid.vs \
    ../_build/bin/dc-manager/resources/shaders/voxelcloud.fs \
    ../_build/bin/dc-manager/resources/shaders/voxelcloud.gs \
    ../_build/bin/dc-manager/resources/shaders/voxelcloud.vs

