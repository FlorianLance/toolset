
#pragma once

#include "base_sample.hpp"


// opengl
#include "opengl/buffer/framebuffer_object.hpp"
#include "opengl/texture/texture_2d_tbo.hpp"

namespace tool::gl{


class OitSample : public BaseGlSample{
    Q_OBJECT
public:
    tool::gl::SphereTrianglesDrawer sphereD;

    ShaderProgram solidShader;
    ShaderProgram transparentShader;
    ShaderProgram compositeShader;
    ShaderProgram screenShader;

    unsigned int quadVAO, quadVBO;
    // unsigned int opaqueFBO, transparentFBO;
    tool::gl::FBO opaqueFBO;
    tool::gl::FBO transparentFBO;

    // tool::gl::Texture2D opaqueTexture;
    // tool::gl::Texture2D depthTexture;

    unsigned int opaqueTexture;
    unsigned int depthTexture;
    GLuint accumTexture;
    GLuint revealTexture;

    // struct FrameBuffers{
    //     GLuint opaque;
    //     GLuint transparent;

    //     auto span() -> std::span<GLuint>{
    //         return std::span<GLuint>(reinterpret_cast<GLuint*>(this),2);
    //     }
    // };
    // FrameBuffers fb;


    geo::Mat4f redModelMat   = geo::transform(geo::Pt3f{1,1,1},geo::Pt3f{0,0,0},geo::Pt3f{0,0,1});
    geo::Mat4f greenModelMat = geo::transform(geo::Pt3f{1,1,1},geo::Pt3f{0,0,0},geo::Pt3f{0,0,0});
    geo::Mat4f blueModelMat  = geo::transform(geo::Pt3f{1,1,1},geo::Pt3f{0,0,0},geo::Pt3f{0,0,2});


    geo::Vec4f zeroFillerVec;
    geo::Vec4f oneFillerVec = {1.f,1.f,1.f,1.f};
    // int COUNTER_BUFFER = 0;
    // int LINKED_LIST_BUFFER = 1;

    // GLuint buffers[2], fsQuad, headPtrTex;
    // GLuint pass1Index, pass2Index;
    // GLuint clearBuf;

    // // Cube cube;
    // // Sphere sphere;

    // float angle, tPrev, rotSpeed;

    CommonUniforms cUniforms;


    auto initialize() -> bool final override;
    auto clean() -> void final override;
    // auto setMatrices() -> void{
    //     cUniforms.update();
    //     cUniforms.set_uniforms(shader);
    // }

    // auto initShaderStorage() -> void{

    //     qDebug() << "m_width " <<  m_width << m_height;

    //     glGenBuffers(2, buffers);
    //     GLuint maxNodes = 20 * m_width * m_height;
    //     GLint nodeSize = 5 * sizeof(GLfloat) + sizeof(GLuint); // The size of a linked list node

    //     // Our atomic counter
    //     glBindBufferBase(GL_ATOMIC_COUNTER_BUFFER, 0, buffers[COUNTER_BUFFER]);
    //     glBufferData(GL_ATOMIC_COUNTER_BUFFER, sizeof(GLuint), NULL, GL_DYNAMIC_DRAW);

    //     // The buffer for the head pointers, as an image texture
    //     glGenTextures(1, &headPtrTex);
    //     glBindTexture(GL_TEXTURE_2D, headPtrTex);
    //     glTexStorage2D(GL_TEXTURE_2D, 1, GL_R32UI, m_width, m_height);
    //     glBindImageTexture(0, headPtrTex, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32UI);

    //     // The buffer of linked lists
    //     glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, buffers[LINKED_LIST_BUFFER]);
    //     glBufferData(GL_SHADER_STORAGE_BUFFER, maxNodes * nodeSize, NULL, GL_DYNAMIC_DRAW);

    //     shader.set_uniform("MaxNodes", maxNodes);

    //     std::vector<GLuint> headPtrClearBuf(m_width * m_height, 0xffffffff);
    //     glGenBuffers(1, &clearBuf);
    //     glBindBuffer(GL_PIXEL_UNPACK_BUFFER, clearBuf);
    //     glBufferData(GL_PIXEL_UNPACK_BUFFER, headPtrClearBuf.size() * sizeof(GLuint),&headPtrClearBuf[0], GL_STATIC_COPY);
    //     // GL::bind_buffer()
    // }

    auto draw( CommonUniforms &cUniforms) -> void final override;

    // auto clearBuffers() -> void{
    //     GLuint zero = 0;
    //     glBindBufferBase(GL_ATOMIC_COUNTER_BUFFER, 0, buffers[COUNTER_BUFFER] );
    //     glBufferSubData(GL_ATOMIC_COUNTER_BUFFER, 0, sizeof(GLuint), &zero);

    //     glBindBuffer(GL_PIXEL_UNPACK_BUFFER, clearBuf);
    //     glBindTexture(GL_TEXTURE_2D, headPtrTex);
    //     glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, m_width, m_height, GL_RED_INTEGER,GL_UNSIGNED_INT, NULL);
    // }


    // auto pass1() -> void{

    //     glUniformSubroutinesuiv( GL_FRAGMENT_SHADER, 1, &pass1Index);

    //     glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //     // view = glm::lookAt(vec3(11.0f * cos(angle),2.0f,11.0f * sin(angle)), vec3(0.0f,0.0f,0.0f), vec3(0.0f,1.0f,0.0f));

    //     // projection = glm::perspective( glm::radians(50.0f), (float)width/height, 1.0f, 1000.0f);

    //     glDepthMask( GL_FALSE );


    //     // uniform vec4 LightPosition;
    //     // uniform vec3 LightIntensity;
    //     // uniform vec4 Kd;            // Diffuse reflectivity
    //     // uniform vec4 Ka;            // Ambient reflectivity

    //     shader.set_uniform("Light.Position", cUniforms.view.multiply_point(m_mobileLightPos1));

    //     // draw scene
    //     cUniforms.model = geo::Mat4f::identity();

    //     cUniforms.update();
    //     cUniforms.set_uniforms(shader);
    //     sphereD.draw();

    //     glFinish();
    // }

    // auto pass2() -> void{

    //     glMemoryBarrier( GL_SHADER_STORAGE_BARRIER_BIT );

    //     glUniformSubroutinesuiv( GL_FRAGMENT_SHADER, 1, &pass2Index);

    //     glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );


    //     cUniforms.set_indentity_uniforms(shader);

    //     // Draw a screen filler
    //     glBindVertexArray(fsQuad);
    //     glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    //     glBindVertexArray(0);
    // }
};
}