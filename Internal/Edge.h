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
    
    /**
     * Returns node1 as an index.
     */
    NodeIndex node1();
    
    /**
     * Returns node2 as an index.
     */
    NodeIndex node2();
    
    /**
     * Returns the non-root node when this edge
     * is a terminal node. Otherwise returns INVALID.
     */
    NodeIndex nonRootNode();
    
    /**
     * Returns the root node when this edge is a
     * terminal node. Otherwise returns INVALID.
     */
    NodeIndex rootNode();
    
    /**
     * Returns the node that is not equal to @p node.
     * When neither node is equal, then NODE_NONE is returned.
     */
    NodeIndex otherNode(NodeIndex node);
    
    /**
     * Returns whether this edge is connected to eiter a
     * SOURCE or a SINK node.
     */
    bool isTerminal();
    
    /**
     * Returns true iff both nodes are not NODE_NONE.
     */
    bool isValid();
    
    /**
     * Set the total capacity that this edge can hold.
     * This capacity is the max capacity in both directions.
     */
    void setCapacity(int capacity);
    
    /**
     * Adds the given amount of flow to the current flow
     * from the direction of the given node.
     */
    void addFlowFromNode(NodeIndex node, int addedFlow);
    
    /**
     * Returns whether the edge is saturated as seen from
     * the given node.
     */
    bool isSaturatedFromNode(NodeIndex node);
    
    /**
     * Returns the current flow from a give node.
     */
    int flowFromNode(NodeIndex node);
    
    /**
     * Returns the capacity of the edge that is left
     * in the given direction.
     */
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
