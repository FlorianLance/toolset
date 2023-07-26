#include "ConnectionState.hpp"

// std
#include <iostream>

// Qt
#include <QtCore/QPointF>

// local
#include "FlowScene.hpp"
#include "Node.hpp"

using QtNodes::ConnectionState;
using QtNodes::Node;

ConnectionState::~ConnectionState(){
    resetLastHoveredNode();
}

void ConnectionState::interactWithNode(Node* node){
    if (node){
        _lastHoveredNode = node;
    }else{
        resetLastHoveredNode();
    }
}

void ConnectionState::setLastHoveredNode(Node* node){
    _lastHoveredNode = node;
}

void ConnectionState::resetLastHoveredNode(){
    if (_lastHoveredNode){
        _lastHoveredNode->resetReactionToConnection();
    }

    _lastHoveredNode = nullptr;
}
