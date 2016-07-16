//
//  vtkGraphCutCostFunctionSimple.cxx
//  vtkGraphCut
//
//  Created by Berend Klein Haneveld.
//
//

#include "vtkGraphCutCostFunctionSimple.h"

#pragma mark - Public


vtkStandardNewMacro(vtkGraphCutCostFunctionSimple);

void vtkGraphCutCostFunctionSimple::PrintSelf(ostream& os, vtkIndent indent) {
	this->Superclass::PrintSelf(os, indent);
}	

void vtkGraphCutCostFunctionSimple::Reset() {
	this->nodes = NULL;
	this->edges = NULL;
	this->imageData = NULL;
}

void vtkGraphCutCostFunctionSimple::Update() {
	if (!this->nodes) {
		std::cout << "Warning: no nodes are set for cost function\n";
		return;
	}

	if (!this->edges) {
		std::cout << "Warning: no edges are set for cost function\n";
		return;
	}

	if (!this->imageData) {
		std::cout << "Warning: no image data is set for cost function\n";
		return;
	}

}

void vtkGraphCutCostFunctionSimple::SetNodes(std::vector<Node>* nodes) {
	vtkGraphCutCostFunction::SetNodes(nodes);
}

void vtkGraphCutCostFunctionSimple::SetEdges(std::vector<Edge>* edges) {
	vtkGraphCutCostFunction::SetEdges(edges);
}

void vtkGraphCutCostFunctionSimple::SetInput(vtkImageData* imageData) {
	vtkGraphCutCostFunction::SetInput(imageData);
}


#pragma mark - Protected


vtkGraphCutCostFunctionSimple::vtkGraphCutCostFunctionSimple() { }
vtkGraphCutCostFunctionSimple::~vtkGraphCutCostFunctionSimple() { }
