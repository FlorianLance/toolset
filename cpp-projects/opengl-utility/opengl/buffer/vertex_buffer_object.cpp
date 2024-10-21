
/*******************************************************************************
** Toolset-opengl-utility                                                     **
** MIT License                                                                **
** Copyright (c) [2018] [Florian Lance]                                       **
**                                                                            **
** Permission is hereby granted, free of charge, to any person obtaining a    **
** copy of this software and associated documentation files (the "Software"), **
** to deal in the Software without restriction, including without limitation  **
** the rights to use, copy, modify, merge, publish, distribute, sublicense,   **
** and/or sell copies of the Software, and to permit persons to whom the      **
** Software is furnished to do so, subject to the following conditions:       **
**                                                                            **
** The above copyright notice and this permission notice shall be included in **
** all copies or substantial portions of the Software.                        **
**                                                                            **
** THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR **
** IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,   **
** FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL    **
** THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER **
** LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING    **
** FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER        **
** DEALINGS IN THE SOFTWARE.                                                  **
**                                                                            **
********************************************************************************/

#include "vertex_buffer_object.hpp"

// base
#include "utility/logger.hpp"

// local
#include "opengl/gl_functions.hpp"

using namespace tool::gl;

VBO::~VBO(){
    clean();
}

auto VBO::initialize() -> void{

    if(is_initialized()){
        Log::error(std::format("[VBO::initialize] Already initialized (id:{}).\n", m_handle));
        return;
    }
    GL::create_buffers(1, &m_handle);
}

auto VBO::clean() -> void{

    if(!is_initialized()){
        return;
    }

    GL::delete_buffers(1, &m_handle);
    m_handle         = 0;
    m_loadedDataSize = 0;
    m_usage          = 0;
}

auto VBO::load_data(const GLint *data, GLsizeiptr size, GLbitfield  usage) -> bool{
    return load_data(reinterpret_cast<const GLvoid *>(data), size, usage);
}

auto VBO::load_data(const GLfloat *data, GLsizeiptr size, GLbitfield  usage) -> bool{
    return load_data(reinterpret_cast<const GLvoid *>(data), size, usage);
}

auto VBO::load_data(const GLvoid *data, GLsizeiptr size, GLbitfield usage) -> bool{

    if(!is_initialized()){
        Log::error("[VBO::load_data] Not initialized.\n");
        return false;
    }

    if(is_data_loaded()){
        Log::error(std::format("[VBO::load_data] Data has already been loaded (id:{}).\n", m_handle));
        return false;
    }

    if(size == 0){
        Log::error(std::format("[VBO::load_data] Size must be > 0 (id:{}).\n", m_handle));
        return false;
    }

    GL::named_buffer_storage(
        m_handle,
        m_loadedDataSize = size,
        data,
        m_usage = usage
    );

    return true;
}

auto VBO::update_data(const GLint *data, GLsizeiptr size, GLintptr offset) -> bool{
    return update_data(reinterpret_cast<const GLvoid*>(data), size, offset);
}

auto VBO::update_data(const GLfloat *data, GLsizeiptr size, GLintptr offset) -> bool{
    return update_data(reinterpret_cast<const GLvoid*>(data), size, offset);
}

auto VBO::update_data(const GLvoid *data, GLsizeiptr size, GLintptr offset) -> bool{

    if(!is_initialized()){
        Log::error("[VBO::update_data] Not initialized.\n");
        return false;
    }

    if(!is_data_loaded()){
        Log::error(std::format("[VBO::update_data] Data must be loaded before update (id:{}).\n", m_handle));
        return false;
    }

    if(!is_dynamic()){
        Log::error(std::format("[VBO::update_data] Buffet storage is not dynamic with usage [{}] (id:{}).\n", m_usage, m_handle));
        return false;
    }

    if(size + offset > loaded_data_size()){
        Log::error(std::format("[VBO::update_data] Loaded data size [{}] is to small with update size [{}] and offset [{}] (id:{}).\n",
        m_loadedDataSize, size, offset, m_handle));
        return false;
    }

    GL::named_buffer_sub_data(
        m_handle,
        offset,
        size,
        data
    );

    return true;
}


auto VBO::attrib(AttriIndex index, AttriSize size, AttriType type, Stride stride, AttribOffset offset) -> bool{

    GL::bind_buffer(GL_ARRAY_BUFFER, m_handle);
    GL::enable_vertex_attrib_array(index.v);

    if(type.v == GL_INT){
        GL::vertex_attrib_i_pointer(index.v, size.v, type.v, stride.v,  offset.v);
    }else{
        GL::vertex_attrib_pointer(index.v, size.v, type.v, GL_FALSE, stride.v,  offset.v);
    }

    GL::bind_buffer(GL_ARRAY_BUFFER, 0);

    return true;
}



