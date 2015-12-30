#include "iostream"
#include "vtkGraphCut.h"

void runExample();

int main(int argc, char const *argv[])
{
	vtkGraphCut* graphCut = vtkGraphCut::New();
	graphCut->Print(std::cout);
	graphCut->Delete();
	return 0;
}

void runExample() {
	vtkImageData* imageData = vtkImageData::New();
	vtkGraphCut* graphCut = vtkGraphCut::New();
	vtkGraphCutCostFunction* costFunction = vtkGraphCutCostFunction::New();
	vtkPoints* foregroundPoints = vtkPoints::New();
	vtkPoints* backgroundPoints = vtkPoints::New();

	graphCut->SetInput(imageData);
	graphCut->SetCostFunction(costFunction);
	graphCut->SetSeedPoints(foregroundPoints, backgroundPoints);
	graphCut->Update();

	vtkImageData* result = graphCut->GetOutput();

	backgroundPoints->Delete();
	foregroundPoints->Delete();
	costFunction->Delete();
	graphCut->Delete();
	imageData->Delete();
}
