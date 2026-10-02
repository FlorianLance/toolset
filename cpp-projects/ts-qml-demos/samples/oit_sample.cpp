
#include "oit_sample.hpp"

using namespace tool::geo;
using namespace tool::gl;

auto OitSample::initialize() -> bool{

    qDebug() << "INIT " << m_width << m_height;
    QString solidVsStr;
    QString solidFsStr;

    // solid
    {
        QFile file1(":/shader/learn/oit/solid.vs");
        file1.open(QIODevice::ReadOnly);
        QFile file2(":/shader/learn/oit/solid.fs");
        file2.open(QIODevice::ReadOnly);
        std::array source = {
            std::make_tuple(ShaderType::vertex,     file1.readAll().toStdString()),
            std::make_tuple(ShaderType::fragment,   file2.readAll().toStdString())
        };
        if(!solidShader.load_from_source_code(source)){
            QtLog::error(u"Cannot load solid shader.");
        }
    }

    // transparent
    {
        QFile file1(":/shader/learn/oit/transparent.vs");
        file1.open(QIODevice::ReadOnly);
        QFile file2(":/shader/learn/oit/transparent.fs");
        file2.open(QIODevice::ReadOnly);
        std::array source = {
            std::make_tuple(ShaderType::vertex,     file1.readAll().toStdString()),
            std::make_tuple(ShaderType::fragment,   file2.readAll().toStdString())
        };
        if(!transparentShader.load_from_source_code(source)){
            QtLog::error(u"Cannot load transparent shader.");
        }
    }

    // composite
    {
        QFile file1(":/shader/learn/oit/composite.vs");
        file1.open(QIODevice::ReadOnly);
        QFile file2(":/shader/learn/oit/composite.fs");
        file2.open(QIODevice::ReadOnly);
        std::array source = {
            std::make_tuple(ShaderType::vertex,     file1.readAll().toStdString()),
            std::make_tuple(ShaderType::fragment,   file2.readAll().toStdString())
        };
        if(!compositeShader.load_from_source_code(source)){
            QtLog::error(u"Cannot load composite shader.");
        }
    }

    // screen
    {
        QFile file1(":/shader/learn/oit/screen.vs");
        file1.open(QIODevice::ReadOnly);
        QFile file2(":/shader/learn/oit/screen.fs");
        file2.open(QIODevice::ReadOnly);
        std::array source = {
            std::make_tuple(ShaderType::vertex,     file1.readAll().toStdString()),
            std::make_tuple(ShaderType::fragment,   file2.readAll().toStdString())
        };
        if(!screenShader.load_from_source_code(source)){
            QtLog::error(u"Cannot load screen shader.");
        }
    }



    float quadVertices[] = {
        // positions        // uv
        -1.0f, -1.0f, 0.0f,	0.0f, 0.0f,
        1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
        1.0f,  1.0f, 0.0f, 1.0f, 1.0f,

        1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
        -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
        -1.0f, -1.0f, 0.0f, 0.0f, 0.0f
    };

    // quad VAO
    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &quadVBO);
    glBindVertexArray(quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glBindVertexArray(0);

    // set up framebuffers
    // GL::create_framebuffers(1, &opaqueFBO);
    // GL::create_framebuffers(1, &transparentFBO);
    opaqueFBO.initialize();
    transparentFBO.initialize();

    // set up attachments for opaque framebuffer
    // opaqueTexture.initialize();
    // opaqueTexture.init_data_f16(m_width, m_height, 1, 4);
    // opaqueTexture.bind(0);

    // opaqueTexture.init_data_f32()

    glGenTextures(1, &opaqueTexture);
    glBindTexture(GL_TEXTURE_2D, opaqueTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, m_width, m_height, 0, GL_RGBA, GL_HALF_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);

    // depthTexture.initialize();
    // opaqueTexture.bind(0);
    glGenTextures(1, &depthTexture);
    glBindTexture(GL_TEXTURE_2D, depthTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, m_width, m_height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glBindTexture(GL_TEXTURE_2D, 0);

    opaqueFBO.bind();
    // opaqueFBO.attach_color0_texture()

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, opaqueTexture, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthTexture, 0);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE){
        QtLog::error(u"ERROR::FRAMEBUFFER:: Opaque framebuffer is not complete!");
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);


    // set up attachments for transparent framebuffer
    glGenTextures(1, &accumTexture);
    glBindTexture(GL_TEXTURE_2D, accumTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, m_width, m_height, 0, GL_RGBA, GL_HALF_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);

    glGenTextures(1, &revealTexture);
    glBindTexture(GL_TEXTURE_2D, revealTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, m_width, m_height, 0, GL_RED, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);

    transparentFBO.bind();
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, accumTexture, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, revealTexture, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthTexture, 0); // opaque framebuffer's depth texture

    const GLenum transparentDrawBuffers[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
    glDrawBuffers(2, transparentDrawBuffers);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE){
        QtLog::error(u"ERROR::FRAMEBUFFER:: Transparent framebuffer is not complete!");
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    sphereD.initialize(0.5f);

    return true;
}

auto OitSample::clean() -> void{

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    glDeleteVertexArrays(1, &quadVAO);
    glDeleteBuffers(1, &quadVBO);
    glDeleteTextures(1, &opaqueTexture);
    glDeleteTextures(1, &depthTexture);
    glDeleteTextures(1, &accumTexture);
    glDeleteTextures(1, &revealTexture);

    // opaqueTexture.clean();
    // depthTexture.clean();

    opaqueFBO.clean();
    transparentFBO.clean();

}

auto OitSample::draw(CommonUniforms &cUniforms) -> void{

    GLuint currentFBO = GL::get_current_framebuffer();

    qDebug() << "DRAW " << currentFBO << opaqueFBO.id() << transparentFBO.id();
    cUniforms.mvpStr = "mvp";

    auto vp = cUniforms.projection * cUniforms.view;

    // draw solid objects (solid pass)
    // ------

    // configure render states
    GL::enable(GL_DEPTH_TEST);
    GL::depth_func(GL_LESS);
    GL::depth_mask(GL_TRUE);
    GL::disable(GL_BLEND);

    GLfloat clearColor[] = {0.0f, 0.0f, 0.0f, 0.0f};
    GL::clear_named_framebuffer_fv(currentFBO, GL_COLOR, 0, clearColor);

    // bind opaque framebuffer to render solid objects
    GLfloat clearDepth = 1.f;
    GL::clear_named_framebuffer_fv(opaqueFBO.id(), GL_COLOR, 0, clearColor);
    GL::clear_named_framebuffer_fv(opaqueFBO.id(), GL_DEPTH, 0, &clearDepth);


    opaqueFBO.bind();
    // glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // use solid shader
    solidShader.use();

    // draw red quad
    solidShader.set_uniform_matrix("mvp",   vp * redModelMat);
    solidShader.set_uniform("color", Vec3f{1.0f, 0.0f, 0.0f});
    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    // draw transparent objects (transparent pass)
    // -----

    // configure render states
    GL::depth_mask(GL_FALSE);
    GL::enable(GL_BLEND);
    GL::blend_func_separate_i(0, GL_ONE, GL_ONE, GL_ONE, GL_ONE);
    GL::blend_func_separate_i(1, GL_ZERO, GL_ONE_MINUS_SRC_COLOR, GL_ZERO, GL_ONE_MINUS_SRC_COLOR);
    GL::blend_equation_separate(GL_FUNC_ADD, GL_FUNC_ADD);

    // bind transparent framebuffer to render transparent objects
    GL::clear_named_framebuffer_fv(transparentFBO.id(), GL_COLOR, 0, &zeroFillerVec[0]);
    GL::clear_named_framebuffer_fv(transparentFBO.id(), GL_COLOR, 1, &oneFillerVec[0]);
    GL::bind_framebuffer(GL_FRAMEBUFFER, transparentFBO.id());

    // use transparent shader
    transparentShader.use();

    // draw green quad
    transparentShader.set_uniform_matrix("mvp",   vp * greenModelMat);
    transparentShader.set_uniform("color",  Vec4f{0.0f, 1.0f, 0.0f, 0.5f});
    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    // draw blue quad
    transparentShader.set_uniform_matrix("mvp",   vp * blueModelMat);
    transparentShader.set_uniform("color",  Vec4f{0.0f, 0.0f, 1.0f, 0.5f});
    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);


    // draw composite image (composite pass)
    // -----

    // set render states
    GL::depth_func(GL_ALWAYS);
    GL::enable(GL_BLEND);
    GL::blend_func_separate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // bind opaque framebuffer
    opaqueFBO.bind();

    // use composite shader
    compositeShader.use();

    // draw screen quad
    GL::bind_textures<2>(0, {accumTexture,revealTexture});

    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    // draw to backbuffer (final pass)
    // -----

    // set render states
    GL::disable(GL_DEPTH_TEST);
    GL::depth_mask(GL_TRUE); // enable depth writes so glClear won't ignore clearing the depth buffer
    GL::disable(GL_BLEND);

    GL::dsa_clear_color(currentFBO, {0.0f, 0.0f, 0.0f, 0.0f});
    GL::dsa_clear_depth_stencil(currentFBO, 1.f, 0);
    // bind backbuffer
    GL::bind_framebuffer(GL_FRAMEBUFFER, currentFBO);

    // use screen shader
    screenShader.use();

    // draw final screen quad
    GL::bind_textures<1>(0, {opaqueTexture});
    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}
