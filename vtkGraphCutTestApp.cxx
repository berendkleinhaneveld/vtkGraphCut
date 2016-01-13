#include "iostream"
#include "vtkGraphCut.h"
#include <assert.h>

void testCreateNodes();
void testCreateEdges();
void testDimensionsForData();
void testIndicesForNeighbours();
void testIsValidCoordinate();
void testIndexForCoordinate();
void testCoordinateForIndex();


vtkImageData* CreateTestImageData();


int main(int argc, char const *argv[]) {
	testCreateNodes();
	testCreateEdges();
	testDimensionsForData();
	testIndicesForNeighbours();
	testIsValidCoordinate();
	testIndexForCoordinate();
	testCoordinateForIndex();
	
	return 0;
}

vtkImageData* CreateTestImageData() {
	vtkImageData* imageData = vtkImageData::New();
	imageData->SetDimensions(2, 3, 1);
	imageData->AllocateScalars(VTK_DOUBLE, 1);
	return imageData;
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

void testCreateEdges() {
	std::cout << __FUNCTION__ << "\n";
	int dimensions[3] = {1, 1, 1};

	vtkGraphCut* graphCut = vtkGraphCut::New();
	std::vector<vtkNode>* nodes = graphCut->CreateNodesForDimensions(dimensions);
	std::vector<vtkEdge>* edges = graphCut->CreateEdgesForNodes(nodes, dimensions, TWENTYSIX);

	assert(edges->size() == 2);

	dimensions[0] = 2;
	dimensions[1] = 1;
	dimensions[2] = 1;

	nodes = graphCut->CreateNodesForDimensions(dimensions);
	edges = graphCut->CreateEdgesForNodes(nodes, dimensions, TWENTYSIX);
	assert(edges->size() == 5);

	edges = graphCut->CreateEdgesForNodes(nodes, dimensions, EIGHTEEN);
	assert(edges->size() == 5);

	edges = graphCut->CreateEdgesForNodes(nodes, dimensions, SIX);
	assert(edges->size() == 5);

	dimensions[0] = 2;
	dimensions[1] = 2;
	dimensions[2] = 1;

	nodes = graphCut->CreateNodesForDimensions(dimensions);
	edges = graphCut->CreateEdgesForNodes(nodes, dimensions, TWENTYSIX);
	assert(edges->size() == 14);

	edges = graphCut->CreateEdgesForNodes(nodes, dimensions, EIGHTEEN);
	assert(edges->size() == 14);

	edges = graphCut->CreateEdgesForNodes(nodes, dimensions, SIX);
	assert(edges->size() == 12);

	dimensions[0] = 2;
	dimensions[1] = 3;
	dimensions[2] = 1;

	nodes = graphCut->CreateNodesForDimensions(dimensions);
	edges = graphCut->CreateEdgesForNodes(nodes, dimensions, TWENTYSIX);

	assert(edges->size() == 23);

	std::cout << "Done!" << "\n";
}

void testDimensionsForData() {
	std::cout << __FUNCTION__ << "\n";
	vtkGraphCut* graphCut = vtkGraphCut::New();

	vtkImageData* imageData = CreateTestImageData();

	int dimensions[3] = {0, 0, 0};
	graphCut->DimensionsForImageData(imageData, dimensions);
	assert(dimensions[0] == 2 && dimensions[1] == 3 && dimensions[2] == 1);

	imageData->SetDimensions(5, 6, 7);
	imageData->AllocateScalars(VTK_DOUBLE, 1);
	
	graphCut->DimensionsForImageData(imageData, dimensions);
	assert(dimensions[0] == 5 && dimensions[1] == 6 && dimensions[2] == 7);

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

