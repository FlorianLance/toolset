
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

####################################### TARGET/TEMPMATE/CONFIG
TARGET = dc-grabber
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
    # MVC
    dcg_model.hpp \
    dcg_view.hpp \
    dcg_controller.hpp \
    # connections
    dcg_signals.hpp \
    # data    
    # ui
    ui/dcg_main_w.hpp \
    ui/dcg_gl_window.hpp \
    ui/dcg_left_panel_child_drawer.hpp \
    ui/dcg_main_menu_bar_drawer.hpp \

SOURCES += \    
    dcg_main.cpp \
    # MVC
    dcg_model.cpp \
    dcg_view.cpp \
    dcg_controller.cpp \
    # connections
    dcg_signals.cpp \    
    # data
    # ui    
    ui/dcg_main_w.cpp \
    ui/dcg_gl_window.cpp \
    ui/dcg_left_panel_child_drawer.cpp \
    ui/dcg_main_menu_bar_drawer.cpp \

DISTFILES += \
    ../_build/bin/dc-grabber/resources/shaders/cloud.fs \
    ../_build/bin/dc-grabber/resources/shaders/cloud.vs \
    ../_build/bin/dc-grabber/resources/shaders/solid.fs \
    ../_build/bin/dc-grabber/resources/shaders/solid.vs \
    ../_build/bin/dc-grabber/resources/shaders/voxelcloud.fs \
    ../_build/bin/dc-grabber/resources/shaders/voxelcloud.gs \
    ../_build/bin/dc-grabber/resources/shaders/voxelcloud.vs

