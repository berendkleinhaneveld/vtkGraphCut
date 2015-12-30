#include "vtkGraphCutCostFunction.h"

#pragma mark - Public

vtkStandardNewMacro(vtkGraphCutCostFunction);

void vtkGraphCutCostFunction::PrintSelf(ostream& os, vtkIndent indent) {
	this->Superclass::PrintSelf(os, indent);
}

void vtkGraphCutCostFunction::Reset() { }
void vtkGraphCutCostFunction::Update() { }
void vtkGraphCutCostFunction::SetNodes(float *nodes) { }

#pragma mark - Protected

vtkGraphCutCostFunction::vtkGraphCutCostFunction() { }
vtkGraphCutCostFunction::~vtkGraphCutCostFunction() { }
