//
//  Edge.cxx
//  vtkGraphCut
//
//  Created by Berend Klein Haneveld on 04/07/16.
//
//

#include "Edge.h"
#include <assert.h>
#include "vtkGraphCutDataTypes.h"


Edge::Edge(NodeIndex firstNode, NodeIndex secondNode) {
    _node1 = firstNode;
    _node2 = secondNode;
    _capacity = 0;
    _flow = 0;
}


NodeIndex Edge::node1() {
    return _node1;
}


NodeIndex Edge::node2() {
    return _node2;
}


NodeIndex Edge::nonRootNode() {
    assert(isTerminal());
    if (_node1 < 0) {
        return _node2;
    }
    if (_node2 < 0) {
        return _node1;
    }
    return NODE_NONE;
}


NodeIndex Edge::rootNode() {
    assert(isTerminal());
    if (_node1 < 0) {
        return _node1;
    }
    if (_node2 < 0) {
        return _node2;
    }
    return NODE_NONE;
}

NodeIndex Edge::otherNode(NodeIndex node) {
    if (_node1 == node) {
        return _node2;
    }
    if (_node2 == node) {
        return _node1;
    }
    return NODE_NONE;
}


bool Edge::isTerminal() {
    return _node1 == NODE_SINK || _node1 == NODE_SOURCE || _node2 == NODE_SINK || _node2 == NODE_SOURCE;
}


bool Edge::isValid() {
    return _node1 != NODE_NONE && _node2 != NODE_NONE;
}


void Edge::setCapacity(int cap) {
    _capacity = cap;
}


void Edge::addFlowFromNode(NodeIndex node, int addedFlow) {
    assert(node == _node1 || node == _node2);
    assert(capacityFromNode(node) >= addedFlow);
    if (node == _node1) {
        _flow += addedFlow;
    } else {
        _flow -= addedFlow;
    }
}


bool Edge::isSaturatedFromNode(NodeIndex node) {
    return capacityFromNode(node) == 0;
}


int Edge::flowFromNode(NodeIndex node) {
    if (node == _node1) {
        return _flow;
    } else {
        return -_flow;
    }
}


int Edge::capacityFromNode(NodeIndex node) {
    return _capacity - flowFromNode(node);
}
