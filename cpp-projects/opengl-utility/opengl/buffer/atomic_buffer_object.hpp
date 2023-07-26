
#pragma once


// local
#include "opengl/utility/gl_utility.hpp"

//    static void bind_ranges(std::vector<GLuint> &buffers, GLuint first, std::vector<GLintptr> offsets = {}){

//        glBindBuffersRange(
//            GL_ATOMIC_COUNTER_BUFFER,               // GLenum target,
//            first,                                  // GLuint first,
//            static_cast<GLsizei>(buffers.size()),   // GLsizei count,
//            buffers.data(),                         // const GLuint *buffers,
//            &offset,                                    // const GLintptr *offsets,
//            &size
//        );
//    }


// glMapBufferRange and glMapNamedBufferRange map all or part of the data store of a specified buffer object into the client's address space. offset and length indicate the range of data in the buffer object that is to be mapped, in terms of basic machine units. access is a bitfield containing flags which describe the requested mapping. These flags are described below.
//     A pointer to the beginning of the mapped range is returned once all pending operations on the buffer object have completed, and may be used to modify and/or query the corresponding range of the data store according to the following flag bits set in access:
//     GL_MAP_READ_BIT indicates that the returned pointer may be used to read buffer object data. No GL error is generated if the pointer is used to query a mapping which excludes this flag, but the result is undefined and system errors (possibly including program termination) may occur.
//     GL_MAP_WRITE_BIT indicates that the returned pointer may be used to modify buffer object data. No GL error is generated if the pointer is used to modify a mapping which excludes this flag, but the result is undefined and system errors (possibly including program termination) may occur.
//     GL_MAP_PERSISTENT_BIT indicates that the mapping is to be made in a persistent fashion and that the client intends to hold and use the returned pointer during subsequent GL operation. It is not an error to call drawing commands (render) while buffers are mapped using this flag. It is an error to specify this flag if the buffer's data store was not allocated through a call to the glBufferStorage command in which the GL_MAP_PERSISTENT_BIT was also set.
//     GL_MAP_COHERENT_BIT indicates that a persistent mapping is also to be coherent. Coherent maps guarantee that the effect of writes to a buffer's data store by either the client or server will eventually become visible to the other without further intervention from the application. In the absence of this bit, persistent mappings are not coherent and modified ranges of the buffer store must be explicitly communicated to the GL, either by unmapping the buffer, or through a call to glFlushMappedBufferRange or glMemoryBarrier.
//     The following optional flag bits in access may be used to modify the mapping:
//     GL_MAP_INVALIDATE_RANGE_BIT indicates that the previous contents of the specified range may be discarded. Data within this range are undefined with the exception of subsequently written data. No GL error is generated if subsequent GL operations access unwritten data, but the result is undefined and system errors (possibly including program termination) may occur. This flag may not be used in combination with GL_MAP_READ_BIT.
//     GL_MAP_INVALIDATE_BUFFER_BIT indicates that the previous contents of the entire buffer may be discarded. Data within the entire buffer are undefined with the exception of subsequently written data. No GL error is generated if subsequent GL operations access unwritten data, but the result is undefined and system errors (possibly including program termination) may occur. This flag may not be used in combination with GL_MAP_READ_BIT.
//     GL_MAP_FLUSH_EXPLICIT_BIT indicates that one or more discrete subranges of the mapping may be modified. When this flag is set, modifications to each subrange must be explicitly flushed by calling glFlushMappedBufferRange. No GL error is set if a subrange of the mapping is modified and not flushed, but data within the corresponding subrange of the buffer are undefined. This flag may only be used in conjunction with GL_MAP_WRITE_BIT. When this option is selected, flushing is strictly limited to regions that are explicitly indicated with calls to glFlushMappedBufferRange prior to unmap; if this option is not selected glUnmapBuffer will automatically flush the entire mapped range when called.
//     GL_MAP_UNSYNCHRONIZED_BIT indicates that the GL should not attempt to synchronize pending operations on the buffer prior to returning from glMapBufferRange or glMapNamedBufferRange. No GL error is generated if pending operations which source or modify the buffer overlap the mapped region, but the result of such previous and any subsequent operations is undefined.

// glMapBufferRange
//        auto data = glMapNamedBufferRange(
//            buffers[COUNTER_BUFFER], // GLuint buffer     : Specifies the name of the buffer object for glMapNamedBufferRange.
//            0, // GLintptr offset   : Specifies the starting offset within the buffer of the range to be mapped.
//            1, // GLsizeiptr length : Specifies the length of the range to be mapped.
//            GL_MAP_WRITE_BIT// GLbitfield access : Specifies a combination of access flags indicating the desired access to the mapped range.
//        );
//        reinterpret_cast<GLuint*>(data)[0] = 0;

//        glMapNamedBufferRange(
//            buffers[COUNTER_BUFFER], // GLuint buffer,
//            0,                       // GLintptr offset,
//            1,                       // GLsizeiptr length,
//        //            GLbitfield access
//        );

namespace tool::gl{

struct ABO{

    ABO() = default;
    ABO(const ABO&) = delete;
    ABO& operator=(const ABO&) = delete;
    ABO(ABO&& other) = default;
    ABO& operator=(ABO&& other) = default;

    ~ABO(){
        clean();
    }

    constexpr GLuint id() const{return m_id;}
    constexpr GLsizeiptr size() const{return m_size;}

    void generate(){

        if(m_id != 0){
            std::cerr << "[GL] ABO already generated: " << m_id << "\n";
            return;
        }
        glCreateBuffers(1, &m_id);
    }

    bool bind_to_index(GLuint index) const{

        if(m_id == 0){
            std::cerr << "[GL] ABO not generated, cannot bind it.\n";
            return false;
        }

        glBindBufferBase(
            GL_ATOMIC_COUNTER_BUFFER,
            index,
            m_id
        );

        return true;
    }

    void set_data_storage(GLsizeiptr sizeData, GLenum usage = 0){

        glNamedBufferStorage(
            m_id,               // GLuint buffer,
            m_size = sizeData,  // GLsizeiptr size,
            nullptr,            // const void *data,
            usage               // GLbitfield flags
        );
    }

    void update_data(GLuint *data, GLsizeiptr sizeData, GLintptr offset = 0){

        if(m_size == 0){
            std::cerr << "[GL] ABO no storage initialized.\n";
            return;
        }

        if(sizeData > m_size){
            std::cerr << "[GL] ABO size is bigger than storage.\n";
            return;
        }

        glNamedBufferSubData(
            m_id,       // GLuint buffer: Specifies the name of the buffer object for glNamedBufferSubData.
            offset,     // GLintptr offset: Specifies the offset into the buffer object's data store where data replacement will begin, measured in bytes.
            sizeData,   // GLsizeiptr size: Specifies the size in bytes of the data store region being replaced.
            data        // const void *data: pecifies a pointer to the new data that will be copied into the data store.
        );
    }


    void clean(){

        if(m_id == 0){
            return;
        }

        glDeleteBuffers(1, &m_id);
        m_id = 0;
    }

private:

    GLuint m_id = 0;
    GLsizeiptr m_size = 0;
};
}

