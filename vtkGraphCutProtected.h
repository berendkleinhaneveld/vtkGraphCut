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
class Tree;
class TreeDepthComparator;


#include <vtkObjectFactory.h>
#include <vector>
#include <queue>
#include "vtkGraphCutDefinitions.h"
#include "vtkGraphCutDataTypes.h"

typedef std::priority_queue<std::pair<int, NodeIndex>, std::vector<std::pair<int, NodeIndex> >, TreeDepthComparator> PriorityQueue;

class VTK_EXPORT vtkGraphCutProtected: public vtkObject
{
public:
    static vtkGraphCutProtected* New();
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
    
protected:
    // Algorithm methods
    EdgeIndex Grow(vtkTreeType tree, bool& foundActiveNodes, PriorityQueue* activeNodes);
    std::vector<NodeIndex>* Augment(EdgeIndex edgeIndex);
    void Adopt(std::vector<NodeIndex>*);
    
    vtkGraphCutProtected();
    ~vtkGraphCutProtected();
    
    vtkImageData* _inputImageData;
    vtkImageData* _outputImageData;
    
    Nodes* _nodes;
    Edges* _edges;

    Tree* _sourceTree;
    Tree* _sinkTree;

    std::vector<NodeIndex>* _orphans;

    vtkPoints* _foregroundPoints;
    vtkPoints* _backgroundPoints;
    
    vtkGraphCutCostFunction* _costFunction;
    
    int _dimensions[3];
    vtkConnectivity _connectivity;
    
private:
    void CalculateCapacitiesForEdges();
};

#endif /* vtkGraphCutProtected_h */
