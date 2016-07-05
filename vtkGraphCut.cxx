//
//  vtkGraphCut.cxx
//  vtkGraphCut
//
//  Created by Berend Klein Haneveld.
//
//

#include "vtkGraphCut.h"
#include "vtkGraphCutProtected.h"

vtkStandardNewMacro(vtkGraphCut);

// Public

void vtkGraphCut::PrintSelf(ostream& os, vtkIndent indent) {
	this->Superclass::PrintSelf(os, indent);
}

void vtkGraphCut::Reset() {
    this->graphCut->Reset();
}

void vtkGraphCut::Update() {
    this->graphCut->Update();
}

vtkImageData* vtkGraphCut::GetOutput() {
    return this->graphCut->GetOutput();
}

void vtkGraphCut::SetInput(vtkImageData* imageData) {
    this->graphCut->SetInput(imageData);
}

vtkImageData* vtkGraphCut::GetInput() {
    return this->graphCut->GetInput();
}

void vtkGraphCut::SetSeedPoints(vtkPoints* foreground, vtkPoints* background) {
    this->graphCut->SetSeedPoints(foreground, background);
}

vtkPoints* vtkGraphCut::GetForegroundPoints() {
    return this->graphCut->GetForegroundPoints();
}

vtkPoints* vtkGraphCut::GetBackgroundPoints() {
    return this->graphCut->GetBackgroundPoints();
}

void vtkGraphCut::SetCostFunction(vtkGraphCutCostFunction* costFunction) {
    this->graphCut->SetCostFunction(costFunction);
}

vtkGraphCutCostFunction* vtkGraphCut::GetCostFunction() {
    return this->graphCut->GetCostFunction();
}

void vtkGraphCut::SetConnectivity(vtkConnectivity connectivity) {
    this->graphCut->SetConnectivity(connectivity);
}

vtkConnectivity vtkGraphCut::GetConnectivity() {
    return this->graphCut->GetConnectivity();
}

// Protected

vtkGraphCut::vtkGraphCut() {
    this->graphCut = vtkGraphCutProtected::New();
}

vtkGraphCut::~vtkGraphCut() {
    this->graphCut->Delete();
}
