//
//  EdgeTest.cxx
//  vtkGraphCut
//
//  Created by Berend Klein Haneveld on 05/07/16.
//
//

#include <assert.h>
#include "Internal/Edge.h"
#include "vtkGraphCutDataTypes.h"


void testEdgeStruct();


int main() {
    testEdgeStruct();
    return 0;
}


/**
 * Tests the methods of an Edge struct.
 * - isTerminal
 * - flowFromNode
 * - capacityFromNode
 * - setCapacity
 * - isSaturatedFromNode
 * - addFlowFromNode
 */
void testEdgeStruct() {
    NodeIndex node0 = (NodeIndex)0;
    NodeIndex node1 = (NodeIndex)1;
    
    Edge edge = Edge(NODE_SINK, node1);
    assert(edge.isTerminal());
    assert(edge.flowFromNode(node0) == 0);
    assert(edge.capacityFromNode(node0) == 0);
    
    edge = Edge(node0, node1);
    assert(!edge.isTerminal());
    
    edge.setCapacity(5);
    assert(edge.flowFromNode(node0) == 0);
    assert(!edge.isSaturatedFromNode(node0));
    assert(!edge.isSaturatedFromNode(node0));
    assert(edge.capacityFromNode(node0) == 5);
    assert(edge.capacityFromNode(node1) == 5);
    
    edge.addFlowFromNode(node0, 5);
    assert(edge.flowFromNode(node0) == 5);
    assert(edge.flowFromNode(node1) == -5);
    assert(edge.isSaturatedFromNode(node0));
    assert(!edge.isSaturatedFromNode(node1));
    assert(edge.capacityFromNode(node0) == 0);
    assert(edge.capacityFromNode(node1) == 10);
    
    edge.addFlowFromNode(node1, 2);
    assert(edge.flowFromNode(node1) == -3);
    assert(edge.flowFromNode(node0) == 3);
    assert(!edge.isSaturatedFromNode(node0));
    assert(!edge.isSaturatedFromNode(node1));
    assert(edge.capacityFromNode(node0) == 2);
    assert(edge.capacityFromNode(node1) == 8);
    
    edge.addFlowFromNode(node1, 8);
    assert(!edge.isSaturatedFromNode(node0));
    assert(edge.isSaturatedFromNode(node1));
    assert(edge.capacityFromNode(node0) == 10);
    assert(edge.capacityFromNode(node1) == 0);
    
    assert(edge.otherNode(node1) == node0);
    assert(edge.otherNode(node0) == node1);
    
    assert(edge.otherNode((NodeIndex)33) == NODE_NONE);
}