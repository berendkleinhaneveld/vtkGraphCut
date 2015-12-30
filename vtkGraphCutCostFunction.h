#ifndef __vtkGraphCutCostFunction_h
#define __vtkGraphCutCostFunction_h

#include <vtkObjectFactory.h>

class VTK_EXPORT vtkGraphCutCostFunction : public vtkObject
{
public:
	static vtkGraphCutCostFunction* New();
	vtkTypeMacro(vtkGraphCutCostFunction, vtkObject);
	void PrintSelf(ostream& os, vtkIndent indent);

	void Reset();
	void Update();

	void SetNodes(float *nodes);

protected:
	vtkGraphCutCostFunction();
	~vtkGraphCutCostFunction();
};

#endif // __vtkGraphCutCostFunction_h
