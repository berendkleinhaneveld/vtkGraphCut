//
//  EdgesTest.cxx
//  vtkGraphCut
//
//  Created by Berend Klein Haneveld on 10/07/16.
//
//

#include <assert.h>
#include "Internal/Edges.h"
#include "Internal/Nodes.h"
#include "Internal/Edge.h"


void testEdgesConstructor();
void testEdgesProperties();
void testEdgesUpdate();
void testEdgesReset();
void testEdgesDoubleUpdate();
void testEdgesConnectivity();

void testCreateEdges();
void testIndexForEdgeFromNodeToNode();
void testEdgeFromNodeToNode();
void testEdgeFromNodeToNodeWithConnectivity(Edges*);


int main() {
    testEdgesConstructor();
    testEdgesProperties();
    testEdgesUpdate();
    testEdgesReset();
    testEdgesDoubleUpdate();
    testEdgesConnectivity();
    
    testCreateEdges();
    testIndexForEdgeFromNodeToNode();
    testEdgeFromNodeToNode();
    return 0;
}


void testEdgesConstructor() {
    Edges* edges = new Edges();
    
    assert(edges->GetNodes() == NULL);
    
    delete edges;
}


void testEdgesProperties() {
    int dimensions[3] = {3, 3, 3};
    
    Nodes* nodes = new Nodes();
    nodes->SetDimensions(dimensions);
    nodes->SetConnectivity(SIX);

    Edges* edges = new Edges();
    edges->SetNodes(nodes);
    
    assert(edges->GetNodes() == nodes);
    
    delete edges;
    delete nodes;
}


void testEdgesUpdate() {
    int dimensions[3] = {3, 3, 3};
    
    Nodes* nodes = new Nodes();    
    nodes->SetDimensions(dimensions);
    nodes->SetConnectivity(SIX);
    nodes->Update();

    Edges* edges = new Edges();
    edges->SetNodes(nodes);

    assert(edges->GetEdge((EdgeIndex)0) == NULL);

    edges->Update();

    Edge* someEdge = edges->GetEdge((EdgeIndex)0);
    assert(someEdge != NULL);
    assert(edges->GetEdge(EDGE_NONE) == NULL);
    assert(edges->GetEdge((EdgeIndex)300) == NULL);
    
    someEdge->setCapacity(1);
    
    assert(edges->GetEdge((EdgeIndex)0)->capacityFromNode((NodeIndex)0) == 1);
    
    delete edges;
    delete nodes;
}


void testEdgesReset() {
    int dimensions[3] = {3, 3, 3};
    
    Nodes* nodes = new Nodes();
    nodes->SetDimensions(dimensions);
    nodes->SetConnectivity(SIX);
    nodes->Update();
    
    Edges* edges = new Edges();
    edges->SetNodes(nodes);
    edges->Update();
    edges->Reset();
    
    assert(edges->GetNodes() == NULL);
    assert(edges->GetEdge((EdgeIndex)0) == NULL);
    
    // Make sure that calling Update on a reset edges object
    // will not fail
    edges->Update();
    
    delete edges;
    delete nodes;
}


void testEdgesDoubleUpdate() {
    int dimensions[3] = {3, 3, 3};
    
    Nodes* nodes = new Nodes();
    nodes->SetDimensions(dimensions);
    nodes->SetConnectivity(SIX);
    nodes->Update();
    
    Edges* edges = new Edges();
    edges->SetNodes(nodes);
    edges->Update();
    
    Edge* edge = edges->GetEdge((EdgeIndex)0);
    
    edges->Update();
    Edge* sameEdge = edges->GetEdge((EdgeIndex)0);
    
    // Should be the exact same object
    assert(edge == sameEdge);
    
    delete edges;
    delete nodes;
}


void testEdgesConnectivity() {
    Edges* edges = new Edges();
    
    assert(edges->NumberOfEdgesForConnectivity(UNCONNECTED) == 0);
    
    delete edges;
}

/**
 * Tests the creation of edges for different dimensions and connectivity values.
 */
void testCreateEdges() {
    int dimensions[3] = {1, 1, 1};
    
    Nodes* nodes = new Nodes();
    nodes->SetDimensions(dimensions);
    nodes->SetConnectivity(TWENTYSIX);
    nodes->Update();
    
    Edges* edges = new Edges();
    edges->SetNodes(nodes);
    std::vector<Edge*>* edgesVector = edges->CreateEdgesForNodes(nodes);
    
    assert(edgesVector->size() == 2);
    
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
    int numberOfNodes = dimensions[0] * dimensions[1] * dimensions[2];
    int numberOfEdges = numberOfNodes * 2 + numberOfNodes * (26);
    assert(edgesVector->size() == 5);
    
    nodes->Reset();
    nodes->SetDimensions(dimensions);
    nodes->SetConnectivity(EIGHTEEN);
    nodes->Update();

    assert(edgesVector->size() == 5);
    
    nodes->Reset();
    nodes->SetDimensions(dimensions);
    nodes->SetConnectivity(SIX);
    nodes->Update();
    edgesVector = edges->CreateEdgesForNodes(nodes);
    numberOfEdges = numberOfNodes * 2 + numberOfNodes * (6);
    assert(edgesVector->size() == 5);
    
    dimensions[0] = 2;
    dimensions[1] = 2;
    dimensions[2] = 1;
    
    nodes->Reset();
    nodes->SetDimensions(dimensions);
    nodes->SetConnectivity(TWENTYSIX);
    nodes->Update();
    edgesVector = edges->CreateEdgesForNodes(nodes);
    
    assert(edgesVector->size() == 14);
    
    nodes->Reset();
    nodes->SetDimensions(dimensions);
    nodes->SetConnectivity(EIGHTEEN);
    nodes->Update();
    
    assert(edgesVector->size() == 14);
    
    nodes->Reset();
    nodes->SetDimensions(dimensions);
    nodes->SetConnectivity(SIX);
    nodes->Update();
    edgesVector = edges->CreateEdgesForNodes(nodes);
    
    assert(edgesVector->size() == 12);
    
    dimensions[0] = 2;
    dimensions[1] = 3;
    dimensions[2] = 1;
    
    nodes->Reset();
    nodes->SetDimensions(dimensions);
    nodes->SetConnectivity(TWENTYSIX);
    nodes->Update();
    edgesVector = edges->CreateEdgesForNodes(nodes);
    
    assert(edgesVector->size() == 23);
    
    dimensions[0] = 20;
    dimensions[1] = 33;
    dimensions[2] = 12;
    
    nodes->Reset();
    nodes->SetDimensions(dimensions);
    nodes->SetConnectivity(TWENTYSIX);
    nodes->Update();
    edgesVector = edges->CreateEdgesForNodes(nodes);

    assert(edgesVector->size() == 107522);
    
    delete edges;
    delete nodes;
}


void testIndexForEdgeFromNodeToNode() {
    int dimensions[3] = {30, 30, 30};
    
    Nodes* nodes = new Nodes();
    nodes->SetDimensions(dimensions);
    nodes->SetConnectivity(SIX);
    nodes->Update();
    
    Edges* edges = new Edges();
    edges->SetNodes(nodes);
    edges->Update();
    
    EdgeIndex index = edges->IndexForEdgeFromNodeToNode(NODE_SOURCE, (NodeIndex)0);
    assert(index == 0);
    index = edges->IndexForEdgeFromNodeToNode((NodeIndex)0, NODE_SOURCE);
    assert(index == 0);
    index = edges->IndexForEdgeFromNodeToNode(NODE_SINK, (NodeIndex)0);
    assert(index == (EdgeIndex)1);
    index = edges->IndexForEdgeFromNodeToNode((NodeIndex)0, NODE_SINK);
    assert(index == (EdgeIndex)1);
    index = edges->IndexForEdgeFromNodeToNode((NodeIndex)0, (NodeIndex)1);
    assert(index == (EdgeIndex)2);
    index = edges->IndexForEdgeFromNodeToNode((NodeIndex)1, (NodeIndex)0);
    assert(index == (EdgeIndex)2);
    index = edges->IndexForEdgeFromNodeToNode((NodeIndex)0, (NodeIndex)2);
    assert(index == EDGE_NONE);
    index = edges->IndexForEdgeFromNodeToNode((NodeIndex)0, (NodeIndex)29);
    assert(index == EDGE_NONE);
    index = edges->IndexForEdgeFromNodeToNode((NodeIndex)0, (NodeIndex)30);
    assert(index == (EdgeIndex)3);
    index = edges->IndexForEdgeFromNodeToNode((NodeIndex)0, (NodeIndex)31);
    assert(index == EDGE_NONE);
    index = edges->IndexForEdgeFromNodeToNode((NodeIndex)0, (NodeIndex)900);
    assert(index == (EdgeIndex)4);
    
    index = edges->IndexForEdgeFromNodeToNode((NodeIndex)1, NODE_SOURCE);
    assert(index == (EdgeIndex)5);
    index = edges->IndexForEdgeFromNodeToNode(NODE_SINK, (NodeIndex)1);
    assert(index == (EdgeIndex)6);
    index = edges->IndexForEdgeFromNodeToNode((NodeIndex)1, NODE_SINK);
    assert(index == (EdgeIndex)6);
    index = edges->IndexForEdgeFromNodeToNode((NodeIndex)1, (NodeIndex)2);
    assert(index == (EdgeIndex)7);
    index = edges->IndexForEdgeFromNodeToNode((NodeIndex)2, (NodeIndex)1);
    assert(index == (EdgeIndex)7);
    index = edges->IndexForEdgeFromNodeToNode((NodeIndex)1, (NodeIndex)3);
    assert(index == EDGE_NONE);
    index = edges->IndexForEdgeFromNodeToNode((NodeIndex)1, (NodeIndex)30);
    assert(index == EDGE_NONE);
    index = edges->IndexForEdgeFromNodeToNode((NodeIndex)1, (NodeIndex)31);
    assert(index == (EdgeIndex)8);
    index = edges->IndexForEdgeFromNodeToNode((NodeIndex)1, (NodeIndex)32);
    assert(index == EDGE_NONE);
    index = edges->IndexForEdgeFromNodeToNode((NodeIndex)1, (NodeIndex)901);
    assert(index == (EdgeIndex)9);
}


void testIndexForEdgeFromNoteToNode18() {
    int dimensions[3] = {30, 30, 30};
    
    Nodes* nodes = new Nodes();
    nodes->SetDimensions(dimensions);
    nodes->SetConnectivity(EIGHTEEN);
    nodes->Update();
    
    Edges* edges = new Edges();
    edges->SetNodes(nodes);
    edges->Update();
    
    EdgeIndex index = edges->IndexForEdgeFromNodeToNode(NODE_SOURCE, (NodeIndex)0);
    assert(index == 0);
    
    delete edges;
    delete nodes;
}


/**
 * Tests getting the edge between two nodes for all connectivy values.
 * See testEdgeFromNodeToNodeWithConnectivity for more details.
 */
void testEdgeFromNodeToNode() {
    int dimensions[3] = {30, 30, 30};
    
    Nodes* nodes = new Nodes();
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
}

/**
 * Tests getting the edge between two nodes for the specified connectivity.
 */
void testEdgeFromNodeToNodeWithConnectivity(Edges* edges) {
    Edge* edge = edges->EdgeFromNodeToNode((NodeIndex)0, (NodeIndex)1);
    assert(edge->node1() == 0);
    assert(edge->node2() == 1);
    
    edge = edges->EdgeFromNodeToNode((NodeIndex)1, (NodeIndex)0);
    assert(edge->node1() == 0);
    assert(edge->node2() == 1);
    
    edge = edges->EdgeFromNodeToNode((NodeIndex)NODE_SOURCE, (NodeIndex)80);
    assert(edge->node1() == NODE_SOURCE);
    assert(edge->node2() == (NodeIndex)80);
    
    edge = edges->EdgeFromNodeToNode((NodeIndex)80, (NodeIndex)NODE_SOURCE);
    assert(edge->node1() == NODE_SOURCE);
    assert(edge->node2() == (NodeIndex)80);
    
    edge = edges->EdgeFromNodeToNode((NodeIndex)80, (NodeIndex)81);
    assert(edge->node1() == (NodeIndex)80);
    assert(edge->node2() == (NodeIndex)81);
    
    edge = edges->EdgeFromNodeToNode((NodeIndex)81, (NodeIndex)80);
    assert(edge->node1() == (NodeIndex)80);
    assert(edge->node2() == (NodeIndex)81);
    
    edge = edges->EdgeFromNodeToNode((NodeIndex)80, NODE_SINK);
    assert(edge->node1() == (NodeIndex)80);
    assert(edge->node2() == NODE_SINK);
    
    edge = edges->EdgeFromNodeToNode(NODE_SINK, (NodeIndex)80);
    assert(edge->node1() == (NodeIndex)80);
    assert(edge->node2() == NODE_SINK);
    
    edge = edges->EdgeFromNodeToNode((NodeIndex)80, (NodeIndex)79);
    assert(edge->node1() == (NodeIndex)79);
    assert(edge->node2() == (NodeIndex)80);
    
    edge = edges->EdgeFromNodeToNode((NodeIndex)79, (NodeIndex)80);
    assert(edge->node1() == (NodeIndex)79);
    assert(edge->node2() == (NodeIndex)80);
    
    edge = edges->EdgeFromNodeToNode((NodeIndex)0, (NodeIndex)100);
    assert(edge == NULL);
}
