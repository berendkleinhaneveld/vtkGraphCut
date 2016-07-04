//
//  vtkEdge.cxx
//  vtkGraphCut
//
//  Created by Berend Klein Haneveld on 04/07/16.
//
//

#include "vtkEdge.h"
#include <assert.h>
#include "vtkGraphCutDataTypes.h"

/**
 * Constructor
 */
vtkEdge::vtkEdge(int firstNode, int secondNode) {
    _node1 = firstNode;
    _node2 = secondNode;
    _capacity = 0;
    _flow = 0;
}

/**
 * Returns node1 as an index.
 */
int vtkEdge::node1() {
    return _node1;
}

/**
 * Returns node2 as an index.
 */
int vtkEdge::node2() {
    return _node2;
}

/**
 * Returns the non-root node when this edge
 * is a terminal node. Otherwise returns INVALID.
 */
int vtkEdge::nonRootNode() {
    assert(isTerminal());
    if (_node1 < 0) {
        return _node2;
    }
    if (_node2 < 0) {
        return _node1;
    }
    return INVALID;
}

/**
 * Returns the root node when this edge is a
 * terminal node. Otherwise returns INVALID.
 */
int vtkEdge::rootNode() {
    assert(isTerminal());
    if (_node1 < 0) {
        return _node1;
    }
    if (_node2 < 0) {
        return _node2;
    }
    return INVALID;
}

/**
 * Returns whether this edge is connected to eiter a
 * SOURCE or a SINK node.
 */
bool vtkEdge::isTerminal() {
    return _node1 == SINK || _node1 == SOURCE || _node2 == SINK || _node2 == SOURCE;
}

/**
 * Returns whether this node is a valid node.
 */
bool vtkEdge::isValid() {
    return _node1 != INVALID && _node2 != INVALID;
}

/**
 * Set the total capacity that this edge can hold.
 * This capacity is the max capacity in both directions.
 */
void vtkEdge::setCapacity(int cap) {
    _capacity = cap;
}

/**
 * Adds the given amount of flow to the current flow
 * from the direction of the given node.
 */
void vtkEdge::addFlowFromNode(int node, int addedFlow) {
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
bool vtkEdge::isSaturatedFromNode(int node) {
    return capacityFromNode(node) == 0;
}

/**
 * Returns the current flow from a give node.
 */
int vtkEdge::flowFromNode(int node) {
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
int vtkEdge::capacityFromNode(int node) {
    return _capacity - flowFromNode(node);
}
