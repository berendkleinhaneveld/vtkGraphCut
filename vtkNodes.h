//
//  vtkNodes.h
//  vtkGraphCut
//
//  Created by Berend Klein Haneveld on 11/07/16.
//
//

#ifndef vtkNodes_h
#define vtkNodes_h

#include <vector>
#include "vtkNode.h"
#include "vtkGraphCutDefinitions.h"

class vtkNodes {
public:
    // Constructors
    vtkNodes();
    ~vtkNodes();
    
    // Properties
    void SetConnectivity(vtkConnectivity);
    vtkConnectivity GetConnectivity();
    
    void SetDimensions(int* dimensions);
    int* GetDimensions();
    
    /**
     * Updates internal state to apply
     * the new properties, if any.
     */
    void Update();
    
    /**
     * Clears internal state so that it can
     * be reused and new properties can be set.
     */
    void Reset();
    
    /**
     * Returns indices of all neighbouring nodes.
     */
    std::vector<int>* GetIndicesForNeighbours(int index);
    
    /**
     * Returns true iff the index is within the internal
     * nodes array and coordinate is pointing to valid value.
     * This function returns false if the index is not valid.
     */
    bool GetCoordinateForIndex(int index, int* coordinate);
    
    /**
     * Returns true iff a node at the proposed offset
     * would be connected with an edge, given the current
     * connectivity.
     * x, y and z should be either -1, 0 or 1.
     */
    bool IsNodeAtOffsetConnected(int x, int y, int z);
    
    /**
     * Returns true iff the coordinate falls between the
     * dimensions of the nodes.
     */
    bool IsValidCoordinate(int* coordinate);
    
    /**
     * Returns the index of a node for a given coordinate.
     */
    int GetIndexForCoordinate(int* coordinate);
    
    /**
     * Returns the node for a given index.
     */
    vtkNode* GetNode(int index);
    
    /**
     * Returns the number of nodes;
     */
    int GetSize();

    /**
     * Returns an iterator to the start of the internal vector.
     */
    std::vector<vtkNode*>::iterator GetIterator();
    
    /**
     * Returns the iterator at the end of the internal vector.
     */
    std::vector<vtkNode*>::iterator GetEnd();
    
    /**
     * Creates and returns a vector of vtkNode objects.
     */
    std::vector<vtkNode*>* CreateNodesForDimensions(int* dimensions);

protected:
    std::vector<vtkNode*>* _nodes;
    vtkConnectivity _connectivity;
    int* _dimensions;
};

#endif /* vtkNodes_h */
