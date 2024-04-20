
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
    opengl/gl_draw.hpp \
    opengl/gl_functions.hpp \
    opengl/gl_material.hpp \
    opengl/gl_types.hpp \    
    opengl/vao.hpp \
    opengl/shapes.hpp \
    ## buffer
    opengl/buffer/atomic_buffer_object.hpp \
    opengl/buffer/pixel_buffer_object.hpp \     
    opengl/buffer/render_buffer_object.hpp \
    opengl/buffer/vertex_buffer_object.hpp \
    opengl/buffer/framebuffer_object.hpp \
    ## shader        
    opengl/shader/shaders_manager.hpp \
    opengl/shader/shader_storage_buffer_object.hpp \
    opengl/shader/uniform_buffer_object.hpp \
    opengl/shader/shader_object.hpp \
    opengl/shader/shader_program.hpp \
    ## texture
    opengl/texture/sampler.hpp \
    opengl/texture/textures_manager.hpp \
    opengl/texture/texture_buffer_object.hpp \
    opengl/texture/cube_map_tbo.hpp \
    opengl/texture/geometry_texture_2d_tbo.hpp \
    opengl/texture/texture_2d_tbo.hpp \
    ## utility
    opengl/utility/gl_utility.hpp \
    ## draw
    opengl/draw/drawers_manager.hpp \
    opengl/draw/base_drawer.hpp \
    opengl/draw/lines_drawers.hpp \
    opengl/draw/lines_renderer.hpp \
    opengl/draw/points_drawers.hpp \
    opengl/draw/points_renderer.hpp \
    opengl/draw/triangles_drawers.hpp \
    opengl/draw/triangles_renderer.hpp \
    opengl/draw/vao_renderer.hpp \
    opengl/draw/voxels_drawers.hpp \
    opengl/draw/voxels_renderer.hpp \

SOURCES += \
    # glfw
    glfw/glfw_gl_windows.cpp \
    # opengl
    opengl/vao.cpp \
    opengl/gl_functions.cpp \
    opengl/shapes.cpp \    
    ## buffer        
    opengl/buffer/vertex_buffer_object.cpp \
    opengl/buffer/atomic_buffer_object.cpp \
    opengl/buffer/framebuffer_object.cpp \
    opengl/buffer/pixel_buffer_object.cpp \
    opengl/buffer/render_buffer_object.cpp \
    ## shader
    opengl/shader/shaders_manager.cpp \
    opengl/shader/shader_storage_buffer_object.cpp \
    opengl/shader/uniform_buffer_object.cpp \
    opengl/shader/shader_object.cpp \
    opengl/shader/shader_program.cpp \
    ## texture
    opengl/texture/texture_buffer_object.cpp \
    opengl/texture/cube_map_tbo.cpp \
    opengl/texture/geometry_texture_2d_tbo.cpp \
    opengl/texture/texture_2d_tbo.cpp \
    opengl/texture/textures_manager.cpp \
    opengl/texture/sampler.cpp \
    ## draw
    opengl/draw/drawers_manager.cpp \
    opengl/draw/base_drawer.cpp \
    opengl/draw/lines_drawers.cpp \
    opengl/draw/lines_renderer.cpp \
    opengl/draw/points_drawers.cpp \
    opengl/draw/points_renderer.cpp \
    opengl/draw/triangles_drawers.cpp \
    opengl/draw/triangles_renderer.cpp \
    opengl/draw/vao_renderer.cpp \
    opengl/draw/voxels_drawers.cpp \
    opengl/draw/voxels_renderer.cpp \






