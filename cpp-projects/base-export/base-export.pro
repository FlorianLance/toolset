
# /*******************************************************************************
# ** base-export                                                                **
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
    TARGET = base-exportd
}
equals(CFG, "release"){
    TARGET = base-export
}

####################################### TEMPLATE
TEMPLATE = lib
CONFIG += dll
CONFIG -= console

####################################### BUILD FILES
OBJECTS_DIR = $$BASE_EXPORT_OBJ
DESTDIR     = $$BASE_EXPORT_DEST

####################################### CONFIG
CONFIG -= qt

####################################### DEPS
INCLUDEPATH += $$BASE_EXPORT_DEP_INCLUDEPATH
LIBS += $$BASE_EXPORT_DEP_LIBS
PRE_TARGETDEPS += $$BASE_EXPORT_PRE_TARGETDEPS

######################################## PROJECT FILES
HEADERS += \
    dc_network_direct_player_export.hpp \
    dc_video_player_export.hpp

SOURCES += \
    dc_network_direct_player_export.cpp \
    dc_video_player_export.cpp
