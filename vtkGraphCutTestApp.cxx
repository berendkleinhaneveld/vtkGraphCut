#include "iostream"
#include "vtkGraphCut.h"
#include "vtkGraphCutCostFunctionSimple.h"
#include <assert.h>

void testEdgeStruct();
void testGraphCutReset();
void testCreateNodes();
void testCreateEdges();
void testIndicesForNeighbours();
void testIsValidCoordinate();
void testIndexForCoordinate();
void testCoordinateForIndex();
void testEdgeFromNodeToNode();
void testSettingSeedPoints();
void testCostFunctionSimple();
void testBasicRunThrough();
void testEdgeFromNodeToNodeWithConnectivity(vtkGraphCut*, std::vector<vtkEdge>*, int*, vtkConnectivity);

vtkImageData* createTestImageData();


int main(int argc, char const *argv[]) {
    testEdgeStruct();
	testCoordinateForIndex();
	testIndexForCoordinate();
	testCreateNodes();
	testCreateEdges();
	testIndicesForNeighbours();
	testIsValidCoordinate();
	testEdgeFromNodeToNode();
	testSettingSeedPoints();
	testCostFunctionSimple();
	testGraphCutReset();
    //	testBasicRunThrough();
	
	return 0;
}

vtkImageData* createTestImageData() {
	vtkImageData* imageData = vtkImageData::New();
	imageData->SetDimensions(2, 3, 1);
	imageData->AllocateScalars(VTK_DOUBLE, 1);
	return imageData;
}

void testEdgeStruct() {
    vtkEdge edge;
    edge.node1 = SINK;
    edge.node2 = 1;
    edge.flow = 0;
    edge.capacity = 0;
    assert(edge.isTerminal());
    
    edge.node1 = 0;
    assert(!edge.isTerminal());
    
    edge.capacity = 5;
    assert(edge.flowFromNode(0) == 0);
    assert(!edge.isSaturatedFromNode(0));
    assert(!edge.isSaturatedFromNode(0));
    assert(edge.capacityFromNode(0) == 5);
    assert(edge.capacityFromNode(1) == 5);
    
    edge.addFlowFromNode(0, 5);
    assert(edge.flow == 5);
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
}

void testCreateNodes() {
	std::cout << __FUNCTION__ << "\n";
	int dimensions[3] = {3, 3, 2};

	vtkGraphCut* graphCut = vtkGraphCut::New();
	std::vector<vtkNode>* nodes = graphCut->CreateNodesForDimensions(dimensions);

	assert(nodes->size() == 18);

	dimensions[0] = 5;
	dimensions[1] = 2;
	dimensions[2] = 7;
	std::vector<vtkNode>* moreNodes = graphCut->CreateNodesForDimensions(dimensions);
	assert(moreNodes->size() == 70);

	std::cout << "Done!" << "\n";
}

void testGraphCutReset() {
	std::cout << __FUNCTION__ << "\n";

	vtkGraphCut* graphCut = vtkGraphCut::New();
	vtkPoints* foregroundPoints = vtkPoints::New();
	foregroundPoints->SetNumberOfPoints(2);
	vtkPoints* backgroundPoints = vtkPoints::New();
	backgroundPoints->SetNumberOfPoints(2);

	graphCut->SetSeedPoints(foregroundPoints, backgroundPoints);

	vtkGraphCutCostFunction* costFunction = vtkGraphCutCostFunctionSimple::New();
	graphCut->SetCostFunction(costFunction);

	vtkImageData* imageData = vtkImageData::New();
	imageData->SetDimensions(5, 6, 7);
	imageData->AllocateScalars(VTK_DOUBLE, 1);
	graphCut->SetInput(imageData);

	assert(graphCut->GetForegroundPoints() != NULL);
	assert(graphCut->GetBackgroundPoints() != NULL);
	assert(graphCut->GetInput() != NULL);
	assert(graphCut->GetCostFunction() != NULL);

	graphCut->Update();
	graphCut->Reset();

	assert(graphCut->GetForegroundPoints() == NULL);
	assert(graphCut->GetBackgroundPoints() == NULL);
	assert(graphCut->GetInput() == NULL);
	assert(graphCut->GetCostFunction() == NULL);
	assert(graphCut->GetOutput() == NULL);

	std::cout << "Done!" << "\n";
}

void testCreateEdges() {
	std::cout << __FUNCTION__ << "\n";
	int dimensions[3] = {1, 1, 1};

	vtkGraphCut* graphCut = vtkGraphCut::New();
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

	std::cout << "Done!" << "\n";
}

void testIndicesForNeighbours() {
	std::cout << __FUNCTION__ << "\n";
	vtkGraphCut* graphCut = vtkGraphCut::New();
	
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

void testIsValidCoordinate() {
	std::cout << __FUNCTION__ << "\n";
	vtkGraphCut* graphCut = vtkGraphCut::New();

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

	std::cout << "Done!" << "\n";
}

void testIndexForCoordinate() {
	std::cout << __FUNCTION__ << "\n";
	vtkGraphCut* graphCut = vtkGraphCut::New();
	
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

	std::cout << "Done!" << "\n";
}

void testCoordinateForIndex() {
	std::cout << __FUNCTION__ << "\n";
	vtkGraphCut* graphCut = vtkGraphCut::New();

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

	std::cout << "Done!" << "\n";
}

void testEdgeFromNodeToNode() {
	std::cout << __FUNCTION__ << "\n";
	int dimensions[3] = {30, 30, 30};
	vtkGraphCut* graphCut = vtkGraphCut::New();
	std::vector<vtkNode>* nodes = graphCut->CreateNodesForDimensions(dimensions);

	std::vector<vtkEdge>* edges = graphCut->CreateEdgesForNodes(nodes, dimensions, SIX);
	testEdgeFromNodeToNodeWithConnectivity(graphCut, edges, dimensions, SIX);
	
	edges = graphCut->CreateEdgesForNodes(nodes, dimensions, EIGHTEEN);
	testEdgeFromNodeToNodeWithConnectivity(graphCut, edges, dimensions, EIGHTEEN);

	edges = graphCut->CreateEdgesForNodes(nodes, dimensions, TWENTYSIX);
	testEdgeFromNodeToNodeWithConnectivity(graphCut, edges, dimensions, TWENTYSIX);

	std::cout << "Done!" << "\n";
}

void testEdgeFromNodeToNodeWithConnectivity(vtkGraphCut* graphCut, std::vector<vtkEdge>* edges, int* dimensions, vtkConnectivity connectivity) {
	vtkEdge edge = graphCut->EdgeFromNodeToNode(edges, 0, 1, dimensions, connectivity);
	assert(edge.node1 == 0);
	assert(edge.node2 == 1);

	edge = graphCut->EdgeFromNodeToNode(edges, 1, 0, dimensions, connectivity);
	assert(edge.node1 == 0);
	assert(edge.node2 == 1);

	edge = graphCut->EdgeFromNodeToNode(edges, SOURCE, 80, dimensions, connectivity);
	assert(edge.node1 == SOURCE);
	assert(edge.node2 == 80);

	edge = graphCut->EdgeFromNodeToNode(edges, 80, SOURCE, dimensions, connectivity);
	assert(edge.node1 == SOURCE);
	assert(edge.node2 == 80);

	edge = graphCut->EdgeFromNodeToNode(edges, 80, 81, dimensions, connectivity);
	assert(edge.node1 == 80);
	assert(edge.node2 == 81);

	edge = graphCut->EdgeFromNodeToNode(edges, 81, 80, dimensions, connectivity);
	assert(edge.node1 == 80);
	assert(edge.node2 == 81);

	edge = graphCut->EdgeFromNodeToNode(edges, 80, SINK, dimensions, connectivity);
	assert(edge.node1 == 80);
	assert(edge.node2 == SINK);

	edge = graphCut->EdgeFromNodeToNode(edges, SINK, 80, dimensions, connectivity);
	assert(edge.node1 == 80);
	assert(edge.node2 == SINK);

	edge = graphCut->EdgeFromNodeToNode(edges, 80, 79, dimensions, connectivity);
	assert(edge.node1 == 79);
	assert(edge.node2 == 80);

	edge = graphCut->EdgeFromNodeToNode(edges, 79, 80, dimensions, connectivity);
	assert(edge.node1 == 79);
	assert(edge.node2 == 80);
}

void testSettingSeedPoints() {
	std::cout << __FUNCTION__ << "\n";
	int dimensions[3] = {30, 30, 30};
	vtkGraphCut* graphCut = vtkGraphCut::New();
	std::vector<vtkNode>* nodes = graphCut->CreateNodesForDimensions(dimensions);
	graphCut->CreateEdgesForNodes(nodes, dimensions, SIX);
	vtkPoints* foregroundPoints = vtkPoints::New();
	vtkPoints* backgroundPoints = vtkPoints::New();

	graphCut->SetSeedPoints(foregroundPoints, backgroundPoints);

	vtkPoints* foregroundResult = graphCut->GetForegroundPoints();
	assert(foregroundPoints == foregroundResult);
	vtkPoints* backgroundResult = graphCut->GetBackgroundPoints();
	assert(backgroundPoints == backgroundResult);

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

	std::cout << "Done!" << "\n";
}

void testCostFunctionSimple() {
	std::cout << __FUNCTION__ << "\n";
	vtkGraphCut* graphCut = vtkGraphCut::New();
	vtkPoints* foregroundPoints = vtkPoints::New();
	foregroundPoints->SetNumberOfPoints(2);
	vtkPoints* backgroundPoints = vtkPoints::New();
	backgroundPoints->SetNumberOfPoints(2);

	graphCut->SetSeedPoints(foregroundPoints, backgroundPoints);

	vtkGraphCutCostFunction* costFunction = vtkGraphCutCostFunctionSimple::New();
	graphCut->SetCostFunction(costFunction);

	assert(graphCut->GetCostFunction() == costFunction);

	vtkImageData* imageData = vtkImageData::New();
	imageData->SetDimensions(5, 6, 7);
	imageData->AllocateScalars(VTK_DOUBLE, 1);
	graphCut->SetInput(imageData);

	graphCut->Update();

	std::cout << "Done!" << "\n";
}

