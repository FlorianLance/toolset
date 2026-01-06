
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

####################################### TARGET/TEMPMATE/CONFIG
TARGET = dc-monitoring
TEMPLATE = app
CONFIG += console
CONFIG += qt
QT  += core gui network concurrent widgets opengl multimedia

####################################### PRI
include(../ts-settings.pri)
include(../ts-projects.pri)
include(../ts-thirdparty.pri)
include(../ts-dependencies.pri)

####################################### INCLUDES

DC_MONITORING_DEP_INCLUDEPATH =\
    #
    $$DC_MONITORING_THIRDPARTY_INCLUDES\
    $$TS_BASE_THIRDPARTY_INCLUDES\
    $$TS_QT_THIRDPARTY_INCLUDES\
    #
    $$TS_BASE_INCLUDES\
    $$TS_QT_INCLUDES\

####################################### LIBS

DC_MONITORING_DEP_LIBS =\
    #
    $$DC_MONITORING_THIRDPARTY_LIBS\
    $$TS_BASE_THIRDPARTY_LIBS\
    $$TS_QT_THIRDPARTY_LIBS\
    #
    $$TS_BASE_LIB\
    $$TS_QT_LIB\

####################################### DEP

DC_PLAYER_PRE_TARGETDEPS =\
    $$TS_BASE_LIB_FILE\
    $$TS_QT_LIB_FILE\

####################################### GENERATE VARIABLES

include(../ts-gen-var.pri)


####################################### PROJECT FILES
HEADERS += \
    avcpp/audioresampler.h \
    avcpp/av.h \
    avcpp/averror.h \
    avcpp/avlog.h \
    avcpp/avtime.h \
    avcpp/avutils.h \
    avcpp/channellayout.h \
    avcpp/codec.h \
    avcpp/codeccontext.h \
    avcpp/dictionary.h \
    avcpp/ffmpeg.h \
    avcpp/filters/buffersink.h \
    avcpp/filters/buffersrc.h \
    avcpp/filters/filter.h \
    avcpp/filters/filtercontext.h \
    avcpp/filters/filtergraph.h \
    avcpp/filters/filterpad.h \
    avcpp/format.h \
    avcpp/formatcontext.h \
    avcpp/frame.h \
    avcpp/linkedlistutils.h \
    avcpp/packet.h \
    avcpp/pixelformat.h \
    avcpp/rational.h \
    avcpp/rect.h \
    avcpp/sampleformat.h \
    avcpp/stream.h \
    avcpp/timestamp.h \
    avcpp/videorescaler.h

SOURCES += \
    avcpp/audioresampler.cpp \
    avcpp/averror.cpp \
    avcpp/avtime.cpp \
    avcpp/avutils.cpp \
    avcpp/channellayout.cpp \
    avcpp/codec.cpp \
    avcpp/codeccontext.cpp \
    avcpp/dictionary.cpp \
    avcpp/filters/buffersink.cpp \
    avcpp/filters/buffersrc.cpp \
    avcpp/filters/filter.cpp \
    avcpp/filters/filtercontext.cpp \
    avcpp/filters/filtergraph.cpp \
    avcpp/filters/filterpad.cpp \
    avcpp/format.cpp \
    avcpp/formatcontext.cpp \
    avcpp/frame.cpp \
    avcpp/packet.cpp \
    avcpp/pixelformat.cpp \
    avcpp/rational.cpp \
    avcpp/rect.cpp \
    avcpp/sampleformat.cpp \
    avcpp/stream.cpp \
    avcpp/timestamp.cpp \
    avcpp/videorescaler.cpp \
    dcmo_main.cpp

DISTFILES += \
    avcpp/CMakeLists.txt \
    avcpp/Config.cmake.in \
    avcpp/libavcpp.pc.in \
    avcpp/meson.build

