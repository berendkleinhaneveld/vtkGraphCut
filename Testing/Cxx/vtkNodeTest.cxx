//
//  vtkNodeTest.cxx
//  vtkGraphCut
//
//  Created by Berend Klein Haneveld on 10/07/16.
//
//

#include <iostream>
#include <assert.h>
#include "Internal/vtkNode.h"

void testNodeConstructor();

int main() {
    testNodeConstructor();
    return 0;
}

void testNodeConstructor() {
    std::cout << __FUNCTION__ << "\n";
    
    vtkNode* node = new vtkNode();
    
    assert(node->active == false);
    assert(node->depthInTree == -1);
    assert(node->tree == TREE_NONE);
    assert(node->parent == NODE_NONE);
    assert(node->orphan == false);
    
    delete node;
    
    std::cout << "Done!" << "\n";
}