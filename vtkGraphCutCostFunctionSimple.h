#ifndef __vtkGraphCutCostFunctionSimple_h
#define __vtkGraphCutCostFunctionSimple_h

#include <vtkObjectFactory.h>
#include "vtkGraphCutCostFunction.h"

class VTK_EXPORT vtkGraphCutCostFunctionSimple : public vtkGraphCutCostFunction
{
public:
	static vtkGraphCutCostFunctionSimple* New();
	vtkTypeMacro(vtkGraphCutCostFunctionSimple, vtkObject);
	void PrintSelf(ostream& os, vtkIndent indent);

	virtual void Reset();
	virtual void Update();

	virtual void SetNodes(std::vector<vtkNode>* nodes);
	virtual void SetEdges(std::vector<vtkEdge>* edges);
	virtual void SetInput(vtkImageData*);

protected:
	vtkGraphCutCostFunctionSimple();
	~vtkGraphCutCostFunctionSimple();

	std::vector<vtkNode>* nodes;
	std::vector<vtkEdge>* edges;
	vtkImageData* imageData;
};

#endif // __vtkGraphCutCostFunctionSimple_h
