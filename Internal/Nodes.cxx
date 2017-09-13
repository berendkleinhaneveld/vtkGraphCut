//
//  Nodes.cxx
//  vtkGraphCut
//
//  Created by Berend Klein Haneveld on 11/07/16.
//
//

#include "Nodes.h"
#include <cstdlib>
#include <assert.h>
#include <stdio.h>


Nodes::Nodes() {
    _nodes = NULL;
    _dimensions = NULL;
    Reset();
}


Nodes::~Nodes() {
    Reset();
}


void Nodes::SetConnectivity(vtkConnectivity connectivity) {
    _connectivity = connectivity;
}


vtkConnectivity Nodes::GetConnectivity() {
    return _connectivity;
}


void Nodes::SetDimensions(int *dimensions) {
    if (_dimensions == NULL) {
        _dimensions = new int[3];
    }
    for (int i = 0; i < 3; i++) {
        _dimensions[i] = dimensions[i];
    }
}


int* Nodes::GetDimensions() {
    return _dimensions;
}


void Nodes::Update() {
    if (_connectivity == UNCONNECTED) {
        printf("No connectivity is specified. Skipping update.");
        return;
    }
    
    if (!_dimensions) {
        printf("No dimensions are set. Skipping update.");
        return;
    }
    
    if (!_nodes) {
        _nodes = CreateNodesForDimensions(_dimensions);
    }
}


void Nodes::Reset() {
    if (_nodes != NULL) {
        for (std::vector<Node*>::iterator i = _nodes->begin(); i != _nodes->end(); ++i) {
            delete *i;
        }
        delete _nodes;
    }
    _nodes = NULL;
    _connectivity = UNCONNECTED;
    if (_dimensions != NULL) {
        delete _dimensions;
    }
    _dimensions = NULL;
}


std::vector<NodeIndex> Nodes::GetIndicesForNeighbours(NodeIndex index) {
    std::vector<NodeIndex> result;
    
    int coordinate[3];
    GetCoordinateForIndex(index, coordinate);
    int coord[3] = {0, 0, 0};
    for (int z = -1; z < 2; ++z) {
        for (int y = -1; y < 2; ++y) {
            for (int x = -1; x < 2; ++x) {
                coord[0] = coordinate[0]+x;
                coord[1] = coordinate[1]+y;
                coord[2] = coordinate[2]+z;
                if (IsNodeAtOffsetConnected(x, y, z)
                    && IsValidCoordinate(coord)) {
                    result.push_back(GetIndexForCoordinate(coord));
                }
            }
        }
    }
    return result;
}


bool Nodes::IsValidCoordinate(int* coordinate) {
    for (int i = 0; i < 3; ++i) {
        if (coordinate[i] >= _dimensions[i] || coordinate[i] < 0) {
            return false;
        }
    }
    return true;
}


NodeIndex Nodes::GetIndexForCoordinate(int* coordinate) {
    assert(coordinate[0] < _dimensions[0]);
    assert(coordinate[1] < _dimensions[1]);
    assert(coordinate[2] < _dimensions[2]);
    assert(coordinate[0] >= 0);
    assert(coordinate[1] >= 0);
    assert(coordinate[2] >= 0);
    
    return (NodeIndex) (coordinate[0]
                        + coordinate[1] * _dimensions[0]
                        + coordinate[2] * _dimensions[0] * _dimensions[1]);
}


bool Nodes::GetCoordinateForIndex(NodeIndex index, int* coordinate) {
    if (index >= _dimensions[0] * _dimensions[1] * _dimensions[2] || index < 0) {
        return false;
    }
    
    int dims = _dimensions[0] * _dimensions[1];
    int rest = index;
    coordinate[2] = rest / dims;
    rest -= coordinate[2] * dims;
    dims = _dimensions[0];
    coordinate[1] = rest / dims;
    rest -= coordinate[1] * dims;
    coordinate[0] = rest;
    
    return true;
}


bool Nodes::IsNodeAtOffsetConnected(int x, int y, int z) {
    assert(std::abs(x) < 2 && std::abs(y) < 2 && std::abs(z) < 2);
    switch (_connectivity) {
        case SIX:
            return (std::abs(x + y) == 1 && z == 0)
            || (std::abs(y + z) == 1 && x == 0)
            || (std::abs(z + x) == 1 && y == 0);
        case EIGHTEEN:
            return (x == 0 || y == 0 || z == 0) && !(x == 0 && y == 0 && z == 0);
        case TWENTYSIX:
            return !(x == 0 && y == 0 && z == 0);
            break;
        default:
            return false;
    }
}


Node* Nodes::GetNode(int index) {
    if (_nodes == NULL || index < 0 || index >= _nodes->size()) {
        return NULL;
    }
    return _nodes->at(index);
}


int Nodes::GetSize() {
    if (_nodes == NULL) {
        return 0;
    }
    return _nodes->size();
}


std::vector<Node*>::iterator Nodes::GetIterator() {
    return _nodes->begin();
}


std::vector<Node*>::iterator Nodes::GetEnd() {
    return _nodes->end();
}


std::vector<Node*>* Nodes::CreateNodesForDimensions(int* dimensions) {
    std::vector<Node*>* result = new std::vector<Node*>();
    
    int numberOfVertices = dimensions[0] * dimensions[1] * dimensions[2];
    result->reserve(numberOfVertices);
    
    for (int i = 0; i < numberOfVertices; i++) {
        Node* node = new Node();
        result->push_back(node);
    }
    
    return result;
}

