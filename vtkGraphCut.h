//
//  vtkGraphCut.h
//  vtkGraphCut
//
//  Created by Berend Klein Haneveld.
//
//

#ifndef __vtkGraphCut_h
#define __vtkGraphCut_h


// Forward declaration of helper types
class vtkEdge;
class vtkNode;
class vtkGraphCutProtected;


// Dependencies
#include <vtkObjectFactory.h>
#include <vtkPoints.h>
#include <vtkImageData.h>
#include "vtkGraphCutDefinitions.h"
#include "vtkGraphCutCostFunction.h"


/**
 * vtkGraphCut is an object that can be used to create a segmentation of
 * a vtkImageData object. In order to do this, it holds a reference to an
 * undirected graph.
 * Besides this graph, a list of vktGraphEdges is kept that hold references
 * to vtkEdgeTypes in the graph. These vtkGraphCutEdges hold information about
 * direction of flow, throughput, connectedness, etc.
 *
 * There are three stages in the algorithm:
 *
 * - Growth
 * - Augmentation
 * - Adoption
 *
 * Stages should be repeated until there are no more active nodes, and the
 * trees T and S are separated by saturated edges. These edges form the
 * minimum-flow cut.
 *
 */
class VTK_EXPORT vtkGraphCut : public vtkObject
{
public:
	static vtkGraphCut* New();
	vtkTypeMacro(vtkGraphCut, vtkObject);
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
	vtkGraphCut();
	~vtkGraphCut();
    
    vtkGraphCutProtected* graphCut;
};

#endif // __vtkGraphCut_h
