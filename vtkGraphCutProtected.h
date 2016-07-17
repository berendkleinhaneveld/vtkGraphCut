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
class Edge;
class Edges;
class vtkGraphCutCostFunction;
class Node;
class Nodes;

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
    
    // Algorithm methods
    EdgeIndex Grow(vtkTreeType tree, bool& foundActiveNodes, std::priority_queue<std::pair<int, NodeIndex> > activeNodes);
    std::vector<NodeIndex>* Augment(EdgeIndex edgeIndex);
    void Adopt(std::vector<NodeIndex>*);
    
protected:
    vtkGraphCutProtected();
    ~vtkGraphCutProtected();
    
    vtkImageData* _inputImageData;
    vtkImageData* _outputImageData;
    
    Nodes* _nodes;
    Edges* _edges;
    
    vtkPoints* _foregroundPoints;
    vtkPoints* _backgroundPoints;
    
    vtkGraphCutCostFunction* _costFunction;
    
    int* _dimensions;
    vtkConnectivity _connectivity;
    
private:
    void CalculateCapacitiesForEdges();
};

#endif /* vtkGraphCutProtected_h */
