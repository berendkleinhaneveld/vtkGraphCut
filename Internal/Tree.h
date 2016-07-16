//
//  vtkTree.hpp
//  vtkGraphCut
//
//  Created by Berend Klein Haneveld on 16/07/16.
//
//

#ifndef vtkTree_h
#define vtkTree_h

#include "vtkGraphCutDataTypes.h"
#include <vector>


class Edges;
class Edge;
class Node;


/**
 * Tree is a mutable structure that is defined
 * by the parent and tree attributes in the 
 * Node objects in the Nodes object.
 */
class Tree {
public:
    Tree(vtkTreeType type, Edges* edges);
    ~Tree();
    
    // Properties
    Edges* GetEdges();
    
    vtkTreeType GetTreeType();
    
    /**
     * Adds the node at index @p child as child to the node
     * at index @p parent. If the child has any children, the
     * @p depthInTree property of all these children will be
     * updated recursively.
     */
    void AddChildToParent(NodeIndex child, NodeIndex parent);
    
    
    /**
     * Returns the path to the root as a vector of edge indices.
     * The value of maxFlow will be updated to hold the value of
     * the maximum flow that is possible to push through the path.
     */
    std::vector<EdgeIndex> PathToRoot(NodeIndex leaf, int* maxFlow);
    
protected:
    Edges* _edges;
    vtkTreeType _treeType;
};



#endif /* vtkTree_h */
