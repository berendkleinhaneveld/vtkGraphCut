//
//  vtkTree.cxx
//  vtkGraphCut
//
//  Created by Berend Klein Haneveld on 16/07/16.
//
//

#include "Tree.h"
#include <iostream>
#include <assert.h>
#include "Internal/Edges.h"
#include "Internal/Edge.h"
#include "Internal/Nodes.h"
#include "Internal/Node.h"


Tree::Tree(vtkTreeType type, Edges* edges) {
    _edges = edges;
    _treeType = type;
};


Tree::~Tree(){
    _edges = NULL;
    _treeType = TREE_NONE;
};


Edges* Tree::GetEdges() {
    return _edges;
}


vtkTreeType Tree::GetTreeType() {
    return _treeType;
}


void Tree::AddChildToParent(NodeIndex childIndex, NodeIndex parentIndex) {
    
}