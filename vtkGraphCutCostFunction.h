//
//  vtkGraphCutCostFunction.h
//  vtkGraphCut
//
//  Created by Berend Klein Haneveld.
//
//

#ifndef __vtkGraphCutCostFunction_h
#define __vtkGraphCutCostFunction_h

class vtkEdge;
struct vtkNode;

#include <vtkObjectFactory.h>
#include <vtkImageData.h>
#include <vector>

class VTK_EXPORT vtkGraphCutCostFunction : public vtkObject
{
public:
	static vtkGraphCutCostFunction* New();
	vtkTypeMacro(vtkGraphCutCostFunction, vtkObject);
	void PrintSelf(ostream& os, vtkIndent indent);

	virtual void Reset();
	virtual void Update();

	// TODO: design API for cost function: what should it do? Does it need
	// the following methods at all? Or should it work in a more functional
	// manor?
	virtual void SetNodes(std::vector<vtkNode>* nodes);
	virtual void SetEdges(std::vector<vtkEdge>* edges);
	virtual void SetInput(vtkImageData*);


protected:
	vtkGraphCutCostFunction();
	~vtkGraphCutCostFunction();
};

#endif // __vtkGraphCutCostFunction_h
