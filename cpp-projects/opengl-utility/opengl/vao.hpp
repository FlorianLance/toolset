
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

namespace tool::gl {


struct VAO{

    inline static GLuint currentId = 0;

    ~VAO(){
        clean();
    }

    GLuint id()const{
        return m_id;
    }

    void generate(){
        if(m_id != 0){
            std::cerr << "VAO already generated: " << m_id << "\n";
            return;
        }
        glCreateVertexArrays(1, &m_id);
    }

    bool bind() const{

        if(m_id == 0){
            std::cerr << "VAO not generated, cannot bind it.\n";
            return false;
        }

        if(currentId == m_id){
            return true;
        }

        glBindVertexArray(m_id);
        currentId = m_id;

        return true;
    }

    void clean(){

        if(m_id == 0){
            return;
        }

        glDeleteVertexArrays(1, &m_id);
        m_id = 0;
    }

    static void unbind(){

        if(currentId == 0){
            return;
        }
        glBindVertexArray(currentId = 0);
    }

private:

    GLuint m_id = 0;
};
}
