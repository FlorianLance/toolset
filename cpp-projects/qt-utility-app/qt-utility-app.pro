
# /*******************************************************************************
# ** Toolset-qt-utility                                                         **
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
    TARGET = qt-utilityd-app
}
equals(CFG, "release"){
    TARGET = qt-utility-app
}

####################################### TEMPLATE
TEMPLATE = app
CONFIG += console

####################################### BUILD FILES
OBJECTS_DIR = $$QT_UTILITY_OBJ
MOC_DIR     = $$QT_UTILITY_MOC
RCC_DIR     = $$QT_UTILITY_RCC
UI_DIR      = $$QT_UTILITY_UI
DESTDIR     = $$QT_UTILITY_DEST

####################################### CONFIG
CONFIG += qt
QT += core gui opengl widgets printsupport network
DEFINES += QWT_DLL

####################################### INCLUDES
INCLUDEPATH += \    
    # base
    $$BASE_INCLUDES \
    # opengl-utility
    $$OPENGL_UTILITY_INCLUDES \
    # qt-utility
    $$QT_UTILITY_INCLUDES \
    # thirdparty
    $$SFML_INCLUDES \
    $$GLEW_INCLUDES \
    $$GLM_INCLUDES \
    $$QWT_INCLUDES \

####################################### LIBRAIRIES

PRE_TARGETDEPS += \
    # tool
    $$BASE_LIB_DEP \
    $$OPENGL_UTILITY_LIB_DEP \
    $$QT_UTILITY_LIB_DEP \

LIBS +=  \
    # tool
    $$BASE_LIB\
    $$OPENGL_UTILITY_LIB \
    $$QT_UTILITY_LIB \
    # third party
    $$WINDOWS_LIBS \
    $$SFML_LIBS \
    $$GLEW_LIBS \
    $$QWT_LIBS \
    $$GLM_LIBS\

####################################### PROJECT FILES
HEADERS += \

SOURCES += \
    qt_utility_main.cpp \



