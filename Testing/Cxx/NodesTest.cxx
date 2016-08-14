//
//  NodesTest.cxx
//  vtkGraphCut
//
//  Created by Berend Klein Haneveld on 10/07/16.
//
//

#include <assert.h>
#include "Internal/Nodes.h"


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
    Nodes* nodes = new Nodes();
    
    assert(nodes->GetConnectivity() == UNCONNECTED);
    assert(nodes->GetDimensions() == NULL);
    
    delete nodes;
}


void testNodesProperties() {
    int dimensions[3] = {3, 3, 3};
    
    Nodes* nodes = new Nodes();
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
}


void testNodesUpdate() {
    int dimensions[3] = {3, 3, 3};
    
    Nodes* nodes = new Nodes();
    
    assert(nodes->GetNode(0) == NULL);
    
    nodes->SetDimensions(dimensions);
    nodes->SetConnectivity(SIX);
    nodes->Update();

    assert(nodes->GetSize() == 27);
    
    Node* someNode = nodes->GetNode(0);
    assert(someNode != NULL);
    assert(nodes->GetNode(-1) == NULL);
    assert(nodes->GetNode(27) == NULL);
    
    someNode->depthInTree = 1;
    
    assert(nodes->GetNode(0)->depthInTree == 1);
    
    delete nodes;
}


void testNodesReset() {
    int dimensions[3] = {3, 3, 3};
    
    Nodes* nodes = new Nodes();
    nodes->SetDimensions(dimensions);
    nodes->SetConnectivity(SIX);
    nodes->Update();
    nodes->Reset();
    
    assert(nodes->GetConnectivity() == UNCONNECTED);
    assert(nodes->GetDimensions() == NULL);
    assert(nodes->GetNode(0) == NULL);
    assert(nodes->GetSize() == 0);
    assert(nodes->IsNodeAtOffsetConnected(0, 0, 0) == false);
    
    delete nodes;
}


/**
 * Tests the creation of nodes for different dimensions.
 * - CreateNodesForDimensions
 */
void testCreateNodes() {
    int dimensions[3] = {3, 3, 2};
    
    Nodes* nodes = new Nodes();
    std::vector<Node*>* nodesVector = nodes->CreateNodesForDimensions(dimensions);
    
    assert(nodesVector->size() == 18);
    
    dimensions[0] = 5;
    dimensions[1] = 2;
    dimensions[2] = 7;
    
    std::vector<Node*>* moreNodes = nodes->CreateNodesForDimensions(dimensions);
    assert(moreNodes->size() == 70);
    
    delete nodes;
}


/**
 * Tests the validaty of given coordinate, given a certain dimension.
 * - IsValidCoordinate
 */
void testIsValidCoordinate() {
    Nodes* nodes = new Nodes();
    
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
}


/**
 * Tests getting the node index for a given coordinate.
 * - IndexForCoordinate
 */
void testIndexForCoordinate() {
    Nodes* nodes = new Nodes();
    
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
}



/**
 * Tests getting the coordinate for a given node index.
 * - CoordinateForIndex
 */
void testCoordinateForIndex() {
    Nodes* nodes = new Nodes();
    
    int dimensions[3] = {2, 3, 4};
    nodes->SetDimensions(dimensions);
    
    int coordinate[3] = {0, 0, 0};
    
    nodes->GetCoordinateForIndex((NodeIndex)0, coordinate);
    assert(coordinate[0] == 0 && coordinate[1] == 0 && coordinate[2] == 0);
    
    nodes->GetCoordinateForIndex((NodeIndex)1, coordinate);
    assert(coordinate[0] == 1 && coordinate[1] == 0 && coordinate[2] == 0);
    
    nodes->GetCoordinateForIndex((NodeIndex)2, coordinate);
    assert(coordinate[0] == 0 && coordinate[1] == 1 && coordinate[2] == 0);
    
    nodes->GetCoordinateForIndex((NodeIndex)6, coordinate);
    assert(coordinate[0] == 0 && coordinate[1] == 0 && coordinate[2] == 1);
    
    nodes->GetCoordinateForIndex((NodeIndex)8, coordinate);
    assert(coordinate[0] == 0 && coordinate[1] == 1 && coordinate[2] == 1);
    
    nodes->GetCoordinateForIndex((NodeIndex)23, coordinate);
    assert(coordinate[0] == 1 && coordinate[1] == 2 && coordinate[2] == 3);
    
    assert(!nodes->GetCoordinateForIndex((NodeIndex)24, coordinate));
    assert(!nodes->GetCoordinateForIndex((NodeIndex)-1, coordinate));
    assert(nodes->GetCoordinateForIndex((NodeIndex)23, coordinate));
    assert(nodes->GetCoordinateForIndex((NodeIndex)0, coordinate));
    assert(nodes->GetCoordinateForIndex((NodeIndex)1, coordinate));
    
    delete nodes;
}


/**
 * Tests the function to get all the neighbour indices for a certain
 * node index and a given connectivity.
 * - IndicesForNeighbours
 */
void testIndicesForNeighbours() {
    Nodes* nodes = new Nodes();
    
    int dimensions[3] = {3, 4, 5};
    nodes->SetDimensions(dimensions);

    int coordinate[3] = {1, 1, 1};

    NodeIndex index = nodes->GetIndexForCoordinate(coordinate);
    nodes->SetConnectivity(SIX);
    std::vector<NodeIndex> indices = nodes->GetIndicesForNeighbours(index);
    assert(indices.size() == 6);
    
    nodes->SetConnectivity(EIGHTEEN);
    indices = nodes->GetIndicesForNeighbours(index);
    assert(indices.size() == 18);
    
    nodes->SetConnectivity(TWENTYSIX);
    indices = nodes->GetIndicesForNeighbours(index);
    assert(indices.size() == 26);
    
    delete nodes;
}
