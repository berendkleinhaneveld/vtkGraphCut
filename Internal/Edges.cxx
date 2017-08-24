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
    
    if (edge->node1() == from && edge->node2() == to) {
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
}
