
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

