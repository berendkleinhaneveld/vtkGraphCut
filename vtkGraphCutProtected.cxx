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
#include "Internal/Node.h"
#include "Internal/Nodes.h"
#include "Internal/Edge.h"
#include "Internal/Edges.h"
#include "Internal/Tree.h"
#include "Internal/TreeDepthComparator.h"
#include <assert.h>
#include "vtkGraphCutHelperFunctions.h"
#include "vtkGraphCutCostFunction.h"


vtkStandardNewMacro(vtkGraphCutProtected);


void vtkGraphCutProtected::PrintSelf(ostream& os, vtkIndent indent) {
    Superclass::PrintSelf(os, indent);
}


void vtkGraphCutProtected::Reset() {
    // Properties set from outside the graph cut
    if (_inputImageData) {
        _inputImageData = NULL;
    }
    if (_foregroundPoints) {
        _foregroundPoints = NULL;
    }
    if (_backgroundPoints) {
        _backgroundPoints = NULL;
    }
    if (_costFunction) {
        _costFunction = NULL;
    }
    _connectivity = UNCONNECTED;
    
    // Instance variables
    if (_outputImageData) {
        _outputImageData->Delete();
        _outputImageData = NULL;
    }
    if (_sourceTree) {
        delete _sourceTree;
        _sourceTree = NULL;
    }
    if (_sinkTree) {
        delete _sinkTree;
        _sinkTree = NULL;
    }
    if (_orphans) {
        _orphans->clear();
        delete _orphans;
        _orphans = NULL;
    }
    memset_s(_dimensions, sizeof(_dimensions), 0, sizeof(_dimensions));
    if (_edges) {
        delete _edges;
        _edges = NULL;
    }
    if (_nodes) {
        delete _nodes;
        _nodes = NULL;
    }
}


vtkImageData* vtkGraphCutProtected::GetOutput() {
    return _outputImageData;
}


void vtkGraphCutProtected::SetInput(vtkImageData* imageData) {
    _inputImageData = imageData;
}


vtkImageData* vtkGraphCutProtected::GetInput() {
    return _inputImageData;
}


void vtkGraphCutProtected::SetSeedPoints(vtkPoints* foreground, vtkPoints* background) {
    _foregroundPoints = foreground;
    _backgroundPoints = background;
}


vtkPoints* vtkGraphCutProtected::GetForegroundPoints() {
    return _foregroundPoints;
}


vtkPoints* vtkGraphCutProtected::GetBackgroundPoints() {
    return _backgroundPoints;
}


void vtkGraphCutProtected::SetCostFunction(vtkGraphCutCostFunction* costFunction) {
    _costFunction = costFunction;
}


vtkGraphCutCostFunction* vtkGraphCutProtected::GetCostFunction() {
    return _costFunction;
}


void vtkGraphCutProtected::SetConnectivity(vtkConnectivity connectivity) {
    _connectivity = connectivity;
}


vtkConnectivity vtkGraphCutProtected::GetConnectivity() {
    return _connectivity;
}


void vtkGraphCutProtected::Update() {
    // Verify all inputs (if changed since last update):
    
    if (_connectivity == UNCONNECTED) {
        vtkWarningMacro(<< "No connectivity was specified. Skipping update.");
        return;
    }
    
    // Verify input image data
    if (!_inputImageData) {
        vtkWarningMacro(<< "No image data was set. Skipping update.");
        return;
    }
    
    // Verify seed points
    if (!_foregroundPoints || !_backgroundPoints) {
        vtkWarningMacro(<< "No fore- or background points were set. Skipping update.");
        return;
    } else if (_foregroundPoints->GetNumberOfPoints() == 0) {
        vtkWarningMacro(<< "Number of foreground points is zero. Skipping update.");
        return;
    } else if (_backgroundPoints->GetNumberOfPoints() == 0) {
        vtkWarningMacro(<< "Number of foreground points is zero. Skipping update.");
        return;
    }
    
    // TODO: check that fore- and background points are located in the image data
    // TODO: Verify cost function

    _inputImageData->GetDimensions(_dimensions);

    // Build nodes data if not exists yet
    if (!_nodes) {
        _nodes = new Nodes();
        _nodes->SetConnectivity(_connectivity);
        _nodes->SetDimensions(_dimensions);
        _nodes->Update();
    }
    // Build edges data if not exists yet
    if (!_edges) {
        _edges = new Edges();
        _edges->SetNodes(_nodes);
        _edges->Update();
    }
    
    CalculateCapacitiesForEdges();

    if (!_sinkTree) {
        _sinkTree = new Tree(TREE_SINK, _edges);
    }
    if (!_sourceTree) {
        _sourceTree = new Tree(TREE_SOURCE, _edges);
    }

    PriorityQueue* activeSourceNodes = new PriorityQueue();
    PriorityQueue* activeSinkNodes = new PriorityQueue();

    if (!_orphans) {
        _orphans = new std::vector<NodeIndex>();
    }
    
    activeSourceNodes->push(std::make_pair(0, NODE_SOURCE));
    activeSinkNodes->push(std::make_pair(0, NODE_SINK));
    
    // Start algorithm
    while (true) {
        // Stage 0: Adopt phase
        // There might be orphans in the trees because the user
        // might have added/removed fore- and background points
        if (_orphans->size() > 0) {
            Adopt(_orphans);
        }
        
        int treeSelector = 0;
        int noActiveNodesCounter = 0;
        EdgeIndex edgeIndexBetweenGraphs = (EdgeIndex)-1;
        
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
            vtkTreeType tree = (treeSelector % 2 == 0) ? TREE_SOURCE : TREE_SINK;
            
            // foundActiveNodes is set when active nodes are found, but no path was found
            // If no path is found, but there was a succesful growing iteration the other tree may grow
            bool foundActiveNodes;
            edgeIndexBetweenGraphs = Grow(tree, foundActiveNodes, tree == TREE_SOURCE ? activeSourceNodes : activeSinkNodes);
            noActiveNodesCounter = foundActiveNodes ? 0 : noActiveNodesCounter + 1;
            
            // If a path has been found, then we can break the loop and proceed to the next part
            if (edgeIndexBetweenGraphs >= 0) {
                break;
            }
            ++treeSelector;
        }
        
        if (edgeIndexBetweenGraphs <= EDGE_NONE) {
            // Didn't find a path and there should be no more active nodes, so we can call it quits
            assert(activeSourceNodes->size() == 0);
            assert(activeSinkNodes->size() == 0);
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
        _orphans = Augment(edgeIndexBetweenGraphs);
        
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
        Adopt(_orphans);
    }
    
    delete activeSinkNodes;
    delete activeSourceNodes;
    
    _outputImageData = vtkImageData::New();
    _outputImageData->SetDimensions(_dimensions);
    _outputImageData->AllocateScalars(VTK_CHAR, 1);
    _outputImageData->SetSpacing(_inputImageData->GetSpacing());
    _outputImageData->SetOrigin(_inputImageData->GetOrigin());

    for (int z = 0; z < _dimensions[2]; ++z) {
        for (int y = 0; y < _dimensions[1]; ++y) {
            for (int x = 0; x < _dimensions[0]; ++x) {
                int coordinate[3];
                coordinate[0] = x;
                coordinate[1] = y;
                coordinate[2] = z;
                NodeIndex nodeIndex = _nodes->GetIndexForCoordinate(coordinate);
                Node* node = _nodes->GetNode(nodeIndex);
                double value = 0;
                if (node->tree == TREE_SOURCE) {
                    value = 1;
                } else if (node->tree == TREE_SINK) {
                    value = -1;
                }
                _outputImageData->SetScalarComponentFromDouble(x, y, z, 0, value);
            }
        }
    }
}


// Algorithm steps

/**
 * Returns EDGE_NONE if no edge has been found.
 */
EdgeIndex vtkGraphCutProtected::Grow(vtkTreeType tree, bool& foundActiveNodes, PriorityQueue* activeNodes) {
    foundActiveNodes = false;
    
    // Get an active node from the tree
    std::pair<int, NodeIndex> active = activeNodes->top();
    
    // Pop until active node is found
    while (true) {
        if (active.second >= 0) {
            Node* node = _nodes->GetNode(active.second);
            if (node->active) {
                break;
            }
            
            assert(activeNodes->size() > 0 && activeNodes->size() < 500);
            activeNodes->pop();

            // If there are no more active nodes, return EDGE_NONE
            if (activeNodes->size() == 0) {
                return EDGE_NONE;
            }
            active = activeNodes->top();
        } else {
            break;
        }
    }
    
    NodeIndex activeNodeIndex = active.second;
    if (activeNodeIndex >= 0) {
        std::vector<NodeIndex> neighbours = _nodes->GetIndicesForNeighbours(activeNodeIndex);
        Edge* edgeBetweenTrees = NULL;
        int index = 0;
        for (std::vector<NodeIndex>::iterator i = neighbours.begin(); i != neighbours.end(); ++i) {
            // Check to see if the edge to the node is saturated or not
            Edge* edge = _edges->EdgeFromNodeToNode(activeNodeIndex, *i);
            if (!edge->isSaturatedFromNode(tree == TREE_SOURCE ? activeNodeIndex : *i)) {
                // If the other node is free, it can be added to the tree
                Node* neighbour = _nodes->GetNode(*i);
                if (neighbour->tree == TREE_NONE) {
                    // Other node is added as a child to active node
                    (tree == TREE_SOURCE) ? _sourceTree->AddChildToParent(*i, activeNodeIndex) : _sinkTree->AddChildToParent(*i, activeNodeIndex);
                    neighbour->active = true;
                    foundActiveNodes = true;
                    activeNodes->push(std::make_pair(neighbour->depthInTree, *i));
                } else if (neighbour->tree != tree) {
                    // If the other node is from the other tree, we have found a path!
                    edgeBetweenTrees = edge;
                    break;
                }
            }
            index += 1;
        }
        
        EdgeIndex edgeIndex = EDGE_NONE;
        // If no edge has been found, then the current node can become inactive
        if (!edgeBetweenTrees) {
            Node* node = _nodes->GetNode(activeNodeIndex);
            node->active = false;
            activeNodes->pop();
        } else {
            edgeIndex = (EdgeIndex)index;
        }
        return edgeIndex;
    } else { // Tree node
        int i = 0;
        EdgeIndex edgeIndex = EDGE_NONE;
        for (std::vector<Node*>::iterator it = _nodes->GetIterator(); it != _nodes->GetEnd(); ++it) {
            // If the other node is free, it can be added to the tree
            Node* node = *it;
            Edge* edge = _edges->EdgeFromNodeToNode(activeNodeIndex, (NodeIndex)i);
            if (!edge->isSaturatedFromNode(tree == TREE_SOURCE ? (NodeIndex)tree : (NodeIndex)i)) {
                if (node->tree == TREE_NONE) {
                    // Other node is added as a child to active node
                    (tree == TREE_SOURCE) ? _sourceTree->AddChildToParent((NodeIndex)i, (NodeIndex)tree) : _sinkTree->AddChildToParent((NodeIndex)i, (NodeIndex)tree);
                    (*it)->active = true;
                    foundActiveNodes = true;
                    activeNodes->push(std::make_pair(node->depthInTree, (NodeIndex)i));
                } else if (node->tree != tree) {
                    // If the other node is from the other tree, we have found a path!
                    edgeIndex = _edges->IndexForEdgeFromNodeToNode(activeNodeIndex, (NodeIndex)i);
                    break;
                }
            }
            ++i;
        }
        
        if (edgeIndex < 0) {
            assert(activeNodes->top().second == NODE_SINK || activeNodes->top().second == NODE_SOURCE);
            activeNodes->pop();
        }
        return edgeIndex;
    }
}


std::vector<NodeIndex>* vtkGraphCutProtected::Augment(EdgeIndex edgeIndex) {
    // Steps:
    // * find path to source and find out max possible flow
    // * find path to sink and find out max possible flow
    // * apply maximum possible flow to both paths
    // * return all orphan nodes
    // When a path is found, the largest possible flow is pushed through the path,
    // thus at least one edge becomes saturated. And the child(ren) of such a saturated
    // path will become an orphan node (not connected to S or T anymore).
    assert(edgeIndex >= 0);
    assert(edgeIndex < _edges->GetSize());
    Edge* edge = _edges->GetEdge(edgeIndex);
    assert(edge->isValid());
    assert(edge->node1() < (int)_nodes->GetSize());
    assert(edge->node2() < (int)_nodes->GetSize());
    
    // Figure out the tree type of the first node of the edge
    int node1Tree = 0;
    NodeIndex fromNode = NODE_NONE;
    if (edge->isTerminal()) {
        node1Tree = edge->rootNode();
        fromNode = node1Tree == NODE_SOURCE ? edge->rootNode() : edge->nonRootNode();
    } else {
        Node* node1 = _nodes->GetNode(edge->node1());
        assert(node1->tree == TREE_SOURCE || node1->tree == TREE_SINK);
        node1Tree = node1->tree;
        fromNode = node1Tree == NODE_SOURCE ? edge->node1() : edge->node2();
    }
    
    assert(fromNode != NODE_NONE);
    int maxPossibleFlow = edge->capacityFromNode(fromNode);
    
    std::vector<EdgeIndex> pathToSource = _sourceTree->PathToRoot(node1Tree == NODE_SOURCE ? edge->node1() : edge->node2(), &maxPossibleFlow);
    std::vector<EdgeIndex> pathToSink = _sinkTree->PathToRoot(node1Tree == NODE_SOURCE ? edge->node2() : edge->node1(), &maxPossibleFlow);
    
    assert(maxPossibleFlow > 0);
    
    edge->addFlowFromNode(fromNode, maxPossibleFlow);

    _sourceTree->PushFlowThroughPath(pathToSource, maxPossibleFlow, _orphans);
    _sinkTree->PushFlowThroughPath(pathToSink, maxPossibleFlow, _orphans);
    
    return _orphans;
}


void vtkGraphCutProtected::Adopt(std::vector<NodeIndex>* orphans) {
    for (std::vector<NodeIndex>::iterator orphan = orphans->begin(); orphan != orphans->end(); ++orphan) {
        Node* node = _nodes->GetNode(*orphan);
        assert(node->orphan);
        node->tree == TREE_SOURCE ? _sourceTree->Adopt(*orphan) : _sinkTree->Adopt(*orphan);
        assert(!node->orphan);
    }

    orphans->clear();
}


// Protected methods

vtkGraphCutProtected::vtkGraphCutProtected() {
    _inputImageData = NULL;
    _outputImageData = NULL;
    _nodes = NULL;
    _edges = NULL;
    _foregroundPoints = NULL;
    _backgroundPoints = NULL;
    _sourceTree = NULL;
    _sinkTree = NULL;
    _orphans = NULL;
    _costFunction = NULL;
    _dimensions[0] = 0;
    _dimensions[1] = 0;
    _dimensions[2] = 0;
    _connectivity = UNCONNECTED;
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
    
    int* dimensions = _inputImageData->GetDimensions();
    for (int z = 0; z < dimensions[2]; ++z) {
        for (int y = 0; y < dimensions[1]; ++y) {
            for (int x = 0; x < dimensions[0]; ++x) {
                double intensity = vtkGraphCutHelper::GetIntensityForVoxel(_inputImageData, x, y, z);
                minimum = std::min(minimum, intensity);
                maximum = std::max(maximum, intensity);
                mean += intensity;
            }
        }
    }
    
    if (minimum == maximum) {
        vtkWarningMacro("Warning: all nodes have the same intensity.");
        return;
    }
    
    int numberOfVoxels = dimensions[0] + dimensions[1] * dimensions[2];
    mean = mean / (double)(numberOfVoxels);
    
    for (int z = 0; z < dimensions[2]; ++z) {
        for (int y = 0; y < dimensions[1]; ++y) {
            for (int x = 0; x < dimensions[0]; ++x) {
                double intensity = vtkGraphCutHelper::GetIntensityForVoxel(_inputImageData, x, y, z);
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
    int numberOfForegroundPoints = _foregroundPoints->GetNumberOfPoints();
    for (int i = 0; i < numberOfForegroundPoints; ++i) {
        double* xyz = _foregroundPoints->GetPoint(i);
        double foregroundIntensity = vtkGraphCutHelper::GetIntensityForVoxel(_inputImageData, xyz);
        foregroundMean += foregroundIntensity / (double)numberOfForegroundPoints;
        foregroundVariance += (foregroundIntensity * foregroundIntensity) / (double)numberOfForegroundPoints;
    }
    
    // Calculate the mean color of the background nodes
    int numberOfBackgroundPoints = _backgroundPoints->GetNumberOfPoints();
    for (int i = 0; i < numberOfBackgroundPoints; ++i) {
        double* xyz = _backgroundPoints->GetPoint(i);
        double backgroundIntensity = vtkGraphCutHelper::GetIntensityForVoxel(_inputImageData, xyz);
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
    
    Nodestatistics statistics;
    statistics.minimum = minimum;
    statistics.maximum = maximum;
    statistics.mean = mean;
    statistics.variance = variance;
    statistics.foregroundMean = foregroundMean;
    statistics.foregroundVariance = foregroundVariance;
    statistics.backgroundMean = backgroundMean;
    statistics.backgroundVariance = backgroundVariance;
    
    for (std::vector<Edge*>::iterator i = _edges->GetBegin(); i != _edges->GetEnd(); ++i) {
        double capacity = vtkGraphCutHelper::CalculateCapacity(_inputImageData, *i, statistics);
        int cap = (int)(255.0 * capacity) + 1;
        (*i)->setCapacity(cap);
    }
}
