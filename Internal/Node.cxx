//
//  Node.cxx
//  vtkGraphCut
//
//  Created by Berend Klein Haneveld on 10/07/16.
//
//

#include "Node.h"

Node::Node() {
    active = false;
    orphan = false;
    seedPoint = false;
    depthInTree = -1;
    tree = TREE_NONE;
    parent = NODE_NONE;
}