
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

#pragma once

// local
#include "opengl/utility/gl_utility.hpp"

namespace tool::gl{

struct EBO{

    EBO() = default;
    EBO(const EBO&) = delete;
    EBO& operator=(const EBO&) = delete;
    EBO(EBO&& other) = default;
    EBO& operator=(EBO&& other) = default;

    ~EBO(){
        clean();
    }

    void generate(){

        if(m_id != 0){
            std::cerr << "[GL] EBO already generated: " << m_id << "\n";
            return;
        }
        glCreateBuffers(1, &m_id);
    }

    bool bind(){

        if(m_id == 0){
            std::cerr << "[GL] EBO not generated, cannot bind it.\n";
            return false;
        }

        // new way: mix vertex buffer with index buffer
        // GLint alignment = GL_NONE;
        // glGetIntegerv(GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT, &alignment);

        // REMOVE
//        glBindBufferRange(
//            GL_ELEMENT_ARRAY_BUFFER,    // GLenum target
//            0,                          // GLuint index
//            m_id,                       // GLuint buffer
//            0,                          // GLintptr offset
//            1                           // GLsizeiptr size
//        );
         glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_id);

        return true;
    }

    void generate_and_load(UintData data, SizeData size){
        generate();
        load_data(data, size);
    }

    bool load_data(UintData data, SizeData size){

        // bit combination of: GL_DYNAMIC_STORAGE_BIT, GL_MAP_READ_BIT GL_MAP_WRITE_BIT, GL_MAP_PERSISTENT_BIT, GL_MAP_COHERENT_BIT, and GL_CLIENT_STORAGE_BIT.
        GLenum usage = 0;
        glNamedBufferStorage(
            m_id,   // GLuint buffer
            size.v, // GLsizeiptr size
            data.v, // const void *data
            usage // GLenum usage
        );

        return true;
    }

    void clean(){

        if(m_id == 0){
            return;
        }
        glDeleteBuffers(1, &m_id);
        m_id = 0;
    }

    static void unbind(){
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }

private:
    GLuint m_id = 0;

};


}


