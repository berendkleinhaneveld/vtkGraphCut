//
//  vtkNodeTest.cpp
//  vtkGraphCut
//
//  Created by Berend Klein Haneveld on 10/07/16.
//
//

#include <iostream>
#include <assert.h>
#include "vtkNodes.h"

void testNodesConstructor();
void testNodesProperties();
void testNodesUpdate();
void testNodesReset();

void testCreateNodes();
void testIsValidCoordinate();
void testIndexForCoordinate();
void testCoordinateForIndex();
void testIndicesForNeighbours();


int main() {
    testNodesConstructor();
    testNodesProperties();
    testNodesUpdate();
    testNodesReset();
    
    testCreateNodes();
    testIsValidCoordinate();
    testIndexForCoordinate();
    testCoordinateForIndex();
    testIndicesForNeighbours();
    return 0;
}


void testNodesConstructor() {
    std::cout << __FUNCTION__ << "\n";
    
    vtkNodes* nodes = new vtkNodes();
    
    assert(nodes->GetConnectivity() == UNCONNECTED);
    assert(nodes->GetDimensions() == NULL);
    
    delete nodes;
    
    std::cout << "Done!" << "\n";
}


void testNodesProperties() {
    std::cout << __FUNCTION__ << "\n";
    
    int dimensions[3] = {3, 3, 3};
    
    vtkNodes* nodes = new vtkNodes();
    nodes->SetDimensions(dimensions);
    
    assert(nodes->GetDimensions()[0] == dimensions[0]);
    assert(nodes->GetDimensions()[1] == dimensions[1]);
    assert(nodes->GetDimensions()[2] == dimensions[2]);
    
    dimensions[0] = 4;
    
    assert(nodes->GetDimensions()[0] != dimensions[0]);
    
    nodes->SetDimensions(dimensions);
    
    assert(nodes->GetDimensions()[0] == dimensions[0]);
    
    nodes->SetConnectivity(SIX);
    
    assert(nodes->GetConnectivity() == SIX);
    
    nodes->SetConnectivity(TWENTYSIX);
    
    assert(nodes->GetConnectivity() == TWENTYSIX);
    
    delete nodes;
    
    std::cout << "Done!" << "\n";
}


void testNodesUpdate() {
    std::cout << __FUNCTION__ << "\n";
    
    int dimensions[3] = {3, 3, 3};
    
    vtkNodes* nodes = new vtkNodes();
    
    assert(nodes->GetNode(0) == NULL);
    
    nodes->SetDimensions(dimensions);
    nodes->SetConnectivity(SIX);
    nodes->Update();

    vtkNode* someNode = nodes->GetNode(0);
    assert(someNode != NULL);
    assert(nodes->GetNode(-1) == NULL);
    assert(nodes->GetNode(27) == NULL);
    
    someNode->depthInTree = 1;
    
    assert(nodes->GetNode(0)->depthInTree == 1);
    
    delete nodes;
    
    std::cout << "Done!" << "\n";
}


void testNodesReset() {
    std::cout << __FUNCTION__ << "\n";
    
    int dimensions[3] = {3, 3, 3};
    
    vtkNodes* nodes = new vtkNodes();
    nodes->SetDimensions(dimensions);
    nodes->SetConnectivity(SIX);
    nodes->Update();
    nodes->Reset();
    
    assert(nodes->GetConnectivity() == UNCONNECTED);
    assert(nodes->GetDimensions() == NULL);
    assert(nodes->GetNode(0) == NULL);
    
    delete nodes;
    
    std::cout << "Done!" << "\n";
}


/**
 * Tests the creation of nodes for different dimensions.
 * - CreateNodesForDimensions
 */
void testCreateNodes() {
    std::cout << __FUNCTION__ << "\n";
    int dimensions[3] = {3, 3, 2};
    
    vtkNodes* nodes = new vtkNodes();
    std::vector<vtkNode*>* nodesVector = nodes->CreateNodesForDimensions(dimensions);
    
    assert(nodesVector->size() == 18);
    
    dimensions[0] = 5;
    dimensions[1] = 2;
    dimensions[2] = 7;
    
    std::vector<vtkNode*>* moreNodes = nodes->CreateNodesForDimensions(dimensions);
    assert(moreNodes->size() == 70);
    
    delete nodes;
    
    std::cout << "Done!\n";
}


/**
 * Tests the validaty of given coordinate, given a certain dimension.
 * - IsValidCoordinate
 */
void testIsValidCoordinate() {
    std::cout << __FUNCTION__ << "\n";
    
    vtkNodes* nodes = new vtkNodes();
    
    int dimensions[3] = {4, 6, 7};
    nodes->SetDimensions(dimensions);
    
    int coordinate[3] = {4, 6, 7};
    assert(!nodes->IsValidCoordinate(coordinate));
    
    coordinate[0] -= 1;
    coordinate[1] -= 1;
    coordinate[2] -= 1;
    assert(nodes->IsValidCoordinate(coordinate));
    
    coordinate[0] = 0;
    coordinate[1] = 0;
    coordinate[2] = 0;
    assert(nodes->IsValidCoordinate(coordinate));
    
    coordinate[0] = -1;
    coordinate[1] = 0;
    coordinate[2] = 0;
    assert(!nodes->IsValidCoordinate(coordinate));
    
    coordinate[0] = 8;
    coordinate[1] = 3;
    coordinate[2] = 3;
    assert(!nodes->IsValidCoordinate(coordinate));
    
    delete nodes;

    std::cout << "Done!\n";
}


/**
 * Tests getting the node index for a given coordinate.
 * - IndexForCoordinate
 */
void testIndexForCoordinate() {
    std::cout << __FUNCTION__ << "\n";
    
    vtkNodes* nodes = new vtkNodes();
    
    int dimensions[3] = {2, 3, 4};
    int coordinate[3] = {0, 0, 0};
    
    nodes->SetDimensions(dimensions);
    
    int index = nodes->GetIndexForCoordinate(coordinate);
    assert(index == 0);
    
    coordinate[0] = 1;
    coordinate[1] = 0;
    coordinate[2] = 0;
    index = nodes->GetIndexForCoordinate(coordinate);
    assert(index == 1);
    
    coordinate[0] = 1;
    coordinate[1] = 1;
    coordinate[2] = 0;
    index = nodes->GetIndexForCoordinate(coordinate);
    assert(index == 3);
    
    coordinate[0] = 0;
    coordinate[1] = 0;
    coordinate[2] = 1;
    index = nodes->GetIndexForCoordinate(coordinate);
    assert(index == 6);
    
    coordinate[0] = 1;
    coordinate[1] = 0;
    coordinate[2] = 1;
    index = nodes->GetIndexForCoordinate(coordinate);
    assert(index == 7);
    
    delete nodes;

    std::cout << "Done!\n";
}



/**
 * Tests getting the coordinate for a given node index.
 * - CoordinateForIndex
 */
void testCoordinateForIndex() {
    std::cout << __FUNCTION__ << "\n";
    
    vtkNodes* nodes = new vtkNodes();
    
    int dimensions[3] = {2, 3, 4};
    nodes->SetDimensions(dimensions);
    
    int coordinate[3] = {0, 0, 0};
    
    nodes->GetCoordinateForIndex(0, coordinate);
    assert(coordinate[0] == 0 && coordinate[1] == 0 && coordinate[2] == 0);
    
    nodes->GetCoordinateForIndex(1, coordinate);
    assert(coordinate[0] == 1 && coordinate[1] == 0 && coordinate[2] == 0);
    
    nodes->GetCoordinateForIndex(2, coordinate);
    assert(coordinate[0] == 0 && coordinate[1] == 1 && coordinate[2] == 0);
    
    nodes->GetCoordinateForIndex(6, coordinate);
    assert(coordinate[0] == 0 && coordinate[1] == 0 && coordinate[2] == 1);
    
    nodes->GetCoordinateForIndex(8, coordinate);
    assert(coordinate[0] == 0 && coordinate[1] == 1 && coordinate[2] == 1);
    
    nodes->GetCoordinateForIndex(23, coordinate);
    assert(coordinate[0] == 1 && coordinate[1] == 2 && coordinate[2] == 3);
    
    assert(!nodes->GetCoordinateForIndex(24, coordinate));
    assert(!nodes->GetCoordinateForIndex(-1, coordinate));
    assert(nodes->GetCoordinateForIndex(23, coordinate));
    assert(nodes->GetCoordinateForIndex(0, coordinate));
    assert(nodes->GetCoordinateForIndex(1, coordinate));
    
    delete nodes;
    
    std::cout << "Done!\n";
}


/**
 * Tests the function to get all the neighbour indices for a certain
 * node index and a given connectivity.
 * - IndicesForNeighbours
 */
void testIndicesForNeighbours() {
    std::cout << __FUNCTION__ << "\n";
    vtkNodes* nodes = new vtkNodes();
    
    int dimensions[3] = {3, 4, 5};
    nodes->SetDimensions(dimensions);

    int coordinate[3] = {1, 1, 1};

    int index = nodes->GetIndexForCoordinate(coordinate);
    nodes->SetConnectivity(SIX);
    std::vector<int>* indices = nodes->GetIndicesForNeighbours(index);
    assert(indices->size() == 6);
    
    nodes->SetConnectivity(EIGHTEEN);
    indices = nodes->GetIndicesForNeighbours(index);
    assert(indices->size() == 18);
    
    nodes->SetConnectivity(TWENTYSIX);
    indices = nodes->GetIndicesForNeighbours(index);
    assert(indices->size() == 26);
    
    delete nodes;

    std::cout << "Done!\n";
}
