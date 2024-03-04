

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
#include "opengl/shader/shader_program.hpp"

namespace tool::gl {

struct UBO{

    UBO() = default;
    UBO(const UBO&) = delete;
    UBO& operator=(const UBO&) = delete;
    UBO(UBO&& other) = default;
    UBO& operator=(UBO&& other) = default;
    ~UBO();

    [[nodiscard]] constexpr auto id() const noexcept -> GLuint{return m_handle;}
    [[nodiscard]] constexpr auto size() const noexcept -> GLsizeiptr{return m_size;}
    [[nodiscard]] virtual auto get_block_name() const -> std::string{return "";}
    [[nodiscard]] virtual auto get_elements_name() const -> std::vector<std::string>{return {};}

    auto generate() -> void;
    auto clean() -> void;

    auto bind(GLuint index) -> void;
    static auto bind_range(const std::vector<UBO> &UBOs, GLuint first) -> void; // TEST

    auto set_data_storage(GLsizeiptr sizeData, void *data = nullptr, GLenum usage = GL_DYNAMIC_STORAGE_BIT) -> void;    
    auto set_data_space_from_shader(ShaderProgram *shader, GLenum usage = GL_DYNAMIC_STORAGE_BIT) -> void;
    auto update_data(void *data, GLsizeiptr sizeData, GLintptr offset = 0) -> void;

protected:

    GLuint m_handle = 0;
    GLsizeiptr m_size = 0;

    std::vector<GLubyte> m_blockBuffer;
    std::vector<GLint> m_offsets;
};


}

//struct GlobalData {
//    mat4 projection;
//    mat4 view;
//    mat4 projectionView;
//    vec3 camPos;
//    vec3 lightPos;
//} globalData;
//glBindBuffer(GL_UNIFORM_BUFFER, ubo);
//glBufferData(GL_UNIFORM_BUFFER, sizeof(GlobalData), &globalData, GL_DYNAMIC_DRAW);
//glBindBuffer(GL_UNIFORM_BUFFER, 0);

//glUniformBlockBinding(shader.GetProgram(), glGetUniformBlockIndex(shader.GetProgram(), "GlobalData"), 0);

//layout (std140) uniform GlobalData {
//    mat4 projection;
//    mat4 view;
//    mat4 projectionView;
//    vec3 camPos;
//    vec3 lightPos;
//};

//struct GlobalData {
//    mat4 projection;
//    mat4 view;
//    mat4 projectionView;
//    alignas(sizeof(float)*4) vec3 camPos;
//    alignas(sizeof(float)*4) vec3 lightPos;
//} globalData;


//#define MAX_NUM_TOTAL_LIGHTS 100
//struct Light {
//    vec3 position;
//    float padding;
//}
//layout (std140) uniform Lights {
//    Light light[MAX_NUM_TOTAL_LIGHTS];
//    int numLights;
//}



//Sample Fragment Shader (UBOs require OpenGL 3.1):

//#version 140 // GL 3.1

//      // Arrays in a UBO must use a constant expression for their size.
//      const int MY_ARRAY_SIZE = 512;

//// The name of the block is used for finding the index location only
//layout (std140) uniform myArrayBlock {
//    int myArray [MY_ARRAY_SIZE]; // This is the important name (in the shader).
//};

//void main (void) {
//    gl_FragColor = vec4 ((float)myArray [0] * 0.1, vec3 (1.0));
//}


//OpenGL Code

//    const int MY_ARRAY_SIZE = 512;

//GLuint myArrayUBO;
//glGenBuffers (1, &myArrayUBO);

//// Allocate storage for the UBO
//glBindBuffer (GL_UNIFORM_BUFFER, myArrayUBO);
//glBufferData (GL_UNIFORM_BUFFER, sizeof (GLint) * MY_ARRAY_SIZE,
//             NULL, GL_DYNAMIC_DRAW);

//[...]

//    // When you want to update the data in your UBO, you do it like you would any
//    //   other buffer object.
//    glBufferSubData (GL_UNIFORM_BUFFER, ...);

//[...]

//    GLuint myArrayBlockIdx = glGetUniformBlockIndex (GLSLProgramID, "myArrayBlock");

//glUniformBlockBinding (GLSLProgramID,     myArrayBlockIdx, 0);
//glBindBufferBase      (GL_UNIFORM_BUFFER, 0,               myArrayUBO);

// https://computergraphics.stackexchange.com/questions/4454/opengl-es-3-uniform-buffer-object-with-float-array
// https://www.khronos.org/opengl/wiki/Interface_Block_(GLSL)
