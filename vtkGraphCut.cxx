#include "vtkGraphCut.h"
#include <assert.h>

struct vtkNodeStatistics
{
	double minimum;
	double maximum;
	double mean;
	double variance;
	double foregroundMean;
	double foregroundVariance;
	double backgroundMean;
	double backgroundVariance;
};

vtkStandardNewMacro(vtkGraphCut);
bool IsNodeConnected(int x, int y, int z, vtkConnectivity connectivity);
int NumberOfEdgesForConnectivity(vtkConnectivity connectivity);

bool IsTerminal(vtkEdge edge);


bool CalculateCoordinateForIndex(int index, int* dimensions, int* coordinate);
double GetIntensityForVoxel(vtkImageData* imageData, int index);
double GetIntensityForVoxel(vtkImageData* imageData, double* xyz);
double GetIntensityForVoxel(vtkImageData* imageData, int* xyz);
double GetIntensityForVoxel(vtkImageData* imageData, int x, int y, int z);
double CalculateTerminalCapacity(double intensity, double mean, double variance);
double CalculateRegionalCapacity(vtkImageData* imageData, vtkEdge edge, double variance);
double CalculateCapacity(vtkImageData* imageData, vtkEdge edge, vtkNodeStatistics statistics);


// Public

void vtkGraphCut::PrintSelf(ostream& os, vtkIndent indent) {
	this->Superclass::PrintSelf(os, indent);
}

void vtkGraphCut::Reset() {
	this->inputImageData = NULL;
	this->outputImageData = NULL;
	this->dimensions = NULL;
	this->edges = NULL;
	this->nodes = NULL;
	this->foregroundPoints = NULL;
	this->backgroundPoints = NULL;
	this->costFunction = NULL;
}

void vtkGraphCut::Update() {
	// Verify all inputs (if changed since last update):

	// Verify input image data
	if (!this->inputImageData) {
		std::cout << "No image data was set. Skipping update.\n";
		return;
	}

	// Verify seed points
	if (!this->foregroundPoints || !this->backgroundPoints) {
		std::cout << "No fore- or background points were set. Skipping update.\n";
		return;
	} else if (this->foregroundPoints->GetNumberOfPoints() == 0) {
		std::cout << "Number of foreground points is zero. Skipping update.\n";
		return;
	} else if (this->backgroundPoints->GetNumberOfPoints() == 0) {
		std::cout << "Number of foreground points is zero. Skipping update.\n";
		return;
	}

	// TODO: check that fore- and background points are located in the image data
	// TODO: Verify cost function

	this->dimensions = this->inputImageData->GetDimensions();

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

	CalculateCapacitiesForEdges();

	std::vector<int>* activeSourceNodes = new std::vector<int>();
	std::vector<int>* activeSinkNodes = new std::vector<int>();
	std::vector<vtkNode>* orphans = new std::vector<vtkNode>();

	activeSourceNodes->push_back(SOURCE);
	activeSinkNodes->push_back(SINK);

	// Start algorithm
	while (true) {
		// Stage 0: Adopt phase
		// There might be orphans in the trees because the user
		// might have added/removed fore- and background points
		if (orphans->size() > 0) {
			Adopt(orphans);
		}

		int treeSelector = 0;
		int noActiveNodesCounter = 0;
		vtkEdge edgeBetweenGraphs;
		edgeBetweenGraphs.node1 = INVALID;
		edgeBetweenGraphs.node2 = INVALID;

		// Stage 1: Growth stage
		// Grow S or T (om-en-om) with breadth-first-search to find an augmenting path
		// P from node s to node t. Once an edge has been found that connects tree S with
		// tree T, then that edge is used in the following stages of the algorithm.
		// Each tree keeps track of active nodes. Only active nodes can grow.
		// The algorithm stops when there are no more active nodes in each of the trees and
		// when no non-saturated edge has been found between S and T.

		// Needs:
		// nodes
		// edges
		// tree type to grow
		// priority queue of active nodes of tree for that type?
		// Returns: edge and bool

		while (noActiveNodesCounter < 2) {
			vtkTreeType tree = (treeSelector % 2 == 0) ? SOURCE : SINK;

			// foundActiveNodes is set when active nodes are found, but no path was found
			// If no path is found, but there was a succesful growing iteration the other tree may grow
			bool foundActiveNodes;
			edgeBetweenGraphs = Grow(tree, foundActiveNodes);
			noActiveNodesCounter = foundActiveNodes ? 0 : noActiveNodesCounter + 1;

			// If a path has been found, then we can break the loop and proceed to the next part
			if (edgeBetweenGraphs.node1 != INVALID) {
				break;
			}
			treeSelector++;
		}

		if (edgeBetweenGraphs.node1 == INVALID) {
			// Didn't find a path and there should be no more active nodes, so we can call it quits
			// assert(activeSourceNodes->size() == 0);
			// assert(activeSinkNodes->size() == 0);
			break;
		}

		// Stage 2: Augment stage
		// When an edge has been found that connects tree S to tree T, then
		// the Augment stage can begin.
		// The augment stage will push as much flow as possible between
		// S and T through the edge found in the growth stage.
		// When a path is found, the largest possible flow is pushed through the path,
	 	// thus at least one edge becomes saturated. And the child(ren) of such a saturated
	 	// path will become an orphan node (not connected to S or T anymore).

	 	// Needs:
	 	// Edge between trees
	 	// Edges
	 	// Nodes
	 	// Orphans -> Should return orphans
	 	orphans = Augment(edgeBetweenGraphs);

		// Stage 3: Adopt stage
		// During the augment stage, orphans might have been created.
		// After the augmentation phase, the trees S and T should become single-tree
		// structures again with only valid (unsaturated) edges.
		// For each orphan node, a new valid parent node is searched for. The new
		// parent should be connected through a non-saturated edge.
		// If no qualifying parent is found, then the orphan node is transformed in a
		// free node. All it's former children will be transformed into orphans,
		// for which the same process is started.

		// Needs:
		// Orphans
		// Edges
		// Nodes
		Adopt(orphans);
	}

	this->outputImageData = vtkImageData::New();
	this->outputImageData->SetDimensions(this->dimensions);
	this->outputImageData->AllocateScalars(VTK_CHAR, 1);
	this->outputImageData->SetSpacing(this->inputImageData->GetSpacing());
	this->outputImageData->SetOrigin(this->inputImageData->GetOrigin());
	// TODO: copy data from tree property of all the nodes
}

vtkImageData* vtkGraphCut::GetOutput() {
	return this->outputImageData;
}

void vtkGraphCut::SetInput(vtkImageData* imageData) {
	this->inputImageData = imageData;
}

vtkImageData* vtkGraphCut::GetInput() {
	return this->inputImageData;
}

void vtkGraphCut::SetSeedPoints(vtkPoints* foreground, vtkPoints* background) {
	this->foregroundPoints = foreground;
	this->backgroundPoints = background;
}

vtkPoints* vtkGraphCut::GetForegroundPoints() {
	return this->foregroundPoints;
}

vtkPoints* vtkGraphCut::GetBackgroundPoints() {
	return this->backgroundPoints;
}

void vtkGraphCut::SetCostFunction(vtkGraphCutCostFunction* costFunction) {
	this->costFunction = costFunction;
}

vtkGraphCutCostFunction* vtkGraphCut::GetCostFunction() {
	return this->costFunction;
}


// Protected

vtkGraphCut::vtkGraphCut() { }
vtkGraphCut::~vtkGraphCut() { }

std::vector<vtkNode>* vtkGraphCut::CreateNodesForDimensions(int* dimensions) {
	std::vector<vtkNode>* result = new std::vector<vtkNode>();

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

std::vector<vtkEdge>* vtkGraphCut::CreateEdgesForNodes(std::vector<vtkNode>* nodes, 
	int* dimensions, vtkConnectivity connectivity) {
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
	return CalculateCoordinateForIndex(index, dimensions, coordinate);
}

vtkEdge vtkGraphCut::EdgeFromNodeToNode(std::vector<vtkEdge>* edges, int sourceIndex,
	int targetIndex, int* dimensions, vtkConnectivity connectivity) {
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


// Algorithm steps

vtkEdge vtkGraphCut::Grow(vtkTreeType tree, bool& foundActiveNodes) {
	// Get an active node from the tree
	// TODO: hier was ik gebleven...
	// vtkNode node = tree == SOURCE ? activeSourceNodes->

	// For all neighbors of the active node, connected through a non-saturated edge:
	// add them to the tree if it is a free node.
	// If it is not a free node and it is from the other tree: return a path

	foundActiveNodes = false;
	vtkEdge result;
	result.node1 = INVALID;
	result.node2 = INVALID;
	return result;
}

std::vector<vtkNode>* vtkGraphCut::Augment(vtkEdge edge) {
	return NULL;
}

void vtkGraphCut::Adopt(std::vector<vtkNode>* orphans) {

}

void vtkGraphCut::CalculateCapacitiesForEdges() {
	double constantK 			= 0.0;
	double lambda 				= 500.0;

	double minimum 				= VTK_DOUBLE_MAX;
	double maximum 				= VTK_DOUBLE_MIN;
	double mean 				= 0.0;
	double variance 			= 0.0;
	double foregroundMean 		= VTK_DOUBLE_MIN;
	double foregroundVariance 	= 0.0;
	double backgroundMean 		= VTK_DOUBLE_MIN;
	double backgroundVariance 	= 0.0;

	int* dimensions = this->inputImageData->GetDimensions();
	for (int z = 0; z < dimensions[2]; z++) {
		for (int y = 0; y < dimensions[1]; y++) {
			for (int x = 0; x < dimensions[0]; x++) {
				double intensity = GetIntensityForVoxel(this->inputImageData, x, y, z);
				minimum = std::min(minimum, intensity);
				maximum = std::max(maximum, intensity);
				mean += intensity;
			}
		}
	}

	if (minimum == maximum) {
		std::cout << "Warning: all nodes have the same intensity.\n";
		return;
	}

	int numberOfVoxels = dimensions[0] + dimensions[1] * dimensions[2];
	mean = mean / (double)(numberOfVoxels);

	for (int z = 0; z < dimensions[2]; z++) {
		for (int y = 0; y < dimensions[1]; y++) {
			for (int x = 0; x < dimensions[0]; x++) {
				double intensity = GetIntensityForVoxel(this->inputImageData, x, y, z);
				variance += pow(intensity - mean, 2);
			}
		}
	}
	
	variance = sqrt(variance / (double)(numberOfVoxels - 1));

	// Reset the statistics
	foregroundMean = 0.0;
	backgroundMean = 0.0;
	foregroundVariance = 0.0;
	backgroundVariance = 0.0;
	
	// Calculate the mean color of the foreground nodes
	int numberOfForegroundPoints = this->foregroundPoints->GetNumberOfPoints();
	for (int i = 0; i < numberOfForegroundPoints; i++) {
		double* xyz = this->foregroundPoints->GetPoint(i);
		double foregroundIntensity = GetIntensityForVoxel(this->inputImageData, xyz);
		foregroundMean += foregroundIntensity / (double)numberOfForegroundPoints;
		foregroundVariance += (foregroundIntensity * foregroundIntensity) / (double)numberOfForegroundPoints;
	}
	
	// Calculate the mean color of the background nodes
	int numberOfBackgroundPoints = this->backgroundPoints->GetNumberOfPoints();
	for (int i = 0; i < numberOfBackgroundPoints; i++) {
		double* xyz = this->backgroundPoints->GetPoint(i);
		double backgroundIntensity = GetIntensityForVoxel(this->inputImageData, xyz);
		backgroundMean += backgroundIntensity / (double)numberOfBackgroundPoints;
		backgroundVariance += (backgroundIntensity * backgroundIntensity) / (double)numberOfBackgroundPoints;
	}
	
	foregroundVariance -= (foregroundMean * foregroundMean);
	foregroundVariance = sqrt(foregroundVariance);
	
	backgroundVariance -= (backgroundMean * backgroundMean);
	backgroundVariance = sqrt(backgroundVariance);
	
	if (numberOfForegroundPoints == 1) {
		// foregroundVariance = 0.0004;
		foregroundVariance = variance;
	}
	if (numberOfBackgroundPoints == 1) {
		// backgroundVariance = 0.0004;
		backgroundVariance = variance;
	}

	vtkNodeStatistics statistics;
	statistics.minimum = minimum;
	statistics.maximum = maximum;
	statistics.mean = mean;
	statistics.variance = variance;
	statistics.foregroundMean = foregroundMean;
	statistics.foregroundVariance = foregroundVariance;
	statistics.backgroundMean = backgroundMean;
	statistics.backgroundVariance = backgroundVariance;

	for (int i = 0; i < this->edges->size(); i++) {
		vtkEdge edge = this->edges->at(i);
		edge.capacity = (int)(255.0 * CalculateCapacity(this->inputImageData, edge, statistics));
	}
}

double GetIntensityForVoxel(vtkImageData* imageData, int index) {
	int coordinate[3] = {0, 0, 0};
	CalculateCoordinateForIndex(index, imageData->GetDimensions(), coordinate);
	return GetIntensityForVoxel(imageData, coordinate);
}

double GetIntensityForVoxel(vtkImageData* imageData, double* xyz) {
	return GetIntensityForVoxel(imageData, (int)xyz[0], (int)xyz[1], (int)xyz[2]);
}

double GetIntensityForVoxel(vtkImageData* imageData, int* xyz) {
	return GetIntensityForVoxel(imageData, xyz[0], xyz[1], xyz[2]);
}

double GetIntensityForVoxel(vtkImageData* imageData, int x, int y, int z) {
	int numberOfComponents = imageData->GetNumberOfScalarComponents();
	double result = 0.0;
	for (int i = 0; i < numberOfComponents; i++) {
		result += imageData->GetScalarComponentAsDouble(x, y, z, i);
	}
	result /= (double)numberOfComponents;
	return result;
}

bool IsTerminal(vtkEdge edge) {
	return edge.node1 == SOURCE || edge.node2 == SINK;
}

double CalculateCapacity(vtkImageData* imageData, vtkEdge edge, vtkNodeStatistics statistics) {
	if (IsTerminal(edge)) {
		int nodeIndex = edge.node1 == SOURCE ? edge.node2 : edge.node1;
		int coordinate[3] = {0, 0, 0};
		CalculateCoordinateForIndex(nodeIndex, imageData->GetDimensions(), coordinate);
		double intensity = GetIntensityForVoxel(imageData, coordinate);
		double mean = edge.node1 == SINK ? statistics.foregroundMean : statistics.backgroundMean;
		double variance = edge.node1 == SINK ? statistics.foregroundVariance : statistics.backgroundVariance;
		return CalculateTerminalCapacity(intensity, mean, variance);
	} else {
		return CalculateRegionalCapacity(imageData, edge, statistics.variance);
	}
}

double CalculateTerminalCapacity(double intensity, double mean, double variance) {
	// The mean and var values have already been normalized. So now the costs are
	// calculated as the absolute difference from the mean devided by the variance
	return fabs(intensity - mean) / variance;
}

double CalculateRegionalCapacity(vtkImageData* imageData, vtkEdge edge, double variance) {
	assert(!IsTerminal(edge));
	double intensity1 = GetIntensityForVoxel(imageData, edge.node1);
	double intensity2 = GetIntensityForVoxel(imageData, edge.node2);
	
	// TODO: could be expanded with distance information
	double result = exp(- pow(intensity1 - intensity2, 2) / (2 * pow(variance, 2)));
	// std::cout << intensity1 << ", " << intensity2 << ", " << variance << ", " << result << "\n";
	return result;
}

bool CalculateCoordinateForIndex(int index, int* dimensions, int* coordinate) {
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
