//
//  Edges.cxx
//  vtkGraphCut
//
//  Created by Berend Klein Haneveld on 11/07/16.
//
//

#include "Edges.h"
#include "Edge.h"
#include "Nodes.h"
#include <assert.h>
#include <iostream>


Edges::Edges() {
    _nodes = NULL;
    _edges = NULL;
}


Edges::~Edges() {
    Reset();
}


void Edges::SetNodes(Nodes* nodes) {
    Reset();
    _nodes = nodes;
}


Nodes* Edges::GetNodes() {
    return _nodes;
}


void Edges::Update() {
    if (!_nodes || _nodes == NULL) {
        std::cout << "Warning: use SetNodes before running Update. Skipping Update().";
        return;
    }

    _edges = CreateEdgesForNodes(_nodes);
}


void Edges::Reset() {
    _nodes = NULL;
    if (_edges) {
        for (std::vector<Edge*>::iterator i = _edges->begin(); i != _edges->end(); ++i) {
            delete *i;
        }
        delete _edges;
    }
    _edges = NULL;
}


Edge* Edges::GetEdge(EdgeIndex index) {
    if (!_edges || index >= _edges->size() || index < 0) {
        return NULL;
    }
    return _edges->at(index);
}


int Edges::GetSize() {
    return _edges->size();
}


std::vector<Edge*>::iterator Edges::GetBegin() {
    return _edges->begin();
}


std::vector<Edge*>::iterator Edges::GetEnd() {
    return _edges->end();
}


EdgeIndex Edges::IndexForEdgeFromNodeToNode(NodeIndex sourceIndex, NodeIndex targetIndex) {
    assert(sourceIndex != NODE_NONE);
    assert(targetIndex != NODE_NONE);
    
    int from;
    int to;
    
    if (targetIndex < 0 || sourceIndex < 0) {
        from = std::max(targetIndex, sourceIndex);
        to = std::min(sourceIndex, targetIndex);
        assert(from >= 0);
    } else if (targetIndex < sourceIndex) {
        from = targetIndex;
        to = sourceIndex;
    } else {
        from = sourceIndex;
        to = targetIndex;
    }
    
    int* dimensions = _nodes->GetDimensions();
    assert(from >= 0);
    assert(from < dimensions[0] * dimensions[1] * dimensions[2]);
    
    int index = from * (2 + NumberOfEdgesForConnectivity(_nodes->GetConnectivity()));
    assert(index >= from);
    
    Edge* edge = _edges->at(index);
    if (to == NODE_SOURCE) {
        assert(edge->node1() == NODE_SOURCE && edge->node2() == from);
        return (EdgeIndex)index;
    } else {
        while (edge->node1() <= from && edge->node2() != to) {
            ++index;
            edge = _edges->at(index);
        }
    }
    
    assert(edge->node1() == from && edge->node2() == to);
    return (EdgeIndex)index;
}


Edge* Edges::EdgeFromNodeToNode(NodeIndex sourceIndex, NodeIndex targetIndex) {
    int index = IndexForEdgeFromNodeToNode(sourceIndex, targetIndex);
    return index >= 0 ? _edges->at(index) : NULL;
}


std::vector<EdgeIndex> Edges::PathToRoot(NodeIndex aNodeIndex, int* maxPossibleFlow) {
    std::vector<EdgeIndex> result;
    Node* node = NULL;
    NodeIndex nodeIndex = aNodeIndex;
    while (true) {
        if (nodeIndex < 0) {
            // Encountered root node
            break;
        }
        node = _nodes->GetNode(nodeIndex);
        EdgeIndex edgeIndex = IndexForEdgeFromNodeToNode(nodeIndex, node->parent);
        assert(edgeIndex >= 0);
        assert(edgeIndex < _edges->size());
        result.push_back(edgeIndex);
        Edge* edge = _edges->at(edgeIndex);
        int possibleFlow = edge->capacityFromNode(node->tree == TREE_SOURCE ? node->parent : nodeIndex);
        *maxPossibleFlow = std::min(possibleFlow, *maxPossibleFlow);
        nodeIndex = node->parent;
        if (edge->isTerminal()) {
            break;
        }
    }
    return result;
}


void Edges::PushFlowThroughEdges(int maxPossibleFlow, std::vector<EdgeIndex> edges, vtkTreeType tree, std::vector<NodeIndex>* orphans) {
    for (std::vector<EdgeIndex>::iterator i = edges.begin(); i != edges.end(); ++i) {
        Edge* edge = _edges->at(*i);
        
        NodeIndex nodes[2] = {edge->node1(), edge->node2()};
        int closestToRoot = -1;
        
        if (edge->isTerminal()) {
            // One of the nodes is a negative number to indicate that it is a sink node
            closestToRoot = edge->node1() < edge->node2() ? 0 : 1;
        } else {
            Node* node = _nodes->GetNode(edge->node1());
            Node* otherNode = _nodes->GetNode(edge->node2());
            closestToRoot = node->depthInTree < otherNode->depthInTree ? 0 : 1;
        }
        
        NodeIndex parent = nodes[closestToRoot];
        NodeIndex child = nodes[(closestToRoot + 1) % 2];
        
        assert(child >= 0);
        
        // When tree type is SINK, then the flow should be pushed from child to parent
        NodeIndex nodeFromWhichToPush = tree == TREE_SOURCE ? parent : child;
        
        // Update the flow
        edge->addFlowFromNode(nodeFromWhichToPush, maxPossibleFlow);
        
        if (edge->isSaturatedFromNode(nodeFromWhichToPush)) {
            // Push the child if the edge becomes saturated
            orphans->push_back(child);
        }
    }
}


std::vector<Edge*>* Edges::CreateEdgesForNodes(Nodes* nodes) {
    std::vector<Edge*>* result = new std::vector<Edge*>();
    
    int numberOfNodes = nodes->GetSize();
    int numberOfEdges = numberOfNodes * 2 + numberOfNodes * NumberOfEdgesForConnectivity(nodes->GetConnectivity());
    assert(numberOfEdges >= 0);
    result->reserve(numberOfEdges);
    
    for (int i = 0; i < numberOfNodes; ++i) {
        Edge* sourceEdge = new Edge(NODE_SOURCE, (NodeIndex)i);
        result->push_back(sourceEdge);
        
        Edge* sinkEdge = new Edge((NodeIndex)i, NODE_SINK);
        result->push_back(sinkEdge);
        
        int coordinate[3] = {0, 0, 0};
        nodes->GetCoordinateForIndex((NodeIndex)i, coordinate);
        int coord[3] = {0, 0, 0};
        int index = 0;
        for (int z = 0; z < 2; ++z) {
            for (int y = 0; y < 2; ++y) {
                for (int x = 0; x < 2; ++x) {
                    coord[0] = coordinate[0]+x;
                    coord[1] = coordinate[1]+y;
                    coord[2] = coordinate[2]+z;
                    if (nodes->IsNodeAtOffsetConnected(x, y, z)) {
                        Edge* nodeEdge = new Edge((NodeIndex)i, nodes->IsValidCoordinate(coord) ? nodes->GetIndexForCoordinate(coord) : NODE_NONE);
                        result->push_back(nodeEdge);
                    }
                    ++index;
                }
            }
        }
    }
    
    return result;
}


int Edges::NumberOfEdgesForConnectivity(vtkConnectivity connectivity) {
    switch (connectivity) {
        case SIX:
            return 3;
        case EIGHTEEN:
            return 6;
        case TWENTYSIX:
            return 7;
        case UNCONNECTED:
            return 0;
    }
    return -1;
}