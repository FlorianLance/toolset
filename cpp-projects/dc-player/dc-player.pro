
# /*******************************************************************************
# ** Toolset-dc-player                                                          **
# ** MIT License                                                                **
# ** Copyright (c) [2025] [Florian Lance]                                       **
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
TARGET = dc-player
TEMPLATE = app
CONFIG += console
CONFIG += opengl
CONFIG -= qt

####################################### PRI
include(../ts-settings.pri)
include(../ts-projects.pri)
include(../ts-thirdparty.pri)
include(../ts-dependencies.pri)

####################################### INCLUDES

DC_PLAYER_DEP_INCLUDEPATH =\
    #
    $$DC_PLAYER_THIRDPARTY_INCLUDES\
    $$TS_IMGUI_GL_ENGINE_THIRDPARTY_INCLUDES\
    $$TS_BASE_THIRDPARTY_INCLUDES\
    $$TS_MESH_THIRDPARTY_INCLUDES\
    $$TS_DATA_THIRDPARTY_INCLUDES\
    $$TS_OPENGL_THIRDPARTY_INCLUDES\
    $$TS_NETWORK_THIRDPARTY_INCLUDES\
    $$TS_DEPTH_CAMERA_THIRDPARTY_INCLUDES\
    #
    $$TS_BASE_INCLUDES\
    $$TS_OPENGL_INCLUDES\
    $$TS_MESH_INCLUDES\
    $$TS_DATA_INCLUDES\
    $$TS_NETWORK_INCLUDES\
    $$TS_DEPTH_CAMERA_INCLUDES\
    $$TS_IMGUI_GL_ENGINE_INCLUDES\

####################################### LIBS

DC_PLAYER_DEP_LIBS =\
    #
    $$DC_PLAYER_THIRDPARTY_LIBS\
    $$TS_IMGUI_GL_ENGINE_THIRDPARTY_LIBS\
    $$TS_OPENGL_THIRDPARTY_LIBS\
    $$TS_BASE_THIRDPARTY_LIBS\
    $$TS_MESH_THIRDPARTY_LIBS\
    $$TS_DATA_THIRDPARTY_LIBS\
    $$TS_NETWORK_THIRDPARTY_LIBS\
    $$TS_DEPTH_CAMERA_THIRDPARTY_LIBS\
    #
    $$TS_BASE_LIB\
    $$TS_OPENGL_LIB\
    $$TS_MESH_LIB\
    $$TS_DATA_LIB\
    $$TS_NETWORK_LIB\
    $$TS_DEPTH_CAMERA_LIB\
    $$TS_IMGUI_GL_ENGINE_LIB\

####################################### DEP

DC_PLAYER_PRE_TARGETDEPS =\
    $$TS_BASE_LIB_FILE\
    $$TS_OPENGL_LIB_FILE\
    $$TS_MESH_LIB_FILE\
    $$TS_DATA_LIB_FILE\
    $$TS_DEPTH_CAMERA_LIB_FILE\
    $$TS_NETWORK_LIB_FILE\
    $$TS_IMGUI_GL_ENGINE_LIB_FILE\

####################################### GENERATE VARIABLES

include(../ts-gen-var.pri)

####################################### PROJECT FILES
HEADERS += \
    # MCV
    dcp_model.hpp \
    dcp_view.hpp \
    dcp_controller.hpp \
    # connections
    dcp_signals.hpp \
    # ui
    ui/dcp_gl_window.hpp \
    ui/dcp_left_panel_child_drawer.hpp \
    ui/dcp_main_window_drawer.hpp \
    ui/dcp_middle_panel_child_drawer.hpp \
    # data
    data/dcp_ui_settings.hpp \

SOURCES += \
    dcp_main.cpp \
    # MCV
    dcp_model.cpp \
    dcp_view.cpp \
    dcp_controller.cpp \
    # connections
    dcp_signals.cpp \
    # ui
    ui/dcp_gl_window.cpp \
    ui/dcp_left_panel_child_drawer.cpp \
    ui/dcp_main_window_drawer.cpp \
    ui/dcp_middle_panel_child_drawer.cpp \

DISTFILES += \
    ../_build/bin/dc-player/resources/shaders/cloud.fs \
    ../_build/bin/dc-player/resources/shaders/cloud.vs \
    ../_build/bin/dc-player/resources/shaders/solid.fs \
    ../_build/bin/dc-player/resources/shaders/solid.vs \
    ../_build/bin/dc-player/resources/shaders/voxelcloud.fs \
    ../_build/bin/dc-player/resources/shaders/voxelcloud.gs \
    ../_build/bin/dc-player/resources/shaders/voxelcloud.vs

