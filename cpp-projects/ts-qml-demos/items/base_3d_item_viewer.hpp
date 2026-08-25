


#pragma once

// Qt
#include <QQuickFramebufferObject>

// base
#include "geometry/camera.hpp"
#include "geometry/screen.hpp"

class Base3dItemViewer : public QQuickFramebufferObject {
    Q_OBJECT
public:

    Base3dItemViewer(QQuickItem *parent = nullptr);

    constexpr auto camera() const noexcept -> tool::geo::Camera {return m_camera;}
    constexpr auto screen() const noexcept -> tool::geo::Screen {return m_screen;}

protected:

    void keyPressEvent(QKeyEvent *event)        override;
    void mousePressEvent(QMouseEvent *event)    override;
    void mouseReleaseEvent(QMouseEvent *event)  override;
    void mouseMoveEvent(QMouseEvent *event)     override;
    void keyReleaseEvent(QKeyEvent *event)      override{}
    void hoverMoveEvent(QHoverEvent *event)     override{}

    void wheelEvent(QWheelEvent *event) override;
    void geometryChange(const QRectF &newGeometry, const QRectF &oldGeometry) override;

protected:

    tool::geo::Screen m_screen;

    // camera
    double m_cameraSpeed = 0.05;
    tool::geo::Camera m_camera;

    // inputs
    bool m_isKeyPressed = false;
    bool m_mouseLeftClickPressed = false;
    bool m_mouseMiddleClickPressed = false;
    bool m_mouseRighClickPressed = false;
    double m_lastX=-1., m_lastY=-1.;
};
