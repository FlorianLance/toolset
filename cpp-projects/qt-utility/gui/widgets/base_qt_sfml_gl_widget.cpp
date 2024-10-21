

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
#include <QWindow>

// opengl-utility
#include "opengl/gl_functions.hpp"

// local
#include "qt_logger.hpp"

using namespace tool;
using namespace tool::gl;
using namespace Qt::Literals::StringLiterals;

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
    return QWidget::eventFilter(object,event);
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
    settings.attributeFlags = sf::ContextSettings::Core;

    // init window
    sf::RenderWindow::create(reinterpret_cast<sf::WindowHandle>(winId()), settings);

    // init glew
    GL::init_glew();
    GL::display_glew_info();

    // init gl
    QtLog::message(u"BaseQtSfmlGlWidget::on_init"_s);
    if(!on_init()){
        return;
    }

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

    sf::RenderWindow::setActive(true);
    on_paint();
    display();
}


auto BaseQtSfmlGlWidget::resizeEvent(QResizeEvent *event) -> void{

    QScreen *screen = this->window()->windowHandle()->screen();
    m_screen.resize(static_cast<unsigned int>(event->size().width()),static_cast<unsigned int>(event->size().height()), screen->devicePixelRatio());
    m_camera.update_projection();

    if(!m_windowInitialized){
        return;
    }

    sf::RenderWindow::setActive(true);
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
    update();
}

auto BaseQtSfmlGlWidget::wheelEvent(QWheelEvent *event) -> void{
    m_camera.move_front(event->angleDelta().y()*0.001);
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

    update();
}


auto BaseQtSfmlGlWidget::on_init() -> bool{


    // change la couleur de la forme pour du vert
    // shape.setFillColor(sf::Color(100, 250, 50));

    return true;
}

auto BaseQtSfmlGlWidget::on_update() -> void{

}

auto BaseQtSfmlGlWidget::on_paint() -> void{

    // gl::ShaderProgram::unbind();
    // draw(shape);
}
