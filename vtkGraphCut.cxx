#include "vtkGraphCut.h"
#include <assert.h>

#pragma mark - Public

vtkStandardNewMacro(vtkGraphCut);
bool IsNodeConnected(int x, int y, int z, vtkConnectivity connectivity);
int NumberOfEdgesForConnectivity(vtkConnectivity connectivity);

void vtkGraphCut::PrintSelf(ostream& os, vtkIndent indent) {
	this->Superclass::PrintSelf(os, indent);
}

void vtkGraphCut::Reset() {}
void vtkGraphCut::Update() {
	// Verify all inputs (if not changed):

	// Verify input image data
	if (!this->inputImageData) {
		std::cout << "No image data was set. Skipping update.\n";
		return;
	}

	// Verify seed points
	// Verify cost function
	assert(this->inputImageData);

	this->dimensions = (int*)calloc(3, sizeof(int));
	DimensionsForImageData(this->inputImageData, this->dimensions);

	// Build nodes data if not exists yet
	if (!this->nodes) {
		this->nodes = CreateNodesForDimensions(this->dimensions);	
		std::cout << "Number of nodes: " << this->nodes->size() << "\n";
	}
	// Build edges data if not exists yet
	if (!this->edges) {
		this->edges = CreateEdgesForNodes(this->nodes, this->dimensions, TWENTYSIX);
		std::cout << "Number of edges: " << this->edges->size() << "\n";
	}

	// Start algorithm
	// Stage 0: Adopt phase
	// There might be orphans in the trees because the user
	// might have added/removed fore- and background points

	// Stage 1: Growth stage
	// Grow S or T (om-en-om) with breadth-first-search to find an augmenting path
	// P from node s to node t. Once an edge has been found that connects tree S with
	// tree T, then that edge is used in the following stages of the algorithm.
	// Each tree keeps track of active nodes. Only active nodes can grow.
	// The algorithm stops when there are no more active nodes in each of the trees and
	// when no non-saturated edge has been found between S and T.

	// Stage 2: Augment stage
	// When an edge has been found that connects tree S to tree T, then
	// the Augment stage can begin.
	// The augment stage will push as much flow as possible between
	// S and T through the edge found in the growth stage.
	// When a path is found, the largest possible flow is pushed through the path,
 	// thus at least one edge becomes saturated. And the child(ren) of such a saturated
 	// path will become an orphan node (not connected to S or T anymore).

	// Stage 3: Adopt stage
	// During the augment stage, orphans might have been created.
	// After the augmentation phase, the trees S and T should become single-tree
	// structures again with only valid (unsaturated) edges.
	// For each orphan node, a new valid parent node is searched for. The new
	// parent should be connected through a non-saturated edge.
	// If no qualifying parent is found, then the orphan node is transformed in a
	// free node. All it's former children will be transformed into orphans,
	// for which the same process is started.
}

vtkImageData* vtkGraphCut::GetOutput() {return NULL;}
void vtkGraphCut::SetInput(vtkImageData* imageData) {
	this->inputImageData = imageData;
}
void vtkGraphCut::SetSeedPoints(vtkPoints* foreground, vtkPoints* background) {}
void vtkGraphCut::SetCostFunction(vtkGraphCutCostFunction* costFunction) {}

#pragma mark - Protected

vtkGraphCut::vtkGraphCut() { }
vtkGraphCut::~vtkGraphCut() { }

std::vector<vtkNode>* vtkGraphCut::CreateNodesForDimensions(int* dimensions) {
	std::vector<vtkNode>* result = new std::vector<vtkNode>();

	// Allocate memory for the vertices so that it doesn't have to resize during this method
	int numberOfVertices = dimensions[0] * dimensions[1] * dimensions[2];
	result->reserve(numberOfVertices);

	for (int i = 0; i < numberOfVertices; i++) {
		vtkNode node;
		node.active = false;
		node.orphan = false;
		node.terminal = false;
		node.seedPoint = false;
		node.depthInTree = -1;
		node.tree = NONE;
		result->push_back(node);
	}
	return result;
}

std::vector<vtkEdge>* vtkGraphCut::CreateEdgesForNodes(std::vector<vtkNode>* nodes, int* dimensions, vtkConnectivity connectivity) {
	std::vector<vtkEdge>* result = new std::vector<vtkEdge>();

	int numberOfNodes = nodes->size();
	int numberOfEdges = numberOfNodes * 2 + numberOfNodes * NumberOfEdgesForConnectivity(connectivity);
	assert(numberOfEdges >= 0);

	result->reserve(numberOfEdges);

	for (int i = 0; i < nodes->size(); ++i) {
		vtkEdge sourceEdge;
		sourceEdge.node1 = SOURCE;
		sourceEdge.node2 = i;
		sourceEdge.flow = 0;
		sourceEdge.capacity = 0;
		result->push_back(sourceEdge);

		vtkEdge sinkEdge;
		sinkEdge.node1 = i;
		sinkEdge.node2 = SINK;
		sinkEdge.flow = 0;
		sinkEdge.capacity = 0;
		result->push_back(sinkEdge);

		int coordinate[3] = {0, 0, 0};
		CoordinateForIndex(i, dimensions, coordinate);
		int coord[3] = {0, 0, 0};
		int index = 0;
		for (int z = 0; z < 2; ++z) {
			for (int y = 0; y < 2; ++y) {
				for (int x = 0; x < 2; ++x) {
					coord[0] = coordinate[0]+x;
					coord[1] = coordinate[1]+y;
					coord[2] = coordinate[2]+z;
					if (IsNodeConnected(x, y, z, connectivity)) {
						vtkEdge nodeEdge;
						nodeEdge.node1 = i;
						nodeEdge.node2 = IsValidCoordinate(coord, dimensions) ? IndexForCoordinate(coord, dimensions) : INVALID;
						nodeEdge.flow = 0;
						nodeEdge.capacity = 0;
						result->push_back(nodeEdge);
					}
					++index;
				}
			}
		}
	}

	return result;
}

void vtkGraphCut::DimensionsForImageData(vtkImageData* imageData, int* dimensions) {
	int numberOfComponents = imageData->GetNumberOfScalarComponents();
	imageData->GetDimensions(dimensions);
}

std::vector<int>* vtkGraphCut::IndicesForNeighbours(int index, int* dimensions, vtkConnectivity connectivity) {
	std::vector<int>* result = new std::vector<int>();

	int coordinate[3];
	CoordinateForIndex(index, dimensions, coordinate);

	int coord[3] = {0, 0, 0};
	for (int z = -1; z < 2; ++z) {
		for (int y = -1; y < 2; ++y) {
			for (int x = -1; x < 2; ++x) {
				coord[0] = coordinate[0]+x;
				coord[1] = coordinate[1]+y;
				coord[2] = coordinate[2]+z;
				if (IsNodeConnected(x, y, z, connectivity) 
					&& IsValidCoordinate(coord, dimensions)) {
					result->push_back(IndexForCoordinate(coord, dimensions));
				}
			}
		}
	}

	return result;
}

bool vtkGraphCut::IsValidCoordinate(int* coordinate, int* dimensions) {
	for (int i = 0; i < 3; ++i) {
		if (coordinate[i] >= dimensions[i] || coordinate[i] < 0) {
			return false;
		}
	}
	return true;
}

int vtkGraphCut::IndexForCoordinate(int* coordinate, int* dimensions) {
	assert(coordinate[0] < dimensions[0]);
	assert(coordinate[1] < dimensions[1]);
	assert(coordinate[2] < dimensions[2]);
	assert(coordinate[0] >= 0);
	assert(coordinate[1] >= 0);
	assert(coordinate[2] >= 0);

	return coordinate[0]
		+ coordinate[1] * dimensions[0]
		+ coordinate[2] * dimensions[0] * dimensions[1];
}

bool vtkGraphCut::CoordinateForIndex(int index, int* dimensions, int* coordinate) {
	if (index >= dimensions[0] * dimensions[1] * dimensions[2] || index < 0) {
		return false;
	}

	int dims = dimensions[0] * dimensions[1];
	int rest = index;
	coordinate[2] = rest / dims;
	rest -= coordinate[2] * dims;
	dims = dimensions[0];
	coordinate[1] = rest / dims;
	rest -= coordinate[1] * dims;
	coordinate[0] = rest;
	
	return true;
}

vtkEdge vtkGraphCut::EdgeFromNodeToNode(std::vector<vtkEdge>* edges, int sourceIndex, int targetIndex, int* dimensions, vtkConnectivity connectivity) {
	assert(sourceIndex != INVALID);
	assert(targetIndex != INVALID);

	int from;
	int to;

	if (targetIndex < 0 || sourceIndex < 0) {
		from = std::max(targetIndex, sourceIndex);
		to = std::min(sourceIndex, targetIndex);
		assert(from >= 0);
	} else if (targetIndex < sourceIndex) {
		from = targetIndex;
		to = sourceIndex;
	} else {
		from = sourceIndex;
		to = targetIndex;
	}

	assert(from >= 0);
	assert(from < dimensions[0] * dimensions[1] * dimensions[2]);

	int index = from * (2 + NumberOfEdgesForConnectivity(connectivity));
	assert(index >= from);

	vtkEdge edge = edges->at(index);
	if (to == SOURCE) {
		assert(edge.node1 == SOURCE && edge.node2 == from);
		return edge;
	} else {
		while (edge.node1 <= from && edge.node2 != to) {
			++index;
			edge = edges->at(index);
		}
	}

	assert(edge.node1 == from && edge.node2 == to);
	return edge;
}

// Private methods

bool IsNodeConnected(int x, int y, int z, vtkConnectivity connectivity) {
	switch (connectivity) {
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

int NumberOfEdgesForConnectivity(vtkConnectivity connectivity) {
	switch (connectivity) {
		case SIX:
			return 3;
		case EIGHTEEN:
			return 6;
		case TWENTYSIX:
			return 7;
	}
	return -1;
}
