//
//  vtkGraphCutTestApp.cxx
//  vtkGraphCut
//
//  Created by Berend Klein Haneveld.
//
//

#include <assert.h>
#include <iostream>
#include <vtkPoints.h>
#include "vtkGraphCutCostFunctionSimple.h"
#include "vtkEdge.h"
#include "vtkNode.h"
#include "vtkGraphCutDataTypes.h"
#include "vtkGraphCutProtected.h"
#include "vtkNodes.h"


void testCreateEdges();
void testEdgeFromNodeToNode();
void testEdgeFromNodeToNodeWithConnectivity(vtkGraphCutProtected*, std::vector<vtkEdge>*, int*, vtkConnectivity);
void testSettingSeedPoints();
void testIncomingEdge();
void testIncomingEdgeWithConnectivity(vtkGraphCutProtected*, vtkNodes*, std::vector<vtkEdge>*, int*, vtkConnectivity);


int main(int argc, char const *argv[]) {
    
    testCreateEdges();
    testEdgeFromNodeToNode();
    testSettingSeedPoints();
//  testIncomingEdge();
    
    return 0;
}


/**
 * Tests the creation of edges for different dimensions and connectivity values.
 */
void testCreateEdges() {
    std::cout << __FUNCTION__ << "\n";
    int dimensions[3] = {1, 1, 1};

    vtkGraphCutProtected* graphCut = vtkGraphCutProtected::New();
    vtkNodes* nodes = new vtkNodes();
    nodes->SetDimensions(dimensions);
    nodes->SetConnectivity(TWENTYSIX);
    nodes->Update();
    
    std::vector<vtkEdge>* edges = graphCut->CreateEdgesForNodes(nodes, dimensions, TWENTYSIX);

    int numberOfNodes = dimensions[0] * dimensions[1] * dimensions[2];
    int numberOfEdges = numberOfNodes * 2 + numberOfNodes * (7);
    assert(edges->size() == numberOfEdges);

    dimensions[0] = 2;
    dimensions[1] = 1;
    dimensions[2] = 1;

    nodes->Reset();
    nodes->SetDimensions(dimensions);
    nodes->SetConnectivity(TWENTYSIX);
    nodes->Update();
    edges = graphCut->CreateEdgesForNodes(nodes, dimensions, TWENTYSIX);
    numberOfNodes = dimensions[0] * dimensions[1] * dimensions[2];
    numberOfEdges = numberOfNodes * 2 + numberOfNodes * (7);
    assert(edges->size() == numberOfEdges);

    nodes->Reset();
    nodes->SetDimensions(dimensions);
    nodes->SetConnectivity(EIGHTEEN);
    nodes->Update();
    edges = graphCut->CreateEdgesForNodes(nodes, dimensions, EIGHTEEN);
    numberOfEdges = numberOfNodes * 2 + numberOfNodes * (6);
    assert(edges->size() == numberOfEdges);

    nodes->Reset();
    nodes->SetDimensions(dimensions);
    nodes->SetConnectivity(SIX);
    nodes->Update();
    edges = graphCut->CreateEdgesForNodes(nodes, dimensions, SIX);
    numberOfEdges = numberOfNodes * 2 + numberOfNodes * (3);
    assert(edges->size() == numberOfEdges);

    dimensions[0] = 2;
    dimensions[1] = 2;
    dimensions[2] = 1;

    nodes->Reset();
    nodes->SetDimensions(dimensions);
    nodes->SetConnectivity(TWENTYSIX);
    nodes->Update();
    edges = graphCut->CreateEdgesForNodes(nodes, dimensions, TWENTYSIX);
    numberOfNodes = dimensions[0] * dimensions[1] * dimensions[2];
    numberOfEdges = numberOfNodes * 2 + numberOfNodes * (7);
    assert(edges->size() == numberOfEdges);

    nodes->Reset();
    nodes->SetDimensions(dimensions);
    nodes->SetConnectivity(EIGHTEEN);
    nodes->Update();
    edges = graphCut->CreateEdgesForNodes(nodes, dimensions, EIGHTEEN);
    numberOfEdges = numberOfNodes * 2 + numberOfNodes * (6);
    assert(edges->size() == numberOfEdges);

    nodes->Reset();
    nodes->SetDimensions(dimensions);
    nodes->SetConnectivity(SIX);
    nodes->Update();
    edges = graphCut->CreateEdgesForNodes(nodes, dimensions, SIX);
    numberOfEdges = numberOfNodes * 2 + numberOfNodes * (3);
    assert(edges->size() == numberOfEdges);

    dimensions[0] = 2;
    dimensions[1] = 3;
    dimensions[2] = 1;

    nodes->Reset();
    nodes->SetDimensions(dimensions);
    nodes->SetConnectivity(TWENTYSIX);
    nodes->Update();
    edges = graphCut->CreateEdgesForNodes(nodes, dimensions, TWENTYSIX);
    numberOfNodes = dimensions[0] * dimensions[1] * dimensions[2];
    numberOfEdges = numberOfNodes * 2 + numberOfNodes * (7);
    assert(edges->size() == numberOfEdges);

    dimensions[0] = 20;
    dimensions[1] = 33;
    dimensions[2] = 12;

    nodes->Reset();
    nodes->SetDimensions(dimensions);
    nodes->SetConnectivity(TWENTYSIX);
    nodes->Update();
    edges = graphCut->CreateEdgesForNodes(nodes, dimensions, TWENTYSIX);
    numberOfNodes = dimensions[0] * dimensions[1] * dimensions[2];
    numberOfEdges = numberOfNodes * 2 + numberOfNodes * (7);
    assert(edges->size() == numberOfEdges);

    graphCut->Delete();
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
    vtkGraphCutProtected* graphCut = vtkGraphCutProtected::New();
    vtkNodes* nodes = new vtkNodes();
    nodes->SetDimensions(dimensions);
    nodes->SetConnectivity(SIX);
    nodes->Update();

    std::vector<vtkEdge>* edges = graphCut->CreateEdgesForNodes(nodes, dimensions, SIX);
    testEdgeFromNodeToNodeWithConnectivity(graphCut, edges, dimensions, SIX);
    
    nodes->Reset();
    nodes->SetDimensions(dimensions);
    nodes->SetConnectivity(EIGHTEEN);
    nodes->Update();
    edges = graphCut->CreateEdgesForNodes(nodes, dimensions, EIGHTEEN);
    testEdgeFromNodeToNodeWithConnectivity(graphCut, edges, dimensions, EIGHTEEN);

    nodes->Reset();
    nodes->SetDimensions(dimensions);
    nodes->SetConnectivity(TWENTYSIX);
    nodes->Update();
    edges = graphCut->CreateEdgesForNodes(nodes, dimensions, TWENTYSIX);
    testEdgeFromNodeToNodeWithConnectivity(graphCut, edges, dimensions, TWENTYSIX);

    graphCut->Delete();
    delete nodes;

    std::cout << "Done!\n";
}

/**
 * Tests getting the edge between two nodes for the specified connectivity.
 */
void testEdgeFromNodeToNodeWithConnectivity(vtkGraphCutProtected* graphCut, std::vector<vtkEdge>* edges, int* dimensions, vtkConnectivity connectivity) {
    vtkEdge edge = graphCut->EdgeFromNodeToNode(edges, 0, 1, dimensions, connectivity);
    assert(edge.node1() == 0);
    assert(edge.node2() == 1);

    edge = graphCut->EdgeFromNodeToNode(edges, 1, 0, dimensions, connectivity);
    assert(edge.node1() == 0);
    assert(edge.node2() == 1);

    edge = graphCut->EdgeFromNodeToNode(edges, SOURCE, 80, dimensions, connectivity);
    assert(edge.node1() == SOURCE);
    assert(edge.node2() == 80);

    edge = graphCut->EdgeFromNodeToNode(edges, 80, SOURCE, dimensions, connectivity);
    assert(edge.node1() == SOURCE);
    assert(edge.node2() == 80);

    edge = graphCut->EdgeFromNodeToNode(edges, 80, 81, dimensions, connectivity);
    assert(edge.node1() == 80);
    assert(edge.node2() == 81);

    edge = graphCut->EdgeFromNodeToNode(edges, 81, 80, dimensions, connectivity);
    assert(edge.node1() == 80);
    assert(edge.node2() == 81);

    edge = graphCut->EdgeFromNodeToNode(edges, 80, SINK, dimensions, connectivity);
    assert(edge.node1() == 80);
    assert(edge.node2() == SINK);

    edge = graphCut->EdgeFromNodeToNode(edges, SINK, 80, dimensions, connectivity);
    assert(edge.node1() == 80);
    assert(edge.node2() == SINK);

    edge = graphCut->EdgeFromNodeToNode(edges, 80, 79, dimensions, connectivity);
    assert(edge.node1() == 79);
    assert(edge.node2() == 80);

    edge = graphCut->EdgeFromNodeToNode(edges, 79, 80, dimensions, connectivity);
    assert(edge.node1() == 79);
    assert(edge.node2() == 80);
}

/**
 * Tests setting the seedpoints properties of the vtkGraphCut.
 */
void testSettingSeedPoints() {
    std::cout << __FUNCTION__ << "\n";
    int dimensions[3] = {30, 30, 30};
    vtkGraphCutProtected* graphCut = vtkGraphCutProtected::New();
    vtkNodes* nodes = new vtkNodes();
    nodes->SetDimensions(dimensions);

    graphCut->CreateEdgesForNodes(nodes, dimensions, SIX);
    vtkPoints* foregroundPoints = vtkPoints::New();
    vtkPoints* backgroundPoints = vtkPoints::New();

    graphCut->SetSeedPoints(foregroundPoints, backgroundPoints);

    vtkPoints* foregroundResult = graphCut->GetForegroundPoints();
    assert(foregroundPoints == foregroundResult);
    vtkPoints* backgroundResult = graphCut->GetBackgroundPoints();
    assert(backgroundPoints == backgroundResult);

    graphCut->Delete();
    delete nodes;
    
    std::cout << "Done!\n";
}



/**
 * Tests retrieving the incoming edge for all possible connectivity values.
 * See testIncomingEdgeWithConnectivity for details.
 */
void testIncomingEdge() {
    std::cout << __FUNCTION__ << "\n";
    int dimensions[3] = {30, 30, 30};
    vtkGraphCutProtected* graphCut = vtkGraphCutProtected::New();
    vtkNodes* nodes = new vtkNodes();
    nodes->SetDimensions(dimensions);

    std::vector<vtkEdge>* edges = graphCut->CreateEdgesForNodes(nodes, dimensions, SIX);
    testIncomingEdgeWithConnectivity(graphCut, nodes, edges, dimensions, SIX);

    edges = graphCut->CreateEdgesForNodes(nodes, dimensions, EIGHTEEN);
    testIncomingEdgeWithConnectivity(graphCut, nodes, edges, dimensions, EIGHTEEN);

    edges = graphCut->CreateEdgesForNodes(nodes, dimensions, TWENTYSIX);
    testIncomingEdgeWithConnectivity(graphCut, nodes, edges, dimensions, TWENTYSIX);

    graphCut->Delete();
    delete nodes;

    std::cout << "Done!\n";
}

/**
 * Tests retrieving the incoming edge for the specified connectivity.
 */
void testIncomingEdgeWithConnectivity(vtkGraphCutProtected* graphCut, vtkNodes* nodes, std::vector<vtkEdge>* edges, int* dimensions, vtkConnectivity connectivity) {
    // int index = graphCut->IndexForEdgeFromNodeToNode(edges, 0, 1, dimensions, connectivity);
    // vtkEdge edge = edges->at(index);
    // assert(edge.child() == -1);
    // assert(edge.parent == -1);
    // assert(edge.node1 == 0);
    // assert(edge.node2 == 1);
    // edge.parent = edge.node1;
    // assert(edge.child() == edge.node2);

    // graphCut->UpdateEdgeAtIndex(edges, index, edge);

    // vtkEdge updatedEdge = graphCut->EdgeFromNodeToNode(edges, 0, 1, dimensions, connectivity);
    // assert(updatedEdge.parent == edge.node1);
    // assert(updatedEdge.child() == edge.node2);

    // int parent = graphCut->ParentForNode(edge.node2, nodes, edges, dimensions, connectivity);
    // assert(parent == edge.node1);
}
