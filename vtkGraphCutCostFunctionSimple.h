//
//  vtkGraphCutCostFunctionSimple.h
//  vtkGraphCut
//
//  Created by Berend Klein Haneveld.
//
//

#ifndef __vtkGraphCutCostFunctionSimple_h
#define __vtkGraphCutCostFunctionSimple_h

#include <vtkObjectFactory.h>
#include "vtkGraphCutCostFunction.h"

class VTK_EXPORT vtkGraphCutCostFunctionSimple : public vtkGraphCutCostFunction
{
public:
	static vtkGraphCutCostFunctionSimple* New();
	void PrintSelf(ostream& os, vtkIndent indent);

	virtual void Reset();
	virtual void Update();

	virtual void SetNodes(std::vector<Node>* nodes);
	virtual void SetEdges(std::vector<Edge>* edges);
	virtual void SetInput(vtkImageData*);

protected:
	vtkGraphCutCostFunctionSimple();
	~vtkGraphCutCostFunctionSimple();

	std::vector<Node>* _nodes;
	std::vector<Edge>* _edges;
	vtkImageData* _imageData;
};

#endif // __vtkGraphCutCostFunctionSimple_h
