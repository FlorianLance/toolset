#pragma once

// std
#include <unordered_map>
#include <tuple>
#include <functional>

// Qt
#include <QtCore/QUuid>
#include <QtWidgets/QGraphicsScene>

// local
#include "QUuidStdHash.hpp"
#include "Export.hpp"
#include "DataModelRegistry.hpp"
#include "TypeConverter.hpp"
#include "memory.hpp"

namespace QtNodes{

class NodeDataModel;
class FlowItemInterface;
class Node;
class NodeGraphicsObject;
class Connection;
class ConnectionGraphicsObject;
class NodeStyle;

/// Scene holds connections and nodes.
class NODE_EDITOR_PUBLIC FlowScene : public QGraphicsScene {
    Q_OBJECT
public:

    FlowScene(std::shared_ptr<DataModelRegistry> registry,QObject * parent = Q_NULLPTR);
    FlowScene(QObject * parent = Q_NULLPTR);
    ~FlowScene();

public:

    std::shared_ptr<Connection> createConnection(PortType connectedPort, Node& node, PortIndex portIndex);
    std::shared_ptr<Connection> createConnection(
        Node& nodeIn,
        PortIndex portIndexIn,
        Node& nodeOut,
        PortIndex portIndexOut,
        TypeConverter const & converter = TypeConverter{}
    );

    void deleteConnection(Connection& connection);

    Node&createNode(std::unique_ptr<NodeDataModel> && dataModel);
    void removeNode(Node& node);

    DataModelRegistry&registry() const;
    void setRegistry(std::shared_ptr<DataModelRegistry> registry);

    void iterateOverNodes(std::function<void(Node*)> const & visitor);
    void iterateOverNodeData(std::function<void(NodeDataModel*)> const & visitor);
    void iterateOverNodeDataDependentOrder(std::function<void(NodeDataModel*)> const & visitor);

    QPointF getNodePosition(Node const& node) const;
    void setNodePosition(Node& node, QPointF const& pos) const;

    QSizeF getNodeSize(Node const& node) const;

public:

    std::unordered_map<QUuid, std::unique_ptr<Node> > const & nodes() const;

    std::unordered_map<QUuid, std::shared_ptr<Connection> > const & connections() const;

    std::vector<Node*> allNodes() const;
    std::vector<Node*> selectedNodes() const;

public:

    void clearScene();

//    void save() const;
//    void load();

//    QByteArray saveToMemory() const;
//    void loadFromMemory(const QByteArray& data);

Q_SIGNALS:

    /**
    * @brief Node has been created but not on the scene yet.
    * @see nodePlaced()
    */
    void nodeCreated(QtNodes::Node &n);

    /**
    * @brief Node has been added to the scene.
    * @details Connect to this signal if need a correct position of node.
    * @see nodeCreated()
    */
    void nodePlaced(QtNodes::Node &n);
    void nodeDeleted(QtNodes::Node &n);
    void connectionCreated(QtNodes::Connection const &c);
    void connectionDeleted(QtNodes::Connection const &c);
    void nodeMoved(QtNodes::Node& n, const QPointF& newLocation);
    void nodeDoubleClicked(QtNodes::Node& n);
    void connectionHovered(QtNodes::Connection& c, QPoint screenPos);
    void nodeHovered(QtNodes::Node& n, QPoint screenPos);
    void connectionHoverLeft(QtNodes::Connection& c);
    void nodeHoverLeft(QtNodes::Node& n);
    void nodeContextMenu(QtNodes::Node& n, const QPointF& pos);

private:

    using SharedConnection = std::shared_ptr<Connection>;
    using UniqueNode       = std::unique_ptr<Node>;

    std::unordered_map<QUuid, SharedConnection> _connections;
    std::unordered_map<QUuid, UniqueNode>       _nodes;
    std::shared_ptr<DataModelRegistry>          _registry;

private Q_SLOTS:

    void setupConnectionSignals(QtNodes::Connection const& c);

    void sendConnectionCreatedToNodes(QtNodes::Connection const& c);
    void sendConnectionDeletedToNodes(QtNodes::Connection const& c);

};

    Node* locateNodeAt(QPointF scenePoint, FlowScene &scene, QTransform const & viewTransform);
}
