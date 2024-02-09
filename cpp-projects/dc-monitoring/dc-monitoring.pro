
# /*******************************************************************************
# ** Toolset-dc-monitoring                                                      **
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
    TARGET = dc-monitoringd
}
equals(CFG, "release"){
    TARGET = dc-monitoring
}

####################################### TEMPLATE
TEMPLATE = app
CONFIG += console
CONFIG += qt

####################################### BUILD FILES
OBJECTS_DIR = $$DC_MONITORING_OBJ
MOC_DIR     = $$DC_MONITORING_MOC
RCC_DIR     = $$DC_MONITORING_RCC
UI_DIR      = $$DC_MONITORING_UI
DESTDIR     = $$DC_MONITORING_DEST

####################################### CONFIG
QT  += core gui network concurrent widgets opengl multimedia

####################################### DEPS
INCLUDEPATH += $$DC_MONITORING_DEP_INCLUDEPATH
LIBS +=  $$DC_MONITORING_DEP_LIBS
PRE_TARGETDEPS += $$DC_MONITORING_PRE_TARGETDEPS

####################################### PROJECT FILES

HEADERS += \

SOURCES += \
    dcmo_main.cpp

