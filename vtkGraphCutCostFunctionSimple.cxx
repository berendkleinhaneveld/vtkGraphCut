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
		std::cout << "Warning: no nodes are set for cost function\n";
		return;
	}

	if (!_edges) {
		std::cout << "Warning: no edges are set for cost function\n";
		return;
	}

	if (!_imageData) {
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
