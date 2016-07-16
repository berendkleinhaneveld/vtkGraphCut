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


class Edges;


class Tree {
public:
    Tree();
    ~Tree();
    
    // Properties
    void SetEdges(Edges*);
    Edges* GetEdges();
    
    void SetTreeType(vtkTreeType);
    vtkTreeType GetTreeType();
    
    /**
     * Adds the node at index @p child as child to the node
     * at index @p parent.
     */
    void AddChildToParent(int child, int parent);
    
protected:
    Edges* _edges;
    vtkTreeType _treeType;
};



#endif /* vtkTree_h */
