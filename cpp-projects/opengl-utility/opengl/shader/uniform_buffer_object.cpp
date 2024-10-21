
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

#include "uniform_buffer_object.hpp"

// base
#include "utility/logger.hpp"

// local
#include "opengl/gl_functions.hpp"

using namespace tool::gl;

UBO::~UBO(){
    clean();
}

auto UBO::initialize() -> void{

    if(is_initialized()){
        Log::error(std::format("[UBO::initialize] Already initialized (id:{}).\n", m_handle));
        return;
    }
    GL::create_buffers(1, &m_handle);
}

auto UBO::clean() -> void{

    if(!is_initialized()){
        return;
    }

    GL::delete_buffers(1, &m_handle);
    m_handle         = 0;
    m_loadedDataSize = 0;
    m_usage          = 0;
}


auto UBO::load_data(const GLvoid *data, GLsizeiptr size, GLenum usage) -> bool{

    if(!is_initialized()){
        Log::error("[UBO::load_data] Not initialized.\n");
        return false;
    }

    if(is_data_loaded()){
        Log::error(std::format("[UBO::load_data] Data has already been loaded (id:{}).\n", m_handle));
        return false;
    }

    if(size == 0){
        Log::error(std::format("[UBO::load_data] Size must be > 0 (id:{}).\n", m_handle));
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

auto UBO::update_data(const GLvoid *data, GLsizeiptr size, GLintptr offset) -> bool{

    if(!is_initialized()){
        Log::error("[UBO::update_data] Not initialized.\n");
        return false;
    }

    if(!is_data_loaded()){
        Log::error(std::format("[UBO::update_data] Data must be loaded before update (id:{}).\n", m_handle));
        return false;
    }

    if(!is_dynamic()){
        Log::error(std::format("[UBO::update_data] Buffet storage is not dynamic with usage [{}] (id:{}).\n", m_usage, m_handle));
        return false;
    }

    if(size + offset > loaded_data_size()){
        Log::error(std::format("[UBO::update_data] Loaded data size [{}] is to small with update size [{}] and offset [{}] (id:{}).\n",
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

auto UBO::set_data_space_from_shader(ShaderProgram *shader, GLenum usage) -> void{

    // shared, packed, std140, and std430
    // https://www.khronos.org/opengl/wiki/Interface_Block_(GLSL)
    if(shader == nullptr){
        Log::error("[UBO::set_data_space_from_shader] Shader program is null.\n");
        return;
    }

    const auto nameStr = get_block_name();
    if(shader->uniformBlocks.count(nameStr) == 0){
        Log::error("[UBO::set_data_space_from_shader] UBO block uniform not found in shader.\n");
        return;
    }

    const auto &blockInfo = shader->uniformBlocks.at(nameStr);
    m_blockBuffer.resize((blockInfo.size));
    m_offsets = blockInfo.get_offsets(get_elements_name());

    load_data(nullptr, static_cast<GLint>(m_blockBuffer.size()), usage);
}



auto UBO::bind(GLuint index) -> void{

    GL::bind_buffer_range(
        GL_UNIFORM_BUFFER,
        index,
        m_handle,
        0,
        m_loadedDataSize
    );
}

auto UBO::bind_range(const std::vector<UBO> &UBOs, GLuint first) -> void{

    // to test
    std::vector<GLuint> buffers;
    buffers.reserve(UBOs.size());

    std::vector<GLintptr> offsets;
    offsets.reserve(UBOs.size());

    std::vector<GLintptr> sizes;
    sizes.reserve(UBOs.size());

    for(const auto &ubo : UBOs){
        buffers.emplace_back(ubo.id());
        offsets.emplace_back(0);
        sizes.emplace_back(ubo.loaded_data_size());
    }

    GL::bind_buffers_range(
        GL_UNIFORM_BUFFER,
        first,
        static_cast<GLsizei>(UBOs.size()),
        buffers.data(),
        offsets.data(),
        sizes.data()
    );
}






