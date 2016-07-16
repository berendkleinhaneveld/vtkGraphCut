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
    Edge(int firstNode, int secondNode);
    
    int node1();
    int node2();
    
    int nonRootNode();
    int rootNode();
    
    bool isTerminal();
    bool isValid();
    void setCapacity(int capacity);
    void addFlowFromNode(int node, int addedFlow);
    bool isSaturatedFromNode(int node);
    int flowFromNode(int node);
    int capacityFromNode(int node);
    
protected:
    // Current flow through edge
    // Positive means from node1 to node2, negative from node2 to node1
    int _node1;
    int _node2;
    int _capacity;
    int _flow;
};


#endif /* Edge_h */
