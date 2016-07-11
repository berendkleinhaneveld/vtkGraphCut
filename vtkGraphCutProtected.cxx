//
//  vtkGraphCutProtected.cpp
//  vtkGraphCut
//
//  Created by Berend Klein Haneveld on 04/07/16.
//
//

#include "vtkGraphCutProtected.h"
#include <vtkImageData.h>
#include <vtkPoints.h>
#include "vtkEdge.h"
#include "vtkNode.h"
#include "vtkNodes.h"
#include "vtkGraphCutHelperFunctions.h"
#include "vtkGraphCutCostFunction.h"


vtkStandardNewMacro(vtkGraphCutProtected);


void vtkGraphCutProtected::PrintSelf(ostream& os, vtkIndent indent) {
    this->Superclass::PrintSelf(os, indent);
}


void vtkGraphCutProtected::Reset() {
    if (inputImageData) {
        inputImageData->Delete();
        inputImageData = NULL;
    }
    if (outputImageData) {
        outputImageData->Delete();
        outputImageData = NULL;
    }
    if (dimensions) {
        delete dimensions;
        dimensions = NULL;
    }
    if (edges) {
        delete edges;
        edges = NULL;
    }
    if (nodes) {
        delete nodes;
        nodes = NULL;
    }
    if (foregroundPoints) {
        foregroundPoints->Delete();
        foregroundPoints = NULL;
    }
    if (backgroundPoints) {
        backgroundPoints->Delete();
        backgroundPoints = NULL;
    }
    if (costFunction) {
        costFunction->Delete();
        costFunction = NULL;
    }
    this->connectivity = UNCONNECTED;
}


vtkImageData* vtkGraphCutProtected::GetOutput() {
    return this->outputImageData;
}


void vtkGraphCutProtected::SetInput(vtkImageData* imageData) {
    this->inputImageData = imageData;
}


vtkImageData* vtkGraphCutProtected::GetInput() {
    return this->inputImageData;
}


void vtkGraphCutProtected::SetSeedPoints(vtkPoints* foreground, vtkPoints* background) {
    this->foregroundPoints = foreground;
    this->backgroundPoints = background;
}


vtkPoints* vtkGraphCutProtected::GetForegroundPoints() {
    return this->foregroundPoints;
}


vtkPoints* vtkGraphCutProtected::GetBackgroundPoints() {
    return this->backgroundPoints;
}


void vtkGraphCutProtected::SetCostFunction(vtkGraphCutCostFunction* costFunction) {
    this->costFunction = costFunction;
}


vtkGraphCutCostFunction* vtkGraphCutProtected::GetCostFunction() {
    return this->costFunction;
}


void vtkGraphCutProtected::SetConnectivity(vtkConnectivity connectivity) {
    this->connectivity = connectivity;
}


vtkConnectivity vtkGraphCutProtected::GetConnectivity() {
    return this->connectivity;
}


void vtkGraphCutProtected::Update() {
    // Verify all inputs (if changed since last update):
    
    if (this->connectivity == UNCONNECTED) {
        std::cout << "No connectivity was specified. Skipping update.\n";
        return;
    }
    
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
    if (!nodes) {
        nodes = new vtkNodes();
        nodes->SetConnectivity(connectivity);
        nodes->SetDimensions(dimensions);
        nodes->Update();
    }
    // Build edges data if not exists yet
    if (!this->edges) {
        this->edges = CreateEdgesForNodes(this->nodes, this->dimensions, this->connectivity);
    }
    
    CalculateCapacitiesForEdges();
    
    std::priority_queue<std::pair<int, int> > activeSourceNodes;
    std::priority_queue<std::pair<int, int> > activeSinkNodes;
    std::vector<int>* orphans = NULL;
    
    activeSourceNodes.push(std::make_pair(0, SOURCE));
    activeSinkNodes.push(std::make_pair(0, SINK));
    
    // Start algorithm
    while (true) {
        // Stage 0: Adopt phase
        // There might be orphans in the trees because the user
        // might have added/removed fore- and background points
        if (orphans != NULL && orphans->size() > 0) {
            Adopt(orphans);
        }
        
        int treeSelector = 0;
        int noActiveNodesCounter = 0;
        int edgeIndexBetweenGraphs = -1;
        
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
            edgeIndexBetweenGraphs = Grow(tree, foundActiveNodes, tree == SOURCE ? activeSourceNodes : activeSinkNodes);
            noActiveNodesCounter = foundActiveNodes ? 0 : noActiveNodesCounter + 1;
            
            // If a path has been found, then we can break the loop and proceed to the next part
            if (edgeIndexBetweenGraphs >= 0) {
                break;
            }
            treeSelector++;
        }
        
        if (edgeIndexBetweenGraphs < 0) {
            // Didn't find a path and there should be no more active nodes, so we can call it quits
            assert(activeSourceNodes.size() == 0);
            assert(activeSinkNodes.size() == 0);
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
        orphans = Augment(edgeIndexBetweenGraphs);
        
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


// Algorithm steps

/**
 * Returns -1 if no edge has been found.
 */
int vtkGraphCutProtected::Grow(vtkTreeType tree, bool& foundActiveNodes, std::priority_queue<std::pair<int, int> > activeNodes) {
    foundActiveNodes = false;
    
    // Get an active node from the tree
    std::pair<int, int> active = activeNodes.top();
    
    // Pop until active node is found
    while (true) {
        if (active.second >= 0) {
            vtkNode* node = this->nodes->GetNode(active.second);
            if (node->active) {
                break;
            }
            
            activeNodes.pop();
            // If there are no more active nodes, just return an invalid edge
            if (activeNodes.size() == 0) {
                return -1;
            }
            active = activeNodes.top();
        } else {
            break;
        }
    }
    
    int nodeIndex = active.second;
    if (nodeIndex >= 0) {
        std::vector<int>* neighbours = nodes->GetIndicesForNeighbours(nodeIndex);
        vtkEdge edgeBetweenTrees = vtkEdge(INVALID, INVALID);
        int edgeIndex = 0;
        for (std::vector<int>::iterator i = neighbours->begin(); i != neighbours->end(); ++i) {
            // Check to see if the edge to the node is saturated or not
            vtkEdge edge = EdgeFromNodeToNode(this->edges, nodeIndex, *i, this->dimensions, this->connectivity);
            if (!edge.isSaturatedFromNode(nodeIndex)) {
                // If the other node is free, it can be added to the tree
                vtkNode* neighbour = nodes->GetNode(*i);
                if (neighbour->tree == NONE) {
                    // Other node is added as a child to active node
                    neighbour->tree = tree;
                    neighbour->depthInTree = active.first + 1;
                    neighbour->active = true;
                    neighbour->parent = nodeIndex;
                    activeNodes.push(std::make_pair(neighbour->depthInTree, *i));
                } else if (neighbour->tree != tree) {
                    // If the other node is from the other tree, we have found a path!
                    edgeBetweenTrees = edge;
                    break;
                }
            }
            edgeIndex += 1;
        }
        
        // If no edge has been found, then the current node can become inactive
        if (!edgeBetweenTrees.isValid()) {
            vtkNode* node = this->nodes->GetNode(nodeIndex);
            node->active = false;
            edgeIndex = -1;
        }
        return edgeIndex;
    } else {
        int i = 0;
        int edgeIndex = -1;
        for (std::vector<vtkNode*>::iterator it = this->nodes->GetIterator(); it != this->nodes->GetEnd(); ++it) {
            // If the other node is free, it can be added to the tree
            vtkNode* node = *it;
            if (node->tree == NONE) {
                // Other node is added as a child to active node
                node->tree = tree;
                node->depthInTree = active.first + 1;
                node->parent = tree;
                node->active = true;
                activeNodes.push(std::make_pair(node->depthInTree, i));
            } else if (node->tree != tree) {
                // If the other node is from the other tree, we have found a path!
                edgeIndex = IndexForEdgeFromNodeToNode(this->edges, nodeIndex, i, this->dimensions, this->connectivity);
                break;
            }
            i++;
        }
        
        if (edgeIndex < 0) {
            activeNodes.pop();
        }
        return edgeIndex;
    }
    
    // For all neighbors of the active node, connected through a non-saturated edge:
    // add them to the tree if it is a free node.
    // If it is not a free node and it is from the other tree: return a path
    
    return -1;
}


std::vector<int>* vtkGraphCutProtected::Augment(int edgeIndex) {
    // Steps:
    // * find path to source and find out max possible flow
    // * find path to sink and find out max possible flow
    // * apply maximum possible flow to both paths
    // * return all orphan nodes
    // When a path is found, the largest possible flow is pushed through the path,
    // thus at least one edge becomes saturated. And the child(ren) of such a saturated
    // path will become an orphan node (not connected to S or T anymore).
    assert(edgeIndex >= 0);
    assert(edgeIndex < this->edges->size());
    vtkEdge edge = this->edges->at(edgeIndex);
    assert(edge.isValid());
    assert(edge.node1() < (int)this->nodes->GetSize());
    assert(edge.node2() < (int)this->nodes->GetSize());
    
    // Figure out the tree type of the first node of the edge
    int node1Tree = 0;
    int fromNode = INVALID;
    if (edge.isTerminal()) {
        node1Tree = edge.rootNode();
        fromNode = node1Tree == SOURCE ? edge.rootNode() : edge.nonRootNode();
    } else {
        vtkNode* node1 = this->nodes->GetNode(edge.node1());
        assert(node1->tree == SOURCE || node1->tree == SINK);
        node1Tree = node1->tree;
        fromNode = node1Tree == SOURCE ? edge.node1() : edge.node2();
    }
    
    assert(fromNode != INVALID);
    int maxPossibleFlow = edge.capacityFromNode(fromNode);
    
    std::vector<int> pathToSource = PathToRoot(node1Tree == SOURCE ? edge.node1() : edge.node2(), this->connectivity, &maxPossibleFlow);
    std::vector<int> pathToSink = PathToRoot(node1Tree == SOURCE ? edge.node2() : edge.node1(), this->connectivity, &maxPossibleFlow);
    
    assert(maxPossibleFlow > 0);
    
    edge.addFlowFromNode(fromNode, maxPossibleFlow);
    this->edges->data()[edgeIndex] = edge;
    
    std::vector<int>* orphans = new std::vector<int>();
    
    PushFlowThroughEdges(maxPossibleFlow, pathToSource, orphans, SOURCE);
    PushFlowThroughEdges(maxPossibleFlow, pathToSink, orphans, SINK);
    
    return orphans;
}


void vtkGraphCutProtected::Adopt(std::vector<int>*) {
    
}


void vtkGraphCutProtected::PushFlowThroughEdges(int maxPossibleFlow, std::vector<int> edges, std::vector<int>* orphans, vtkTreeType tree) {
    for (std::vector<int>::iterator i = edges.begin(); i != edges.end(); ++i) {
        vtkEdge edge = this->edges->at(*i);
        
        int nodes[2] = {edge.node1(), edge.node2()};
        int closestToRoot = -1;
        
        if (edge.isTerminal()) {
            // One of the nodes is a negative number to indicate that it is a sink node
            closestToRoot = edge.node1() < edge.node2() ? 0 : 1;
        } else {
            vtkNode* node = this->nodes->GetNode(edge.node1());
            vtkNode* otherNode = this->nodes->GetNode(edge.node2());
            closestToRoot = node->depthInTree < otherNode->depthInTree ? 0 : 1;
        }
        
        int parent = nodes[closestToRoot];
        int child = nodes[(closestToRoot + 1) % 2];
        
        assert(child >= 0);
        
        // When tree type is SINK, then the flow should be pushed from child to parent
        int nodeFromWhichToPush = tree == SOURCE ? parent : child;
        
        // Update the flow
        edge.addFlowFromNode(nodeFromWhichToPush, maxPossibleFlow);
        this->edges->data()[*i] = edge;
        
        if (edge.isSaturatedFromNode(nodeFromWhichToPush)) {
            // Push the child if the edge becomes saturated
            orphans->push_back(child);
        }
    }
}


std::vector<int> vtkGraphCutProtected::PathToRoot(int aNodeIndex, vtkConnectivity connectivity, int* maxPossibleFlow) {
    std::vector<int> edges;
    vtkNode* node = NULL;
    int nodeIndex = aNodeIndex;
    while (true) {
        if (nodeIndex < 0) {
            // Encountered root node
            break;
        }
        node = this->nodes->GetNode(nodeIndex);
        int edgeIndex = IndexForEdgeFromNodeToNode(this->edges, nodeIndex, node->parent, this->dimensions, connectivity);
        assert(edgeIndex >= 0);
        assert(edgeIndex < this->edges->size());
        edges.push_back(edgeIndex);
        vtkEdge edge = this->edges->at(edgeIndex);
        int possibleFlow = edge.capacityFromNode(node->tree == SOURCE ? node->parent : nodeIndex);
        *maxPossibleFlow = std::min(possibleFlow, *maxPossibleFlow);
        nodeIndex = node->parent;
        if (edge.isTerminal()) {
            break;
        }
    }
    return edges;
}


int vtkGraphCutProtected::IndexForEdgeFromNodeToNode(std::vector<vtkEdge>* edges, int sourceIndex,
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
    
    int index = from * (2 + vtkGraphCutHelper::NumberOfEdgesForConnectivity(connectivity));
    assert(index >= from);
    
    vtkEdge edge = edges->at(index);
    if (to == SOURCE) {
        assert(edge.node1() == SOURCE && edge.node2() == from);
        return index;
    } else {
        while (edge.node1() <= from && edge.node2() != to) {
            ++index;
            edge = edges->at(index);
        }
    }
    
    assert(edge.node1() == from && edge.node2() == to);
    return index;
}


vtkEdge vtkGraphCutProtected::EdgeFromNodeToNode(std::vector<vtkEdge>* edges, int sourceIndex,
                                                 int targetIndex, int* dimensions, vtkConnectivity connectivity) {
    return edges->at(IndexForEdgeFromNodeToNode(edges, sourceIndex, targetIndex, dimensions, connectivity));
}


std::vector<vtkEdge>* vtkGraphCutProtected::CreateEdgesForNodes(vtkNodes* nodes,
                                                       int* dimensions, vtkConnectivity connectivity) {
    std::vector<vtkEdge>* result = new std::vector<vtkEdge>();
    
    int numberOfNodes = nodes->GetSize();
    int numberOfEdges = numberOfNodes * 2 + numberOfNodes * vtkGraphCutHelper::NumberOfEdgesForConnectivity(connectivity);
    assert(numberOfEdges >= 0);
    result->reserve(numberOfEdges);
    
    for (int i = 0; i < numberOfNodes; ++i) {
        vtkEdge sourceEdge = vtkEdge(SOURCE, i);
        result->push_back(sourceEdge);
        
        vtkEdge sinkEdge = vtkEdge(i, SINK);
        result->push_back(sinkEdge);
        
        int coordinate[3] = {0, 0, 0};
        nodes->GetCoordinateForIndex(i, coordinate);
        int coord[3] = {0, 0, 0};
        int index = 0;
        for (int z = 0; z < 2; ++z) {
            for (int y = 0; y < 2; ++y) {
                for (int x = 0; x < 2; ++x) {
                    coord[0] = coordinate[0]+x;
                    coord[1] = coordinate[1]+y;
                    coord[2] = coordinate[2]+z;
                    if (nodes->IsNodeAtOffsetConnected(x, y, z)) {
                        vtkEdge nodeEdge = vtkEdge(i, nodes->IsValidCoordinate(coord) ? nodes->GetIndexForCoordinate(coord) : INVALID);
                        result->push_back(nodeEdge);
                    }
                    ++index;
                }
            }
        }
    }
    
    return result;
}


// Protected methods

vtkGraphCutProtected::vtkGraphCutProtected() {
    inputImageData = NULL;
    outputImageData = NULL;
    nodes = NULL;
    edges = NULL;
    foregroundPoints = NULL;
    backgroundPoints = NULL;
    costFunction = NULL;
    dimensions = NULL;
    connectivity = UNCONNECTED;
    Reset();
}

vtkGraphCutProtected::~vtkGraphCutProtected() {
    Reset();
}


// Private methods

void vtkGraphCutProtected::CalculateCapacitiesForEdges() {
    //	double constantK 			= 0.0;
    //	double lambda 				= 500.0;
    
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
                double intensity = vtkGraphCutHelper::GetIntensityForVoxel(this->inputImageData, x, y, z);
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
                double intensity = vtkGraphCutHelper::GetIntensityForVoxel(this->inputImageData, x, y, z);
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
        double foregroundIntensity = vtkGraphCutHelper::GetIntensityForVoxel(this->inputImageData, xyz);
        foregroundMean += foregroundIntensity / (double)numberOfForegroundPoints;
        foregroundVariance += (foregroundIntensity * foregroundIntensity) / (double)numberOfForegroundPoints;
    }
    
    // Calculate the mean color of the background nodes
    int numberOfBackgroundPoints = this->backgroundPoints->GetNumberOfPoints();
    for (int i = 0; i < numberOfBackgroundPoints; i++) {
        double* xyz = this->backgroundPoints->GetPoint(i);
        double backgroundIntensity = vtkGraphCutHelper::GetIntensityForVoxel(this->inputImageData, xyz);
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
    
    for (std::vector<vtkEdge>::iterator i = this->edges->begin(); i != this->edges->end(); ++i) {
        double capacity = vtkGraphCutHelper::CalculateCapacity(this->inputImageData, *i, statistics);
        int cap = (int)(255.0 * capacity) + 1;
        i->setCapacity(cap);
    }
}
