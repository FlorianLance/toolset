
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
    # opengl
    ## buffer
    opengl/buffer/atomic_buffer_object.hpp \
    opengl/buffer/element_buffer_object.hpp \
    opengl/buffer/framebuffer_object.hpp \
    opengl/buffer/pixel_buffer_object.hpp \
    opengl/buffer/shader_storage_buffer_object.hpp \
    opengl/buffer/texture_buffer_object.hpp \
    opengl/buffer/uniform_buffer_object.hpp \
    opengl/buffer/render_buffer_object.hpp \
    opengl/buffer/buffer-utility.hpp \
    opengl/buffer/vertex_buffer_object.hpp \
    ## shader    
    opengl/draw/geometry_data.hpp \
    opengl/shader/shader.hpp \
    ## utility
    opengl/utility/gl_error.hpp \
    opengl/utility/gl_utility.hpp \
    opengl/utility/glew_utility.hpp \
    ## drawer
    opengl/draw/drawer.hpp \
    #
    opengl/gl_draw.hpp \
    opengl/gl_material.hpp \
    opengl/gl_texture.hpp \    
    opengl/sampler.hpp \      
    opengl/vao.hpp \
    opengl/shapes.hpp \
    opengl/renderer.hpp \        
    # glfw
    glfw/glfw_gl_windows.hpp \

SOURCES += \
    # opengl
    ## buffer
    opengl/buffer/texture_buffer_object.cpp \
    ## shader
    opengl/buffer/uniform_buffer_object.cpp \
    opengl/buffer/vertex_buffer_object.cpp \
    opengl/draw/geometry_data.cpp \
    opengl/shader/shader.cpp \
    ## drawer
    opengl/draw/drawer.cpp \
    #
    opengl/gl_texture.cpp \    
    opengl/shapes.cpp \
    # glfw
    glfw/glfw_gl_windows.cpp \

