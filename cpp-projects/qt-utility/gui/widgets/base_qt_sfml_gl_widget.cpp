

/*******************************************************************************
** Toolset-qt-utility                                                         **
** MIT License                                                                **
** Copyright (c) [2024] [Florian Lance]                                       **
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

#include "base_qt_sfml_gl_widget.hpp"

// Qt
#include <QKeyEvent>

// opengl-utility
#include "opengl/gl_draw.hpp"

#include "opengl/gl_functions.hpp"
#include "utility/logger.hpp"

using namespace tool;
using namespace tool::gl;

BaseQtSfmlGlWidget::BaseQtSfmlGlWidget(QWidget *Parent) : QWidget(Parent), m_camera(&m_screen){

    // rendering in widget
    setAttribute(Qt::WA_PaintOnScreen);
    setAttribute(Qt::WA_OpaquePaintEvent);
    setAttribute(Qt::WA_NoSystemBackground);

    // allow keyboard inputs
    setFocusPolicy(Qt::StrongFocus);
    setMouseTracking(true);

    // events
    this->installEventFilter(this);
}

auto BaseQtSfmlGlWidget::paintEngine() const -> QPaintEngine*{
    return nullptr;
}

auto BaseQtSfmlGlWidget::eventFilter(QObject *object, QEvent *event) -> bool{
    Q_UNUSED(event)
    if(object != this){
        return false;
    }

    // ...

    return false;
}

auto BaseQtSfmlGlWidget::showEvent(QShowEvent *) -> void{

    if(m_windowInitialized){
        return;
    }

    // set context
    sf::ContextSettings settings;
    settings.majorVersion = 4;
    settings.minorVersion = 6;
    settings.depthBits = 24;
    settings.stencilBits = 8;
    settings.antialiasingLevel = 4;
    settings.attributeFlags = sf::ContextSettings::Debug;

    // init window
    sf::Window::create(reinterpret_cast<sf::WindowHandle>(winId()),settings);

    // init glew
    GL::init_glew();
    GL::display_glew_info();

    // init gl
    on_init();

    // connections
    connect(&m_renderTimer, &QTimer::timeout, this, [&]{
        repaint();
    });

    // timer
    m_renderTimer.start(1000/60);
    m_windowInitialized = true;
}

auto BaseQtSfmlGlWidget::paintEvent(QPaintEvent *) -> void{

    if(!m_windowInitialized){
        return;
    }

    on_update();
    on_draw();
    display();
}

auto BaseQtSfmlGlWidget::resizeEvent(QResizeEvent *event) -> void{

    m_screen.resize(static_cast<unsigned int>(event->size().width()),static_cast<unsigned int>(event->size().height()));
    m_camera.update_projection();    

    if(!m_windowInitialized){
        return;
    }

    glViewport(0, 0, static_cast<GLsizei>(m_screen.width()), static_cast<GLsizei>(m_screen.height()));
    on_resize();
}

auto BaseQtSfmlGlWidget::mousePressEvent(QMouseEvent *event) -> void{
    mouseLeftClickPressed   = event->button() == Qt::MouseButton::LeftButton;
    mouseMiddleClickPressed = event->button() == Qt::MouseButton::MiddleButton;
    mouseRighClickPressed   = event->button() == Qt::MouseButton::RightButton;
}

auto BaseQtSfmlGlWidget::mouseReleaseEvent(QMouseEvent *event) -> void{
    if(event->button() == Qt::MouseButton::LeftButton){
        mouseLeftClickPressed = false;
    }else if(event->button() == Qt::MouseButton::MiddleButton){
        mouseMiddleClickPressed = false;
    }else if(event->button() == Qt::MouseButton::RightButton){
        mouseRighClickPressed = false;
    }
    lastX = -1.;
    lastY = -1.;
}

auto BaseQtSfmlGlWidget::mouseMoveEvent(QMouseEvent *event) -> void{

    if(lastX < 0.){
        lastX = event->pos().x();
        lastY = event->pos().y();
    }

    double xoffset = event->pos().x() - lastX;
    double yoffset = event->pos().y() - lastY;
    lastX = event->pos().x();
    lastY = event->pos().y();

    double sensitivity = 0.05;
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    if(mouseLeftClickPressed){
        m_camera.set_direction(xoffset,-yoffset,0.);
    }else if(mouseMiddleClickPressed){
        m_camera.move_up(-0.1*yoffset);
        m_camera.move_right(0.1*xoffset);
    }else if(mouseRighClickPressed){
        m_camera.set_direction(0.,0.,xoffset);
    }
    // qDebug() << "move mouse " << lastX << lastY;
    update();
}

auto BaseQtSfmlGlWidget::wheelEvent(QWheelEvent *event) -> void{
    m_camera.move_front(event->angleDelta().y()*0.001);
    // qDebug() << "wheel mode " << event->angleDelta().y();
    update();
}

auto BaseQtSfmlGlWidget::keyPressEvent(QKeyEvent *event) -> void {

    isKeyPressed = true;

    if(event->key() == Qt::Key_Up){
        m_camera.move_front(m_cameraSpeed);
    }
    if(event->key() == Qt::Key_Left){
        m_camera.move_left(m_cameraSpeed);
    }
    if(event->key() == Qt::Key_Right){
        m_camera.move_right(m_cameraSpeed);
    }
    if(event->key() == Qt::Key_Down){
        m_camera.move_back(m_cameraSpeed);
    }
    if(event->key() == Qt::Key_R){
        m_camera.reset_init_values();
        m_camera.set_direction(0.,0.,0.);
    }

    // qDebug() << "key press " << event->key();

    update();
}


auto BaseQtSfmlGlWidget::on_init() -> void{


    // enable / disable
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_PROGRAM_POINT_SIZE);

    // change la couleur de la forme pour du vert
    shape.setFillColor(sf::Color(100, 250, 50));

    // init cloud
    cloud.resize(100000);
    for(size_t id = 0; id < 100000; ++id){
        cloud.vertices[id] ={
                0.001f*((rand()%1000) -500),
                0.001f*((rand()%1000) -500),
                0.001f*((rand()%1000) -500)
            };
        cloud.colors[id] = {
                0.001f*(rand()%1000),
                0.001f*(rand()%1000),
                0.001f*(rand()%1000)
            };
    }
    cloudD.init_and_load(cloud);


    axesD = std::make_unique<gl::AxesDrawer>();
    gridD = std::make_unique<gl::GridDrawer>();
    // axesD.init(1.f);

    std::string vsPath = "D:/DEV/EPFL/lnco-exvr/exvr/toolset/cpp-projects/_build/bin/dc-grabber/resources/shaders/solid.vs";
    std::string fsPath = "D:/DEV/EPFL/lnco-exvr/exvr/toolset/cpp-projects/_build/bin/dc-grabber/resources/shaders/solid.fs";
    shaderP.load_from_files({vsPath, fsPath});
    // std::cout << "LOAD SHADER: "  << shaderP.load_from_files({vsPath, fsPath}) << "\n";
    // std::cout << "is liked: " << shaderP.is_linked() << "\n";
    // std::cout << "id: " << shaderP.id() << "\n";

    // std::cout << "load cloud shader: " << cloudShaderP.load_from_source_code({
    //     {gl::Shader::Type::vertex, cloudVS},
    //     {gl::Shader::Type::fragment, cloudFS}
    // });

    std::string cloudVsPath = "D:/DEV/EPFL/lnco-exvr/exvr/toolset/cpp-projects/_build/bin/dc-grabber/resources/shaders/cloud.vs";
    std::string cloudFsPath = "D:/DEV/EPFL/lnco-exvr/exvr/toolset/cpp-projects/_build/bin/dc-grabber/resources/shaders/cloud.fs";
    cloudShaderP.load_from_files({cloudVsPath, cloudFsPath});
    // std::cout << "load cloud shader " << cloudShaderP.load_from_files({cloudVsPath, cloudFsPath}) << "\n";

    // std::cout << "\n";
}

auto BaseQtSfmlGlWidget::on_update() -> void{

}

auto BaseQtSfmlGlWidget::on_draw() -> void{

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_STENCIL_TEST);
    glEnable(GL_MULTISAMPLE);
    glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
    glEnable(GL_PROGRAM_POINT_SIZE);

    // clean
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // set polygon mode
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    shaderP.use();
    shaderP.set_uniform("view",         m_camera.view().conv<float>());
    shaderP.set_uniform("model",        geo::Mat4f::identity());
    shaderP.set_uniform("projection",   m_camera.projection().conv<float>());
    shaderP.set_uniform("enable_unicolor", false);
    // cloudD.draw();
    // axesD->draw();
    gridD->draw();


    cloudShaderP.use();
    cloudShaderP.set_uniform("view", m_camera.view().conv<float>());
    cloudShaderP.set_uniform("projection", m_camera.projection().conv<float>());
    cloudShaderP.set_uniform("model", geo::Mat4f::identity());
    // cloudShaderP.set_uniform("enable_unicolor", false);
    // cloudShaderP.set_uniform("unicolor", geo::Pt4f{1.f,0.f,0.f, 1.f});
    // cloudShaderP.set_uniform("factor_unicolor", 1.f);
    cloudShaderP.set_uniform("size_pt", 5.f);
    cloudShaderP.set_uniform("camera_position", m_camera.position().conv<float>());

    cloudD.draw();
    // cDrawer.draw();

    gl::ShaderProgram::unbind();

    // gl::FBO::unbind();

    // test
    draw(shape);

}
