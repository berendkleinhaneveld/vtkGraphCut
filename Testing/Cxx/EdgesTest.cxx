//
//  EdgesTest.cxx
//  vtkGraphCut
//
//  Created by Berend Klein Haneveld on 10/07/16.
//
//

#include <iostream>
#include <assert.h>
#include "Edges.h"
#include "vtkNodes.h"
#include "vtkEdge.h"

void testEdgesConstructor();
void testEdgesProperties();
void testEdgesUpdate();
void testEdgesReset();

void testCreateEdges();
void testEdgeFromNodeToNode();
void testEdgeFromNodeToNodeWithConnectivity(Edges*);


int main() {
    testEdgesConstructor();
    testEdgesProperties();
    testEdgesUpdate();
    testEdgesReset();
    
    testCreateEdges();
    testEdgeFromNodeToNode();
    return 0;
}


void testEdgesConstructor() {
    std::cout << __FUNCTION__ << "\n";
    
    Edges* edges = new Edges();
    
    assert(edges->GetNodes() == NULL);
    
    delete edges;
    
    std::cout << "Done!" << "\n";
}


void testEdgesProperties() {
    std::cout << __FUNCTION__ << "\n";
    
    int dimensions[3] = {3, 3, 3};
    
    vtkNodes* nodes = new vtkNodes();
    nodes->SetDimensions(dimensions);
    nodes->SetConnectivity(SIX);

    Edges* edges = new Edges();
    edges->SetNodes(nodes);
    
    assert(edges->GetNodes() == nodes);
    
    delete edges;
    delete nodes;
    
    std::cout << "Done!" << "\n";
}


void testEdgesUpdate() {
    std::cout << __FUNCTION__ << "\n";
    
    int dimensions[3] = {3, 3, 3};
    
    vtkNodes* nodes = new vtkNodes();    
    nodes->SetDimensions(dimensions);
    nodes->SetConnectivity(SIX);
    nodes->Update();

    Edges* edges = new Edges();
    edges->SetNodes(nodes);

    assert(edges->GetEdge(0) == NULL);

    edges->Update();

    vtkEdge* someEdge = edges->GetEdge(0);
    assert(someEdge != NULL);
    assert(edges->GetEdge(-1) == NULL);
    assert(edges->GetEdge(300) == NULL);
    
    someEdge->setCapacity(1);
    
    assert(edges->GetEdge(0)->capacityFromNode(0) == 1);
    
    delete edges;
    delete nodes;
    
    std::cout << "Done!" << "\n";
}


void testEdgesReset() {
    std::cout << __FUNCTION__ << "\n";
    
    int dimensions[3] = {3, 3, 3};
    
    vtkNodes* nodes = new vtkNodes();
    nodes->SetDimensions(dimensions);
    nodes->SetConnectivity(SIX);
    nodes->Update();
    
    Edges* edges = new Edges();
    edges->SetNodes(nodes);
    edges->Update();
    edges->Reset();
    
    assert(edges->GetNodes() == NULL);
    assert(edges->GetEdge(0) == NULL);
    
    delete nodes;
    
    std::cout << "Done!" << "\n";
}


/**
 * Tests the creation of edges for different dimensions and connectivity values.
 */
void testCreateEdges() {
    std::cout << __FUNCTION__ << "\n";
    int dimensions[3] = {1, 1, 1};
    
    vtkNodes* nodes = new vtkNodes();
    nodes->SetDimensions(dimensions);
    nodes->SetConnectivity(TWENTYSIX);
    nodes->Update();
    
    Edges* edges = new Edges();
    edges->SetNodes(nodes);
    std::vector<vtkEdge*>* edgesVector = edges->CreateEdgesForNodes(nodes);
    
    int numberOfNodes = dimensions[0] * dimensions[1] * dimensions[2];
    int numberOfEdges = numberOfNodes * 2 + numberOfNodes * (7);
    assert(edgesVector->size() == numberOfEdges);
    
    dimensions[0] = 2;
    dimensions[1] = 1;
    dimensions[2] = 1;
    
    nodes->Reset();
    nodes->SetDimensions(dimensions);
    nodes->SetConnectivity(TWENTYSIX);
    nodes->Update();
    
    edges->SetNodes(nodes);
    edges->Update();
    
    edgesVector = edges->CreateEdgesForNodes(nodes);
    numberOfNodes = dimensions[0] * dimensions[1] * dimensions[2];
    numberOfEdges = numberOfNodes * 2 + numberOfNodes * (7);
    assert(edgesVector->size() == numberOfEdges);
    
    nodes->Reset();
    nodes->SetDimensions(dimensions);
    nodes->SetConnectivity(EIGHTEEN);
    nodes->Update();
    edgesVector = edges->CreateEdgesForNodes(nodes);
    numberOfEdges = numberOfNodes * 2 + numberOfNodes * (6);
    assert(edgesVector->size() == numberOfEdges);
    
    nodes->Reset();
    nodes->SetDimensions(dimensions);
    nodes->SetConnectivity(SIX);
    nodes->Update();
    edgesVector = edges->CreateEdgesForNodes(nodes);
    numberOfEdges = numberOfNodes * 2 + numberOfNodes * (3);
    assert(edgesVector->size() == numberOfEdges);
    
    dimensions[0] = 2;
    dimensions[1] = 2;
    dimensions[2] = 1;
    
    nodes->Reset();
    nodes->SetDimensions(dimensions);
    nodes->SetConnectivity(TWENTYSIX);
    nodes->Update();
    edgesVector = edges->CreateEdgesForNodes(nodes);
    numberOfNodes = dimensions[0] * dimensions[1] * dimensions[2];
    numberOfEdges = numberOfNodes * 2 + numberOfNodes * (7);
    assert(edgesVector->size() == numberOfEdges);
    
    nodes->Reset();
    nodes->SetDimensions(dimensions);
    nodes->SetConnectivity(EIGHTEEN);
    nodes->Update();
    edgesVector = edges->CreateEdgesForNodes(nodes);
    numberOfEdges = numberOfNodes * 2 + numberOfNodes * (6);
    assert(edgesVector->size() == numberOfEdges);
    
    nodes->Reset();
    nodes->SetDimensions(dimensions);
    nodes->SetConnectivity(SIX);
    nodes->Update();
    edgesVector = edges->CreateEdgesForNodes(nodes);
    numberOfEdges = numberOfNodes * 2 + numberOfNodes * (3);
    assert(edgesVector->size() == numberOfEdges);
    
    dimensions[0] = 2;
    dimensions[1] = 3;
    dimensions[2] = 1;
    
    nodes->Reset();
    nodes->SetDimensions(dimensions);
    nodes->SetConnectivity(TWENTYSIX);
    nodes->Update();
    edgesVector = edges->CreateEdgesForNodes(nodes);
    numberOfNodes = dimensions[0] * dimensions[1] * dimensions[2];
    numberOfEdges = numberOfNodes * 2 + numberOfNodes * (7);
    assert(edgesVector->size() == numberOfEdges);
    
    dimensions[0] = 20;
    dimensions[1] = 33;
    dimensions[2] = 12;
    
    nodes->Reset();
    nodes->SetDimensions(dimensions);
    nodes->SetConnectivity(TWENTYSIX);
    nodes->Update();
    edgesVector = edges->CreateEdgesForNodes(nodes);
    numberOfNodes = dimensions[0] * dimensions[1] * dimensions[2];
    numberOfEdges = numberOfNodes * 2 + numberOfNodes * (7);
    assert(edgesVector->size() == numberOfEdges);
    
    delete edges;
    delete nodes;
    
    std::cout << "Done!" << "\n";
}



/**
 * Tests getting the edge between two nodes for all connectivy values.
 * See testEdgeFromNodeToNodeWithConnectivity for more details.
 */
void testEdgeFromNodeToNode() {
    std::cout << __FUNCTION__ << "\n";
    int dimensions[3] = {30, 30, 30};
    
    vtkNodes* nodes = new vtkNodes();
    nodes->SetDimensions(dimensions);
    nodes->SetConnectivity(SIX);
    nodes->Update();
    
    Edges* edges = new Edges();
    edges->SetNodes(nodes);
    edges->Update();

    testEdgeFromNodeToNodeWithConnectivity(edges);
    
    nodes->Reset();
    nodes->SetDimensions(dimensions);
    nodes->SetConnectivity(EIGHTEEN);
    nodes->Update();

    edges->SetNodes(nodes);
    edges->Update();

    testEdgeFromNodeToNodeWithConnectivity(edges);
    
    nodes->Reset();
    nodes->SetDimensions(dimensions);
    nodes->SetConnectivity(TWENTYSIX);
    nodes->Update();

    edges->SetNodes(nodes);
    edges->Update();

    testEdgeFromNodeToNodeWithConnectivity(edges);
    
    delete edges;
    delete nodes;
    
    std::cout << "Done!\n";
}

/**
 * Tests getting the edge between two nodes for the specified connectivity.
 */
void testEdgeFromNodeToNodeWithConnectivity(Edges* edges) {
    vtkEdge* edge = edges->EdgeFromNodeToNode(0, 1);
    assert(edge->node1() == 0);
    assert(edge->node2() == 1);
    
    edge = edges->EdgeFromNodeToNode(1, 0);
    assert(edge->node1() == 0);
    assert(edge->node2() == 1);
    
    edge = edges->EdgeFromNodeToNode(NODE_SOURCE, 80);
    assert(edge->node1() == NODE_SOURCE);
    assert(edge->node2() == 80);
    
    edge = edges->EdgeFromNodeToNode(80, NODE_SOURCE);
    assert(edge->node1() == NODE_SOURCE);
    assert(edge->node2() == 80);
    
    edge = edges->EdgeFromNodeToNode(80, 81);
    assert(edge->node1() == 80);
    assert(edge->node2() == 81);
    
    edge = edges->EdgeFromNodeToNode(81, 80);
    assert(edge->node1() == 80);
    assert(edge->node2() == 81);
    
    edge = edges->EdgeFromNodeToNode(80, NODE_SINK);
    assert(edge->node1() == 80);
    assert(edge->node2() == NODE_SINK);
    
    edge = edges->EdgeFromNodeToNode(NODE_SINK, 80);
    assert(edge->node1() == 80);
    assert(edge->node2() == NODE_SINK);
    
    edge = edges->EdgeFromNodeToNode(80, 79);
    assert(edge->node1() == 79);
    assert(edge->node2() == 80);
    
    edge = edges->EdgeFromNodeToNode(79, 80);
    assert(edge->node1() == 79);
    assert(edge->node2() == 80);
}
