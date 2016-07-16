//
//  Edges.h
//  vtkGraphCut
//
//  Created by Berend Klein Haneveld on 11/07/16.
//
//

#ifndef Edges_h
#define Edges_h


class vtkEdge;
class vtkNodes;


#include <vector>
#include "vtkGraphCutDefinitions.h"
#include "vtkGraphCutDataTypes.h"


class Edges {
public:
    // Constructors
    Edges();
    ~Edges();
    
    // Properties
    void SetNodes(vtkNodes*);
    vtkNodes* GetNodes();
    
    /**
     * Updates internal state to apply
     * the new properties, if any.
     * Make sure to set the Nodes property
     * before calling Update.
     */
    void Update();
    
    /**
     * Clears internal state so that it can
     * be reused and new properties can be set.
     */
    void Reset();
    
    /**
     * Returns edge object at the specified index or
     * NULL if it doesn't exist.
     */
    vtkEdge* GetEdge(int index);
    
    /**
     * Returns the number of edges.
     */
    int GetSize();
    
    /**
     * Returns the begin iterator of the internal vector.
     */
    std::vector<vtkEdge*>::iterator GetBegin();
    
    /**
     * Returns the end iterator of the internal vector.
     */
    std::vector<vtkEdge*>::iterator GetEnd();
    
    /**
     * Returns the index for the edge that connect the node
     * at sourceIndex to the node at targetIndex. When there
     * is no valid index, returns -1.
     */
    int IndexForEdgeFromNodeToNode(int sourceIndex, int targetIndex);
    
    /**
     * Returns the edge at the index that is found by
     * IndexForEdgeFromNodeToNode. If an invalid edge is requested,
     * then this function will return NULL.
     */
    vtkEdge* EdgeFromNodeToNode(int sourceIndex, int targetIndex);
    
    /**
     * Returns a vector of indices of edges that form a path from the 
     * node at aNodeIndex to the root of the tree. The node at the given
     * index should be part of a tree.
     * The value that maxPossibleFlow is pointing at will be the maximum
     * possible flow that will be possible to push through the returned path.
     */
    // TODO: return vtkEdge* vector instead of indices
    std::vector<int> PathToRoot(int aNodeIndex, int* maxPossibleFlow);
    
    /**
     * Pushes the given flow through all of the edges at the given indices.
     * Whenever an edge becomes saturated because of the pushed flow, then
     * the index of the parent node of that edge will be added to the orphans
     * vector passed into this function. The treetype is used to define whether
     * the flow should be pushed from root to leaf or from leaf to root.
     */
    void PushFlowThroughEdges(int flow, std::vector<int> edges, vtkTreeType tree, std::vector<int>* orphans);
    
    /**
     * Creates and returns a vector of vtkEdge objects. The amount of objects
     * depends on the connectivity property of the vtkNodes object.
     */
    std::vector<vtkEdge*>* CreateEdgesForNodes(vtkNodes*);
    
    /**
     * Returns the number of edges for a given connectivity.
     */
    int NumberOfEdgesForConnectivity(vtkConnectivity connectivity);
    
protected:
    std::vector<vtkEdge*>* _edges;
    vtkNodes* _nodes;
};

#endif /* Edges_h */
