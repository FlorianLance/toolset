
#pragma once

// gl
#include "opengl/gl_functions.hpp"

// Qt
#include <QQuickFramebufferObject>
#include <QOpenGLFramebufferObject>

// gl
#include "opengl/draw/points_drawers.hpp"
#include "opengl/draw/lines_drawers.hpp"
#include "opengl/draw/triangles_drawers.hpp"

// local
#include "common.hpp"
#include "scenes/base_scene.hpp"
#include "base_sample.hpp"

class SampleRenderer : public QQuickFramebufferObject::Renderer {
public:

    SampleRenderer();
    ~SampleRenderer();

    void synchronize(QQuickFramebufferObject *item) override;
    void render() override;

    QOpenGLFramebufferObject *createFramebufferObject(const QSize &size) override;

protected:

    virtual auto initialize_gl() -> void;

protected:

    CommonUniforms cUniforms;
    std::shared_ptr<tool::gl::BaseScene> currentScene = nullptr;

    bool initSample = false;
    tool::gl::BaseGlSample *currentSample = nullptr;

    // to remove
    tool::gl::ShaderProgram solidShaderP;
    tool::gl::GridLinesDrawer gridD;
    tool::geo::Pt4f m_backgoundColor = {0.2f, 0.3f, 0.3f, 1.0f};
    bool m_initialized = false;
};

