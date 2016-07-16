//
//  vtkTreeTest.cxx
//  vtkGraphCut
//
//  Created by Berend Klein Haneveld on 10/07/16.
//
//

#include <iostream>
#include <assert.h>
#include "vtkGraphCutDataTypes.h"
#include "Internal/Tree.h"
#include "Internal/Edges.h"
#include "Internal/Nodes.h"
#include "Internal/Edge.h"


void testTreeConstructor();
void testTreeProperties();


/**
 * Creates and returns a test Edges object.
 */
Edges* createTestData() {
    int dimensions[3] = {30, 30, 30};
    
    Nodes* nodes = new Nodes();
    nodes->SetDimensions(dimensions);
    nodes->SetConnectivity(SIX);
    nodes->Update();
    
    Edges* edges = new Edges();
    edges->SetNodes(nodes);
    edges->Update();
    
    return edges;
}


/**
 * Deletes the given Edges object and
 * its accompanying Nodes object.
 */
void clearTestData(Edges* edges) {
    delete edges->GetNodes();
    delete edges;
}


int main() {
    testTreeConstructor();
    testTreeProperties();
    
    return 0;
}


void testTreeConstructor() {
    std::cout << __FUNCTION__ << "\n";
    
    Tree* tree = new Tree(TREE_NONE, NULL);
    
    assert(tree->GetEdges() == NULL);
    assert(tree->GetTreeType() == TREE_NONE);
    
    delete tree;
    
    std::cout << "Done!" << "\n";
}


void testTreeProperties() {
    std::cout << __FUNCTION__ << "\n";
    
    Edges* edges = createTestData();

    Tree* tree = new Tree(TREE_SINK, edges);

    assert(tree->GetEdges() == edges);
    assert(tree->GetTreeType() == TREE_SINK);
    
    clearTestData(edges);

    delete tree;
    
    std::cout << "Done!" << "\n";
}

