
# /*******************************************************************************
# ** Toolset-test                                                                  **
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
    TARGET = base-testd
}
equals(CFG, "release"){
    TARGET = base-test
}

####################################### TEMPLATE
TEMPLATE = app
CONFIG += console

####################################### BUILD FILES
OBJECTS_DIR = $$BASE_OBJ
DESTDIR     = $$BASE_DEST

####################################### CONFIG
CONFIG -= qt

####################################### DEPS
INCLUDEPATH += $$BASE_TEST_DEP_INCLUDEPATH
LIBS += $$BASE_TEST_DEP_LIBS
PRE_TARGETDEPS += $$BASE_TEST_PRE_TARGETDEPS

####################################### PROJECT FILES

SOURCES += \
#    camera_tests.cpp \
#    cloud_tests.cpp \
#    exp_tests.cpp \
    main.cpp \       
    geometry_tests.cpp \

HEADERS += \
    glm_utility.hpp

DISTFILES += \
    CREDITS.md \
    LICENSE \




