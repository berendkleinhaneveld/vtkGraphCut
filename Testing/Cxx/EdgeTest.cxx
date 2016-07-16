//
//  EdgeTest.cxx
//  vtkGraphCut
//
//  Created by Berend Klein Haneveld on 05/07/16.
//
//

#include <iostream>
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
    std::cout << __FUNCTION__ << "\n";
    Edge edge = Edge(NODE_SINK, 1);
    assert(edge.isTerminal());
    assert(edge.flowFromNode(0) == 0);
    assert(edge.capacityFromNode(0) == 0);
    
    edge = Edge(0, 1);
    assert(!edge.isTerminal());
    
    edge.setCapacity(5);
    assert(edge.flowFromNode(0) == 0);
    assert(!edge.isSaturatedFromNode(0));
    assert(!edge.isSaturatedFromNode(0));
    assert(edge.capacityFromNode(0) == 5);
    assert(edge.capacityFromNode(1) == 5);
    
    edge.addFlowFromNode(0, 5);
    assert(edge.flowFromNode(0) == 5);
    assert(edge.flowFromNode(1) == -5);
    assert(edge.isSaturatedFromNode(0));
    assert(!edge.isSaturatedFromNode(1));
    assert(edge.capacityFromNode(0) == 0);
    assert(edge.capacityFromNode(1) == 10);
    
    edge.addFlowFromNode(1, 2);
    assert(edge.flowFromNode(1) == -3);
    assert(edge.flowFromNode(0) == 3);
    assert(!edge.isSaturatedFromNode(0));
    assert(!edge.isSaturatedFromNode(1));
    assert(edge.capacityFromNode(0) == 2);
    assert(edge.capacityFromNode(1) == 8);
    
    edge.addFlowFromNode(1, 8);
    assert(!edge.isSaturatedFromNode(0));
    assert(edge.isSaturatedFromNode(1));
    assert(edge.capacityFromNode(0) == 10);
    assert(edge.capacityFromNode(1) == 0);
    
    std::cout << "Done!" << "\n";
}