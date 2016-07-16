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

/**
 * Constructor
 */
Edge::Edge(NodeIndex firstNode, NodeIndex secondNode) {
    _node1 = firstNode;
    _node2 = secondNode;
    _capacity = 0;
    _flow = 0;
}

/**
 * Returns node1 as an index.
 */
NodeIndex Edge::node1() {
    return _node1;
}

/**
 * Returns node2 as an index.
 */
NodeIndex Edge::node2() {
    return _node2;
}

/**
 * Returns the non-root node when this edge
 * is a terminal node. Otherwise returns INVALID.
 */
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

/**
 * Returns the root node when this edge is a
 * terminal node. Otherwise returns INVALID.
 */
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

/**
 * Returns whether this edge is connected to eiter a
 * SOURCE or a SINK node.
 */
bool Edge::isTerminal() {
    return _node1 == NODE_SINK || _node1 == NODE_SOURCE || _node2 == NODE_SINK || _node2 == NODE_SOURCE;
}

/**
 * Returns whether this node is a valid node.
 */
bool Edge::isValid() {
    return _node1 != NODE_NONE && _node2 != NODE_NONE;
}

/**
 * Set the total capacity that this edge can hold.
 * This capacity is the max capacity in both directions.
 */
void Edge::setCapacity(int cap) {
    _capacity = cap;
}

/**
 * Adds the given amount of flow to the current flow
 * from the direction of the given node.
 */
void Edge::addFlowFromNode(NodeIndex node, int addedFlow) {
    assert(node == _node1 || node == _node2);
    assert(capacityFromNode(node) >= addedFlow);
    if (node == _node1) {
        _flow += addedFlow;
    } else {
        _flow -= addedFlow;
    }
}

/**
 * Returns whether the edge is saturated as seen from
 * the given node.
 */
bool Edge::isSaturatedFromNode(NodeIndex node) {
    return capacityFromNode(node) == 0;
}

/**
 * Returns the current flow from a give node.
 */
int Edge::flowFromNode(NodeIndex node) {
    if (node == _node1) {
        return _flow;
    } else {
        return -_flow;
    }
}

/**
 * Returns the capacity of the edge that is left
 * in the given direction.
 */
int Edge::capacityFromNode(NodeIndex node) {
    return _capacity - flowFromNode(node);
}
