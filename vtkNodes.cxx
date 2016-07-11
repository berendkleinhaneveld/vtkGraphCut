//
//  vtkNodes.cxx
//  vtkGraphCut
//
//  Created by Berend Klein Haneveld on 11/07/16.
//
//

#include "vtkNodes.h"
#include <cstdlib>
#include <assert.h>
#include <stdio.h>


vtkNodes::vtkNodes() {
    _nodes = NULL;
    _dimensions = NULL;
    Reset();
}


vtkNodes::~vtkNodes() {
    Reset();
}


void vtkNodes::SetConnectivity(vtkConnectivity connectivity) {
    _connectivity = connectivity;
}


vtkConnectivity vtkNodes::GetConnectivity() {
    return _connectivity;
}


void vtkNodes::SetDimensions(int *dimensions) {
    if (_dimensions == NULL) {
        _dimensions = new int[3];
    }
    for (int i = 0; i < 3; i++) {
        _dimensions[i] = dimensions[i];
    }
}


int* vtkNodes::GetDimensions() {
    return _dimensions;
}


void vtkNodes::Update() {
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


void vtkNodes::Reset() {
    if (_nodes != NULL) {
        for (std::vector<vtkNode*>::iterator i = _nodes->begin(); i != _nodes->end(); ++i) {
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


std::vector<int>* vtkNodes::GetIndicesForNeighbours(int index) {
    std::vector<int>* result = new std::vector<int>();
    
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
                    result->push_back(GetIndexForCoordinate(coord));
                }
            }
        }
    }
    return result;
}


bool vtkNodes::IsValidCoordinate(int* coordinate) {
    for (int i = 0; i < 3; ++i) {
        if (coordinate[i] >= _dimensions[i] || coordinate[i] < 0) {
            return false;
        }
    }
    return true;
}


int vtkNodes::GetIndexForCoordinate(int* coordinate) {
    assert(coordinate[0] < _dimensions[0]);
    assert(coordinate[1] < _dimensions[1]);
    assert(coordinate[2] < _dimensions[2]);
    assert(coordinate[0] >= 0);
    assert(coordinate[1] >= 0);
    assert(coordinate[2] >= 0);
    
    return coordinate[0]
    + coordinate[1] * _dimensions[0]
    + coordinate[2] * _dimensions[0] * _dimensions[1];
}


bool vtkNodes::GetCoordinateForIndex(int index, int* coordinate) {
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


bool vtkNodes::IsNodeAtOffsetConnected(int x, int y, int z) {
    switch (_connectivity) {
        case SIX:
            return (std::abs(x + y) == 1 && z == 0)
            || (std::abs(y + z) == 1 && x == 0)
            || (std::abs(z + x) == 1 && y == 0);
        case EIGHTEEN:
            return (x != 0 || y != 0 || z != 0)
            && (std::abs(x) + std::abs(y) + std::abs(z) != 3);
        case TWENTYSIX:
            return (x != 0 || y != 0 || z != 0);
        default:
            return false;
    }
}


vtkNode* vtkNodes::GetNode(int index) {
    if (_nodes == NULL || index < 0 || index >= _nodes->size()) {
        return NULL;
    }
    return _nodes->at(index);
}


std::vector<vtkNode*>* vtkNodes::CreateNodesForDimensions(int* dimensions) {
    std::vector<vtkNode*>* result = new std::vector<vtkNode*>();
    
    int numberOfVertices = dimensions[0] * dimensions[1] * dimensions[2];
    result->reserve(numberOfVertices);
    
    for (int i = 0; i < numberOfVertices; i++) {
        vtkNode* node = new vtkNode();
        result->push_back(node);
    }
    
    return result;
}

