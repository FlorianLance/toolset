
/*******************************************************************************
** Toolset-ts-opengl                                                          **
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

#include "pixel_buffer_object.hpp"

// base
#include "utility/logger.hpp"

// local
#include "opengl/gl_functions.hpp"

using namespace tool::gl;

PBO::~PBO(){
    clean();
}

auto PBO::initialize() -> void{
    
    if(m_handle != 0){
        Log::error(std::format("[PBO::initialize] Already initialized (id:{}).\n", m_handle));
        return;
    }
    GL::create_buffers(1, &m_handle);
}

auto PBO::clean() -> void{
    
    if(m_handle == 0){
        return;
    }

    GL::delete_buffers(1, &m_handle);
    m_handle         = 0;
    m_loadedDataSize = 0;
    m_usage          = 0;
}

auto PBO::load_data(const GLuint *data, GLsizeiptr size, GLbitfield usage) -> bool{

    if(!is_initialized()){
        Log::error("[PBO::load_data] Not initialized.\n");
        return false;
    }

    if(is_data_loaded()){
        Log::error(std::format("[PBO::load_data] Data has already been loaded (id:{}).\n", m_handle));
        return false;
    }

    if(size == 0){
        Log::error(std::format("[PBO::load_data] Size must be > 0 (id:{}).\n", m_handle));
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

auto PBO::bind_pack(GLuint index) const -> bool{

    if(!is_initialized()){
        Log::error("[PBO::bind_pack] Not initialized.\n");
        return false;
    }

    glBindBufferBase(GL_PIXEL_PACK_BUFFER, index, m_handle);

    // GL::bind_buffers_range(
    //     GL_PIXEL_PACK_BUFFER,
    //     index,
    //     1,
    //     &m_handle,
    //     nullptr,
    //     &m_loadedDataSize
    // );

    return true;
}

auto PBO::bind_unpack(GLuint index) const -> bool{

    if(!is_initialized()){
        Log::error("[PBO::bind_unpack] Not initialized.\n");
        return false;
    }

    glBindBufferBase(GL_PIXEL_UNPACK_BUFFER, index, m_handle);

    return true;
}
