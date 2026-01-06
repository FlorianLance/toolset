#pragma once

// Qt
#include <QtGui/QPainter>
#include <QIcon>

namespace QtNodes{

class ConnectionGeometry;
class ConnectionState;
class Connection;

class ConnectionPainter{
public:
    static void paint(QPainter* painter, Connection const& connection);
    static QPainterPath getPainterStroke(ConnectionGeometry const& geom);

    static inline QIcon icon = QIcon(":convert.png");
    static inline std::unique_ptr<QPixmap> pixmap = nullptr;
};
}
