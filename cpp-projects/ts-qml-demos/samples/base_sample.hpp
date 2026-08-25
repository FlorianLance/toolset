

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

// local
#include "scenes/base_scene.hpp"
#include "samples/common.hpp"

#include "opengl/draw/triangles_drawers.hpp"

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
    virtual auto draw( CommonUniforms &cUniforms) -> void{static_cast<void>(cUniforms);}
    virtual auto update() -> void{}

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

        // shader.set_uniform_matrix("ModelViewMatrix",    cUniforms.mv.conv<float>(), true);
        shader.set_uniform_matrix("NormalMatrix",       cUniforms.normal.conv<float>(), true);
        // shader.set_uniform_matrix("MVP",                cUniforms.mvp.conv<float>(), true);
        shader.set_uniform_matrix("MVP",                (cUniforms.projection*cUniforms.view*cUniforms.model).conv<float>(), true);

        // shader.set_uniform_matrix("model",      geo::transform(geo::Pt3f{1.f,1.f,1.f},m_mobileLightPos1.xyz(),{}));
        sphereD.draw();
    }

    Q_INVOKABLE void test2(int a){
        QtLog::message(QString("[TEST] %1 %2 %3\n").arg(QString::number(m_color.redF()),QString::number(m_color.greenF()),QString::number(m_color.blueF())));
    }

private:
    QColor m_color = Qt::blue;
};

}