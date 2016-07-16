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

namespace {
    
    /**
     * Update the tree depths of the children of the given node and
     * recursively calls this method for each child.
     */
    void UpdateTreeDepthOfChildren(NodeIndex childIndex, int depth, Nodes* nodes);
    
}


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
    assert(childIndex != parentIndex);
    
    Edge* edge = _edges->EdgeFromNodeToNode(childIndex, parentIndex);
    assert(edge != NULL);
    
    Nodes* nodes = _edges->GetNodes();
    Node* child = nodes->GetNode(childIndex);
    
    child->tree = _treeType;
    if (edge->isTerminal()) {
        assert(edge->rootNode() == parentIndex);
        assert(edge->rootNode() == (int)_treeType);
        child->parent = parentIndex;
        child->depthInTree = 1;
    } else {
        Node* parent = nodes->GetNode(parentIndex);
        assert(parent->tree == _treeType);
        child->parent = parentIndex;
        child->depthInTree = parent->depthInTree + 1;
    }
    
    UpdateTreeDepthOfChildren(childIndex, child->depthInTree, nodes);
}


std::vector<EdgeIndex> Tree::PathToRoot(NodeIndex leafIndex, int* maxFlow) {
    std::vector<EdgeIndex> path;
    
    NodeIndex childIndex = leafIndex;
    do {
        Node* child = _edges->GetNodes()->GetNode(childIndex);
        NodeIndex parentIndex = child->parent;
        assert(parentIndex != NODE_NONE);
        EdgeIndex edgeIndex = _edges->IndexForEdgeFromNodeToNode(childIndex, parentIndex);
        Edge* edge = _edges->GetEdge(edgeIndex);
        path.push_back(edgeIndex);
        NodeIndex pushFrom = _treeType == TREE_SOURCE ? parentIndex : childIndex;
        int capacity = edge->capacityFromNode(pushFrom);
        *maxFlow = (*maxFlow < 0 ? capacity : std::min(*maxFlow, capacity));
        childIndex = parentIndex;
    } while (childIndex >= 0);
    
    return path;
}


std::vector<NodeIndex> Tree::PushFlowThroughPath(std::vector<EdgeIndex> path, int flow) {
    std::vector<NodeIndex> orphans;
    
    Nodes* nodes = _edges->GetNodes();
    
    for (std::vector<EdgeIndex>::iterator edgeIndex = path.begin(); edgeIndex != path.end(); ++edgeIndex) {
        Edge* edge = _edges->GetEdge(*edgeIndex);
        NodeIndex childIndex = NODE_NONE;
        NodeIndex parentIndex = NODE_NONE;
        if (edge->isTerminal()) {
            childIndex = edge->nonRootNode();
            parentIndex = edge->rootNode();
            assert(nodes->GetNode(childIndex)->depthInTree == 1);
        } else {
            Node* node1 = nodes->GetNode(edge->node1());
            childIndex = node1->parent == edge->node2() ? edge->node1() : edge->node2();
            parentIndex = edge->node1() == childIndex ? edge->node2() : edge->node1();
            assert(nodes->GetNode(parentIndex)->depthInTree < nodes->GetNode(childIndex)->depthInTree);
        }
        assert(childIndex != parentIndex);
        NodeIndex pushFrom = _treeType == TREE_SOURCE ? parentIndex : childIndex;
        edge->addFlowFromNode(pushFrom, flow);
        if (edge->isSaturatedFromNode(pushFrom)) {
            orphans.push_back(childIndex);
            nodes->GetNode(childIndex)->orphan = true;
        }
    }
    
    return orphans;
}


namespace {
    
    void UpdateTreeDepthOfChildren(NodeIndex parentIndex, int depth, Nodes* nodes) {
        std::vector<NodeIndex>* children = nodes->GetIndicesForNeighbours(parentIndex);
        for (std::vector<NodeIndex>::iterator child = children->begin(); child != children->end(); ++child) {
            Node* node = nodes->GetNode(*child);
            if (node->parent == parentIndex) {
                node->depthInTree = depth + 1;
                UpdateTreeDepthOfChildren(*child, node->depthInTree, nodes);
            }
        }
    }
    
}
