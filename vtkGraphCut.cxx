#include "vtkGraphCut.h"

#pragma mark - Public

vtkStandardNewMacro(vtkGraphCut);

void vtkGraphCut::PrintSelf(ostream& os, vtkIndent indent) {
	this->Superclass::PrintSelf(os, indent);
}

void vtkGraphCut::Reset() {}
void vtkGraphCut::Update() {}

vtkImageData* vtkGraphCut::GetOutput() {return NULL;}
void vtkGraphCut::SetInput(vtkImageData* imageData) {}
void vtkGraphCut::SetSeedPoints(vtkPoints* foreground, vtkPoints* background) {}
void vtkGraphCut::SetCostFunction(vtkGraphCutCostFunction* costFunction) {}

#pragma mark - Protected

vtkGraphCut::vtkGraphCut() { }
vtkGraphCut::~vtkGraphCut() { }
