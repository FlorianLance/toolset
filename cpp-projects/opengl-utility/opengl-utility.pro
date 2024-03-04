
## /*******************************************************************************
## ** Toolset-opengl-utility                                                     **
## ** MIT License                                                                **
## ** Copyright (c) [2018] [Florian Lance]                                       **
## **                                                                            **
## ** Permission is hereby granted, free of charge, to any person obtaining a    **
## ** copy of this software and associated documentation files (the "Software"), **
## ** to deal in the Software without restriction, including without limitation  **
## ** the rights to use, copy, modify, merge, publish, distribute, sublicense,   **
## ** and/or sell copies of the Software, and to permit persons to whom the      **
## ** Software is furnished to do so, subject to the following conditions:       **
## **                                                                            **
## ** The above copyright notice and this permission notice shall be included in **
## ** all copies or substantial portions of the Software.                        **
## **                                                                            **
## ** THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR **
## ** IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,   **
## ** FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL    **
## ** THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER **
## ** LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING    **
## ** FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER        **
## ** DEALINGS IN THE SOFTWARE.                                                  **
## **                                                                            **
## ********************************************************************************/

####################################### repo
TOOLSET_REPOSITORY_DIR      = $$PWD"/../.."

####################################### TARGET/TEMPMATE/CONFIG
TARGET = opengl-utility
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
    # glfw
    glfw/glfw_gl_windows.hpp \
    # opengl
    opengl/buffer/framebuffer_object.hpp \
    opengl/draw/points_mesh_data.hpp \
    opengl/gl_draw.hpp \
    opengl/gl_functions.hpp \
    opengl/gl_material.hpp \
    opengl/gl_types.hpp \
    opengl/sampler.hpp \
    opengl/shader/shader_program.hpp \
    opengl/vao.hpp \
    opengl/shapes.hpp \
    ## buffer
    opengl/buffer/atomic_buffer_object.hpp \
    opengl/buffer/element_buffer_object.hpp \
    opengl/buffer/pixel_buffer_object.hpp \     
    opengl/buffer/render_buffer_object.hpp \
    opengl/buffer/buffer-utility.hpp \
    opengl/buffer/vertex_buffer_object.hpp \
    ## shader        
    opengl/shader/shader.hpp \
    opengl/shader/shaders_manager.hpp \
    opengl/shader/shader_storage_buffer_object.hpp \
    opengl/shader/uniform_buffer_object.hpp \
    ## texture
    opengl/texture/textures_manager.hpp \
    opengl/texture/texture_buffer_object.hpp \
    opengl/texture/cube_map_tbo.hpp \
    opengl/texture/geometry_texture_2d_tbo.hpp \
    opengl/texture/texture_2d_tbo.hpp \
    ## utility
    opengl/utility/gl_utility.hpp \
    ## draw
    opengl/draw/geometry_data.hpp \
    opengl/draw/drawer.hpp \
    opengl/draw/drawers_manager.hpp \

SOURCES += \
    # glfw
    glfw/glfw_gl_windows.cpp \
    # opengl
    opengl/buffer/atomic_buffer_object.cpp \
    opengl/buffer/element_buffer_object.cpp \
    opengl/buffer/framebuffer_object.cpp \
    opengl/buffer/pixel_buffer_object.cpp \
    opengl/buffer/render_buffer_object.cpp \
    opengl/draw/points_mesh_data.cpp \
    opengl/gl_functions.cpp \
    opengl/shader/shader_program.cpp \
    opengl/shapes.cpp \
    opengl/sampler.cpp \
    ## buffer        
    opengl/buffer/vertex_buffer_object.cpp \
    ## shader
    opengl/shader/shader.cpp \
    opengl/shader/shaders_manager.cpp \
    opengl/shader/shader_storage_buffer_object.cpp \
    opengl/shader/uniform_buffer_object.cpp \
    ## texture
    opengl/texture/texture_buffer_object.cpp \
    opengl/texture/cube_map_tbo.cpp \
    opengl/texture/geometry_texture_2d_tbo.cpp \
    opengl/texture/texture_2d_tbo.cpp \
    opengl/texture/textures_manager.cpp \
    ## draw
    opengl/draw/geometry_data.cpp \
    opengl/draw/drawer.cpp \
    opengl/draw/drawers_manager.cpp \
    opengl/vao.cpp





