
# /*******************************************************************************
# ** Toolset-ts-network                                                         **
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

####################################### REPOSITORY
TOOLSET_REPOSITORY_DIR      = $$PWD"/../.."

####################################### TARGET/TEMPMATE/CONFIG
TARGET = ts-network
TEMPLATE = lib
CONFIG += staticlib
CONFIG -= console
CONFIG -= qt

####################################### PRI
include(../ts-settings.pri)
include(../ts-projects.pri)
include(../ts-thirdparty.pri)
include(../ts-dependencies.pri)

####################################### INCLUDES

TS_NETWORK_DEP_INCLUDEPATH =\
    $$TS_BASE_THIRDPARTY_INCLUDES\
    $$TS_NETWORK_THIRDPARTY_INCLUDES\
    $$TS_BASE_INCLUDES\

####################################### LIBS

TS_NETWORK_DEP_LIBS =\
    $$TS_BASE_THIRDPARTY_LIBS\
    $$TS_NETWORK_THIRDPARTY_LIBS\
    $$TS_BASE_LIB\

####################################### DEP

TS_NETWORK_PRE_TARGETDEPS =\
    $$TS_BASE_LIB_FILE\

####################################### GENERATE VARIABLES

include(../ts-gen-var.pri)

####################################### PROJECT FILES
HEADERS += \
    # network
    network/boost_asio.hpp \
    network/asio_utility.hpp \
    network/udp_client.hpp \
    network/tcp_reader.hpp \
    network/tcp_sender.hpp \
    network/tcp_server.hpp \
    network/udp_reader.hpp \
    network/udp_sender.hpp \    
    ## settings
    # network/settings/remote_server_settings.hpp \
    # network/settings/udp_connection_settings.hpp \
    # network/settings/udp_server_settings.hpp \

SOURCES += \
    # network
    network/asio_utility.cpp \
    network/tcp_reader.cpp \
    network/tcp_sender.cpp \
    network/tcp_server.cpp \
    network/udp_reader.cpp \
    network/udp_sender.cpp \
    ## settings
    # network/settings/remote_server_settings.cpp \
    # network/settings/udp_connection_settings.cpp \
    # network/settings/udp_server_settings.cpp \
