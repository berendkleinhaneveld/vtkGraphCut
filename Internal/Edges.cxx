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
    _dirty = true;
}


Edges::~Edges() {
    Reset();
}


void Edges::SetNodes(Nodes* nodes) {
    Reset();
    _nodes = nodes;
    _dirty = true;
}


Nodes* Edges::GetNodes() {
    return _nodes;
}


void Edges::Update() {
    if (!_nodes || _nodes == NULL) {
        std::cout << "Warning: use SetNodes before running Update. Skipping Update().\n";
        return;
    }
    
    if (!_dirty) {
        return;
    }

    _edges = CreateEdgesForNodes(_nodes);
    _dirty = false;
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
    _dirty = true;
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
        // Sink nodes
        from = std::max(sourceIndex, targetIndex);
        to = std::min(sourceIndex, targetIndex);
        assert(from >= 0);
    } else {
        from = std::min(sourceIndex, targetIndex);
        to = std::max(sourceIndex, targetIndex);
    }
    
    int* dimensions = _nodes->GetDimensions();
    assert(from >= 0);
    assert(from < dimensions[0] * dimensions[1] * dimensions[2]);
    
    int index = 0;
    
    Edge* edge = _edges->at(index);
    while (!((edge->node1() == from && edge->node2() == to) || (edge->node1() == to && edge->node2() == from))) {
        ++index;
        if (index >= _edges->size()) {
            break;
        }
        edge = _edges->at(index);
    }
    
    if ((edge->node1() == from && edge->node2() == to) || (edge->node2() == from && edge->node1() == to)) {
        return (EdgeIndex)index;
    }

    return EDGE_NONE;
}


Edge* Edges::EdgeFromNodeToNode(NodeIndex sourceIndex, NodeIndex targetIndex) {
    assert(sourceIndex != NODE_NONE);
    assert(targetIndex != NODE_NONE);
    int index = IndexForEdgeFromNodeToNode(sourceIndex, targetIndex);
    if (index >= 0) {
        assert(_edges->at(index)->isValid());
        return _edges->at(index);
    }

    return NULL;
}


std::vector<Edge*>* Edges::CreateEdgesForNodes(Nodes* nodes) {
    std::vector<Edge*>* result = new std::vector<Edge*>();
    
    int numberOfNodes = nodes->GetSize();
    int numberOfEdges = numberOfNodes * 2 + numberOfNodes * NumberOfEdgesForConnectivity(nodes->GetConnectivity());

    assert(numberOfEdges >= 0);
    result->reserve(numberOfEdges);
    
    for (int index = 0; index < numberOfNodes; ++index) {
        Edge* sourceEdge = new Edge(NODE_SOURCE, (NodeIndex)index);
        result->push_back(sourceEdge);
        
        Edge* sinkEdge = new Edge((NodeIndex)index, NODE_SINK);
        result->push_back(sinkEdge);
        
        int coordinate[3] = {0, 0, 0};
        nodes->GetCoordinateForIndex((NodeIndex)index, coordinate);
        int coord[3] = {0, 0, 0};
        for (int z = -1; z <= 1; ++z) {
            for (int y = -1; y <= 1; ++y) {
                for (int x = -1; x <= 1; ++x) {
                    coord[0] = coordinate[0]+x;
                    coord[1] = coordinate[1]+y;
                    coord[2] = coordinate[2]+z;
                    if (nodes->IsNodeAtOffsetConnected(x, y, z)) {
                        NodeIndex neighbour_index = nodes->IsValidCoordinate(coord) ? nodes->GetIndexForCoordinate(coord) : NODE_NONE;
                        if (neighbour_index < index) {
                            neighbour_index = NODE_NONE;
                        }
                        
                        if (neighbour_index != NODE_NONE) {
                            Edge* nodeEdge = new Edge((NodeIndex)index, neighbour_index);
                            result->push_back(nodeEdge);
                        }
                    }
                }
            }
        }
    }
    
    return result;
}


int Edges::NumberOfEdgesForConnectivity(vtkConnectivity connectivity) {
    switch (connectivity) {
        case SIX:
            return 6;
        case EIGHTEEN:
            return 18;
        case TWENTYSIX:
            return 26;
        case UNCONNECTED:
            return 0;
    }
}
