//
//  Edge.h
//  vtkGraphCut
//
//  Created by Berend Klein Haneveld on 04/07/16.
//
//

#ifndef Edge_h
#define Edge_h

#include <stdio.h>
#include "vtkGraphCutDataTypes.h"


/**
 * Edge describes an edge between two Nodes.
 * The nodes are not saved by reference or pointer
 * but by index.
 *
 * A Edge can have a certain capacity and a flow.
 * The capacity and flow are dependent on the direction,
 * so use the methods *fromNode to manipulate these
 * values.
 */
class Edge
{
public:
    Edge(NodeIndex firstNode, NodeIndex secondNode);
    
    NodeIndex node1();
    NodeIndex node2();
    
    NodeIndex nonRootNode();
    NodeIndex rootNode();
    
    bool isTerminal();
    bool isValid();
    void setCapacity(int capacity);
    void addFlowFromNode(NodeIndex node, int addedFlow);
    bool isSaturatedFromNode(NodeIndex node);
    int flowFromNode(NodeIndex node);
    int capacityFromNode(NodeIndex node);
    
protected:
    // Current flow through edge
    // Positive means from node1 to node2, negative from node2 to node1
    NodeIndex _node1;
    NodeIndex _node2;
    int _capacity;
    int _flow;
};


#endif /* Edge_h */
