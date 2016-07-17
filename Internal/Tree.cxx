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
    
    /**
     * Returns all the neighbours that have the given @p parent as
     * their parent.
     */
    std::vector<NodeIndex> ChildrenForNode(NodeIndex parent, Nodes* nodes);
    
    /**
     * Returns the node index for the child of the given @p edge.
     */
    NodeIndex ChildForEdge(Edge* edge, Nodes* nodes);
    
}


Tree::Tree(vtkTreeType type, Edges* edges) {
    _edges = edges;
    _nodes = edges ? edges->GetNodes() : NULL;
    _treeType = type;
};


Tree::~Tree(){
    _edges = NULL;
    _nodes = NULL;
    _treeType = TREE_NONE;
};


Edges* Tree::GetEdges() {
    return _edges;
}

Nodes* Tree::GetNodes() {
    return _nodes;
}

vtkTreeType Tree::GetTreeType() {
    return _treeType;
}


void Tree::AddChildToParent(NodeIndex childIndex, NodeIndex parentIndex) {
    assert(childIndex != parentIndex);
    
    Edge* edge = _edges->EdgeFromNodeToNode(childIndex, parentIndex);
    assert(edge != NULL);
    
    Node* child = _nodes->GetNode(childIndex);
    
    child->tree = _treeType;
    if (edge->isTerminal()) {
        assert(edge->rootNode() == parentIndex);
        assert(edge->rootNode() == (int)_treeType);
        child->parent = parentIndex;
        child->depthInTree = 1;
    } else {
        Node* parent = _nodes->GetNode(parentIndex);
        assert(parent->tree == _treeType);
        child->parent = parentIndex;
        child->depthInTree = parent->depthInTree + 1;
    }
    // TODO: should orphan be updated here?
    child->orphan = false;
    
    UpdateTreeDepthOfChildren(childIndex, child->depthInTree, _nodes);
}


std::vector<EdgeIndex> Tree::PathToRoot(NodeIndex leafIndex, int* maxFlow) {
    std::vector<EdgeIndex> path;

    if (leafIndex < 0) {
        return path;
    }

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
    
    for (std::vector<EdgeIndex>::iterator edgeIndex = path.begin(); edgeIndex != path.end(); ++edgeIndex) {
        Edge* edge = _edges->GetEdge(*edgeIndex);
        NodeIndex childIndex = NODE_NONE;
        NodeIndex parentIndex = NODE_NONE;
        if (edge->isTerminal()) {
            childIndex = edge->nonRootNode();
            parentIndex = edge->rootNode();
            assert(_nodes->GetNode(childIndex)->depthInTree == 1);
        } else {
            Node* node1 = _nodes->GetNode(edge->node1());
            childIndex = node1->parent == edge->node2() ? edge->node1() : edge->node2();
            parentIndex = edge->node1() == childIndex ? edge->node2() : edge->node1();
            assert(_nodes->GetNode(parentIndex)->depthInTree < _nodes->GetNode(childIndex)->depthInTree);
        }
        assert(childIndex != parentIndex);
        NodeIndex pushFrom = _treeType == TREE_SOURCE ? parentIndex : childIndex;
        edge->addFlowFromNode(pushFrom, flow);
        if (edge->isSaturatedFromNode(pushFrom)) {
            orphans.push_back(childIndex);
            _nodes->GetNode(childIndex)->orphan = true;
        }
    }
    
    return orphans;
}


void Tree::Adopt(NodeIndex orphanIndex) {
    assert(_nodes->GetNode(orphanIndex)->tree == _treeType);
    std::vector<NodeIndex> neighbours = _nodes->GetIndicesForNeighbours(orphanIndex);
    neighbours.push_back((NodeIndex)_treeType);
    NodeIndex bestParent = NODE_NONE;
    int bestDepthInTree = -1;
    for (std::vector<NodeIndex>::iterator neighbour = neighbours.begin(); neighbour != neighbours.end(); ++neighbour) {
        Edge* edge = _edges->EdgeFromNodeToNode(orphanIndex, *neighbour);
        int depthInTree = -1;
        if (!edge->isTerminal()) {
            Node* node = _nodes->GetNode(*neighbour);
            if (node->parent == orphanIndex) {
                continue;
            }
            
            if (node->tree != _treeType) {
                continue;
            }
            
            // TODO: maybe check the whole route to root for orphans?
            if (node->orphan) {
                continue;
            }
            
            depthInTree = node->depthInTree;
        } else {
            depthInTree = 0;
        }
        
        NodeIndex childIndex = ChildForEdge(edge, _nodes);
        NodeIndex parentIndex = edge->otherNode(childIndex);
        NodeIndex pushFrom = _treeType == TREE_SOURCE ? parentIndex : childIndex;
        if (edge->isSaturatedFromNode(pushFrom)) {
            continue;
        }
        
        if (bestDepthInTree < 0 || depthInTree < bestDepthInTree) {
            bestParent = *neighbour;
            bestDepthInTree = depthInTree;
        }
    }
    
    if (bestParent != NODE_NONE) {
        AddChildToParent(orphanIndex, bestParent);
    } else {
        Node* node = _nodes->GetNode(orphanIndex);
        node->orphan = false;
        node->parent = NODE_NONE;
        node->depthInTree = -1;
        
        std::vector<NodeIndex> children = ChildrenForNode(orphanIndex, _nodes);
        for (std::vector<NodeIndex>::iterator childIndex = children.begin(); childIndex != children.end(); ++childIndex) {
            Node* child = _nodes->GetNode(*childIndex);
            child->orphan = true;
            child->parent = NODE_NONE;
            Adopt(*childIndex);
        }
    }
    
}


namespace {
    
    void UpdateTreeDepthOfChildren(NodeIndex parentIndex, int depth, Nodes* nodes) {
        std::vector<NodeIndex> children = ChildrenForNode(parentIndex, nodes);
        for (std::vector<NodeIndex>::iterator child = children.begin(); child != children.end(); ++child) {
            Node* node = nodes->GetNode(*child);
            if (node->parent == parentIndex) {
                node->depthInTree = depth + 1;
                // TODO: should orphan be updated here?
                node->orphan = false;
                UpdateTreeDepthOfChildren(*child, node->depthInTree, nodes);
            }
        }
    }
    
    std::vector<NodeIndex> ChildrenForNode(NodeIndex parentIndex, Nodes* nodes) {
        std::vector<NodeIndex> neighbours = nodes->GetIndicesForNeighbours(parentIndex);
        std::vector<NodeIndex> children;
        for (std::vector<NodeIndex>::iterator neighbour = neighbours.begin(); neighbour != neighbours.end(); ++neighbour) {
            if (nodes->GetNode(*neighbour)->parent == parentIndex) {
                children.push_back(*neighbour);
            }
        }
        return children;
    }
    
    NodeIndex ChildForEdge(Edge* edge, Nodes* nodes) {
        NodeIndex childIndex = NODE_NONE;
        if (edge->isTerminal()) {
            childIndex = edge->nonRootNode();
        } else {
            Node* node1 = nodes->GetNode(edge->node1());
            childIndex = node1->parent == edge->node2() ? edge->node1() : edge->node2();
        }
        return childIndex;
    }
    
}
