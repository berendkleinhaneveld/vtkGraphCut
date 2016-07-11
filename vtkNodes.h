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
     * 
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
     * Creates and returns a vector of vtkNode objects.
     */
    std::vector<vtkNode*>* CreateNodesForDimensions(int* dimensions);

protected:
    std::vector<vtkNode*>* _nodes;
    vtkConnectivity _connectivity;
    int* _dimensions;
};

#endif /* vtkNodes_h */
