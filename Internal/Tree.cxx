//
//  vtkTree.cxx
//  vtkGraphCut
//
//  Created by Berend Klein Haneveld on 16/07/16.
//
//

#include "Tree.h"
#include <iostream>


Tree::Tree(){
    _edges = NULL;
    _treeType = TREE_NONE;
};


Tree::~Tree(){
    _edges = NULL;
    _treeType = TREE_NONE;
};


void Tree::SetEdges(Edges* edges) {
    _edges = edges;
}


Edges* Tree::GetEdges() {
    return _edges;
}


void Tree::SetTreeType(vtkTreeType treeType) {
    _treeType = treeType;
}


vtkTreeType Tree::GetTreeType() {
    return _treeType;
}


void Tree::AddChildToParent(int child, int parent) {
    
}