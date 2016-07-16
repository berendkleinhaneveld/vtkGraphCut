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
     * at index @p parent. If the child was connected to
     * another parent before, that edge will be broken.
     */
    void AddChildToParent(NodeIndex child, NodeIndex parent);
    
protected:
    Edges* _edges;
    vtkTreeType _treeType;
};



#endif /* vtkTree_h */
