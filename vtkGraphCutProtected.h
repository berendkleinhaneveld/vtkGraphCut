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
class vtkNode;
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
    int Grow(vtkTreeType tree, bool& foundActiveNodes, std::priority_queue<std::pair<int, int> > activeNodes);
    std::vector<int>* Augment(int edgeIndex);
    void Adopt(std::vector<int>*);
    
protected:
    vtkGraphCutProtected();
    ~vtkGraphCutProtected();
    
    vtkImageData* inputImageData;
    vtkImageData* outputImageData;
    
    Nodes* nodes;
    Edges* edges;
    
    vtkPoints* foregroundPoints;
    vtkPoints* backgroundPoints;
    
    vtkGraphCutCostFunction* costFunction;
    
    int* dimensions;
    vtkConnectivity connectivity;
    
private:
    void CalculateCapacitiesForEdges();
};

#endif /* vtkGraphCutProtected_h */
