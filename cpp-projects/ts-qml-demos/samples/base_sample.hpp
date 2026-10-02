

#pragma once

// Qt
#include <QObject>
#include <QtQml>

// base
#include "geometry/camera.hpp"
#include "geometry/screen.hpp"

// opengl
#include "opengl/texture/textures_manager.hpp"
#include "opengl/shader/shaders_manager.hpp"
#include "opengl/gl_material.hpp"
#include "opengl/buffer/framebuffer_object.hpp"

// local
#include "scenes/base_scene.hpp"
#include "samples/common.hpp"

#include "opengl/draw/triangles_drawers.hpp"
#include "opengl/gl_functions.hpp"

#include <QDebug>

#include "qt_logger.hpp"

namespace tool::gl{

class BaseGlSample : public QObject{
Q_OBJECT

public:
    BaseGlSample(){
        m_shadersM  = graphics::ShadersManager::get_instance();
        m_texturesM = gl::TexturesManager::get_instance();
    }

    virtual ~BaseGlSample(){}
    virtual auto initialize() -> bool{return false;}
    virtual auto clean() -> void{}
    virtual auto draw( CommonUniforms &cUniforms) -> void{static_cast<void>(cUniforms);}
    virtual auto update() -> void{}
    virtual auto resize(size_t width, size_t height) -> void{
        m_width = width;
        m_height = height;
    }

    auto reload_current_shader() -> void{}


    Q_INVOKABLE void test(int a){
        QtLog::message(u"[TEST].\n");
    }

protected:

    virtual auto draw_scene() -> void{}

private:

    auto draw_common_elements() -> void{}

protected:

    graphics::ShadersManager *m_shadersM   = nullptr;
    gl::TexturesManager *m_texturesM  = nullptr;

    tool::gl::ShaderProgram shader;

    std::shared_ptr<ShaderProgram> m_shader = nullptr;
    std::shared_ptr<BaseScene> m_scene = nullptr;

    geo::Pt4f m_mobileLightPos1 = geo::Pt4f(5.f, 5.f, 5.f, 1.f);

    size_t m_width = 0;
    size_t m_height = 0;
};

class FlatGlSample : public BaseGlSample{
    Q_OBJECT
public:

    tool::gl::SphereTrianglesDrawer sphereD;
    tool::gl::LightUBO lightUBO;
    gl::MaterialUBO materialUBO;

    img::LightInfo lInfo = {{},{0.4f, 0.4f, 0.4f},{1.0f, 1.0f, 1.0f},{1.0f, 1.0f, 1.0f}};
    img::MaterialInfo mInfo = {{0.5f, 0.5f, 0.5f},{0.5f, 0.5f, 0.5f},{0.8f, 0.8f, 0.8f},10.0f};
    geo::Pt4f worldLight{0.f,10.f,0.f,1.0f};

    auto initialize() -> bool final override{

        QString solidVsStr;
        QString solidFsStr;

        {
            QFile file(":/shader/ch3/flat.vs");
            file.open(QIODevice::ReadOnly);
            solidVsStr = file.readAll();
        }

        {
            QFile file(":/shader/ch3/flat.fs");
            file.open(QIODevice::ReadOnly);
            solidFsStr = file.readAll();
        }

        std::array solidShaderSource = {
            std::make_tuple(ShaderType::vertex,   solidVsStr.toStdString()),
            std::make_tuple(ShaderType::fragment, solidFsStr.toStdString())
        };
        if(!shader.load_from_source_code(solidShaderSource)){
            QtLog::error(u"Cannot load solid shader.");
            return false;
        }

        sphereD.initialize(0.5f);

        lightUBO.clean();
        lightUBO.initialize();
        lightUBO.set_data_space_from_shader(&shader);

        materialUBO.clean();
        materialUBO.initialize();
        materialUBO.set_data_space_from_shader(&shader);


        return true;
    }

    auto draw( CommonUniforms &cUniforms) -> void final override{

        lInfo.Position = cUniforms.view.multiply_point(worldLight);

        shader.use();

        lightUBO.update(lInfo);
        lightUBO.bind(0);

        materialUBO.update(mInfo);
        materialUBO.bind(1);


        shader.set_uniform_matrix("view",       cUniforms.view);
        shader.set_uniform_matrix("projection", cUniforms.projection);

        cUniforms.model = geo::Mat4f::identity();// geo::transform(geo::Pt3f{1.f,1.f,1.f},m_mobileLightPos1.xyz(),{});
        cUniforms.update();
        shader.set_uniform_matrix("model",    cUniforms.model);
        shader.set_uniform("camPosition",    cUniforms.camPosition);
        shader.set_uniform_matrix("ModelViewMatrix",    (cUniforms.view*cUniforms.model).conv<float>(), true);

        shader.set_uniform_matrix("NormalMatrix",       cUniforms.normal.conv<float>(), true);
        shader.set_uniform_matrix("MVP",                (cUniforms.projection*cUniforms.view*cUniforms.model).conv<float>(), true);

        sphereD.draw();
    }

};

class EdgeDetectionGlSample : public BaseGlSample{

    tool::gl::QuadTrianglesDrawer quadD;
    tool::gl::SphereTrianglesDrawer sphereD;

    gl::LightUBO lightUBO;
    gl::MaterialUBO materialUBO;

    bool enable = true;
    float edgeThreshold = 0.05f;
    // screen FBO
    gl::FBO screenFBO;
    gl::RBO screenDepthBuffer;
    gl::Texture2D screenRenderTexture;

    img::LightInfo lInfo = {{},{0.4f, 0.4f, 0.4f},{1.0f, 1.0f, 1.0f},{1.0f, 1.0f, 1.0f}};
    img::MaterialInfo mInfo = {{0.5f, 0.5f, 0.5f},{0.5f, 0.5f, 0.5f},{0.8f, 0.8f, 0.8f},10.0f};

    auto initialize() -> bool final override{

        QString solidVsStr;
        QString solidFsStr;

        {
            QFile file(":/shader/ch6/edge-detection-filter.vs");
            file.open(QIODevice::ReadOnly);
            solidVsStr = file.readAll();
        }

        {
            QFile file(":/shader/ch6/edge-detection-filter.fs");
            file.open(QIODevice::ReadOnly);
            solidFsStr = file.readAll();
        }

        std::array solidShaderSource = {
            std::make_tuple(ShaderType::vertex,     solidVsStr.toStdString()),
            std::make_tuple(ShaderType::fragment,   solidFsStr.toStdString())
        };
        if(!shader.load_from_source_code(solidShaderSource)){
            QtLog::error(u"Cannot load solid shader.");
        }


        // lightUBO.initialize();
        // lightUBO.set_data_space_from_shader(&shader);
        // materialUBO.initialize();
        // materialUBO.set_data_space_from_shader(&shader);

        sphereD.initialize(0.5f);
        quadD.initialize(false);
        QtLog::message(u"INITIALIZED");


        // Generate and bind the framebuffer
        screenFBO.clean();
        screenFBO.initialize();
        screenFBO.bind();

        // Create the texture object
        screenRenderTexture.clean();
        screenRenderTexture.init_render(1920,1080);

        TextureOptions options;
        options.minFilter = TextureMinFilter::nearest;
        options.magFilter = TextureMagFilter::nearest;
        options.maxLevel = 0;
        screenRenderTexture.set_texture_options(options);

        // Create the depth buffer
        screenDepthBuffer.clean();
        screenDepthBuffer.initialize();
        screenDepthBuffer.bind();
        screenDepthBuffer.set_data_storage(1920,1080);

        // Bind the texture to the FBO
        screenFBO.attach_color0_texture(screenRenderTexture);

        // Bind the depth buffer to the FBO
        screenFBO.attach_depth_buffer(screenDepthBuffer);

        // set colors buffers to be drawn
        screenFBO.set_draw_buffers({
            FrameBuffer::Color0
        });
        screenFBO.check_validity();

        // Unbind the framebuffer, and revert to default framebuffer
        screenFBO.unbind();

        return true;
    }
    auto draw( CommonUniforms &cUniforms) -> void final override{

        // pass 1 : blinnphong
        // if(enable){
            screenFBO.bind();
            GL::enable(GL_DEPTH_TEST);
            GL::clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        // }

        shader.use();
        shader.set_uniform("Pass", 1);
        shader.set_uniform("EdgeThreshold", edgeThreshold);

        lInfo.Position = cUniforms.view.multiply_point(m_mobileLightPos1);

        // lightUBO.update(lInfo);
        // lightUBO.bind(0);

        // materialUBO.update(mInfo);
        // materialUBO.bind(1);

        shader.set_uniform_matrix("view",       cUniforms.view);
        shader.set_uniform_matrix("projection", cUniforms.projection);

        cUniforms.model = geo::Mat4f::identity();// geo::transform(geo::Pt3f{1.f,1.f,1.f},m_mobileLightPos1.xyz(),{});
        cUniforms.update();
        shader.set_uniform_matrix("model",    cUniforms.model);
        shader.set_uniform("camPosition",    cUniforms.camPosition);
        shader.set_uniform_matrix("ModelViewMatrix",    (cUniforms.view*cUniforms.model).conv<float>(), true);

        shader.set_uniform_matrix("NormalMatrix",       cUniforms.normal.conv<float>(), true);
        shader.set_uniform_matrix("MVP",                (cUniforms.projection*cUniforms.view*cUniforms.model).conv<float>(), true);

        sphereD.draw();

        // pass 2
        glFlush();

        gl::FBO::unbind();
        screenRenderTexture.bind(0);

        GL::disable(GL_DEPTH_TEST);
        GL::clear(GL_COLOR_BUFFER_BIT);

        shader.set_uniform("Pass", 2);

        shader.set_uniform_matrix("view",       geo::Mat4f::identity());
        shader.set_uniform_matrix("projection", geo::Mat4f::identity());

        cUniforms.model = geo::Mat4f::identity();// geo::transform(geo::Pt3f{1.f,1.f,1.f},m_mobileLightPos1.xyz(),{});
        cUniforms.update();
        shader.set_uniform_matrix("model",          geo::Mat4f::identity());
        shader.set_uniform("camPosition",           cUniforms.camPosition);
        shader.set_uniform_matrix("ModelViewMatrix",  geo::Mat4f::identity(), true);

        shader.set_uniform_matrix("NormalMatrix",   cUniforms.normal.conv<float>(), true);
        shader.set_uniform_matrix("MVP",            geo::Mat4f::identity(), true);

        quadD.draw();

        // draw_screen_quad(sampleShader.get());


    }
};


class SilhouetteSample : public BaseGlSample{
    Q_OBJECT
public:

    tool::gl::SphereTrianglesDrawer sphereD;

    auto initialize() -> bool final override{

        QString solidVsStr;
        QString solidGsStr;
        QString solidFsStr;

        {
            QFile file(":/shader/ch7/silhouette.vs");
            file.open(QIODevice::ReadOnly);
            solidVsStr = file.readAll();
        }
        {
            QFile file(":/shader/ch7/silhouette.gs");
            file.open(QIODevice::ReadOnly);
            solidGsStr = file.readAll();
        }
        {
            QFile file(":/shader/ch7/silhouette.fs");
            file.open(QIODevice::ReadOnly);
            solidFsStr = file.readAll();
        }

        std::array solidShaderSource = {
            std::make_tuple(ShaderType::vertex,     solidVsStr.toStdString()),
            std::make_tuple(ShaderType::geometry,   solidGsStr.toStdString()),
            std::make_tuple(ShaderType::fragment,   solidFsStr.toStdString())
        };
        if(!shader.load_from_source_code(solidShaderSource)){
            QtLog::error(u"Cannot load solid shader.");
        }


        shader.set_uniform("EdgeWidth", 0.015f);
        shader.set_uniform("PctExtend", 0.25f);
        shader.set_uniform("LineColor", geo::Pt4f(0.05f,0.0f,0.05f,1.0f));
        shader.set_uniform("Material.Kd", geo::Pt3f(0.7f, 0.5f, 0.2f));
        shader.set_uniform("Material.Ka", geo::Pt3f(0.2f, 0.2f, 0.2f));
        shader.set_uniform("Light.Intensity", geo::Pt3f(1.0f, 1.0f, 1.0f));

       sphereD.initialize(0.5f);
        QtLog::message(u"INITIALIZED");
        // return (m_shader = m_shadersM->get_shader("ch3/diffuse").lock()) != nullptr;
        return true;
    }

    auto draw( CommonUniforms &cUniforms) -> void final override{

        GL::enable(GL_DEPTH_TEST);

        shader.use();

        shader.set_uniform("Light.Position", cUniforms.view.multiply_point(m_mobileLightPos1));

        shader.set_uniform_matrix("view",       cUniforms.view);
        shader.set_uniform_matrix("projection", cUniforms.projection);

        cUniforms.model = geo::Mat4f::identity();// geo::transform(geo::Pt3f{1.f,1.f,1.f},m_mobileLightPos1.xyz(),{});
        cUniforms.update();
        shader.set_uniform_matrix("model",    cUniforms.model);
        shader.set_uniform("camPosition",    cUniforms.camPosition);
        shader.set_uniform_matrix("ModelViewMatrix",    (cUniforms.view*cUniforms.model).conv<float>(), true);

        shader.set_uniform_matrix("NormalMatrix",       cUniforms.normal.conv<float>(), true);
        shader.set_uniform_matrix("MVP",                (cUniforms.projection*cUniforms.view*cUniforms.model).conv<float>(), true);

        sphereD.draw();

        glFinish();
    }


};


class DiffuseGlSample : public BaseGlSample{
Q_OBJECT
public:

    Q_PROPERTY(QColor color MEMBER m_color)

    tool::gl::SphereTrianglesDrawer sphereD;

    geo::Pt3f kd = geo::Pt3f{0.9f, 0.5f, 0.3f};
    geo::Pt3f ld = geo::Pt3f{0.5f, 0.5f, 0.5f};

    auto initialize() -> bool final override{

        QString solidVsStr;
        QString solidFsStr;

        {
            QFile file(":/shader/ch3/diffuse.vs");
            file.open(QIODevice::ReadOnly);
            solidVsStr = file.readAll();
        }

        {
            QFile file(":/shader/ch3/diffuse.fs");
            file.open(QIODevice::ReadOnly);
            solidFsStr = file.readAll();
        }

        std::array solidShaderSource = {
            std::make_tuple(ShaderType::vertex,   solidVsStr.toStdString()),
            std::make_tuple(ShaderType::fragment, solidFsStr.toStdString())
        };
        if(!shader.load_from_source_code(solidShaderSource)){
            QtLog::error(u"Cannot load solid shader.");
        }




        sphereD.initialize(0.5f);
        QtLog::message(u"INITIALIZED");
        // return (m_shader = m_shadersM->get_shader("ch3/diffuse").lock()) != nullptr;
        return true;
    }

    auto draw( CommonUniforms &cUniforms) -> void final override{

        shader.use();
        shader.set_uniform("Kd", kd);
        shader.set_uniform("Ld", ld);
        shader.set_uniform("LightPosition", cUniforms.view.multiply_point(m_mobileLightPos1));

        shader.set_uniform_matrix("view",       cUniforms.view);
        shader.set_uniform_matrix("projection", cUniforms.projection);

        cUniforms.model = geo::Mat4f::identity();// geo::transform(geo::Pt3f{1.f,1.f,1.f},m_mobileLightPos1.xyz(),{});
        cUniforms.update();
        shader.set_uniform_matrix("model",    cUniforms.model);
        shader.set_uniform("camPosition",    cUniforms.camPosition);
        shader.set_uniform_matrix("ModelViewMatrix",    (cUniforms.view*cUniforms.model).conv<float>(), true);

        shader.set_uniform_matrix("NormalMatrix",       cUniforms.normal.conv<float>(), true);
        shader.set_uniform_matrix("MVP",                (cUniforms.projection*cUniforms.view*cUniforms.model).conv<float>(), true);

        sphereD.draw();
    }

    Q_INVOKABLE void test2(int a){
        QtLog::message(QString("[TEST] %1 %2 %3\n").arg(QString::number(m_color.redF()),QString::number(m_color.greenF()),QString::number(m_color.blueF())));
    }

private:
    QColor m_color = Qt::blue;
};

}