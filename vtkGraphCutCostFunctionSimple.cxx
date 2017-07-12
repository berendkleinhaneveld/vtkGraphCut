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
	Superclass::PrintSelf(os, indent);
}	

void vtkGraphCutCostFunctionSimple::Reset() {
	_nodes = NULL;
	_edges = NULL;
	_imageData = NULL;
}

void vtkGraphCutCostFunctionSimple::Update() {
	if (!_nodes) {
		vtkWarningMacro(<< "No nodes are set for cost function.");
		return;
	}

	if (!_edges) {
		vtkWarningMacro(<< "No edges are set for cost function.");
		return;
	}

	if (!_imageData) {
		vtkWarningMacro(<< "No image data is set for cost function.");
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
