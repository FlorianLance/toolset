
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

#pragma once

// Qt
#include <QWidget>
#include <QTimer>

// sfml
#include <SFML/Graphics.hpp>

// base
#include "graphics/camera/camera.hpp"

namespace tool {

class BaseQtSfmlGlWidget :  public QWidget, public sf::RenderWindow{
public:

    BaseQtSfmlGlWidget(QWidget* Parent = nullptr);

protected:

    // Qt
    auto paintEngine() const -> QPaintEngine*;
    auto eventFilter(QObject *object, QEvent *event) -> bool;

    // # events
    auto showEvent(QShowEvent*) -> void;
    auto paintEvent(QPaintEvent*) -> void;
    auto resizeEvent(QResizeEvent *event) -> void;
    virtual auto mousePressEvent(QMouseEvent *event) -> void;
    virtual auto mouseReleaseEvent(QMouseEvent *event) -> void;
    virtual auto mouseMoveEvent(QMouseEvent *event) -> void;
    virtual auto wheelEvent(QWheelEvent *event) -> void;
    virtual auto keyPressEvent(QKeyEvent *event) -> void;

    virtual auto on_init() -> bool;
    virtual auto on_update() -> void;
    virtual auto on_paint() -> void;
    virtual auto on_resize() -> void{}

    // camera
    graphics::Screen m_screen;
    graphics::Camera m_camera;
    double m_cameraSpeed = 0.05;

    // inputs
    bool isKeyPressed = false;
    bool mouseLeftClickPressed = false;
    bool mouseMiddleClickPressed = false;
    bool mouseRighClickPressed = false;
    double lastX=-1., lastY=-1.;

    // sfml test
    // sf::CircleShape shape = sf::CircleShape(50.f);

protected:

    bool m_windowInitialized = false;
    QTimer m_renderTimer;
};


}
