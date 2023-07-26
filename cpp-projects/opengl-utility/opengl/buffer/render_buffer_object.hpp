
#pragma once


// local
#include "opengl/utility/gl_utility.hpp"

namespace tool::gl{

struct RBO{

    RBO() = default;
    RBO(const RBO&) = delete;
    RBO& operator=(const RBO&) = delete;
    RBO(RBO&& other) = default;
    RBO& operator=(RBO&& other) = default;

    ~RBO(){
        clean();
    }

    void generate(){

        if(m_id != 0){
            std::cerr << "RBO already generated: " << m_id << "\n";
            return;
        }
        glCreateRenderbuffers(1, &m_id);
    }

    void bind(){
        glBindRenderbuffer(GL_RENDERBUFFER, m_id);
    }

    void set_multisample_data_storage(GLsizei width = 512, GLsizei height = 512, GLsizei  samples = 4){
        glNamedRenderbufferStorageMultisample(m_id, samples, GL_DEPTH24_STENCIL8, width, height);
    }

    void set_data_storage(GLsizei width = 512, GLsizei height = 512){
        glNamedRenderbufferStorage(m_id, GL_DEPTH24_STENCIL8, width, height);
        // glNamedRenderbufferStorage(m_id, GL_DEPTH_COMPONENT, width, height);
        // GL_DEPTH_COMPONENT16 	GL_DEPTH_COMPONENT 	16
        // GL_DEPTH_COMPONENT24 	GL_DEPTH_COMPONENT 	24
        // GL_DEPTH_COMPONENT32F 	GL_DEPTH_COMPONENT 	f32
        // GL_DEPTH24_STENCIL8 	GL_DEPTH_STENCIL 	24 8
        // GL_DEPTH32F_STENCIL8 	GL_DEPTH_STENCIL 	f32 8
        // GL_STENCIL_INDEX8       GL_STENCIL          8
    }

    static void unbind(){
        glBindRenderbuffer(GL_RENDERBUFFER, 0);
    }


    void clean(){

        if(m_id == 0){
            return;
        }
        glDeleteBuffers(1, &m_id);
        m_id = 0;
    }

    inline GLuint id() const{return m_id;}

private:

    GLuint m_id = 0;
};
}

