//
//  vtkGraphCutProtected.h
//  vtkGraphCut
//
//  Created by Berend Klein Haneveld on 04/07/16.
//
//

#ifndef vtkGraphCutProtected_h
#define vtkGraphCutProtected_h

class vtkImageData;
class vtkPoints;
class vtkEdge;
class vtkGraphCutCostFunction;
class vtkNode;

#include <vtkObjectFactory.h>
#include <stdio.h>
#include <vector>
#include <queue>
#include "vtkGraphCutDefinitions.h"
#include "vtkGraphCutDataTypes.h"

class VTK_EXPORT vtkGraphCutProtected: public vtkObject
{
public:
    static vtkGraphCutProtected* New();
    vtkTypeMacro(vtkGraphCutProtected, vtkObject);
    void PrintSelf(ostream& os, vtkIndent indent);
    
    void Reset();
    void Update();
    
    vtkImageData* GetOutput();
    void SetInput(vtkImageData *);
    vtkImageData* GetInput();
    void SetSeedPoints(vtkPoints *foreground, vtkPoints *background);
    void SetCostFunction(vtkGraphCutCostFunction*);
    vtkGraphCutCostFunction* GetCostFunction();
    void SetConnectivity(vtkConnectivity);
    vtkConnectivity GetConnectivity();
    
    vtkPoints* GetForegroundPoints();
    vtkPoints* GetBackgroundPoints();
    
    std::vector<vtkNode>* CreateNodesForDimensions(int* dimensions);
    std::vector<vtkEdge>* CreateEdgesForNodes(std::vector<vtkNode>* nodes, int* dimensions, vtkConnectivity connectivity);
    std::vector<int>* IndicesForNeighbours(int index, int* dimensions, vtkConnectivity connectivity);
    bool IsValidCoordinate(int* coordinate, int* dimensions);
    int IndexForCoordinate(int* coordinate, int* dimensions);
    bool CoordinateForIndex(int index, int* dimensions, int* coordinate);
    
    int IndexForEdgeFromNodeToNode(std::vector<vtkEdge>* edges, int sourceIndex, int targetIndex, int* dimensions, vtkConnectivity connectivity);
    vtkEdge EdgeFromNodeToNode(std::vector<vtkEdge>* edges, int sourceIndex, int targetIndex, int* dimensions, vtkConnectivity connectivity);
    std::vector<int> PathToRoot(int aNodeIndex, vtkConnectivity connectivity, int* maxPossibleFlow);
    void PushFlowThroughEdges(int flow, std::vector<int> edges, std::vector<int>* orphans, vtkTreeType);
    
    // Algorithm methods
    int Grow(vtkTreeType tree, bool& foundActiveNodes, std::priority_queue<std::pair<int, int> > activeNodes);
    std::vector<int>* Augment(int edgeIndex);
    void Adopt(std::vector<int>*);
    
protected:
    vtkGraphCutProtected();
    ~vtkGraphCutProtected();
    
    vtkImageData* inputImageData;
    vtkImageData* outputImageData;
    
    std::vector<vtkNode>* nodes;
    std::vector<vtkEdge>* edges;
    
    vtkPoints* foregroundPoints;
    vtkPoints* backgroundPoints;
    
    vtkGraphCutCostFunction* costFunction;
    
    int* dimensions;
    vtkConnectivity connectivity;
    
private:
    void CalculateCapacitiesForEdges();
};

#endif /* vtkGraphCutProtected_h */
