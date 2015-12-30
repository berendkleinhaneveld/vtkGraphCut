#ifndef __vtkGraphCut_h
#define __vtkGraphCut_h

#include <vtkObjectFactory.h>
#include <vtkPoints.h>
#include <vtkImageData.h>
#include "vtkGraphCutCostFunction.h"


class VTK_EXPORT vtkGraphCut : public vtkObject
{
public:
	static vtkGraphCut* New();
	vtkTypeMacro(vtkGraphCut, vtkObject);
	void PrintSelf(ostream& os, vtkIndent indent);

	void Reset();
	void Update();

	vtkImageData* GetOutput();
	void SetInput(vtkImageData*);
	void SetSeedPoints(vtkPoints *foreground, vtkPoints *background);
	void SetCostFunction(vtkGraphCutCostFunction*);

protected:
	vtkGraphCut();
	~vtkGraphCut();
};

#endif // __vtkGraphCut_h
