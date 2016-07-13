//
//  vtkEdges.cxx
//  vtkGraphCut
//
//  Created by Berend Klein Haneveld on 11/07/16.
//
//

#include "vtkEdges.h"
#include "vtkEdge.h"
#include "vtkNodes.h"
#include <assert.h>
#include <iostream>


vtkEdges::vtkEdges() {
    _nodes = NULL;
    _edges = NULL;
}


vtkEdges::~vtkEdges() {
    Reset();
}


void vtkEdges::SetNodes(vtkNodes* nodes) {
    Reset();
    _nodes = nodes;
}


vtkNodes* vtkEdges::GetNodes() {
    return _nodes;
}


void vtkEdges::Update() {
    if (!_nodes || _nodes == NULL) {
        std::cout << "Warning: use SetNodes before running Update. Skipping Update().";
        return;
    }

    _edges = CreateEdgesForNodes(_nodes);
}


void vtkEdges::Reset() {
    _nodes = NULL;
    if (_edges) {
        for (std::vector<vtkEdge*>::iterator i = _edges->begin(); i != _edges->end(); ++i) {
            delete *i;
        }
        delete _edges;
    }
    _edges = NULL;
}


vtkEdge* vtkEdges::GetEdge(int index) {
    if (!_edges || index >= _edges->size() || index < 0) {
        return NULL;
    }
    return _edges->at(index);
}


int vtkEdges::GetSize() {
    return _edges->size();
}


std::vector<vtkEdge*>::iterator vtkEdges::GetBegin() {
    return _edges->begin();
}


std::vector<vtkEdge*>::iterator vtkEdges::GetEnd() {
    return _edges->end();
}


int vtkEdges::IndexForEdgeFromNodeToNode(int sourceIndex, int targetIndex) {
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
    
    vtkEdge* edge = _edges->at(index);
    if (to == NODE_SOURCE) {
        assert(edge->node1() == NODE_SOURCE && edge->node2() == from);
        return index;
    } else {
        while (edge->node1() <= from && edge->node2() != to) {
            ++index;
            edge = _edges->at(index);
        }
    }
    
    assert(edge->node1() == from && edge->node2() == to);
    return index;
}


vtkEdge* vtkEdges::EdgeFromNodeToNode(int sourceIndex, int targetIndex) {
    int index = IndexForEdgeFromNodeToNode(sourceIndex, targetIndex);
    return index >= 0 ? _edges->at(index) : NULL;
}


std::vector<int> vtkEdges::PathToRoot(int aNodeIndex, int* maxPossibleFlow) {
    std::vector<int> result;
    vtkNode* node = NULL;
    int nodeIndex = aNodeIndex;
    while (true) {
        if (nodeIndex < 0) {
            // Encountered root node
            break;
        }
        node = _nodes->GetNode(nodeIndex);
        int edgeIndex = IndexForEdgeFromNodeToNode(nodeIndex, node->parent);
        assert(edgeIndex >= 0);
        assert(edgeIndex < _edges->size());
        result.push_back(edgeIndex);
        vtkEdge* edge = _edges->at(edgeIndex);
        int possibleFlow = edge->capacityFromNode(node->tree == TREE_SOURCE ? node->parent : nodeIndex);
        *maxPossibleFlow = std::min(possibleFlow, *maxPossibleFlow);
        nodeIndex = node->parent;
        if (edge->isTerminal()) {
            break;
        }
    }
    return result;
}


void vtkEdges::PushFlowThroughEdges(int maxPossibleFlow, std::vector<int> edges, vtkTreeType tree, std::vector<int>* orphans) {
    for (std::vector<int>::iterator i = edges.begin(); i != edges.end(); ++i) {
        vtkEdge* edge = _edges->at(*i);
        
        int nodes[2] = {edge->node1(), edge->node2()};
        int closestToRoot = -1;
        
        if (edge->isTerminal()) {
            // One of the nodes is a negative number to indicate that it is a sink node
            closestToRoot = edge->node1() < edge->node2() ? 0 : 1;
        } else {
            vtkNode* node = _nodes->GetNode(edge->node1());
            vtkNode* otherNode = _nodes->GetNode(edge->node2());
            closestToRoot = node->depthInTree < otherNode->depthInTree ? 0 : 1;
        }
        
        int parent = nodes[closestToRoot];
        int child = nodes[(closestToRoot + 1) % 2];
        
        assert(child >= 0);
        
        // When tree type is SINK, then the flow should be pushed from child to parent
        int nodeFromWhichToPush = tree == TREE_SOURCE ? parent : child;
        
        // Update the flow
        edge->addFlowFromNode(nodeFromWhichToPush, maxPossibleFlow);
        
        if (edge->isSaturatedFromNode(nodeFromWhichToPush)) {
            // Push the child if the edge becomes saturated
            orphans->push_back(child);
        }
    }
}


std::vector<vtkEdge*>* vtkEdges::CreateEdgesForNodes(vtkNodes* nodes) {
    std::vector<vtkEdge*>* result = new std::vector<vtkEdge*>();
    
    int numberOfNodes = nodes->GetSize();
    int numberOfEdges = numberOfNodes * 2 + numberOfNodes * NumberOfEdgesForConnectivity(nodes->GetConnectivity());
    assert(numberOfEdges >= 0);
    result->reserve(numberOfEdges);
    
    for (int i = 0; i < numberOfNodes; ++i) {
        vtkEdge* sourceEdge = new vtkEdge(NODE_SOURCE, i);
        result->push_back(sourceEdge);
        
        vtkEdge* sinkEdge = new vtkEdge(i, NODE_SINK);
        result->push_back(sinkEdge);
        
        int coordinate[3] = {0, 0, 0};
        nodes->GetCoordinateForIndex(i, coordinate);
        int coord[3] = {0, 0, 0};
        int index = 0;
        for (int z = 0; z < 2; ++z) {
            for (int y = 0; y < 2; ++y) {
                for (int x = 0; x < 2; ++x) {
                    coord[0] = coordinate[0]+x;
                    coord[1] = coordinate[1]+y;
                    coord[2] = coordinate[2]+z;
                    if (nodes->IsNodeAtOffsetConnected(x, y, z)) {
                        vtkEdge* nodeEdge = new vtkEdge(i, nodes->IsValidCoordinate(coord) ? nodes->GetIndexForCoordinate(coord) : NODE_NONE);
                        result->push_back(nodeEdge);
                    }
                    ++index;
                }
            }
        }
    }
    
    return result;
}


int vtkEdges::NumberOfEdgesForConnectivity(vtkConnectivity connectivity) {
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