//
//  vtkGraphCutTestApp.cxx
//  vtkGraphCut
//
//  Created by Berend Klein Haneveld.
//
//

#include <assert.h>
#include <iostream>
#include "vtkGraphCut.h"
#include "vtkGraphCutCostFunctionSimple.h"
#include "vtkEdge.h"
#include "vtkGraphCutDataTypes.h"
#include "vtkGraphCutProtected.h"

void testEdgeStruct();
void testCreateNodes();
void testCreateEdges();
void testIsValidCoordinate();
void testIndexForCoordinate();
void testCoordinateForIndex();
void testIndicesForNeighbours();
void testEdgeFromNodeToNode();
void testEdgeFromNodeToNodeWithConnectivity(vtkGraphCutProtected*, std::vector<vtkEdge>*, int*, vtkConnectivity);
void testSettingSeedPoints();
void testGraphCutReset();
void testIncomingEdge();
void testIncomingEdgeWithConnectivity(vtkGraphCutProtected*, std::vector<vtkNode>*, std::vector<vtkEdge>*, int*, vtkConnectivity);
void testCostFunctionSimple();
void testBasicRunThrough();

vtkImageData* createTestImageData();


int main(int argc, char const *argv[]) {
    testEdgeStruct();
    testCreateNodes();
    testCreateEdges();
    testIsValidCoordinate();
    testIndexForCoordinate();
	testCoordinateForIndex();
	testIndicesForNeighbours();
	testEdgeFromNodeToNode();
    testSettingSeedPoints();
	testGraphCutReset();
//	testIncomingEdge();
//	testCostFunctionSimple();
//	testBasicRunThrough();
	
	return 0;
}

/**
 * Convenience method for creating a simple dataset.
 */
vtkImageData* createTestImageData() {
	vtkImageData* imageData = vtkImageData::New();
    int dimensions[3] = {2, 3, 1};
	imageData->SetDimensions(dimensions[0], dimensions[1], dimensions[2]);
	imageData->AllocateScalars(VTK_DOUBLE, 1);
    for (int z = 0; z < dimensions[2]; z++) {
        for (int y = 0; y < dimensions[1]; y++) {
            for (int x = 0; x < dimensions[0]; x++) {
                imageData->SetScalarComponentFromDouble(x, y, z, 0, rand() % 100);
            }
        }
    }
	return imageData;
}

/**
 * Tests the methods of an vtkEdge struct.
 * - isTerminal
 * - flowFromNode
 * - capacityFromNode
 * - setCapacity
 * - isSaturatedFromNode
 * - addFlowFromNode
 */
void testEdgeStruct() {
    std::cout << __FUNCTION__ << "\n";
    vtkEdge edge = vtkEdge(SINK, 1);
    assert(edge.isTerminal());
    assert(edge.flowFromNode(0) == 0);
    assert(edge.capacityFromNode(0) == 0);
    
    edge = vtkEdge(0, 1);
    assert(!edge.isTerminal());
    
    edge.setCapacity(5);
    assert(edge.flowFromNode(0) == 0);
    assert(!edge.isSaturatedFromNode(0));
    assert(!edge.isSaturatedFromNode(0));
    assert(edge.capacityFromNode(0) == 5);
    assert(edge.capacityFromNode(1) == 5);
    
    edge.addFlowFromNode(0, 5);
    assert(edge.flowFromNode(0) == 5);
    assert(edge.flowFromNode(1) == -5);
    assert(edge.isSaturatedFromNode(0));
    assert(!edge.isSaturatedFromNode(1));
    assert(edge.capacityFromNode(0) == 0);
    assert(edge.capacityFromNode(1) == 10);
    
    edge.addFlowFromNode(1, 2);
    assert(edge.flowFromNode(1) == -3);
    assert(edge.flowFromNode(0) == 3);
    assert(!edge.isSaturatedFromNode(0));
    assert(!edge.isSaturatedFromNode(1));
    assert(edge.capacityFromNode(0) == 2);
    assert(edge.capacityFromNode(1) == 8);
    
    edge.addFlowFromNode(1, 8);
    assert(!edge.isSaturatedFromNode(0));
    assert(edge.isSaturatedFromNode(1));
    assert(edge.capacityFromNode(0) == 10);
    assert(edge.capacityFromNode(1) == 0);

    std::cout << "Done!" << "\n";
}

/**
 * Tests the creation of nodes for different dimensions.
 * - CreateNodesForDimensions
 */
void testCreateNodes() {
	std::cout << __FUNCTION__ << "\n";
	int dimensions[3] = {3, 3, 2};

	vtkGraphCutProtected* graphCut = vtkGraphCutProtected::New();
	std::vector<vtkNode>* nodes = graphCut->CreateNodesForDimensions(dimensions);

	assert(nodes->size() == 18);

	dimensions[0] = 5;
	dimensions[1] = 2;
	dimensions[2] = 7;
	std::vector<vtkNode>* moreNodes = graphCut->CreateNodesForDimensions(dimensions);
	assert(moreNodes->size() == 70);

    graphCut->Delete();
	std::cout << "Done!" << "\n";
}

/**
 * Tests the creation of edges for different dimensions and connectivity values.
 */
void testCreateEdges() {
	std::cout << __FUNCTION__ << "\n";
	int dimensions[3] = {1, 1, 1};

	vtkGraphCutProtected* graphCut = vtkGraphCutProtected::New();
	std::vector<vtkNode>* nodes = graphCut->CreateNodesForDimensions(dimensions);
	std::vector<vtkEdge>* edges = graphCut->CreateEdgesForNodes(nodes, dimensions, TWENTYSIX);

	int numberOfNodes = dimensions[0] * dimensions[1] * dimensions[2];
	int numberOfEdges = numberOfNodes * 2 + numberOfNodes * (7);
	assert(edges->size() == numberOfEdges);

	dimensions[0] = 2;
	dimensions[1] = 1;
	dimensions[2] = 1;

	nodes = graphCut->CreateNodesForDimensions(dimensions);
	edges = graphCut->CreateEdgesForNodes(nodes, dimensions, TWENTYSIX);
	numberOfNodes = dimensions[0] * dimensions[1] * dimensions[2];
	numberOfEdges = numberOfNodes * 2 + numberOfNodes * (7);
	assert(edges->size() == numberOfEdges);

	edges = graphCut->CreateEdgesForNodes(nodes, dimensions, EIGHTEEN);
	numberOfEdges = numberOfNodes * 2 + numberOfNodes * (6);
	assert(edges->size() == numberOfEdges);

	edges = graphCut->CreateEdgesForNodes(nodes, dimensions, SIX);
	numberOfEdges = numberOfNodes * 2 + numberOfNodes * (3);
	assert(edges->size() == numberOfEdges);

	dimensions[0] = 2;
	dimensions[1] = 2;
	dimensions[2] = 1;

	nodes = graphCut->CreateNodesForDimensions(dimensions);
	edges = graphCut->CreateEdgesForNodes(nodes, dimensions, TWENTYSIX);
	numberOfNodes = dimensions[0] * dimensions[1] * dimensions[2];
	numberOfEdges = numberOfNodes * 2 + numberOfNodes * (7);
	assert(edges->size() == numberOfEdges);

	edges = graphCut->CreateEdgesForNodes(nodes, dimensions, EIGHTEEN);
	numberOfEdges = numberOfNodes * 2 + numberOfNodes * (6);
	assert(edges->size() == numberOfEdges);

	edges = graphCut->CreateEdgesForNodes(nodes, dimensions, SIX);
	numberOfEdges = numberOfNodes * 2 + numberOfNodes * (3);
	assert(edges->size() == numberOfEdges);

	dimensions[0] = 2;
	dimensions[1] = 3;
	dimensions[2] = 1;

	nodes = graphCut->CreateNodesForDimensions(dimensions);
	edges = graphCut->CreateEdgesForNodes(nodes, dimensions, TWENTYSIX);
	numberOfNodes = dimensions[0] * dimensions[1] * dimensions[2];
	numberOfEdges = numberOfNodes * 2 + numberOfNodes * (7);
	assert(edges->size() == numberOfEdges);

	dimensions[0] = 20;
	dimensions[1] = 33;
	dimensions[2] = 12;

	nodes = graphCut->CreateNodesForDimensions(dimensions);
	edges = graphCut->CreateEdgesForNodes(nodes, dimensions, TWENTYSIX);
	numberOfNodes = dimensions[0] * dimensions[1] * dimensions[2];
	numberOfEdges = numberOfNodes * 2 + numberOfNodes * (7);
	assert(edges->size() == numberOfEdges);

    graphCut->Delete();
	std::cout << "Done!" << "\n";
}

/**
 * Tests the validaty of given coordinate, given a certain dimension.
 * - IsValidCoordinate
 */
void testIsValidCoordinate() {
    std::cout << __FUNCTION__ << "\n";
    vtkGraphCutProtected* graphCut = vtkGraphCutProtected::New();

    int dimensions[3] = {4, 6, 7};
    int coordinate[3] = {4, 6, 7};
    assert(!graphCut->IsValidCoordinate(coordinate, dimensions));

    coordinate[0] -= 1;
    coordinate[1] -= 1;
    coordinate[2] -= 1;
    assert(graphCut->IsValidCoordinate(coordinate, dimensions));

    coordinate[0] = 0;
    coordinate[1] = 0;
    coordinate[2] = 0;
    assert(graphCut->IsValidCoordinate(coordinate, dimensions));

    coordinate[0] = -1;
    coordinate[1] = 0;
    coordinate[2] = 0;
    assert(!graphCut->IsValidCoordinate(coordinate, dimensions));

    coordinate[0] = 8;
    coordinate[1] = 3;
    coordinate[2] = 3;
    assert(!graphCut->IsValidCoordinate(coordinate, dimensions));

    graphCut->Delete();
    std::cout << "Done!" << "\n";
}

/**
 * Tests getting the node index for a given coordinate.
 * - IndexForCoordinate
 */
void testIndexForCoordinate() {
	std::cout << __FUNCTION__ << "\n";
	vtkGraphCutProtected* graphCut = vtkGraphCutProtected::New();
	
	int dimensions[3] = {2, 3, 4};
	int coordinate[3] = {0, 0, 0};

	int index = graphCut->IndexForCoordinate(coordinate, dimensions);
	assert(index == 0);
	
	coordinate[0] = 1;
	coordinate[1] = 0;
	coordinate[2] = 0;
	index = graphCut->IndexForCoordinate(coordinate, dimensions);
	assert(index == 1);
	
	coordinate[0] = 1;
	coordinate[1] = 1;
	coordinate[2] = 0;
	index = graphCut->IndexForCoordinate(coordinate, dimensions);
	assert(index == 3);
	
	coordinate[0] = 0;
	coordinate[1] = 0;
	coordinate[2] = 1;
	index = graphCut->IndexForCoordinate(coordinate, dimensions);
	assert(index == 6);
	
	coordinate[0] = 1;
	coordinate[1] = 0;
	coordinate[2] = 1;
	index = graphCut->IndexForCoordinate(coordinate, dimensions);
	assert(index == 7);

    graphCut->Delete();
	std::cout << "Done!" << "\n";
}

/**
 * Tests getting the coordinate for a given node index.
 * - CoordinateForIndex
 */
void testCoordinateForIndex() {
	std::cout << __FUNCTION__ << "\n";
	vtkGraphCutProtected* graphCut = vtkGraphCutProtected::New();

	int dimensions[3] = {2, 3, 4};
	int coordinate[3] = {0, 0, 0};

	graphCut->CoordinateForIndex(0, dimensions, coordinate);
	assert(coordinate[0] == 0 && coordinate[1] == 0 && coordinate[2] == 0);

	graphCut->CoordinateForIndex(1, dimensions, coordinate);
	assert(coordinate[0] == 1 && coordinate[1] == 0 && coordinate[2] == 0);

	graphCut->CoordinateForIndex(2, dimensions, coordinate);
	assert(coordinate[0] == 0 && coordinate[1] == 1 && coordinate[2] == 0);
	
	graphCut->CoordinateForIndex(6, dimensions, coordinate);
	assert(coordinate[0] == 0 && coordinate[1] == 0 && coordinate[2] == 1);
	
	graphCut->CoordinateForIndex(8, dimensions, coordinate);
	assert(coordinate[0] == 0 && coordinate[1] == 1 && coordinate[2] == 1);
	
	graphCut->CoordinateForIndex(23, dimensions, coordinate);
	assert(coordinate[0] == 1 && coordinate[1] == 2 && coordinate[2] == 3);
	
	assert(!graphCut->CoordinateForIndex(24, dimensions, coordinate));
	assert(!graphCut->CoordinateForIndex(-1, dimensions, coordinate));
	assert(graphCut->CoordinateForIndex(23, dimensions, coordinate));
	assert(graphCut->CoordinateForIndex(0, dimensions, coordinate));
	assert(graphCut->CoordinateForIndex(1, dimensions, coordinate));

    graphCut->Delete();
	std::cout << "Done!" << "\n";
}

/**
 * Tests the function to get all the neighbour indices for a certain
 * node index and a given connectivity.
 * - IndicesForNeighbours
 */
void testIndicesForNeighbours() {
    std::cout << __FUNCTION__ << "\n";
    vtkGraphCutProtected* graphCut = vtkGraphCutProtected::New();

    int coordinate[3] = {1, 1, 1};
    int dimensions[3] = {3, 4, 5};
    int index = graphCut->IndexForCoordinate(coordinate, dimensions);
    std::vector<int>* indices = graphCut->IndicesForNeighbours(index, dimensions, SIX);
    assert(indices->size() == 6);

    indices = graphCut->IndicesForNeighbours(index, dimensions, EIGHTEEN);
    assert(indices->size() == 18);

    indices = graphCut->IndicesForNeighbours(index, dimensions, TWENTYSIX);
    assert(indices->size() == 26);

    graphCut->Delete();
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
	std::vector<vtkNode>* nodes = graphCut->CreateNodesForDimensions(dimensions);

	std::vector<vtkEdge>* edges = graphCut->CreateEdgesForNodes(nodes, dimensions, SIX);
	testEdgeFromNodeToNodeWithConnectivity(graphCut, edges, dimensions, SIX);
	
	edges = graphCut->CreateEdgesForNodes(nodes, dimensions, EIGHTEEN);
	testEdgeFromNodeToNodeWithConnectivity(graphCut, edges, dimensions, EIGHTEEN);

	edges = graphCut->CreateEdgesForNodes(nodes, dimensions, TWENTYSIX);
	testEdgeFromNodeToNodeWithConnectivity(graphCut, edges, dimensions, TWENTYSIX);

    graphCut->Delete();
	std::cout << "Done!" << "\n";
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
    std::vector<vtkNode>* nodes = graphCut->CreateNodesForDimensions(dimensions);
    graphCut->CreateEdgesForNodes(nodes, dimensions, SIX);
    vtkPoints* foregroundPoints = vtkPoints::New();
    vtkPoints* backgroundPoints = vtkPoints::New();

    graphCut->SetSeedPoints(foregroundPoints, backgroundPoints);

    vtkPoints* foregroundResult = graphCut->GetForegroundPoints();
    assert(foregroundPoints == foregroundResult);
    vtkPoints* backgroundResult = graphCut->GetBackgroundPoints();
    assert(backgroundPoints == backgroundResult);

    graphCut->Delete();
    std::cout << "Done!" << "\n";
}

/**
 * Tests the default state of a new vtkGraphCut object and tests whether
 * the Reset function resets all the cached data.
 * - Reset
 * - SetConnectivity
 * - SetSeedPoints
 * - SetCostFunction
 * - SetInput
 * - GetForegroundPoints
 * - GetBackgroundPoints
 * - GetInput
 * - GetCostFunction
 * - GetConnectivity
 * - GetOutput
 */
void testGraphCutReset() {
    std::cout << __FUNCTION__ << "\n";

    vtkGraphCut* graphCut = vtkGraphCut::New();

    assert(graphCut->GetForegroundPoints() == NULL);
    assert(graphCut->GetBackgroundPoints() == NULL);
    assert(graphCut->GetInput() == NULL);
    assert(graphCut->GetCostFunction() == NULL);
    assert(graphCut->GetConnectivity() == UNCONNECTED);
    assert(graphCut->GetOutput() == NULL);

    vtkPoints* foregroundPoints = vtkPoints::New();
    foregroundPoints->SetNumberOfPoints(2);
    vtkPoints* backgroundPoints = vtkPoints::New();
    backgroundPoints->SetNumberOfPoints(2);

    assert(graphCut->GetConnectivity() == UNCONNECTED);

    graphCut->SetConnectivity(TWENTYSIX);
    graphCut->SetSeedPoints(foregroundPoints, backgroundPoints);

    vtkGraphCutCostFunction* costFunction = vtkGraphCutCostFunctionSimple::New();
    graphCut->SetCostFunction(costFunction);

    vtkImageData* imageData = vtkImageData::New();
    imageData->SetDimensions(5, 6, 7);
    imageData->AllocateScalars(VTK_DOUBLE, 1);
    graphCut->SetInput(imageData);

    assert(graphCut->GetForegroundPoints() == foregroundPoints);
    assert(graphCut->GetBackgroundPoints() == backgroundPoints);
    assert(graphCut->GetInput() == imageData);
    assert(graphCut->GetCostFunction() == costFunction);
    assert(graphCut->GetConnectivity() == TWENTYSIX);

    graphCut->Reset();

    assert(graphCut->GetForegroundPoints() == NULL);
    assert(graphCut->GetBackgroundPoints() == NULL);
    assert(graphCut->GetInput() == NULL);
    assert(graphCut->GetCostFunction() == NULL);
    assert(graphCut->GetConnectivity() == UNCONNECTED);
    assert(graphCut->GetOutput() == NULL);
    
    graphCut->Delete();
    std::cout << "Done!" << "\n";
}

/**
 * Tests retrieving the incoming edge for all possible connectivity values.
 * See testIncomingEdgeWithConnectivity for details.
 */
void testIncomingEdge() {
	std::cout << __FUNCTION__ << "\n";
	int dimensions[3] = {30, 30, 30};
	vtkGraphCutProtected* graphCut = vtkGraphCutProtected::New();
	std::vector<vtkNode>* nodes = graphCut->CreateNodesForDimensions(dimensions);

	std::vector<vtkEdge>* edges = graphCut->CreateEdgesForNodes(nodes, dimensions, SIX);
	testIncomingEdgeWithConnectivity(graphCut, nodes, edges, dimensions, SIX);

	edges = graphCut->CreateEdgesForNodes(nodes, dimensions, EIGHTEEN);
	testIncomingEdgeWithConnectivity(graphCut, nodes, edges, dimensions, EIGHTEEN);

	edges = graphCut->CreateEdgesForNodes(nodes, dimensions, TWENTYSIX);
	testIncomingEdgeWithConnectivity(graphCut, nodes, edges, dimensions, TWENTYSIX);

    graphCut->Delete();
	std::cout << "Done!" << "\n";
}

/**
 * Tests retrieving the incoming edge for the specified connectivity.
 */
void testIncomingEdgeWithConnectivity(vtkGraphCutProtected* graphCut, std::vector<vtkNode>* nodes, std::vector<vtkEdge>* edges, int* dimensions, vtkConnectivity connectivity) {
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

void testCostFunctionSimple() {
    std::cout << __FUNCTION__ << "\n";
    vtkGraphCut* graphCut = vtkGraphCut::New();
    vtkPoints* foregroundPoints = vtkPoints::New();
    foregroundPoints->SetNumberOfPoints(2);
    foregroundPoints->SetPoint(1, 0, 0, 1);

    vtkPoints* backgroundPoints = vtkPoints::New();
    backgroundPoints->SetNumberOfPoints(2);
    backgroundPoints->SetPoint(0, 2, 3, 4);
    backgroundPoints->SetPoint(1, 2, 3, 5);

    graphCut->SetSeedPoints(foregroundPoints, backgroundPoints);

    vtkGraphCutCostFunction* costFunction = vtkGraphCutCostFunctionSimple::New();
    graphCut->SetCostFunction(costFunction);

    assert(graphCut->GetCostFunction() == costFunction);

    vtkImageData* imageData = vtkImageData::New();
    imageData->SetDimensions(5, 6, 7);
    imageData->AllocateScalars(VTK_DOUBLE, 1);
    for (int z = 0; z < 7; z++) {
        for (int y = 0; y < 6; y++) {
            for (int x = 0; x < 5; x++) {
                imageData->SetScalarComponentFromDouble(x, y, z, 0, rand() % 100);
            }
        }
    }
    graphCut->SetInput(imageData);
    graphCut->SetConnectivity(SIX);

    graphCut->Update();
    
    graphCut->Delete();
    std::cout << "Done!" << "\n";
}

void testBasicRunThrough() {
	std::cout << __FUNCTION__ << "\n";

	vtkImageData* input = vtkImageData::New();
	input->SetDimensions(2, 3, 4);
	input->AllocateScalars(VTK_DOUBLE, 1);
	input->SetOrigin(0.0, 0.5, 1.0);

	for (int z = 0; z < 4; ++z)	{
		for (int y = 0; y < 3; ++y)	{
			for (int x = 0; x < 2; ++x)	{
				input->SetScalarComponentFromDouble(x, y, z, 0, (double)(rand() % 100));
			}
		}
	}

	vtkPoints* foregroundPoints = vtkPoints::New();
	foregroundPoints->SetNumberOfPoints(1);
	foregroundPoints->SetPoint(0, 0, 0, 0);
	vtkPoints* backgroundPoints = vtkPoints::New();
	backgroundPoints->SetNumberOfPoints(1);
	foregroundPoints->SetPoint(0, 1, 1, 1);

	vtkGraphCut* graphCut = vtkGraphCut::New();
	graphCut->SetSeedPoints(foregroundPoints, backgroundPoints);
	graphCut->SetInput(input);

	assert(graphCut->GetOutput() == NULL);

	graphCut->Update();

	vtkImageData* output = graphCut->GetOutput();
	assert(output != NULL);
	int* inputDimensions = input->GetDimensions();
	int* outputDimensions = output->GetDimensions();
	assert(inputDimensions[0] == outputDimensions[0]);
	assert(inputDimensions[1] == outputDimensions[1]);
	assert(inputDimensions[2] == outputDimensions[2]);

	double* inputSpacing = input->GetSpacing();
	double* outputSpacing = output->GetSpacing();
	assert(inputSpacing[0] == outputSpacing[0]);
	assert(inputSpacing[1] == outputSpacing[1]);
	assert(inputSpacing[2] == outputSpacing[2]);

	double* inputOrigin = input->GetOrigin();
	double* outputOrigin = output->GetOrigin();
	assert(inputOrigin[0] == outputOrigin[0]);
	assert(inputOrigin[1] == outputOrigin[1]);
	assert(inputOrigin[2] == outputOrigin[2]);

	float outputPoint1 = output->GetScalarComponentAsFloat(0, 0, 0, 0);
	float outputPoint2 = output->GetScalarComponentAsFloat(1, 1, 1, 0);
	assert(outputPoint2 < outputPoint1);
	assert(outputPoint2 == 0.0);
	assert(outputPoint1 == 1.0);

	graphCut->Reset();
	assert(graphCut->GetOutput() == NULL);

    graphCut->Delete();
	std::cout << "Done!" << "\n";
}

