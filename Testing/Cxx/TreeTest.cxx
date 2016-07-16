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

void testAddToParent();


/**
 * Creates and returns a test Edges object.
 */
Tree* createTestData(vtkTreeType type) {
    int dimensions[3] = {30, 30, 30};
    
    Nodes* nodes = new Nodes();
    nodes->SetDimensions(dimensions);
    nodes->SetConnectivity(SIX);
    nodes->Update();
    
    Edges* edges = new Edges();
    edges->SetNodes(nodes);
    edges->Update();
    
    Tree* tree = new Tree(type, edges);
    return tree;
}


/**
 * Deletes the given Edges object and
 * its accompanying Nodes object.
 */
void clearTestData(Tree* tree) {
    Edges* edges = tree->GetEdges();
    delete edges->GetNodes();
    delete edges;
    delete tree;
}


int main() {
    testTreeConstructor();
    testTreeProperties();

    testAddToParent();
    return 0;
}


void testTreeConstructor() {
    Tree* tree = new Tree(TREE_NONE, NULL);
    
    assert(tree->GetEdges() == NULL);
    assert(tree->GetTreeType() == TREE_NONE);
    
    delete tree;
}


void testTreeProperties() {
    Tree* tree = createTestData(TREE_SINK);

    assert(tree->GetEdges() != NULL);
    assert(tree->GetEdges()->GetSize() > 0);
    assert(tree->GetTreeType() == TREE_SINK);
    
    clearTestData(tree);
}


void testAddToParent() {
    Tree* tree = createTestData(TREE_SOURCE);
    
    Edges* edges = tree->GetEdges();
    Nodes* nodes = edges->GetNodes();
    Node* node = nodes->GetNode(0);

    assert(node->parent == NODE_NONE);
    assert(node->depthInTree == -1);
    assert(node->tree == TREE_NONE);
    
    tree->AddChildToParent((NodeIndex)0, NODE_SOURCE);
    
    assert(node->parent == NODE_SOURCE);
    assert(node->depthInTree == 1);
    assert(node->tree == TREE_SOURCE);

    tree->AddChildToParent((NodeIndex)1, (NodeIndex)0);
    
    Node* someChild = nodes->GetNode(1);
    assert(someChild->parent == (NodeIndex)0);
    assert(someChild->depthInTree == 2);
    assert(someChild->tree == TREE_SOURCE);
    
    NodeIndex firstIndex = (NodeIndex)31;
    NodeIndex secondIndex = (NodeIndex)30;
    Node* firstChild = nodes->GetNode(firstIndex);
    Node* secondChild = nodes->GetNode(secondIndex);
    
    tree->AddChildToParent(firstIndex, (NodeIndex)1);
    
    assert(firstChild->parent == (NodeIndex)1);
    assert(firstChild->depthInTree == 3);
    assert(firstChild->tree == TREE_SOURCE);
    
    tree->AddChildToParent(secondIndex, firstIndex);
    
    assert(secondChild->parent == firstIndex);
    assert(secondChild->depthInTree == 4);
    
    tree->AddChildToParent(firstIndex, NODE_SOURCE);
    
    assert(firstChild->parent == NODE_SOURCE);
    assert(firstChild->depthInTree == 1);
    assert(firstChild->tree == TREE_SOURCE);
    
    // Test whether the depth has updated when the firstIndex
    // node was added as a child of a terminal node
    assert(secondChild->parent == firstIndex);
    assert(secondChild->depthInTree == 2);
    
    clearTestData(tree);
}
