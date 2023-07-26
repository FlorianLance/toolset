
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

// local
#include "opengl/utility/gl_utility.hpp"

using namespace tool::gl;

UBO::~UBO(){
    clean();
}

auto UBO::generate() -> void{

    if(m_id != 0){
        std::cerr << "[GL] UBO already generated: " << m_id << "\n";
        return;
    }
    glCreateBuffers(1, &m_id);
}

auto UBO::set_data_storage(GLsizeiptr sizeData, void *data, GLenum usage) -> void{

    glNamedBufferStorage(
        m_id,              // GLuint buffer,
        m_size = sizeData, // GLsizeiptr size,
        data,              // const void *data,
        usage              // GLbitfield flags
    );
}

auto UBO::update_data(void *data, GLsizeiptr sizeData, GLintptr offset) -> void{

    if(m_size == 0){
        std::cerr << "[GL] UBO no storage initialized.\n";
        return;
    }

    if(sizeData > m_size){
        std::cerr << "[GL] UBO size is bigger than storage.\n";
        return;
    }

    glNamedBufferSubData(
        m_id,       // GLuint buffer: Specifies the name of the buffer object for glNamedBufferSubData.
        offset,     // GLintptr offset: Specifies the offset into the buffer object's data store where data replacement will begin, measured in bytes.
        sizeData,   // GLsizeiptr size: Specifies the size in bytes of the data store region being replaced.
        data        // const void *data: pecifies a pointer to the new data that will be copied into the data store.
    );
}

auto UBO::bind(GLuint ubIndex) -> void{

    // bind a range within a buffer object to an indexed buffer target
    glBindBufferRange(
        GL_UNIFORM_BUFFER,  // GLenum target
        ubIndex,            // GLuint index
        m_id,               // GLuint buffer
        0,                  // GLintptr offset
        m_size              // GLsizeiptr size
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
        sizes.emplace_back(ubo.size());
    }

    glBindBuffersRange(
        GL_UNIFORM_BUFFER,                  // GLenum target,
        first,                              // GLuint first,
        static_cast<GLsizei>(UBOs.size()),  // GLsizei count,
        buffers.data(),                     // const GLuint *buffers,
        offsets.data(),                     // const GLintptr *offsets,
        sizes.data()                        // const GLintptr *sizes);
    );
}

auto UBO::clean() -> void{

    if(m_id == 0){
        return;
    }

    glDeleteBuffers(1, &m_id);
    m_id = 0;
}

auto UBO::set_data_space_from_shader(ShaderProgram *shader, GLenum usage) -> void{

    // shared, packed, std140, and std430
    // https://www.khronos.org/opengl/wiki/Interface_Block_(GLSL)
    if(shader == nullptr){
        std::cerr << "[GL] MaterialUBO shader is null.\n";
        return;
    }

    const auto nameStr = get_block_name();
    if(shader->uniformBlocks.count(nameStr) == 0){
        std::cerr << "[GL] MaterialUBO block uniform not found in shader.\n";
        return;
    }

    const auto &blockInfo = shader->uniformBlocks.at(nameStr);
    m_blockBuffer.resize((blockInfo.size.v));
    m_offsets = blockInfo.get_offsets(get_elements_name());

    set_data_storage(static_cast<GLint>(m_blockBuffer.size()), nullptr, usage);
}
