
# /*******************************************************************************
# ** Toolset-ts-graphics                                                        **
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
TARGET = ts-graphics
TEMPLATE = lib
CONFIG += staticlib
CONFIG -= console
CONFIG -= qt

####################################### PRI
include(../ts-settings.pri)
include(../ts-projects.pri)
include(../ts-thirdparty.pri)
include(../ts-dependencies.pri)

####################################### PROJECT FILES
HEADERS += \
    # graphics
    graphics/light.hpp \
    graphics/mesh.hpp \
    graphics/mesh_generation.hpp \
    graphics/image_io.hpp \
    ## animation
    graphics/animation/animation.hpp \
    graphics/animation/bones.hpp \
    ## material
    graphics/material/material_enum.hpp \
    graphics/material/material.hpp \
    ## texture
    graphics/model/__models_manager.hpp \
    graphics/texture/texture.hpp \
    graphics/texture/cube_map.hpp \
    graphics/texture/texture_2d.hpp \
    graphics/texture/texture_enum.hpp \
    graphics/texture/texture_options.hpp \
    ## model
    graphics/model/model_mesh.hpp \
    ## camera
    graphics/camera/camera_matrices.hpp \
    graphics/camera/camera.hpp \
    graphics/camera/screen.hpp \    

SOURCES += \
    # graphics
    graphics/mesh.cpp \
    graphics/mesh_generation.cpp \
    ## animation
    graphics/animation/animation.cpp \
    graphics/animation/bones.cpp \
    ## model
    graphics/model/__models_manager.cpp \
    graphics/model/model_mesh.cpp \
    ## texture
    graphics/texture/texture.cpp \
    graphics/texture/cube_map.cpp \
    graphics/texture/texture_2d.cpp \
    ## camera
    graphics/camera/camera.cpp \
