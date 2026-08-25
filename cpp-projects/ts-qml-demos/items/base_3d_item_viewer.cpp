
#include "base_3d_item_viewer.hpp"

using namespace tool;
using namespace tool::geo;
using namespace Qt::Literals::StringLiterals;

Base3dItemViewer::Base3dItemViewer(QQuickItem *parent) : QQuickFramebufferObject(parent), m_camera(Camera({},{0.0, -180.0, 180.0})){

    // We need to set this flag to tell Qt that our item should be
    // re-rendered on every frame, otherwise update() won't work correctly.
    setMirrorVertically(true);
    setAcceptedMouseButtons(Qt::AllButtons);
    setAcceptHoverEvents(true);

    setFlag(ItemAcceptsInputMethod, true); // For keyboard events
    setFlag(ItemIsFocusScope, true);       // Allow the item to receive focus
    setFlag(ItemHasContents, true);
}

void Base3dItemViewer::keyPressEvent(QKeyEvent *event){

    m_isKeyPressed = true;
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
    }

    update();
}

void Base3dItemViewer::mousePressEvent(QMouseEvent *event){

    setFocus(true);
    m_mouseLeftClickPressed   = event->button() == Qt::MouseButton::LeftButton;
    m_mouseMiddleClickPressed = event->button() == Qt::MouseButton::MiddleButton;
    m_mouseRighClickPressed   = event->button() == Qt::MouseButton::RightButton;

    if(m_mouseLeftClickPressed){
        // auto ray = screen_raycast({event->pos().x(),event->pos().y()}, m_screen, m_camera);
        // rayStart.push_back(ray.origin.conv<float>());
        // rayEnd.push_back((ray.origin + ray.direction*100.0).conv<float>());
        update();
    }
}

void Base3dItemViewer::mouseReleaseEvent(QMouseEvent *event){

    if(event->button() == Qt::MouseButton::LeftButton){
        m_mouseLeftClickPressed = false;
    }else if(event->button() == Qt::MouseButton::MiddleButton){
        m_mouseMiddleClickPressed = false;
    }else if(event->button() == Qt::MouseButton::RightButton){
        m_mouseRighClickPressed = false;
    }
    m_lastX = -1.;
    m_lastY = -1.;
}

void Base3dItemViewer::mouseMoveEvent(QMouseEvent *event){

    if(m_lastX < 0.){
        m_lastX = event->pos().x();
        m_lastY = event->pos().y();
    }

    double xoffset = event->pos().x() - m_lastX;
    double yoffset = event->pos().y() - m_lastY;
    m_lastX = event->pos().x();
    m_lastY = event->pos().y();

    double sensitivity = 0.05;
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    if(m_mouseLeftClickPressed){
        m_camera.rotate({xoffset,yoffset,0.});
    }else if(m_mouseMiddleClickPressed){
        m_camera.move_up(-0.1*yoffset);
        m_camera.move_right(0.1*xoffset);
    }else if(m_mouseRighClickPressed){
        m_camera.rotate({0.,0.,xoffset});
    }
    update();
}

void Base3dItemViewer::wheelEvent(QWheelEvent *event){
    m_camera.move_front(event->angleDelta().y()*0.001);
    update();
}

void Base3dItemViewer::geometryChange(const QRectF &newGeometry, const QRectF &oldGeometry){
    m_screen.resize(newGeometry.width(), newGeometry.height());
    update();
}
