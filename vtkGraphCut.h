#ifndef __vtkGraphCut_h
#define __vtkGraphCut_h

#include <vtkObjectFactory.h>
#include <vtkPoints.h>
#include <vtkImageData.h>
#include "vtkGraphCutDataTypes.h"
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
	void SetSeedPoints(vtkPoints *foreground, vtkPoints *background);
	void SetCostFunction(vtkGraphCutCostFunction*);

	vtkPoints* GetForegroundPoints();
	vtkPoints* GetBackgroundPoints();

	std::vector<vtkNode>* CreateNodesForDimensions(int* dimensions);
	std::vector<vtkEdge>* CreateEdgesForNodes(std::vector<vtkNode>* nodes, int* dimensions, vtkConnectivity connectivity);
	void DimensionsForImageData(vtkImageData* imageData, int* dimensions);
	std::vector<int>* IndicesForNeighbours(int index, int* dimensions, vtkConnectivity connectivity);
	bool IsValidCoordinate(int* coordinate, int* dimensions);
	int IndexForCoordinate(int* coordinate, int* dimensions);
	bool CoordinateForIndex(int index, int* dimensions, int* coordinate);

	vtkEdge EdgeFromNodeToNode(std::vector<vtkEdge>* edges, int sourceIndex, int targetIndex, int* dimensions, vtkConnectivity connectivity);

protected:
	vtkGraphCut();
	~vtkGraphCut();

	vtkImageData* inputImageData;
	vtkImageData* outputImageData;

	std::vector<vtkNode>* nodes;
	std::vector<vtkEdge>* edges;

	vtkPoints* foregroundPoints;
	vtkPoints* backgroundPoints;

	int* dimensions;
};

#endif // __vtkGraphCut_h
