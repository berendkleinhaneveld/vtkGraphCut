//
//  vtkNodeTest.cpp
//  vtkGraphCut
//
//  Created by Berend Klein Haneveld on 10/07/16.
//
//

#include <iostream>
#include <assert.h>
#include "vtkNodes.h"

void testNodesConstructor();

int main() {
    testNodesConstructor();
    return 0;
}

void testNodesConstructor() {
    std::cout << __FUNCTION__ << "\n";
    
    vtkNodes* nodes = new vtkNodes();
    
    delete nodes;
    
    std::cout << "Done!" << "\n";
}