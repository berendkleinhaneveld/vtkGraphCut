//
//  vtkGraphCutCostFunction.cxx
//  vtkGraphCut
//
//  Created by Berend Klein Haneveld.
//
//

#include "vtkGraphCutCostFunction.h"
#include "vtkNode.h"
#include "Edge.h"


vtkStandardNewMacro(vtkGraphCutCostFunction);


void vtkGraphCutCostFunction::PrintSelf(ostream& os, vtkIndent indent) {
	this->Superclass::PrintSelf(os, indent);
}

void vtkGraphCutCostFunction::Reset() { }
void vtkGraphCutCostFunction::Update() { }
void vtkGraphCutCostFunction::SetNodes(std::vector<vtkNode>* nodes) {
	
}
void vtkGraphCutCostFunction::SetEdges(std::vector<Edge>* edges) {
	
}
void vtkGraphCutCostFunction::SetInput(vtkImageData* imageData) {
	
}


#pragma mark - Protected

vtkGraphCutCostFunction::vtkGraphCutCostFunction() { }
vtkGraphCutCostFunction::~vtkGraphCutCostFunction() { }
