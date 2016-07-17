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
	Superclass::PrintSelf(os, indent);
}

void vtkGraphCut::Reset() {
    _graphCut->Reset();
}

void vtkGraphCut::Update() {
    _graphCut->Update();
}

vtkImageData* vtkGraphCut::GetOutput() {
    return _graphCut->GetOutput();
}

void vtkGraphCut::SetInput(vtkImageData* imageData) {
    _graphCut->SetInput(imageData);
}

vtkImageData* vtkGraphCut::GetInput() {
    return _graphCut->GetInput();
}

void vtkGraphCut::SetSeedPoints(vtkPoints* foreground, vtkPoints* background) {
    _graphCut->SetSeedPoints(foreground, background);
}

vtkPoints* vtkGraphCut::GetForegroundPoints() {
    return _graphCut->GetForegroundPoints();
}

vtkPoints* vtkGraphCut::GetBackgroundPoints() {
    return _graphCut->GetBackgroundPoints();
}

void vtkGraphCut::SetCostFunction(vtkGraphCutCostFunction* costFunction) {
    _graphCut->SetCostFunction(costFunction);
}

vtkGraphCutCostFunction* vtkGraphCut::GetCostFunction() {
    return _graphCut->GetCostFunction();
}

void vtkGraphCut::SetConnectivity(vtkConnectivity connectivity) {
    _graphCut->SetConnectivity(connectivity);
}

vtkConnectivity vtkGraphCut::GetConnectivity() {
    return _graphCut->GetConnectivity();
}

// Protected

vtkGraphCut::vtkGraphCut() {
    _graphCut = vtkGraphCutProtected::New();
}

vtkGraphCut::~vtkGraphCut() {
    _graphCut->Delete();
}
